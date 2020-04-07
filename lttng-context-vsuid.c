/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-vsuid.c
 *
 * LTTng namespaced saved set-user-ID context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *               2019 Michael Jeanson <mjeanson@efficios.com>
 *
 */

#include <linux/module.h>
#include <linux/sched.h>
#include <lttng-events.h>
#include <lttng-tracer.h>
#include <wrapper/ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <wrapper/user_namespace.h>

static
size_t vsuid_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(uid_t));
	size += sizeof(uid_t);
	return size;
}

static
void vsuid_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	uid_t vsuid;

	vsuid = lttng_current_vsuid();
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(vsuid));
	chan->ops->event_write(ctx, &vsuid, sizeof(vsuid));
}

static
void vsuid_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	value->s64 = lttng_current_vsuid();
}

int lttng_add_vsuid_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "vsuid")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "vsuid";
	field->event_field.type.atype = atype_integer;
	field->event_field.type.u.integer.size = sizeof(uid_t) * CHAR_BIT;
	field->event_field.type.u.integer.alignment = lttng_alignof(uid_t) * CHAR_BIT;
	field->event_field.type.u.integer.signedness = lttng_is_signed_type(uid_t);
	field->event_field.type.u.integer.reverse_byte_order = 0;
	field->event_field.type.u.integer.base = 10;
	field->event_field.type.u.integer.encoding = lttng_encode_none;
	field->get_size = vsuid_get_size;
	field->record = vsuid_record;
	field->get_value = vsuid_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_all();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_vsuid_to_ctx);
