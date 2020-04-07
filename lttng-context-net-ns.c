/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-net-ns.c
 *
 * LTTng net namespace context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *               2019 Michael Jeanson <mjeanson@efficios.com>
 *
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/nsproxy.h>
#include <net/net_namespace.h>
#include <lttng-events.h>
#include <wrapper/ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <wrapper/namespace.h>
#include <lttng-tracer.h>

#if defined(CONFIG_NET_NS) && \
	(LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))

static
size_t net_ns_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(unsigned int));
	size += sizeof(unsigned int);
	return size;
}

static
void net_ns_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	unsigned int net_ns_inum = 0;

	/*
	 * nsproxy can be NULL when scheduled out of exit.
	 *
	 * As documented in 'linux/nsproxy.h' namespaces access rules, no
	 * precautions should be taken when accessing the current task's
	 * namespaces, just dereference the pointers.
	 */
	if (current->nsproxy)
		net_ns_inum = current->nsproxy->net_ns->lttng_ns_inum;

	lib_ring_buffer_align_ctx(ctx, lttng_alignof(net_ns_inum));
	chan->ops->event_write(ctx, &net_ns_inum, sizeof(net_ns_inum));
}

static
void net_ns_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	unsigned int net_ns_inum = 0;

	/*
	 * nsproxy can be NULL when scheduled out of exit.
	 *
	 * As documented in 'linux/nsproxy.h' namespaces access rules, no
	 * precautions should be taken when accessing the current task's
	 * namespaces, just dereference the pointers.
	 */
	if (current->nsproxy)
		net_ns_inum = current->nsproxy->net_ns->lttng_ns_inum;

	value->s64 = net_ns_inum;
}

int lttng_add_net_ns_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "net_ns")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "net_ns";
	field->event_field.type.atype = atype_integer;
	field->event_field.type.u.integer.size = sizeof(unsigned int) * CHAR_BIT;
	field->event_field.type.u.integer.alignment = lttng_alignof(unsigned int) * CHAR_BIT;
	field->event_field.type.u.integer.signedness = lttng_is_signed_type(unsigned int);
	field->event_field.type.u.integer.reverse_byte_order = 0;
	field->event_field.type.u.integer.base = 10;
	field->event_field.type.u.integer.encoding = lttng_encode_none;
	field->get_size = net_ns_get_size;
	field->record = net_ns_record;
	field->get_value = net_ns_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_all();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_net_ns_to_ctx);

#endif
