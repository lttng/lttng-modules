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
#include <lttng/tracer.h>
#include <blacklist/kprobes.h>

static
int lttng_kprobes_event_handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	struct lttng_kernel_event_common_private *event_priv =
		container_of(p, struct lttng_kernel_event_common_private, u.kprobe.kp);
	struct lttng_kernel_event_common *event = event_priv->pub;
	struct lttng_probe_ctx lttng_probe_ctx = {
		.event = event,
		.interruptible = !lttng_regs_irqs_disabled(regs),
	};
	unsigned long data = (unsigned long) p->addr;

	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
	{
		struct lttng_kernel_event_recorder_private *event_recorder_priv =
			container_of(event_priv, struct lttng_kernel_event_recorder_private, parent);
		struct lttng_kernel_event_recorder *event_recorder =
			event_recorder_priv->pub;
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

		lib_ring_buffer_ctx_init(&ctx, chan->chan, &lttng_probe_ctx, sizeof(data),
					 lttng_alignof(data), -1);
		ret = chan->ops->event_reserve(&ctx, event_recorder->priv->id);
		if (ret < 0)
			return 0;
		lib_ring_buffer_align_ctx(&ctx, lttng_alignof(data));
		chan->ops->event_write(&ctx, &data, sizeof(data));
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
 * Create event description
 */
static
int lttng_create_kprobe_event(const char *name, struct lttng_kernel_event_recorder *event_recorder)
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
 * Create event_notifier description
 */
static
int lttng_create_kprobe_event_notifier(const char *name, struct lttng_kernel_event_notifier *event_notifier)
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

static
int _lttng_kprobes_register(const char *symbol_name,
			   uint64_t offset,
			   uint64_t addr,
			   struct lttng_kprobe *lttng_kp,
			   kprobe_pre_handler_t pre_handler)
{
	int ret;

	/* Kprobes expects a NULL symbol name if unused */
	if (symbol_name[0] == '\0')
		symbol_name = NULL;

	memset(&lttng_kp->kp, 0, sizeof(lttng_kp->kp));
	lttng_kp->kp.pre_handler = pre_handler;

	if (symbol_name) {
		lttng_kp->symbol_name =
			kzalloc(LTTNG_KERNEL_ABI_SYM_NAME_LEN * sizeof(char),
				GFP_KERNEL);
		if (!lttng_kp->symbol_name) {
			ret = -ENOMEM;
			goto name_error;
		}
		memcpy(lttng_kp->symbol_name, symbol_name,
		       LTTNG_KERNEL_ABI_SYM_NAME_LEN * sizeof(char));
		lttng_kp->kp.symbol_name = lttng_kp->symbol_name;
	}

	lttng_kp->kp.offset = offset;
	lttng_kp->kp.addr = (void *) (unsigned long) addr;

	/*
	 * Ensure the memory we just allocated don't notify page faults.
	 * Well.. kprobes itself puts the page fault handler on the blacklist,
	 * but we can never be too careful.
	 */
	wrapper_vmalloc_sync_mappings();

	ret = register_kprobe(&lttng_kp->kp);
	if (ret)
		goto register_error;

	return 0;

register_error:
	kfree(lttng_kp->symbol_name);
name_error:
	return ret;
}

int lttng_kprobes_register_event(const char *name,
			   const char *symbol_name,
			   uint64_t offset,
			   uint64_t addr,
			   struct lttng_kernel_event_recorder *event_recorder)
{
	int ret;

	ret = lttng_create_kprobe_event(name, event_recorder);
	if (ret)
		goto error;

	ret = _lttng_kprobes_register(symbol_name, offset, addr,
		&event_recorder->priv->parent.u.kprobe, lttng_kprobes_event_handler_pre);
	if (ret)
		goto register_error;

	return 0;

register_error:
	kfree(event_recorder->priv->parent.desc->fields);
	kfree(event_recorder->priv->parent.desc->event_name);
	kfree(event_recorder->priv->parent.desc);
error:
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_kprobes_register_event);

int lttng_kprobes_register_event_notifier(const char *symbol_name,
			   uint64_t offset,
			   uint64_t addr,
			   struct lttng_kernel_event_notifier *event_notifier)
{
	int ret;
	ret = lttng_create_kprobe_event_notifier(symbol_name, event_notifier);
	if (ret)
		goto error;

	ret = _lttng_kprobes_register(symbol_name, offset, addr,
		&event_notifier->priv->parent.u.kprobe, lttng_kprobes_event_handler_pre);
	if (ret)
		goto register_error;

	return 0;

register_error:
	kfree(event_notifier->priv->parent.desc->event_name);
	kfree(event_notifier->priv->parent.desc);
error:
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_kprobes_register_event_notifier);

void lttng_kprobes_unregister_event(struct lttng_kernel_event_recorder *event_recorder)
{
	unregister_kprobe(&event_recorder->priv->parent.u.kprobe.kp);
}
EXPORT_SYMBOL_GPL(lttng_kprobes_unregister_event);

void lttng_kprobes_unregister_event_notifier(struct lttng_kernel_event_notifier *event_notifier)
{
	unregister_kprobe(&event_notifier->priv->parent.u.kprobe.kp);
}
EXPORT_SYMBOL_GPL(lttng_kprobes_unregister_event_notifier);

void lttng_kprobes_destroy_event_private(struct lttng_kernel_event_recorder *event_recorder)
{
	kfree(event_recorder->priv->parent.u.kprobe.symbol_name);
	kfree(event_recorder->priv->parent.desc->fields[0]);
	kfree(event_recorder->priv->parent.desc->fields);
	kfree(event_recorder->priv->parent.desc->event_name);
	kfree(event_recorder->priv->parent.desc);
}
EXPORT_SYMBOL_GPL(lttng_kprobes_destroy_event_private);

void lttng_kprobes_destroy_event_notifier_private(struct lttng_kernel_event_notifier *event_notifier)
{
	kfree(event_notifier->priv->parent.u.kprobe.symbol_name);
	kfree(event_notifier->priv->parent.desc->event_name);
	kfree(event_notifier->priv->parent.desc);
}
EXPORT_SYMBOL_GPL(lttng_kprobes_destroy_event_notifier_private);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng kprobes probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
