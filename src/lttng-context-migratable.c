/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-migratable.c
 *
 * LTTng migratable context.
 *
 * Copyright (C) 2009-2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/irqflags.h>
#include <lttng/events.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <lttng/tracer.h>

static
size_t migratable_get_size(void *priv, struct lttng_probe_ctx *probe_ctx, size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(uint8_t));
	size += sizeof(uint8_t);
	return size;
}

static
void migratable_record(void *priv, struct lttng_probe_ctx *probe_ctx,
		struct lib_ring_buffer_ctx *ctx,
		struct lttng_channel *chan)
{
	uint8_t migratable = !current->migrate_disable;

	lib_ring_buffer_align_ctx(ctx, lttng_alignof(migratable));
	chan->ops->event_write(ctx, &migratable, sizeof(migratable));
}

static
void migratable_get_value(void *priv,
		struct lttng_probe_ctx *lttng_probe_ctx,
		struct lttng_ctx_value *value)
{
	value->u.s64 = !current->migrate_disable;
}

static const struct lttng_kernel_ctx_field *ctx_field = lttng_kernel_static_ctx_field(
	lttng_kernel_static_event_field("migratable",
		lttng_kernel_static_type_integer_from_type(uint8_t, __BYTE_ORDER, 10),
		false, false, false),
	migratable_get_size,
	migratable_record,
	migratable_get_value,
	NULL, NULL);

int lttng_add_migratable_to_ctx(struct lttng_kernel_ctx **ctx)
{
	int ret;

	if (lttng_kernel_find_context(*ctx, ctx_field->event_field->name))
		return -EEXIST;
	ret = lttng_kernel_context_append(ctx, ctx_field);
	wrapper_vmalloc_sync_mappings();
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_add_migratable_to_ctx);
