/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-vppid.c
 *
 * LTTng vPPID context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <lttng/tracer.h>

static
size_t vppid_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(pid_t));
	size += sizeof(pid_t);
	return size;
}

static
void vppid_record(struct lttng_kernel_ctx_field *field,
		  struct lib_ring_buffer_ctx *ctx,
		  struct lttng_channel *chan)
{
	struct task_struct *parent;
	pid_t vppid;

	/*
	 * current nsproxy can be NULL when scheduled out of exit. pid_vnr uses
	 * the current thread nsproxy to perform the lookup.
	 */

	/*
	 * TODO: when we eventually add RCU subsystem instrumentation,
	 * taking the rcu read lock here will trigger RCU tracing
	 * recursively. We should modify the kernel synchronization so
	 * it synchronizes both for RCU and RCU sched, and rely on
	 * rcu_read_lock_sched_notrace.
	 */

	rcu_read_lock();
	parent = rcu_dereference(current->real_parent);
	if (!current->nsproxy)
		vppid = 0;
	else
		vppid = task_tgid_vnr(parent);
	rcu_read_unlock();
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(vppid));
	chan->ops->event_write(ctx, &vppid, sizeof(vppid));
}

static
void vppid_get_value(struct lttng_kernel_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	struct task_struct *parent;
	pid_t vppid;

	/*
	 * current nsproxy can be NULL when scheduled out of exit. pid_vnr uses
	 * the current thread nsproxy to perform the lookup.
	 */

	/*
	 * TODO: when we eventually add RCU subsystem instrumentation,
	 * taking the rcu read lock here will trigger RCU tracing
	 * recursively. We should modify the kernel synchronization so
	 * it synchronizes both for RCU and RCU sched, and rely on
	 * rcu_read_lock_sched_notrace.
	 */

	rcu_read_lock();
	parent = rcu_dereference(current->real_parent);
	if (!current->nsproxy)
		vppid = 0;
	else
		vppid = task_tgid_vnr(parent);
	rcu_read_unlock();
	value->s64 = vppid;
}

static const struct lttng_kernel_ctx_field *ctx_field = lttng_kernel_static_ctx_field(
	lttng_kernel_static_event_field("vppid",
		lttng_kernel_static_type_integer_from_type(pid_t, __BYTE_ORDER, 10),
		false, false, false),
	vppid_get_size,
	NULL,
	vppid_record,
	vppid_get_value,
	NULL, NULL);

int lttng_add_vppid_to_ctx(struct lttng_kernel_ctx **ctx)
{
	int ret;

	if (lttng_kernel_find_context(*ctx, ctx_field->event_field->name))
		return -EEXIST;
	ret = lttng_kernel_context_append(ctx, ctx_field);
	wrapper_vmalloc_sync_mappings();
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_add_vppid_to_ctx);
