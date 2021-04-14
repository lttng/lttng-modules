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
#include <ringbuffer/frontend_types.h>
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
	struct lttng_event *event[2];	/* ENTRY and EXIT */
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
	struct lttng_event *event =
		lttng_krp->event[type];
	struct lttng_probe_ctx lttng_probe_ctx = {
		.event = event,
		.interruptible = !lttng_regs_irqs_disabled(regs),
	};
	struct lttng_channel *chan = event->chan;
	struct lib_ring_buffer_ctx ctx;
	int ret;
	struct {
		unsigned long ip;
		unsigned long parent_ip;
	} payload;

	if (unlikely(!LTTNG_READ_ONCE(chan->session->active)))
		return 0;
	if (unlikely(!LTTNG_READ_ONCE(chan->enabled)))
		return 0;
	if (unlikely(!LTTNG_READ_ONCE(event->enabled)))
		return 0;

	payload.ip = (unsigned long) lttng_get_kretprobe(krpi)->kp.addr;
	payload.parent_ip = (unsigned long) krpi->ret_addr;

	lib_ring_buffer_ctx_init(&ctx, chan->chan, &lttng_probe_ctx, sizeof(payload),
				 lttng_alignof(payload), -1);
	ret = chan->ops->event_reserve(&ctx, event->id);
	if (ret < 0)
		return 0;
	lib_ring_buffer_align_ctx(&ctx, lttng_alignof(payload));
	chan->ops->event_write(&ctx, &payload, sizeof(payload));
	chan->ops->event_commit(&ctx);
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

static const struct lttng_kernel_type_common *event_type =
	lttng_kernel_static_type_integer_from_type(unsigned long, __BYTE_ORDER, 16);

/*
 * Create event description
 */
static
int lttng_create_kprobe_event(const char *name, struct lttng_event *event,
			      enum lttng_kretprobe_type type)
{
	const struct lttng_kernel_event_field **fieldp_array;
	struct lttng_kernel_event_field *field;
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
	desc->nr_fields = 2;
	fieldp_array = kzalloc(desc->nr_fields * sizeof(struct lttng_kernel_event_field *), GFP_KERNEL);
	if (!fieldp_array) {
		ret = -ENOMEM;
		goto error_fieldp_array;
	}
	desc->fields = fieldp_array;

	field = kzalloc(sizeof(struct lttng_kernel_event_field), GFP_KERNEL);
	if (!field) {
		ret = -ENOMEM;
		goto error_field0;
	}
	field->name = "ip";
	field->type = event_type;
	desc->fields[0] = field;

	field = kzalloc(sizeof(struct lttng_kernel_event_field), GFP_KERNEL);
	if (!field) {
		ret = -ENOMEM;
		goto error_field1;
	}
	field->name = "parent_ip";
	field->type = event_type;
	desc->fields[1] = field;

	desc->owner = THIS_MODULE;
	event->desc = desc;

	return 0;

error_field1:
	kfree(desc->fields[0]);
error_field0:
	kfree(fieldp_array);
error_fieldp_array:
	kfree(desc->event_name);
error_str:
	kfree(desc);
	return ret;
}

int lttng_kretprobes_register(const char *name,
			   const char *symbol_name,
			   uint64_t offset,
			   uint64_t addr,
			   struct lttng_event *event_entry,
			   struct lttng_event *event_exit)
{
	int ret;
	struct lttng_krp *lttng_krp;

	/* Kprobes expects a NULL symbol name if unused */
	if (symbol_name[0] == '\0')
		symbol_name = NULL;

	ret = lttng_create_kprobe_event(name, event_entry, EVENT_ENTRY);
	if (ret)
		goto error;
	ret = lttng_create_kprobe_event(name, event_exit, EVENT_EXIT);
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
		event_entry->u.kretprobe.symbol_name =
			alloc_symbol;
		event_exit->u.kretprobe.symbol_name =
			alloc_symbol;
	}
	lttng_krp->krp.kp.offset = offset;
	lttng_krp->krp.kp.addr = (void *) (unsigned long) addr;

	/* Allow probe handler to find event structures */
	lttng_krp->event[EVENT_ENTRY] = event_entry;
	lttng_krp->event[EVENT_EXIT] = event_exit;
	event_entry->u.kretprobe.lttng_krp = lttng_krp;
	event_exit->u.kretprobe.lttng_krp = lttng_krp;

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
	kfree(event_exit->desc->fields[0]);
	kfree(event_exit->desc->fields[1]);
	kfree(event_exit->desc->fields);
	kfree(event_exit->desc->event_name);
	kfree(event_exit->desc);
event_exit_error:
	kfree(event_entry->desc->fields[0]);
	kfree(event_entry->desc->fields[1]);
	kfree(event_entry->desc->fields);
	kfree(event_entry->desc->event_name);
	kfree(event_entry->desc);
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

void lttng_kretprobes_unregister(struct lttng_event *event)
{
	kref_put(&event->u.kretprobe.lttng_krp->kref_register,
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

void lttng_kretprobes_destroy_private(struct lttng_event *event)
{
	kfree(event->desc->fields[0]);
	kfree(event->desc->fields[1]);
	kfree(event->desc->fields);
	kfree(event->desc->event_name);
	kfree(event->desc);
	kref_put(&event->u.kretprobe.lttng_krp->kref_alloc,
		_lttng_kretprobes_release);
}
EXPORT_SYMBOL_GPL(lttng_kretprobes_destroy_private);

int lttng_kretprobes_event_enable_state(struct lttng_event *event,
		int enable)
{
	struct lttng_event *event_exit;
	struct lttng_krp *lttng_krp;

	if (event->instrumentation != LTTNG_KERNEL_ABI_KRETPROBE) {
		return -EINVAL;
	}
	if (event->enabled == enable) {
		return -EBUSY;
	}
	lttng_krp = event->u.kretprobe.lttng_krp;
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
