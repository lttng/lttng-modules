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

#include <linux/fdtable.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <lttng/events.h>
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
	struct lttng_event *event = uprobe_handler->u.event;
	struct lttng_probe_ctx lttng_probe_ctx = {
		.event = event,
		.interruptible = !lttng_regs_irqs_disabled(regs),
	};
	struct lttng_channel *chan = event->chan;
	struct lib_ring_buffer_ctx ctx;
	int ret;

	struct {
		unsigned long ip;
	} payload;

	if (unlikely(!LTTNG_READ_ONCE(chan->session->active)))
		return 0;
	if (unlikely(!LTTNG_READ_ONCE(chan->enabled)))
		return 0;
	if (unlikely(!LTTNG_READ_ONCE(event->enabled)))
		return 0;

	lib_ring_buffer_ctx_init(&ctx, chan->chan, &lttng_probe_ctx,
		sizeof(payload), lttng_alignof(payload), -1);

	ret = chan->ops->event_reserve(&ctx, event->id);
	if (ret < 0)
		return 0;

	/* Event payload. */
	payload.ip = (unsigned long)instruction_pointer(regs);

	lib_ring_buffer_align_ctx(&ctx, lttng_alignof(payload));
	chan->ops->event_write(&ctx, &payload, sizeof(payload));
	chan->ops->event_commit(&ctx);
	return 0;
}

static
int lttng_uprobes_event_notifier_handler_pre(struct uprobe_consumer *uc, struct pt_regs *regs)
{
	struct lttng_uprobe_handler *uprobe_handler =
		container_of(uc, struct lttng_uprobe_handler, up_consumer);
	struct lttng_event_notifier *event_notifier = uprobe_handler->u.event_notifier;

	if (unlikely(!READ_ONCE(event_notifier->enabled)))
		return 0;

	event_notifier->send_notification(event_notifier, NULL, NULL);
	return 0;
}

/*
 * Create event description.
 */
static
int lttng_create_uprobe_event(const char *name, struct lttng_event *event)
{
	struct lttng_event_desc *desc;
	struct lttng_event_field *fields;
	int ret;

	desc = kzalloc(sizeof(*event->desc), GFP_KERNEL);
	if (!desc)
		return -ENOMEM;
	desc->name = kstrdup(name, GFP_KERNEL);
	if (!desc->name) {
		ret = -ENOMEM;
		goto error_str;
	}

	desc->nr_fields = 1;
	desc->fields = fields =
		kzalloc(1 * sizeof(struct lttng_event_field), GFP_KERNEL);

	if (!desc->fields) {
		ret = -ENOMEM;
		goto error_fields;
	}
	fields[0].name = "ip";
	fields[0].type.atype = atype_integer;
	fields[0].type.u.integer.size = sizeof(unsigned long) * CHAR_BIT;
	fields[0].type.u.integer.alignment = lttng_alignof(unsigned long) * CHAR_BIT;
	fields[0].type.u.integer.signedness = lttng_is_signed_type(unsigned long);
	fields[0].type.u.integer.reverse_byte_order = 0;
	fields[0].type.u.integer.base = 16;
	fields[0].type.u.integer.encoding = lttng_encode_none;

	desc->owner = THIS_MODULE;
	event->desc = desc;

	return 0;

error_fields:
	kfree(desc->name);
error_str:
	kfree(desc);
	return ret;
}

/*
 * Create event_notifier description.
 */
static
int lttng_create_uprobe_event_notifier(const char *name, struct lttng_event_notifier *event_notifier)
{
	struct lttng_event_desc *desc;
	int ret;

	desc = kzalloc(sizeof(*event_notifier->desc), GFP_KERNEL);
	if (!desc)
		return -ENOMEM;
	desc->name = kstrdup(name, GFP_KERNEL);
	if (!desc->name) {
		ret = -ENOMEM;
		goto error_str;
	}

	desc->nr_fields = 0;

	desc->owner = THIS_MODULE;
	event_notifier->desc = desc;

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
	file = fcheck(fd);
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
	struct lttng_kernel_event_callsite __user *callsite,
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

	/* Ensure the memory we just allocated don't event_notifier page faults. */
	wrapper_vmalloc_sync_mappings();

	uprobe_handler->u.event = priv_data;
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

int lttng_uprobes_event_add_callsite(struct lttng_event *event,
	struct lttng_kernel_event_callsite __user *callsite)
{
	return lttng_uprobes_add_callsite(&event->u.uprobe, callsite,
		lttng_uprobes_event_handler_pre, event);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_event_add_callsite);

int lttng_uprobes_event_notifier_add_callsite(struct lttng_event_notifier *event_notifier,
	struct lttng_kernel_event_callsite __user *callsite)
{
	return lttng_uprobes_add_callsite(&event_notifier->u.uprobe, callsite,
		lttng_uprobes_event_notifier_handler_pre, event_notifier);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_event_notifier_add_callsite);

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

int lttng_uprobes_register_event(const char *name, int fd, struct lttng_event *event)
{
	int ret = 0;

	ret = lttng_create_uprobe_event(name, event);
	if (ret)
		goto error;

	ret = lttng_uprobes_register(&event->u.uprobe, fd);
	if (ret)
		goto register_error;

	return 0;

register_error:
	kfree(event->desc->name);
	kfree(event->desc);
error:
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_uprobes_register_event);

int lttng_uprobes_register_event_notifier(const char *name, int fd,
		struct lttng_event_notifier *event_notifier)
{
	int ret = 0;

	ret = lttng_create_uprobe_event_notifier(name, event_notifier);
	if (ret)
		goto error;

	ret = lttng_uprobes_register(&event_notifier->u.uprobe, fd);
	if (ret)
		goto register_error;

	return 0;

register_error:
	kfree(event_notifier->desc->name);
	kfree(event_notifier->desc);
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

void lttng_uprobes_unregister_event(struct lttng_event *event)
{
	lttng_uprobes_unregister(event->u.uprobe.inode, &event->u.uprobe.head);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_unregister_event);

void lttng_uprobes_unregister_event_notifier(struct lttng_event_notifier *event_notifier)
{
	lttng_uprobes_unregister(event_notifier->u.uprobe.inode, &event_notifier->u.uprobe.head);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_unregister_event_notifier);

void lttng_uprobes_destroy_event_private(struct lttng_event *event)
{
	iput(event->u.uprobe.inode);
	kfree(event->desc->name);
	kfree(event->desc);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_destroy_event_private);

void lttng_uprobes_destroy_event_notifier_private(struct lttng_event_notifier *event_notifier)
{
	iput(event_notifier->u.uprobe.inode);
	kfree(event_notifier->desc->name);
	kfree(event_notifier->desc);
}
EXPORT_SYMBOL_GPL(lttng_uprobes_destroy_event_notifier_private);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Yannick Brosseau");
MODULE_DESCRIPTION("Linux Trace Toolkit Uprobes Support");
