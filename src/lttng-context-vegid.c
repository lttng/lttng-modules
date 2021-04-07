/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-vegid.c
 *
 * LTTng namespaced effective group ID context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *               2019 Michael Jeanson <mjeanson@efficios.com>
 *
 */

#include <linux/module.h>
#include <linux/sched.h>
#include <lttng/events.h>
#include <lttng/tracer.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <wrapper/user_namespace.h>

static
size_t vegid_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(gid_t));
	size += sizeof(gid_t);
	return size;
}

static
void vegid_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	gid_t vegid;

	vegid = lttng_current_vegid();
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(vegid));
	chan->ops->event_write(ctx, &vegid, sizeof(vegid));
}

static
void vegid_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	value->s64 = lttng_current_vegid();
}

int lttng_add_vegid_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "vegid")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "vegid";
	field->event_field.type.type = lttng_kernel_type_integer;
	field->event_field.type.u.integer.size = sizeof(gid_t) * CHAR_BIT;
	field->event_field.type.u.integer.alignment = lttng_alignof(gid_t) * CHAR_BIT;
	field->event_field.type.u.integer.signedness = lttng_is_signed_type(gid_t);
	field->event_field.type.u.integer.reverse_byte_order = 0;
	field->event_field.type.u.integer.base = 10;
	field->event_field.type.u.integer.encoding = lttng_encode_none;
	field->get_size = vegid_get_size;
	field->record = vegid_record;
	field->get_value = vegid_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_mappings();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_vegid_to_ctx);
