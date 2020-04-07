/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-pid-ns.c
 *
 * LTTng pid namespace context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *               2019 Michael Jeanson <mjeanson@efficios.com>
 *
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/pid_namespace.h>
#include <lttng-events.h>
#include <wrapper/ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <wrapper/namespace.h>
#include <lttng-tracer.h>

#if defined(CONFIG_PID_NS) && \
	(LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))

static
size_t pid_ns_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(unsigned int));
	size += sizeof(unsigned int);
	return size;
}

static
void pid_ns_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	struct pid_namespace *ns;
	unsigned int pid_ns_inum = 0;

	/*
	 * The pid namespace is an exception -- it's accessed using
	 * task_active_pid_ns. The pid namespace in nsproxy is the
	 * namespace that children will use.
	 */
	ns = task_active_pid_ns(current);

	if (ns)
		pid_ns_inum = ns->lttng_ns_inum;

	lib_ring_buffer_align_ctx(ctx, lttng_alignof(pid_ns_inum));
	chan->ops->event_write(ctx, &pid_ns_inum, sizeof(pid_ns_inum));
}

static
void pid_ns_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	struct pid_namespace *ns;
	unsigned int pid_ns_inum = 0;

	/*
	 * The pid namespace is an exception -- it's accessed using
	 * task_active_pid_ns. The pid namespace in nsproxy is the
	 * namespace that children will use.
	 */
	ns = task_active_pid_ns(current);

	if (ns)
		pid_ns_inum = ns->lttng_ns_inum;

	value->s64 = pid_ns_inum;
}

int lttng_add_pid_ns_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "pid_ns")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "pid_ns";
	field->event_field.type.atype = atype_integer;
	field->event_field.type.u.integer.size = sizeof(unsigned int) * CHAR_BIT;
	field->event_field.type.u.integer.alignment = lttng_alignof(unsigned int) * CHAR_BIT;
	field->event_field.type.u.integer.signedness = lttng_is_signed_type(unsigned int);
	field->event_field.type.u.integer.reverse_byte_order = 0;
	field->event_field.type.u.integer.base = 10;
	field->event_field.type.u.integer.encoding = lttng_encode_none;
	field->get_size = pid_ns_get_size;
	field->record = pid_ns_record;
	field->get_value = pid_ns_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_all();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_pid_ns_to_ctx);

#endif
