/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-context-user-ns.c
 *
 * LTTng user namespace context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *               2019 Michael Jeanson <mjeanson@efficios.com>
 *
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/user_namespace.h>
#include <lttng-events.h>
#include <wrapper/ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <wrapper/namespace.h>
#include <lttng-tracer.h>

#if defined(CONFIG_USER_NS) && \
	(LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))

static
size_t user_ns_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(unsigned int));
	size += sizeof(unsigned int);
	return size;
}

static
void user_ns_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	unsigned int user_ns_inum = 0;

	if (current_user_ns())
		user_ns_inum = current_user_ns()->lttng_ns_inum;

	lib_ring_buffer_align_ctx(ctx, lttng_alignof(user_ns_inum));
	chan->ops->event_write(ctx, &user_ns_inum, sizeof(user_ns_inum));
}

static
void user_ns_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	unsigned int user_ns_inum = 0;

	if (current_user_ns())
		user_ns_inum = current_user_ns()->lttng_ns_inum;

	value->s64 = user_ns_inum;
}

int lttng_add_user_ns_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "user_ns")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "user_ns";
	field->event_field.type.atype = atype_integer;
	field->event_field.type.u.basic.integer.size = sizeof(unsigned int) * CHAR_BIT;
	field->event_field.type.u.basic.integer.alignment = lttng_alignof(unsigned int) * CHAR_BIT;
	field->event_field.type.u.basic.integer.signedness = lttng_is_signed_type(unsigned int);
	field->event_field.type.u.basic.integer.reverse_byte_order = 0;
	field->event_field.type.u.basic.integer.base = 10;
	field->event_field.type.u.basic.integer.encoding = lttng_encode_none;
	field->get_size = user_ns_get_size;
	field->record = user_ns_record;
	field->get_value = user_ns_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_all();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_user_ns_to_ctx);

#endif
