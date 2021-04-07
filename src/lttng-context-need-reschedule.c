/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-need-reschedule.c
 *
 * LTTng need_reschedule context.
 *
 * Copyright (C) 2009-2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/irqflags.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <lttng/tracer.h>

static
size_t need_reschedule_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(uint8_t));
	size += sizeof(uint8_t);
	return size;
}

static
void need_reschedule_record(struct lttng_kernel_ctx_field *field,
		struct lib_ring_buffer_ctx *ctx,
		struct lttng_channel *chan)
{
	uint8_t need_reschedule = test_tsk_need_resched(current);

	lib_ring_buffer_align_ctx(ctx, lttng_alignof(need_reschedule));
	chan->ops->event_write(ctx, &need_reschedule, sizeof(need_reschedule));
}

static
void need_reschedule_get_value(struct lttng_kernel_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	value->s64 = test_tsk_need_resched(current);;
}

static const struct lttng_kernel_ctx_field *ctx_field = lttng_kernel_static_ctx_field(
	lttng_kernel_static_event_field("need_reschedule",
		lttng_kernel_static_type_integer_from_type(uint8_t, __BYTE_ORDER, 10),
		false, false, false),
	need_reschedule_get_size,
	NULL,
	need_reschedule_record,
	need_reschedule_get_value,
	NULL, NULL);

int lttng_add_need_reschedule_to_ctx(struct lttng_kernel_ctx **ctx)
{
	int ret;

	if (lttng_kernel_find_context(*ctx, ctx_field->event_field->name))
		return -EEXIST;
	ret = lttng_kernel_context_append(ctx, ctx_field);
	wrapper_vmalloc_sync_mappings();
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_add_need_reschedule_to_ctx);
