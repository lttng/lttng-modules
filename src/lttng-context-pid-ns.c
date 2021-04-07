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
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <wrapper/namespace.h>
#include <lttng/tracer.h>

#if defined(CONFIG_PID_NS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))

static
size_t pid_ns_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(unsigned int));
	size += sizeof(unsigned int);
	return size;
}

static
void pid_ns_record(struct lttng_kernel_ctx_field *field,
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
void pid_ns_get_value(struct lttng_kernel_ctx_field *field,
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

static const struct lttng_kernel_ctx_field *ctx_field = lttng_kernel_static_ctx_field(
	lttng_kernel_static_event_field("pid_ns",
		lttng_kernel_static_type_integer_from_type(unsigned int, __BYTE_ORDER, 10),
		false, false, false),
	pid_ns_get_size,
	NULL,
	pid_ns_record,
	pid_ns_get_value,
	NULL, NULL);

int lttng_add_pid_ns_to_ctx(struct lttng_kernel_ctx **ctx)
{
	int ret;

	if (lttng_kernel_find_context(*ctx, ctx_field->event_field->name))
		return -EEXIST;
	ret = lttng_kernel_context_append(ctx, ctx_field);
	wrapper_vmalloc_sync_mappings();
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_add_pid_ns_to_ctx);

#endif
