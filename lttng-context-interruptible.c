/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-context-interruptible.c
 *
 * LTTng interruptible context.
 *
 * Copyright (C) 2009-2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/irqflags.h>
#include <lttng-events.h>
#include <wrapper/ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <lttng-tracer.h>

/*
 * Interruptible at value -1 means "unknown".
 */

static
size_t interruptible_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(int8_t));
	size += sizeof(int8_t);
	return size;
}

static
void interruptible_record(struct lttng_ctx_field *field,
		struct lib_ring_buffer_ctx *ctx,
		struct lttng_channel *chan)
{
	struct lttng_probe_ctx *lttng_probe_ctx = ctx->priv;
	int8_t interruptible = lttng_probe_ctx->interruptible;

	lib_ring_buffer_align_ctx(ctx, lttng_alignof(interruptible));
	chan->ops->event_write(ctx, &interruptible, sizeof(interruptible));
}

static
void interruptible_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	int8_t interruptible = lttng_probe_ctx->interruptible;

	value->s64 = interruptible;
}

int lttng_add_interruptible_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "interruptible")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "interruptible";
	field->event_field.type.atype = atype_integer;
	field->event_field.type.u.basic.integer.size = sizeof(int8_t) * CHAR_BIT;
	field->event_field.type.u.basic.integer.alignment = lttng_alignof(int8_t) * CHAR_BIT;
	field->event_field.type.u.basic.integer.signedness = lttng_is_signed_type(int8_t);
	field->event_field.type.u.basic.integer.reverse_byte_order = 0;
	field->event_field.type.u.basic.integer.base = 10;
	field->event_field.type.u.basic.integer.encoding = lttng_encode_none;
	field->get_size = interruptible_get_size;
	field->record = interruptible_record;
	field->get_value = interruptible_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_mappings();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_interruptible_to_ctx);
