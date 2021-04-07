/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-ipc-ns.c
 *
 * LTTng ipc namespace context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *               2019 Michael Jeanson <mjeanson@efficios.com>
 *
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/ipc_namespace.h>
#include <lttng/events.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <wrapper/namespace.h>
#include <lttng/tracer.h>

#if defined(CONFIG_IPC_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))

static
size_t ipc_ns_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(unsigned int));
	size += sizeof(unsigned int);
	return size;
}

static
void ipc_ns_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	unsigned int ipc_ns_inum = 0;

	/*
	 * nsproxy can be NULL when scheduled out of exit.
	 *
	 * As documented in 'linux/nsproxy.h' namespaces access rules, no
	 * precautions should be taken when accessing the current task's
	 * namespaces, just dereference the pointers.
	 */
	if (current->nsproxy)
		ipc_ns_inum = current->nsproxy->ipc_ns->lttng_ns_inum;

	lib_ring_buffer_align_ctx(ctx, lttng_alignof(ipc_ns_inum));
	chan->ops->event_write(ctx, &ipc_ns_inum, sizeof(ipc_ns_inum));
}

static
void ipc_ns_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	unsigned int ipc_ns_inum = 0;

	/*
	 * nsproxy can be NULL when scheduled out of exit.
	 *
	 * As documented in 'linux/nsproxy.h' namespaces access rules, no
	 * precautions should be taken when accessing the current task's
	 * namespaces, just dereference the pointers.
	 */
	if (current->nsproxy)
		ipc_ns_inum = current->nsproxy->ipc_ns->lttng_ns_inum;

	value->s64 = ipc_ns_inum;
}

int lttng_add_ipc_ns_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "ipc_ns")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "ipc_ns";
	field->event_field.type.type = lttng_kernel_type_integer;
	field->event_field.type.u.integer.size = sizeof(unsigned int) * CHAR_BIT;
	field->event_field.type.u.integer.alignment = lttng_alignof(unsigned int) * CHAR_BIT;
	field->event_field.type.u.integer.signedness = lttng_is_signed_type(unsigned int);
	field->event_field.type.u.integer.reverse_byte_order = 0;
	field->event_field.type.u.integer.base = 10;
	field->event_field.type.u.integer.encoding = lttng_kernel_string_encoding_none;
	field->get_size = ipc_ns_get_size;
	field->record = ipc_ns_record;
	field->get_value = ipc_ns_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_mappings();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_ipc_ns_to_ctx);

#endif
