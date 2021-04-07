/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-procname.c
 *
 * LTTng procname context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <lttng/events.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <lttng/tracer.h>
#include <lttng/endian.h>

static
size_t procname_get_size(size_t offset)
{
	size_t size = 0;

	size += sizeof(current->comm);
	return size;
}

/*
 * Racy read of procname. We simply copy its whole array size.
 * Races with /proc/<task>/procname write only.
 * Otherwise having to take a mutex for each event is cumbersome and
 * could lead to crash in IRQ context and deadlock of the lockdep tracer.
 */
static
void procname_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	chan->ops->event_write(ctx, current->comm, sizeof(current->comm));
}

static
void procname_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	value->str = current->comm;
}

static const struct lttng_type procname_array_elem_type =
	__type_integer(char, 0, 0, -1, __BYTE_ORDER, 10, UTF8);

int lttng_add_procname_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "procname")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "procname";
	field->event_field.type.type = lttng_kernel_type_array_nestable;
	field->event_field.type.u.array_nestable.elem_type = &procname_array_elem_type;
	field->event_field.type.u.array_nestable.length = sizeof(current->comm);
	field->event_field.type.u.array_nestable.alignment = 0;

	field->get_size = procname_get_size;
	field->record = procname_record;
	field->get_value = procname_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_mappings();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_procname_to_ctx);
