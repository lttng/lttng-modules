/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * probes/lttng-kretprobes.c
 *
 * LTTng kretprobes integration module.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <wrapper/kprobes.h>
#include <linux/slab.h>
#include <linux/kref.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/compiler_attributes.h>
#include <wrapper/vmalloc.h>
#include <wrapper/irqflags.h>
#include <wrapper/rcu.h>
#include <lttng/tracer.h>
#include <blacklist/kprobes.h>

enum lttng_kretprobe_type {
	EVENT_ENTRY	= 0,
	EVENT_EXIT	= 1,
};

struct lttng_krp {
	struct kretprobe krp;
	char *symbol_name;
	uint64_t addr;
	uint64_t offset;
	kretprobe_handler_t entry_handler;
	kretprobe_handler_t exit_handler;
	struct lttng_kernel_event_common *event[NR_LTTNG_KRETPROBE_ENTRYEXIT];	/* entry/exit events */

	int refcount_register;			/* register kretprobe when > 0 */
	struct kref kref_alloc;
};

static
int _lttng_kretprobes_handler(struct kretprobe_instance *krpi,
			      struct pt_regs *regs,
			      enum lttng_kretprobe_type type)
{
	struct lttng_krp *lttng_krp =
		container_of(lttng_get_kretprobe(krpi), struct lttng_krp, krp);
	struct lttng_kernel_event_common *event = lttng_krp->event[type];
	struct lttng_kernel_channel_common *chan_common;
	struct lttng_kernel_probe_ctx lttng_probe_ctx = {
		.event = event,
		.interruptible = !lttng_regs_irqs_disabled(regs),
	};
	struct {
		unsigned long ip;
		unsigned long parent_ip;
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

		payload.ip = (unsigned long) lttng_get_kretprobe(krpi)->kp.addr;
		payload.parent_ip = lttng_get_kretprobe_retaddr(krpi);

		lib_ring_buffer_ctx_init(&ctx, event_recorder, sizeof(payload),
					 lttng_alignof(payload), &lttng_probe_ctx);
		ret = chan->ops->event_reserve(&ctx);
		if (ret < 0)
			return 0;
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
		(void) event_counter->chan->ops->counter_hit(event_counter,
			NULL, NULL, &event_counter_ctx);
		break;
	}
	default:
		WARN_ON_ONCE(1);
	}
end:
	return 0;
}

static
int lttng_kretprobes_handler_entry(struct kretprobe_instance *krpi,
				   struct pt_regs *regs)
{
	return _lttng_kretprobes_handler(krpi, regs, EVENT_ENTRY);
}

static
int lttng_kretprobes_handler_exit(struct kretprobe_instance *krpi,
				  struct pt_regs *regs)
{
	return _lttng_kretprobes_handler(krpi, regs, EVENT_EXIT);
}

static const struct lttng_kernel_event_field *event_fields[] = {
	lttng_kernel_static_event_field("ip",
		lttng_kernel_static_type_integer_from_type(unsigned long, __BYTE_ORDER, 16),
		false, false),
	lttng_kernel_static_event_field("parent_ip",
		lttng_kernel_static_type_integer_from_type(unsigned long, __BYTE_ORDER, 16),
		false, false),
};

static const struct lttng_kernel_tracepoint_class tp_class = {
	.nr_fields = ARRAY_SIZE(event_fields),
	.fields = event_fields,
};

static
int lttng_krp_init(struct lttng_krp *lttng_krp,
		const char *symbol_name, uint64_t offset, uint64_t addr,
		kretprobe_handler_t entry_handler,
		kretprobe_handler_t exit_handler)
{
	/* Kretprobes expects a NULL symbol name if unused */
	if (symbol_name[0] == '\0')
		symbol_name = NULL;

	lttng_krp->entry_handler = entry_handler;
	lttng_krp->exit_handler = exit_handler;

	if (symbol_name) {
		lttng_krp->symbol_name =
			kzalloc(LTTNG_KERNEL_ABI_SYM_NAME_LEN * sizeof(char),
				GFP_KERNEL);
		if (!lttng_krp->symbol_name)
			return -ENOMEM;
		memcpy(lttng_krp->symbol_name, symbol_name,
		       LTTNG_KERNEL_ABI_SYM_NAME_LEN * sizeof(char));
	}

	lttng_krp->offset = offset;
	lttng_krp->addr = addr;
	return 0;
}

struct lttng_krp *lttng_kretprobes_create_krp(const char *symbol_name,
			uint64_t offset, uint64_t addr)
{
	struct lttng_krp *lttng_krp;
	int ret;

	lttng_krp = kzalloc(sizeof(*lttng_krp), GFP_KERNEL);
	if (!lttng_krp)
		return NULL;
	kref_init(&lttng_krp->kref_alloc);
	ret = lttng_krp_init(lttng_krp, symbol_name, offset, addr,
			lttng_kretprobes_handler_entry,
			lttng_kretprobes_handler_exit);
	if (ret)
		goto error_init;
	return lttng_krp;

error_init:
	kfree(lttng_krp);
	return NULL;
}

static
void _lttng_kretprobes_release(struct kref *kref)
{
	struct lttng_krp *lttng_krp = container_of(kref, struct lttng_krp, kref_alloc);

	kfree(lttng_krp->krp.kp.symbol_name);
	kfree(lttng_krp);
}

void lttng_kretprobes_put_krp(struct lttng_krp *krp)
{
	kref_put(&krp->kref_alloc, _lttng_kretprobes_release);
}

/*
 * Initialize event
 */
int lttng_kretprobes_init_event(const char *name,
		enum lttng_kretprobe_entryexit entryexit,
		struct lttng_kernel_event_common *event,
		struct lttng_krp *krp)
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
	event->priv->u.kretprobe.lttng_krp = krp;
	event->priv->u.kretprobe.entryexit = entryexit;
	kref_get(&krp->kref_alloc);
	krp->event[entryexit] = event;
	return 0;

error_str:
	kfree(desc);
	return ret;
}

static
int _lttng_kretprobes_register(struct lttng_krp *lttng_krp)
{
	if (lttng_krp->refcount_register++ != 0)
		return 0;	/* Already registered */

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
	memset(&lttng_krp->krp, 0, sizeof(lttng_krp->krp));
	lttng_krp->krp.kp.symbol_name = lttng_krp->symbol_name;
	lttng_krp->krp.kp.addr = (void *)(unsigned long)lttng_krp->addr;
	lttng_krp->krp.kp.offset = lttng_krp->offset;
	lttng_krp->krp.entry_handler = lttng_krp->entry_handler;
	lttng_krp->krp.handler = lttng_krp->exit_handler;
	return register_kretprobe(&lttng_krp->krp);
}

int lttng_kretprobes_register_event(struct lttng_kernel_event_common *event)
{
	return _lttng_kretprobes_register(event->priv->u.kretprobe.lttng_krp);
}

void lttng_kretprobes_unregister_event(struct lttng_kernel_event_common *event)
{
	struct lttng_krp *lttng_krp = event->priv->u.kretprobe.lttng_krp;

	WARN_ON_ONCE(!lttng_krp->refcount_register);
	if (--lttng_krp->refcount_register != 0)
		return;		/* Already unregistered */
	unregister_kretprobe(&lttng_krp->krp);
}

int lttng_kretprobes_match_check(const char *symbol_name, uint64_t offset, uint64_t addr)
{
	struct lttng_krp lttng_krp;
	int ret = 0;

	memset(&lttng_krp, 0, sizeof(lttng_krp));
	ret = lttng_krp_init(&lttng_krp, symbol_name, offset, addr, NULL, NULL);
	if (ret)
		return ret;
	ret = _lttng_kretprobes_register(&lttng_krp);
	if (ret) {
		ret = -ENOENT;
		goto end;
	}
	unregister_kretprobe(&lttng_krp.krp);
end:
	kfree(lttng_krp.symbol_name);
	return ret;
}

void lttng_kretprobes_destroy_event_private(struct lttng_kernel_event_common *event)
{
	kfree(event->priv->desc->event_name);
	kfree(event->priv->desc);
	lttng_kretprobes_put_krp(event->priv->u.kretprobe.lttng_krp);
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng kretprobes probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
