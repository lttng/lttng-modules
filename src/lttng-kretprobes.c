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
	struct lttng_kernel_event_common *event[2];	/* ENTRY and EXIT */
	struct kref kref_register;
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

/*
 * Create event description
 */
struct lttng_kernel_event_desc *lttng_create_kretprobes_event_desc(const char *name)
{
	struct lttng_kernel_event_desc *desc;
	char *alloc_name;
	size_t name_len;

	desc = kzalloc(sizeof(*desc), GFP_KERNEL);
	if (!desc)
		return NULL;
	name_len = strlen(name);
	alloc_name = kmalloc(name_len + 1, GFP_KERNEL);
	if (!alloc_name)
		goto error_str;
	strcpy(alloc_name, name);
	desc->event_name = alloc_name;
	desc->tp_class = &tp_class;
	desc->owner = THIS_MODULE;
	return desc;

error_str:
	kfree(desc);
	return NULL;
}

static
int _lttng_kretprobes_register(const char *symbol_name,
			   uint64_t offset,
			   uint64_t addr,
			   struct lttng_krp *lttng_krp,
			   kretprobe_handler_t entry_handler,
			   kretprobe_handler_t exit_handler,
			   struct lttng_kernel_event_common *event_entry,
			   struct lttng_kernel_event_common *event_exit)
{
	int ret;

	/* Kprobes expects a NULL symbol name if unused */
	if (symbol_name[0] == '\0')
		symbol_name = NULL;
	lttng_krp->krp.entry_handler = entry_handler;
	lttng_krp->krp.handler = exit_handler;
	if (symbol_name) {
		char *alloc_symbol;

		alloc_symbol = kstrdup(symbol_name, GFP_KERNEL);
		if (!alloc_symbol) {
			ret = -ENOMEM;
			goto name_error;
		}
		lttng_krp->krp.kp.symbol_name = alloc_symbol;
		if (event_entry)
			event_entry->priv->u.kretprobe.symbol_name = alloc_symbol;
		if (event_exit)
			event_exit->priv->u.kretprobe.symbol_name = alloc_symbol;
	}
	lttng_krp->krp.kp.offset = offset;
	lttng_krp->krp.kp.addr = (void *) (unsigned long) addr;

	/* Allow probe handler to find event structures */
	lttng_krp->event[EVENT_ENTRY] = event_entry;
	lttng_krp->event[EVENT_EXIT] = event_exit;
	if (event_entry)
		event_entry->priv->u.kretprobe.lttng_krp = lttng_krp;
	if (event_exit)
		event_exit->priv->u.kretprobe.lttng_krp = lttng_krp;

	/*
	 * Both events must be unregistered before the kretprobe is
	 * unregistered. Same for memory allocation.
	 */
	kref_init(&lttng_krp->kref_alloc);
	kref_get(&lttng_krp->kref_alloc);	/* inc refcount to 2, no overflow. */
	kref_init(&lttng_krp->kref_register);
	kref_get(&lttng_krp->kref_register);	/* inc refcount to 2, no overflow. */

	/*
	 * Ensure the memory we just allocated don't trigger page faults.
	 * Well.. kprobes itself puts the page fault handler on the blacklist,
	 * but we can never be too careful.
	 */
	wrapper_vmalloc_sync_mappings();

	ret = register_kretprobe(&lttng_krp->krp);
	if (ret)
		goto register_error;
	return 0;

register_error:
	kfree(lttng_krp->krp.kp.symbol_name);
name_error:
	return ret;
}

int lttng_kretprobes_register(const char *symbol_name,
			   uint64_t offset,
			   uint64_t addr,
			   struct lttng_kernel_event_common *event_entry,
			   struct lttng_kernel_event_common *event_exit)
{
	int ret = -ENOMEM;
	struct lttng_krp *lttng_krp;

	lttng_krp = kzalloc(sizeof(*lttng_krp), GFP_KERNEL);
	if (!lttng_krp)
		goto krp_error;
	ret = _lttng_kretprobes_register(symbol_name, offset, addr, lttng_krp,
			lttng_kretprobes_handler_entry, lttng_kretprobes_handler_exit,
			event_entry, event_exit);
	if (ret)
		goto register_error;
	return 0;

register_error:
	kfree(lttng_krp);
krp_error:
	return ret;
}

static
void _lttng_kretprobes_unregister_release(struct kref *kref)
{
	struct lttng_krp *lttng_krp =
		container_of(kref, struct lttng_krp, kref_register);
	unregister_kretprobe(&lttng_krp->krp);
}

void lttng_kretprobes_unregister(struct lttng_kernel_event_common *event)
{
	kref_put(&event->priv->u.kretprobe.lttng_krp->kref_register,
		_lttng_kretprobes_unregister_release);
}

static
void _lttng_kretprobes_release(struct kref *kref)
{
	struct lttng_krp *lttng_krp =
		container_of(kref, struct lttng_krp, kref_alloc);
	kfree(lttng_krp->krp.kp.symbol_name);
}

int lttng_kretprobes_match_check(const char *symbol_name, uint64_t offset, uint64_t addr)
{
	struct lttng_krp lttng_krp;
	int ret;

	memset(&lttng_krp, 0, sizeof(lttng_krp));
	ret = _lttng_kretprobes_register(symbol_name, offset, addr, &lttng_krp, NULL, NULL,
			NULL, NULL);
	if (ret)
		return -ENOENT;
	unregister_kretprobe(&lttng_krp.krp);
	kfree(lttng_krp.krp.kp.symbol_name);
	return 0;
}

void lttng_kretprobes_destroy_private(struct lttng_kernel_event_common *event)
{
	kfree(event->priv->desc->event_name);
	kfree(event->priv->desc);
	kref_put(&event->priv->u.kretprobe.lttng_krp->kref_alloc,
		_lttng_kretprobes_release);
}

int lttng_kretprobes_event_enable_state(struct lttng_kernel_event_common *event,
		int enable)
{
	struct lttng_kernel_event_common *event_exit;
	struct lttng_krp *lttng_krp;

	if (event->priv->instrumentation != LTTNG_KERNEL_ABI_KRETPROBE) {
		return -EINVAL;
	}
	if (event->enabled == enable) {
		return -EBUSY;
	}
	lttng_krp = event->priv->u.kretprobe.lttng_krp;
	event_exit = lttng_krp->event[EVENT_EXIT];
	WRITE_ONCE(event->enabled, enable);
	WRITE_ONCE(event_exit->enabled, enable);
	return 0;
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng kretprobes probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
