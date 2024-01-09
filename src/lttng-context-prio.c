/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-prio.c
 *
 * LTTng priority context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <lttng/tracer.h>

/*
 * task_prio() has been implemented as p->prio - MAX_RT_PRIO since at
 * least kernel v3.0.
 */
static
int wrapper_task_prio(struct task_struct *p)
{
	return p->prio - MAX_RT_PRIO;
}

static
size_t prio_get_size(void *priv, struct lttng_kernel_probe_ctx *probe_ctx, size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(int));
	size += sizeof(int);
	return size;
}

static
void prio_record(void *priv, struct lttng_kernel_probe_ctx *probe_ctx,
		struct lttng_kernel_ring_buffer_ctx *ctx,
		struct lttng_kernel_channel_buffer *chan)
{
	int prio;

	prio = wrapper_task_prio(current);
	chan->ops->event_write(ctx, &prio, sizeof(prio), lttng_alignof(prio));
}

static
void prio_get_value(void *priv,
		struct lttng_kernel_probe_ctx *lttng_probe_ctx,
		struct lttng_ctx_value *value)
{
	value->u.s64 = wrapper_task_prio(current);
}

static const struct lttng_kernel_ctx_field *ctx_field = lttng_kernel_static_ctx_field(
	lttng_kernel_static_event_field("prio",
		lttng_kernel_static_type_integer_from_type(int, __BYTE_ORDER, 10),
		false, false),
	prio_get_size,
	prio_record,
	prio_get_value,
	NULL, NULL);

int lttng_add_prio_to_ctx(struct lttng_kernel_ctx **ctx)
{
	int ret;

	if (lttng_kernel_find_context(*ctx, ctx_field->event_field->name))
		return -EEXIST;
	ret = lttng_kernel_context_append(ctx, ctx_field);
	wrapper_vmalloc_sync_mappings();
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_add_prio_to_ctx);
