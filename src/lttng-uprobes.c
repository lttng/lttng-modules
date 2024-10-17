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
#include <linux/file.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/uprobes.h>

#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <lttng/tracer.h>
#include <wrapper/irqflags.h>
#include <wrapper/rcu.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>

static
int lttng_uprobes_event_handler_pre(struct uprobe_consumer *uc, struct pt_regs *regs)
{
	struct lttng_uprobe_handler *uprobe_handler =
		container_of(uc, struct lttng_uprobe_handler, up_consumer);
	struct lttng_kernel_event_common *event = uprobe_handler->event;
	struct lttng_kernel_channel_common *chan_common;
	struct lttng_kernel_probe_ctx lttng_probe_ctx = {
		.event = event,
		.interruptible = !lttng_regs_irqs_disabled(regs),
	};
	struct {
		unsigned long ip;
	} payload;

	if (unlikely(!LTTNG_READ_ONCE(event->enabled)))
		goto end;

	chan_common = lttng_kernel_get_chan_common_from_event_common(event);
	if (chan_common) {
		struct lttng_kernel_session *session = chan_common->session;
		struct lttng_kernel_id_tracker_rcu *lf;

		if (unlikely(!LTTNG_READ_ONCE(session->active)))
			goto end;
		if (unlikely(!LTTNG_READ_ONCE(chan_common->enabled)))
			goto end;
		lf = lttng_rcu_dereference(session->pid_tracker.p);
		if (lf && likely(!lttng_id_tracker_lookup(lf, current->tgid)))
			goto end;
		lf = lttng_rcu_dereference(session->vpid_tracker.p);
		if (lf && likely(!lttng_id_tracker_lookup(lf, task_tgid_vnr(current))))
			goto end;
		lf = lttng_rcu_dereference(session->uid_tracker.p);
		if (lf && likely(!lttng_id_tracker_lookup(lf, from_kuid_munged(&init_user_ns, current_uid()))))
			goto end;
		lf = lttng_rcu_dereference(session->vuid_tracker.p);
		if (lf && likely(!lttng_id_tracker_lookup(lf, from_kuid_munged(current_user_ns(), current_uid()))))
			goto end;
		lf = lttng_rcu_dereference(session->gid_tracker.p);
		if (lf && likely(!lttng_id_tracker_lookup(lf, from_kgid_munged(&init_user_ns, current_gid()))))
			goto end;
		lf = lttng_rcu_dereference(session->vgid_tracker.p);
		if (lf && likely(!lttng_id_tracker_lookup(lf, from_kgid_munged(current_user_ns(), current_gid()))))
			goto end;
	}
	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
	{
		struct lttng_kernel_event_recorder *event_recorder =
			container_of(event, struct lttng_kernel_event_recorder, parent);
		struct lttng_kernel_channel_buffer *chan = event_recorder->chan;
		struct lttng_kernel_ring_buffer_ctx ctx;
		int ret;

		lib_ring_buffer_ctx_init(&ctx, event_recorder,
			sizeof(payload), lttng_alignof(payload), &lttng_probe_ctx);

		ret = chan->ops->event_reserve(&ctx);
		if (ret < 0)
			return 0;

		/* Event payload. */
		payload.ip = (unsigned long)instruction_pointer(regs);

		chan->ops->event_write(&ctx, &payload, sizeof(payload), lttng_alignof(payload));
		chan->ops->event_commit(&ctx);
		break;
	}
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
	{
		struct lttng_kernel_event_notifier *event_notifier =
			container_of(event, struct lttng_kernel_event_notifier, parent);
		struct lttng_kernel_notification_ctx notif_ctx;

		notif_ctx.eval_capture = false;
		event_notifier->notification_send(event_notifier, NULL, NULL, &notif_ctx);
		break;
	}
	case LTTNG_KERNEL_EVENT_TYPE_COUNTER:
	{
		struct lttng_kernel_event_counter *event_counter =
			container_of(event, struct lttng_kernel_event_counter, parent);
		struct lttng_kernel_event_counter_ctx event_counter_ctx;

		event_counter_ctx.args_available = false;
		/* uprobes is invoked with preemption enabled. */
		rcu_read_lock_sched_notrace();
		(void) event_counter->chan->ops->counter_hit(event_counter,
			NULL, NULL, &event_counter_ctx);
		rcu_read_unlock_sched_notrace();
		break;
	}
	default:
		WARN_ON_ONCE(1);
	}
end:
	return 0;
}

static const struct lttng_kernel_event_field *event_fields[] = {
	lttng_kernel_static_event_field("ip",
		lttng_kernel_static_type_integer_from_type(unsigned long, __BYTE_ORDER, 16),
		false, false),
};

static const struct lttng_kernel_tracepoint_class tp_class = {
	.nr_fields = ARRAY_SIZE(event_fields),
	.fields = event_fields,
};

/*
 * Create event description.
 */
static
int lttng_create_uprobe_event(const char *name, struct lttng_kernel_event_common *event)
{
	struct lttng_kernel_event_desc *desc;
	int ret;

	desc = kzalloc(sizeof(*desc), GFP_KERNEL);
	if (!desc)
		return -ENOMEM;
	desc->tp_class = &tp_class;
	desc->event_name = kstrdup(name, GFP_KERNEL);
	if (!desc->event_name) {
		ret = -ENOMEM;
		goto error_str;
	}
	desc->owner = THIS_MODULE;
	event->priv->desc = desc;

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
	file = lttng_lookup_fdget_rcu(fd);
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
	if (file)
		fput(file);
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

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,12,0))
	ret = 0;
	uprobe_handler->uprobe = uprobe_register(uprobe->inode,
		      uprobe_handler->offset, 0, &uprobe_handler->up_consumer);
	if (IS_ERR(uprobe_handler->uprobe))
		ret = -1;
#else
	ret = uprobe_register(uprobe->inode,
		      uprobe_handler->offset, &uprobe_handler->up_consumer);
#endif
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

int lttng_uprobes_register_event(const char *name, int fd, struct lttng_kernel_event_common *event)
{
	int ret = 0;

	ret = lttng_create_uprobe_event(name, event);
	if (ret)
		goto error;

	ret = lttng_uprobes_register(&event->priv->u.uprobe, fd);
	if (ret)
		goto register_error;

	return 0;

register_error:
	kfree(event->priv->desc->event_name);
	kfree(event->priv->desc);
error:
	return ret;
}

static
void lttng_uprobes_unregister(struct inode *inode, struct list_head *head)
{
	struct lttng_uprobe_handler *iter, *tmp;

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,12,0))
	/*
	 * Iterate over the list of handler, unregister each uprobe.
	 */
	list_for_each_entry(iter, head, node) {
		uprobe_unregister_nosync(iter->uprobe, &iter->up_consumer);
		iter->uprobe = NULL;
	}

	/*
	 * Call synchronize_srcu() on uprobes_srcu.
	 */
	uprobe_unregister_sync();

	/*
	 * Iterate over the list of handler, remove each handler from the list
	 * and free the struct.
	 */
	list_for_each_entry_safe(iter, tmp, head, node) {
		list_del(&iter->node);
		kfree(iter);
	}
#else
	/*
	 * Iterate over the list of handler, unregister each uprobe, remove
	 * each handler from the list and free the struct.
	 */
	list_for_each_entry_safe(iter, tmp, head, node) {
		uprobe_unregister(inode, iter->offset, &iter->up_consumer);
		list_del(&iter->node);
		kfree(iter);
	}
#endif
}

void lttng_uprobes_unregister_event(struct lttng_kernel_event_common *event)
{
	lttng_uprobes_unregister(event->priv->u.uprobe.inode, &event->priv->u.uprobe.head);
}

void lttng_uprobes_destroy_event_private(struct lttng_kernel_event_common *event)
{
	iput(event->priv->u.uprobe.inode);
	kfree(event->priv->desc->event_name);
	kfree(event->priv->desc);
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Yannick Brosseau");
MODULE_DESCRIPTION("Linux Trace Toolkit Uprobes Support");
