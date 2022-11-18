/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-user-ns.c
 *
 * LTTng user namespace context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *               2019 Michael Jeanson <mjeanson@efficios.com>
 *
 */

#ifdef CONFIG_USER_NS

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/user_namespace.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <lttng/tracer.h>

static
size_t user_ns_get_size(void *priv, struct lttng_kernel_probe_ctx *probe_ctx, size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(unsigned int));
	size += sizeof(unsigned int);
	return size;
}

static
void user_ns_record(void *priv, struct lttng_kernel_probe_ctx *probe_ctx,
		 struct lttng_kernel_ring_buffer_ctx *ctx,
		 struct lttng_kernel_channel_buffer *chan)
{
	unsigned int user_ns_inum = 0;

	if (current_user_ns())
		user_ns_inum = current_user_ns()->ns.inum;

	chan->ops->event_write(ctx, &user_ns_inum, sizeof(user_ns_inum), lttng_alignof(user_ns_inum));
}

static
void user_ns_get_value(void *priv,
		struct lttng_kernel_probe_ctx *lttng_probe_ctx,
		struct lttng_ctx_value *value)
{
	unsigned int user_ns_inum = 0;

	if (current_user_ns())
		user_ns_inum = current_user_ns()->ns.inum;

	value->u.s64 = user_ns_inum;
}

static const struct lttng_kernel_ctx_field *ctx_field = lttng_kernel_static_ctx_field(
	lttng_kernel_static_event_field("user_ns",
		lttng_kernel_static_type_integer_from_type(unsigned int, __BYTE_ORDER, 10),
		false, false),
	user_ns_get_size,
	user_ns_record,
	user_ns_get_value,
	NULL, NULL);

int lttng_add_user_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	int ret;

	if (lttng_kernel_find_context(*ctx, ctx_field->event_field->name))
		return -EEXIST;
	ret = lttng_kernel_context_append(ctx, ctx_field);
	wrapper_vmalloc_sync_mappings();
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_add_user_ns_to_ctx);

#endif
