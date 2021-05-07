/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-preemptible.c
 *
 * LTTng preemptible context.
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

/*
 * We nest twice in preempt disabling within LTTng: one nesting is done
 * by the instrumentation (tracepoint, kprobes, kretprobes, syscall
 * tracepoint), and the second is within the lib ring buffer
 * lib_ring_buffer_get_cpu().
 */
#define LTTNG_PREEMPT_DISABLE_NESTING	2

static
size_t preemptible_get_size(void *priv, struct lttng_kernel_probe_ctx *probe_ctx, size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(uint8_t));
	size += sizeof(uint8_t);
	return size;
}

static
void preemptible_record(void *priv, struct lttng_kernel_probe_ctx *probe_ctx,
		struct lttng_kernel_ring_buffer_ctx *ctx,
		struct lttng_kernel_channel_buffer *chan)
{
	int pc = preempt_count();
	uint8_t preemptible = 0;

	WARN_ON_ONCE(pc < LTTNG_PREEMPT_DISABLE_NESTING);
	if (pc == LTTNG_PREEMPT_DISABLE_NESTING)
		preemptible = 1;
	chan->ops->event_write(ctx, &preemptible, sizeof(preemptible), lttng_alignof(preemptible));
}

static
void preemptible_get_value(void *priv,
		struct lttng_kernel_probe_ctx *lttng_probe_ctx,
		struct lttng_ctx_value *value)
{
	int pc = preempt_count();

	WARN_ON_ONCE(pc < LTTNG_PREEMPT_DISABLE_NESTING);
	if (pc == LTTNG_PREEMPT_DISABLE_NESTING)
		value->u.s64 = 1;
	else
		value->u.s64 = 0;
}

static const struct lttng_kernel_ctx_field *ctx_field = lttng_kernel_static_ctx_field(
	lttng_kernel_static_event_field("preemptible",
		lttng_kernel_static_type_integer_from_type(uint8_t, __BYTE_ORDER, 10),
		false, false, false),
	preemptible_get_size,
	preemptible_record,
	preemptible_get_value,
	NULL, NULL);

int lttng_add_preemptible_to_ctx(struct lttng_kernel_ctx **ctx)
{
	int ret;

	if (lttng_kernel_find_context(*ctx, ctx_field->event_field->name))
		return -EEXIST;
	ret = lttng_kernel_context_append(ctx, ctx_field);
	wrapper_vmalloc_sync_mappings();
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_add_preemptible_to_ctx);
