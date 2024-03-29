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
#include <linux/user_namespace.h>

#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <lttng/tracer.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>

static
size_t vsuid_get_size(void *priv, struct lttng_kernel_probe_ctx *probe_ctx, size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(uid_t));
	size += sizeof(uid_t);
	return size;
}

static
void vsuid_record(void *priv, struct lttng_kernel_probe_ctx *probe_ctx,
		 struct lttng_kernel_ring_buffer_ctx *ctx,
		 struct lttng_kernel_channel_buffer *chan)
{
	uid_t vsuid;

	vsuid = from_kuid_munged(current_user_ns(), current_suid());
	chan->ops->event_write(ctx, &vsuid, sizeof(vsuid), lttng_alignof(vsuid));
}

static
void vsuid_get_value(void *priv,
		struct lttng_kernel_probe_ctx *lttng_probe_ctx,
		struct lttng_ctx_value *value)
{
	value->u.s64 = from_kuid_munged(current_user_ns(), current_suid());
}

static const struct lttng_kernel_ctx_field *ctx_field = lttng_kernel_static_ctx_field(
	lttng_kernel_static_event_field("vsuid",
		lttng_kernel_static_type_integer_from_type(uid_t, __BYTE_ORDER, 10),
		false, false),
	vsuid_get_size,
	vsuid_record,
	vsuid_get_value,
	NULL, NULL);

int lttng_add_vsuid_to_ctx(struct lttng_kernel_ctx **ctx)
{
	int ret;

	if (lttng_kernel_find_context(*ctx, ctx_field->event_field->name))
		return -EEXIST;
	ret = lttng_kernel_context_append(ctx, ctx_field);
	wrapper_vmalloc_sync_mappings();
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_add_vsuid_to_ctx);
