/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * probes/lttng-uprobes.c
 *
 * LTTng uprobes integration module.
 *
 * Copyright (C) 2013 Yannick Brosseau <yannick.brosseau@gmail.com>
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 */

#include <wrapper/fdtable.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <lttng/tracer.h>
#include <wrapper/irqflags.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/uprobes.h>
#include <wrapper/vmalloc.h>

static
int lttng_uprobes_event_handler_pre(struct uprobe_consumer *uc, struct pt_regs *regs)
{
	struct lttng_uprobe_handler *uprobe_handler =
		container_of(uc, struct lttng_uprobe_handler, up_consumer);
	struct lttng_kernel_event_common *event = uprobe_handler->event;
	struct lttng_probe_ctx lttng_probe_ctx = {
		.event = event,
		.interruptible = !lttng_regs_irqs_disabled(regs),
	};
	struct {
		unsigned long ip;
	} payload;

	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
	{
		struct lttng_kernel_event_recorder *event_recorder =
			container_of(event, struct lttng_kernel_event_recorder, parent);
		struct lttng_channel *chan = event_recorder->chan;

		if (unlikely(!LTTNG_READ_ONCE(chan->session->active)))
			return 0;
		if (unlikely(!LTTNG_READ_ONCE(chan->enabled)))
			return 0;
		break;
	}
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
		break;
	default:
		WARN_ON_ONCE(1);
	}

	if (unlikely(!LTTNG_READ_ONCE(event->enabled)))
		return 0;

	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
	{
		struct lttng_kernel_event_recorder *event_recorder =
			container_of(event, struct lttng_kernel_event_recorder, parent);
		struct lttng_channel *chan = event_recorder->chan;
		struct lib_ring_buffer_ctx ctx;
		int ret;

		lib_ring_buffer_ctx_init(&ctx, chan->chan,
			sizeof(payload), lttng_alignof(payload), -1, &lttng_probe_ctx);

		ret = chan->ops->event_reserve(&ctx, event_recorder->priv->id);
		if (ret < 0)
			return 0;

		/* Event payload. */
		payload.ip = (unsigned long)instruction_pointer(regs);

		lib_ring_buffer_align_ctx(&ctx, lttng_alignof(payload));
		chan->ops->event_write(&ctx, &payload, sizeof(payload));
		chan->ops->event_commit(&ctx);
		break;
	}
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
	{
		struct lttng_kernel_event_notifier *event_notifier =
			container_of(event, struct lttng_kernel_event_notifier, parent);
		struct lttng_kernel_notification_ctx notif_ctx;

		notif_ctx.eval_capture = LTTNG_READ_ONCE(event_notifier->eval_capture);
		event_notifier->notification_send(event_notifier, NULL, NULL, &notif_ctx);
		break;
	}
	default:
		WARN_ON_ONCE(1);
	}
	return 0;
}

static const struct lttng_kernel_type_common *event_type =
	lttng_kernel_static_type_integer_from_type(unsigned long, __BYTE_ORDER, 16);

/*
 * Create event description.
 */
static
int lttng_create_uprobe_event(const char *name, struct lttng_kernel_event_recorder *event_recorder)
{
	const struct lttng_kernel_event_field **fieldp_array;
	struct lttng_kernel_event_field *field;
	struct lttng_kernel_event_desc *desc;
	int ret;

	desc = kzalloc(sizeof(*desc), GFP_KERNEL);
	if (!desc)
		return -ENOMEM;
	desc->event_name = kstrdup(name, GFP_KERNEL);
	if (!desc->event_name) {
		ret = -ENOMEM;
		goto error_str;
	}
	desc->nr_fields = 1;
	fieldp_array = kzalloc(1 * sizeof(struct lttng_kernel_event_field *), GFP_KERNEL);
	if (!fieldp_array) {
		ret = -ENOMEM;
		goto error_fieldp_array;
	}
	desc->fields = fieldp_array;
	desc->fields[0] = field =
		kzalloc(sizeof(struct lttng_kernel_event_field), GFP_KERNEL);
	if (!field) {
		ret = -ENOMEM;
		goto error_field;
	}
	field->name = "ip";
	field->type = event_type;
	desc->owner = THIS_MODULE;
	event_recorder->priv->parent.desc = desc;

	return 0;

error_field:
	kfree(fieldp_array);
error_fieldp_array:
	kfree(desc->event_name);
error_str:
	kfree(desc);
	return ret;
}

/*
 * Create event_notifier description.
 */
static
int lttng_create_uprobe_event_notifier(const char *name, struct lttng_kernel_event_notifier *event_notifier)
{
	struct lttng_kernel_event_desc *desc;
	int ret;

	desc = kzalloc(sizeof(*desc), GFP_KERNEL);
	if (!desc)
		return -ENOMEM;
	desc->event_name = kstrdup(name, GFP_KERNEL);
	if (!desc->event_name) {
		ret = -ENOMEM;
		goto error_str;
	}

	desc->nr_fields = 0;

	desc->owner = THIS_MODULE;
	event_notifier->priv->parent.desc = desc;

	return 0;

error_str:
	kfree(desc);
	return ret;
}

/*
 * Returns the inode struct from the current task and an fd. The inode is
 * grabbed by this function and must be put once we are done with it using
 * iput().
 */
static struct inode *get_inode_from_fd(int fd)
{
	struct file *file;
	struct inode *inode;

	rcu_read_lock();
	/*
	 * Returns the file backing the given fd. Needs to be done inside an RCU
	 * critical section.
	 */
	file = lttng_lookup_fd_rcu(fd);
	if (file == NULL) {
		printk(KERN_WARNING "LTTng: Cannot access file backing the fd(%d)\n", fd);
		inode = NULL;
		goto error;
	}

	/* Grab a reference on the inode. */
	inode = igrab(file->f_path.dentry->d_inode);
	if (inode == NULL)
		printk(KERN_WARNING "LTTng: Cannot grab a reference on the inode.\n");
error:
	rcu_read_unlock();
	return inode;
}


static
int lttng_uprobes_add_callsite(struct lttng_uprobe *uprobe,
	struct lttng_kernel_abi_event_callsite __user *callsite,
	int (*handler)(struct uprobe_consumer *self, struct pt_regs *regs),
	void *priv_data)
{
	int ret = 0;
	struct lttng_uprobe_handler *uprobe_handler;

	if (!priv_data) {
		ret = -EINVAL;
		goto end;
	}

	uprobe_handler = kzalloc(sizeof(struct lttng_uprobe_handler), GFP_KERNEL);
	if (!uprobe_handler) {
		printk(KERN_WARNING "LTTng: Error allocating uprobe_handler");
		ret = -ENOMEM;
		goto end;
	}

	/* Ensure the memory we just allocated don't notify page faults. */
	wrapper_vmalloc_sync_mappings();

	uprobe_handler->event = priv_data;
	uprobe_handler->up_consumer.handler = handler;

	ret = copy_from_user(&uprobe_handler->offset, &callsite->u.uprobe.offset, sizeof(uint64_t));
	if (ret) {
		goto register_error;
	}

	ret = wrapper_uprobe_register(uprobe->inode,
		      uprobe_handler->offset, &uprobe_handler->up_consumer);
	if (ret) {
		printk(KERN_WARNING "LTTng: Error registering probe on inode %lu "
		       "and offset 0x%llx\n", uprobe->inode->i_ino,
		       uprobe_handler->offset);
		ret = -1;
		goto register_error;
	}

	list_add(&uprobe_handler->node, &uprobe->head);

	return ret;

register_error:
	kfree(uprobe_handler);
end:
	return ret;
}

int lttng_uprobes_event_add_callsite(struct lttng_kernel_event_common *event,
	struct lttng_kernel_abi_event_callsite __user *callsite)
{
	return lttng_uprobes_add_callsite(&event->priv->u.uprobe, callsite,
		lttng_uprobes_event_handler_pre, event);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_event_add_callsite);

static
int lttng_uprobes_register(struct lttng_uprobe *uprobe, int fd)
{
	int ret = 0;
	struct inode *inode;

	inode = get_inode_from_fd(fd);
	if (!inode) {
		printk(KERN_WARNING "LTTng: Cannot get inode from fd\n");
		ret = -EBADF;
		goto inode_error;
	}
	uprobe->inode = inode;
	INIT_LIST_HEAD(&uprobe->head);

inode_error:
	return ret;
}

int lttng_uprobes_register_event(const char *name, int fd, struct lttng_kernel_event_recorder *event_recorder)
{
	int ret = 0;

	ret = lttng_create_uprobe_event(name, event_recorder);
	if (ret)
		goto error;

	ret = lttng_uprobes_register(&event_recorder->priv->parent.u.uprobe, fd);
	if (ret)
		goto register_error;

	return 0;

register_error:
	kfree(event_recorder->priv->parent.desc->event_name);
	kfree(event_recorder->priv->parent.desc);
error:
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_uprobes_register_event);

int lttng_uprobes_register_event_notifier(const char *name, int fd,
		struct lttng_kernel_event_notifier *event_notifier)
{
	int ret = 0;

	ret = lttng_create_uprobe_event_notifier(name, event_notifier);
	if (ret)
		goto error;

	ret = lttng_uprobes_register(&event_notifier->priv->parent.u.uprobe, fd);
	if (ret)
		goto register_error;

	return 0;

register_error:
	kfree(event_notifier->priv->parent.desc->event_name);
	kfree(event_notifier->priv->parent.desc);
error:
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_uprobes_register_event_notifier);

static
void lttng_uprobes_unregister(struct inode *inode, struct list_head *head)
{
	struct lttng_uprobe_handler *iter, *tmp;

	/*
	 * Iterate over the list of handler, remove each handler from the list
	 * and free the struct.
	 */
	list_for_each_entry_safe(iter, tmp, head, node) {
		wrapper_uprobe_unregister(inode, iter->offset, &iter->up_consumer);
		list_del(&iter->node);
		kfree(iter);
	}
}

void lttng_uprobes_unregister_event(struct lttng_kernel_event_recorder *event_recorder)
{
	lttng_uprobes_unregister(event_recorder->priv->parent.u.uprobe.inode, &event_recorder->priv->parent.u.uprobe.head);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_unregister_event);

void lttng_uprobes_unregister_event_notifier(struct lttng_kernel_event_notifier *event_notifier)
{
	lttng_uprobes_unregister(event_notifier->priv->parent.u.uprobe.inode, &event_notifier->priv->parent.u.uprobe.head);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_unregister_event_notifier);

void lttng_uprobes_destroy_event_private(struct lttng_kernel_event_recorder *event_recorder)
{
	iput(event_recorder->priv->parent.u.uprobe.inode);
	kfree(event_recorder->priv->parent.desc->fields[0]);
	kfree(event_recorder->priv->parent.desc->fields);
	kfree(event_recorder->priv->parent.desc->event_name);
	kfree(event_recorder->priv->parent.desc);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_destroy_event_private);

void lttng_uprobes_destroy_event_notifier_private(struct lttng_kernel_event_notifier *event_notifier)
{
	iput(event_notifier->priv->parent.u.uprobe.inode);
	kfree(event_notifier->priv->parent.desc->event_name);
	kfree(event_notifier->priv->parent.desc);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_destroy_event_notifier_private);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Yannick Brosseau");
MODULE_DESCRIPTION("Linux Trace Toolkit Uprobes Support");
