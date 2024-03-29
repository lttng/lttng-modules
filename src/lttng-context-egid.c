/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-egid.c
 *
 * LTTng effective group ID context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *               2019 Michael Jeanson <mjeanson@efficios.com>
 *
 */

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/user_namespace.h>

#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <lttng/tracer.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>

static
size_t egid_get_size(void *priv, struct lttng_kernel_probe_ctx *probe_ctx, size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(gid_t));
	size += sizeof(gid_t);
	return size;
}

static
void egid_record(void *priv, struct lttng_kernel_probe_ctx *probe_ctx,
		 struct lttng_kernel_ring_buffer_ctx *ctx,
		 struct lttng_kernel_channel_buffer *chan)
{
	gid_t egid;

	egid = from_kgid_munged(&init_user_ns, current_egid());
	chan->ops->event_write(ctx, &egid, sizeof(egid), lttng_alignof(egid));
}

static
void egid_get_value(void *priv,
		struct lttng_kernel_probe_ctx *lttng_probe_ctx,
		struct lttng_ctx_value *value)
{
	value->u.s64 = from_kgid_munged(&init_user_ns, current_egid());
}

static const struct lttng_kernel_ctx_field *ctx_field = lttng_kernel_static_ctx_field(
	lttng_kernel_static_event_field("egid",
		lttng_kernel_static_type_integer_from_type(gid_t, __BYTE_ORDER, 10),
		false, false),
	egid_get_size,
	egid_record,
	egid_get_value,
	NULL, NULL);

int lttng_add_egid_to_ctx(struct lttng_kernel_ctx **ctx)
{
	int ret;

	if (lttng_kernel_find_context(*ctx, ctx_field->event_field->name))
		return -EEXIST;
	ret = lttng_kernel_context_append(ctx, ctx_field);
	wrapper_vmalloc_sync_mappings();
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_add_egid_to_ctx);
