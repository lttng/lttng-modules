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
	struct lttng_kernel_probe_ctx lttng_probe_ctx = {
		.event = event,
		.interruptible = !lttng_regs_irqs_disabled(regs),
	};
	struct {
		unsigned long ip;
		unsigned long parent_ip;
	} payload;

	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
	{
		struct lttng_kernel_event_recorder *event_recorder =
			container_of(event, struct lttng_kernel_event_recorder, parent);
		struct lttng_kernel_channel_buffer *chan = event_recorder->chan;

		if (unlikely(!LTTNG_READ_ONCE(chan->parent.session->active)))
			return 0;
		if (unlikely(!LTTNG_READ_ONCE(chan->parent.enabled)))
			return 0;
		break;
	}
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
		lttng_fallthrough;
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
		lttng_fallthrough;
	default:
		WARN_ON_ONCE(1);
	}
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
static
int lttng_create_kprobe_event(const char *name, struct lttng_kernel_event_recorder *event_recorder,
			      enum lttng_kretprobe_type type)
{
	struct lttng_kernel_event_desc *desc;
	char *alloc_name;
	size_t name_len;
	const char *suffix = NULL;
	int ret;

	desc = kzalloc(sizeof(*desc), GFP_KERNEL);
	if (!desc)
		return -ENOMEM;
	name_len = strlen(name);
	switch (type) {
	case EVENT_ENTRY:
		suffix = "_entry";
		break;
	case EVENT_EXIT:
		suffix = "_exit";
		break;
	}
	name_len += strlen(suffix);
	alloc_name = kmalloc(name_len + 1, GFP_KERNEL);
	if (!alloc_name) {
		ret = -ENOMEM;
		goto error_str;
	}
	strcpy(alloc_name, name);
	strcat(alloc_name, suffix);
	desc->event_name = alloc_name;
	desc->tp_class = &tp_class;
	desc->owner = THIS_MODULE;
	event_recorder->priv->parent.desc = desc;

	return 0;

error_str:
	kfree(desc);
	return ret;
}

int lttng_kretprobes_register(const char *name,
			   const char *symbol_name,
			   uint64_t offset,
			   uint64_t addr,
			   struct lttng_kernel_event_recorder *event_recorder_entry,
			   struct lttng_kernel_event_recorder *event_recorder_exit)
{
	int ret;
	struct lttng_krp *lttng_krp;

	/* Kprobes expects a NULL symbol name if unused */
	if (symbol_name[0] == '\0')
		symbol_name = NULL;

	ret = lttng_create_kprobe_event(name, event_recorder_entry, EVENT_ENTRY);
	if (ret)
		goto error;
	ret = lttng_create_kprobe_event(name, event_recorder_exit, EVENT_EXIT);
	if (ret)
		goto event_exit_error;
	lttng_krp = kzalloc(sizeof(*lttng_krp), GFP_KERNEL);
	if (!lttng_krp)
		goto krp_error;
	lttng_krp->krp.entry_handler = lttng_kretprobes_handler_entry;
	lttng_krp->krp.handler = lttng_kretprobes_handler_exit;
	if (symbol_name) {
		char *alloc_symbol;

		alloc_symbol = kstrdup(symbol_name, GFP_KERNEL);
		if (!alloc_symbol) {
			ret = -ENOMEM;
			goto name_error;
		}
		lttng_krp->krp.kp.symbol_name =
			alloc_symbol;
		event_recorder_entry->priv->parent.u.kretprobe.symbol_name =
			alloc_symbol;
		event_recorder_exit->priv->parent.u.kretprobe.symbol_name =
			alloc_symbol;
	}
	lttng_krp->krp.kp.offset = offset;
	lttng_krp->krp.kp.addr = (void *) (unsigned long) addr;

	/* Allow probe handler to find event structures */
	lttng_krp->event[EVENT_ENTRY] = &event_recorder_entry->parent;
	lttng_krp->event[EVENT_EXIT] = &event_recorder_exit->parent;
	event_recorder_entry->priv->parent.u.kretprobe.lttng_krp = lttng_krp;
	event_recorder_exit->priv->parent.u.kretprobe.lttng_krp = lttng_krp;

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
	kfree(lttng_krp);
krp_error:
	kfree(event_recorder_exit->priv->parent.desc->event_name);
	kfree(event_recorder_exit->priv->parent.desc);
event_exit_error:
	kfree(event_recorder_entry->priv->parent.desc->event_name);
	kfree(event_recorder_entry->priv->parent.desc);
error:
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_kretprobes_register);

static
void _lttng_kretprobes_unregister_release(struct kref *kref)
{
	struct lttng_krp *lttng_krp =
		container_of(kref, struct lttng_krp, kref_register);
	unregister_kretprobe(&lttng_krp->krp);
}

void lttng_kretprobes_unregister(struct lttng_kernel_event_recorder *event_recorder)
{
	kref_put(&event_recorder->priv->parent.u.kretprobe.lttng_krp->kref_register,
		_lttng_kretprobes_unregister_release);
}
EXPORT_SYMBOL_GPL(lttng_kretprobes_unregister);

static
void _lttng_kretprobes_release(struct kref *kref)
{
	struct lttng_krp *lttng_krp =
		container_of(kref, struct lttng_krp, kref_alloc);
	kfree(lttng_krp->krp.kp.symbol_name);
}

void lttng_kretprobes_destroy_private(struct lttng_kernel_event_recorder *event_recorder)
{
	kfree(event_recorder->priv->parent.desc->event_name);
	kfree(event_recorder->priv->parent.desc);
	kref_put(&event_recorder->priv->parent.u.kretprobe.lttng_krp->kref_alloc,
		_lttng_kretprobes_release);
}
EXPORT_SYMBOL_GPL(lttng_kretprobes_destroy_private);

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
EXPORT_SYMBOL_GPL(lttng_kretprobes_event_enable_state);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng kretprobes probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
