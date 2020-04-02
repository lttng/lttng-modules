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
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <wrapper/irqflags.h>
#include <lttng/tracer.h>
#include <blacklist/kprobes.h>

static
int lttng_kprobes_event_handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	struct lttng_event *event =
		container_of(p, struct lttng_event, u.kprobe.kp);
	struct lttng_probe_ctx lttng_probe_ctx = {
		.event = event,
		.interruptible = !lttng_regs_irqs_disabled(regs),
	};
	struct lttng_channel *chan = event->chan;
	struct lib_ring_buffer_ctx ctx;
	int ret;
	unsigned long data = (unsigned long) p->addr;

	if (unlikely(!LTTNG_READ_ONCE(chan->session->active)))
		return 0;
	if (unlikely(!LTTNG_READ_ONCE(chan->enabled)))
		return 0;
	if (unlikely(!LTTNG_READ_ONCE(event->enabled)))
		return 0;

	lib_ring_buffer_ctx_init(&ctx, chan->chan, &lttng_probe_ctx, sizeof(data),
				 lttng_alignof(data), -1);
	ret = chan->ops->event_reserve(&ctx, event->id);
	if (ret < 0)
		return 0;
	lib_ring_buffer_align_ctx(&ctx, lttng_alignof(data));
	chan->ops->event_write(&ctx, &data, sizeof(data));
	chan->ops->event_commit(&ctx);
	return 0;
}

static
int lttng_kprobes_event_notifier_handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	struct lttng_event_notifier *event_notifier =
		container_of(p, struct lttng_event_notifier, u.kprobe.kp);

	if (unlikely(!READ_ONCE(event_notifier->enabled)))
		return 0;

	event_notifier->send_notification(event_notifier, NULL, NULL);

	return 0;
}

/*
 * Create event description
 */
static
int lttng_create_kprobe_event(const char *name, struct lttng_event *event)
{
	struct lttng_event_field *field;
	struct lttng_event_desc *desc;
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
	desc->fields = field =
		kzalloc(1 * sizeof(struct lttng_event_field), GFP_KERNEL);
	if (!field) {
		ret = -ENOMEM;
		goto error_field;
	}
	field->name = "ip";
	field->type.atype = atype_integer;
	field->type.u.integer.size = sizeof(unsigned long) * CHAR_BIT;
	field->type.u.integer.alignment = lttng_alignof(unsigned long) * CHAR_BIT;
	field->type.u.integer.signedness = lttng_is_signed_type(unsigned long);
	field->type.u.integer.reverse_byte_order = 0;
	field->type.u.integer.base = 16;
	field->type.u.integer.encoding = lttng_encode_none;
	desc->owner = THIS_MODULE;
	event->desc = desc;

	return 0;

error_field:
	kfree(desc->name);
error_str:
	kfree(desc);
	return ret;
}

/*
 * Create event_notifier description
 */
static
int lttng_create_kprobe_event_notifier(const char *name, struct lttng_event_notifier *event_notifier)
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
			kzalloc(LTTNG_KERNEL_SYM_NAME_LEN * sizeof(char),
				GFP_KERNEL);
		if (!lttng_kp->symbol_name) {
			ret = -ENOMEM;
			goto name_error;
		}
		memcpy(lttng_kp->symbol_name, symbol_name,
		       LTTNG_KERNEL_SYM_NAME_LEN * sizeof(char));
		lttng_kp->kp.symbol_name = lttng_kp->symbol_name;
	}

	lttng_kp->kp.offset = offset;
	lttng_kp->kp.addr = (void *) (unsigned long) addr;

	/*
	 * Ensure the memory we just allocated don't event_notifier page faults.
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
			   struct lttng_event *event)
{
	int ret;

	ret = lttng_create_kprobe_event(name, event);
	if (ret)
		goto error;

	ret = _lttng_kprobes_register(symbol_name, offset, addr,
		&event->u.kprobe, lttng_kprobes_event_handler_pre);
	if (ret)
		goto register_error;

	return 0;

register_error:
	kfree(event->desc->fields);
	kfree(event->desc->name);
	kfree(event->desc);
error:
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_kprobes_register_event);

int lttng_kprobes_register_event_notifier(const char *symbol_name,
			   uint64_t offset,
			   uint64_t addr,
			   struct lttng_event_notifier *event_notifier)
{
	int ret;
	ret = lttng_create_kprobe_event_notifier(symbol_name, event_notifier);
	if (ret)
		goto error;

	ret = _lttng_kprobes_register(symbol_name, offset, addr,
		&event_notifier->u.kprobe, lttng_kprobes_event_notifier_handler_pre);
	if (ret)
		goto register_error;

	return 0;

register_error:
	kfree(event_notifier->desc->name);
	kfree(event_notifier->desc);
error:
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_kprobes_register_event_notifier);

void lttng_kprobes_unregister_event(struct lttng_event *event)
{
	unregister_kprobe(&event->u.kprobe.kp);
}
EXPORT_SYMBOL_GPL(lttng_kprobes_unregister_event);

void lttng_kprobes_unregister_event_notifier(struct lttng_event_notifier *event_notifier)
{
	unregister_kprobe(&event_notifier->u.kprobe.kp);
}
EXPORT_SYMBOL_GPL(lttng_kprobes_unregister_event_notifier);

void lttng_kprobes_destroy_event_private(struct lttng_event *event)
{
	kfree(event->u.kprobe.symbol_name);
	kfree(event->desc->fields);
	kfree(event->desc->name);
	kfree(event->desc);
}
EXPORT_SYMBOL_GPL(lttng_kprobes_destroy_event_private);

void lttng_kprobes_destroy_event_notifier_private(struct lttng_event_notifier *event_notifier)
{
	kfree(event_notifier->u.kprobe.symbol_name);
	kfree(event_notifier->desc->name);
	kfree(event_notifier->desc);
}
EXPORT_SYMBOL_GPL(lttng_kprobes_destroy_event_notifier_private);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng kprobes probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
