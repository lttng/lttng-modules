/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-context-vuid.c
 *
 * LTTng namespaced real user ID context.
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
size_t vuid_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(uid_t));
	size += sizeof(uid_t);
	return size;
}

static
void vuid_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	uid_t vuid;

	vuid = lttng_current_vuid();
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(vuid));
	chan->ops->event_write(ctx, &vuid, sizeof(vuid));
}

static
void vuid_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	value->s64 = lttng_current_vuid();
}

int lttng_add_vuid_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "vuid")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "vuid";
	field->event_field.type.atype = atype_integer;
	field->event_field.type.u.basic.integer.size = sizeof(uid_t) * CHAR_BIT;
	field->event_field.type.u.basic.integer.alignment = lttng_alignof(uid_t) * CHAR_BIT;
	field->event_field.type.u.basic.integer.signedness = lttng_is_signed_type(uid_t);
	field->event_field.type.u.basic.integer.reverse_byte_order = 0;
	field->event_field.type.u.basic.integer.base = 10;
	field->event_field.type.u.basic.integer.encoding = lttng_encode_none;
	field->get_size = vuid_get_size;
	field->record = vuid_record;
	field->get_value = vuid_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_all();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_vuid_to_ctx);
