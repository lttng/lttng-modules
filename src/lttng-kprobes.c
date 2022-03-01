/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * probes/lttng-kprobes.c
 *
 * LTTng kprobes integration module.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/slab.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <wrapper/irqflags.h>
#include <wrapper/rcu.h>
#include <lttng/tracer.h>
#include <blacklist/kprobes.h>

static
int lttng_kprobes_event_handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	struct lttng_kernel_event_common_private *event_priv =
		container_of(p, struct lttng_kernel_event_common_private, u.kprobe.kp);
	struct lttng_kernel_event_common *event = event_priv->pub;
	struct lttng_kernel_channel_common *chan_common;
	struct lttng_kernel_probe_ctx lttng_probe_ctx = {
		.event = event,
		.interruptible = !lttng_regs_irqs_disabled(regs),
	};
	unsigned long data = (unsigned long) p->addr;

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

		lib_ring_buffer_ctx_init(&ctx, event_recorder, sizeof(data),
					 lttng_alignof(data), &lttng_probe_ctx);
		ret = chan->ops->event_reserve(&ctx);
		if (ret < 0)
			return 0;
		chan->ops->event_write(&ctx, &data, sizeof(data), lttng_alignof(data));
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
	case LTTNG_KERNEL_EVENT_TYPE_COUNTER:
	{
		struct lttng_kernel_event_counter *event_counter =
			container_of(event, struct lttng_kernel_event_counter, parent);

		(void) event_counter->chan->ops->event_counter_add(event_counter, 1);
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

static
int lttng_kp_init(struct lttng_kprobe *lttng_kp, const char *symbol_name, uint64_t offset, uint64_t addr,
		kprobe_pre_handler_t pre_handler)
{
	/* Kprobes expects a NULL symbol name if unused */
	if (symbol_name[0] == '\0')
		symbol_name = NULL;

	lttng_kp->pre_handler = pre_handler;

	if (symbol_name) {
		lttng_kp->symbol_name =
			kzalloc(LTTNG_KERNEL_ABI_SYM_NAME_LEN * sizeof(char),
				GFP_KERNEL);
		if (!lttng_kp->symbol_name)
			return -ENOMEM;
		memcpy(lttng_kp->symbol_name, symbol_name,
		       LTTNG_KERNEL_ABI_SYM_NAME_LEN * sizeof(char));
	}

	lttng_kp->offset = offset;
	lttng_kp->addr = addr;
	return 0;
}

/*
 * Initialize event
 */
int lttng_kprobes_init_event(const char *name,
		const char *symbol_name,
		uint64_t offset,
		uint64_t addr,
		struct lttng_kernel_event_common *event)
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
	ret = lttng_kp_init(&event->priv->u.kprobe, symbol_name, offset, addr,
			lttng_kprobes_event_handler_pre);
	if (ret)
		goto error_init;
	return 0;

error_init:
	kfree(desc->event_name);
error_str:
	kfree(desc);
	return ret;
}

static
int _lttng_kprobes_register(struct lttng_kprobe *lttng_kp)
{
	/*
	 * Ensure the memory we just allocated don't notify page faults.
	 * Well.. kprobes itself puts the page fault handler on the blacklist,
	 * but we can never be too careful.
	 */
	wrapper_vmalloc_sync_mappings();

	/*
	 * Populate struct kprobe on each registration because kprobe internally
	 * does destructive changes to its state (e.g. addr=NULL).
	 */
	memset(&lttng_kp->kp, 0, sizeof(lttng_kp->kp));
	lttng_kp->kp.symbol_name = lttng_kp->symbol_name;
	lttng_kp->kp.addr = (void *)(unsigned long)lttng_kp->addr;
	lttng_kp->kp.offset = lttng_kp->offset;
	lttng_kp->kp.pre_handler = lttng_kp->pre_handler;
	return register_kprobe(&lttng_kp->kp);
}

int lttng_kprobes_register_event(struct lttng_kernel_event_common *event)
{
	return _lttng_kprobes_register(&event->priv->u.kprobe);
}

void lttng_kprobes_unregister_event(struct lttng_kernel_event_common *event)
{
	unregister_kprobe(&event->priv->u.kprobe.kp);
}

int lttng_kprobes_match_check(const char *symbol_name, uint64_t offset, uint64_t addr)
{
	struct lttng_kprobe lttng_kp;
	int ret = 0;

	memset(&lttng_kp, 0, sizeof(lttng_kp));
	ret = lttng_kp_init(&lttng_kp, symbol_name, offset, addr, NULL);
	if (ret)
		return ret;
	ret = _lttng_kprobes_register(&lttng_kp);
	if (ret) {
		ret = -ENOENT;
		goto end;
	}
	unregister_kprobe(&lttng_kp.kp);
end:
	kfree(lttng_kp.symbol_name);
	return ret;

}

void lttng_kprobes_destroy_event_private(struct lttng_kernel_event_common *event)
{
	kfree(event->priv->u.kprobe.symbol_name);
	kfree(event->priv->desc->event_name);
	kfree(event->priv->desc);
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng kprobes probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
