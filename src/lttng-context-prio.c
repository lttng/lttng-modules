/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-prio.c
 *
 * LTTng priority context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <wrapper/kallsyms.h>
#include <lttng/tracer.h>

static
int (*wrapper_task_prio_sym)(struct task_struct *t);

int wrapper_task_prio_init(void)
{
	wrapper_task_prio_sym = (void *) kallsyms_lookup_funcptr("task_prio");
	if (!wrapper_task_prio_sym) {
		printk(KERN_WARNING "LTTng: task_prio symbol lookup failed.\n");
		return -EINVAL;
	}
	return 0;
}

/*
 * Canary function to check for 'task_prio()' at compile time.
 *
 * From 'include/linux/sched.h':
 *
 *   extern int task_prio(const struct task_struct *p);
 */
__attribute__((unused)) static
int __canary__task_prio(const struct task_struct *p)
{
	return task_prio(p);
}

static
size_t prio_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(int));
	size += sizeof(int);
	return size;
}

static
void prio_record(struct lttng_kernel_ctx_field *field,
		struct lib_ring_buffer_ctx *ctx,
		struct lttng_channel *chan)
{
	int prio;

	prio = wrapper_task_prio_sym(current);
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(prio));
	chan->ops->event_write(ctx, &prio, sizeof(prio));
}

static
void prio_get_value(struct lttng_kernel_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	value->s64 = wrapper_task_prio_sym(current);
}

static const struct lttng_kernel_ctx_field *ctx_field = lttng_kernel_static_ctx_field(
	lttng_kernel_static_event_field("prio",
		lttng_kernel_static_type_integer_from_type(int, __BYTE_ORDER, 10),
		false, false, false),
	prio_get_size,
	NULL,
	prio_record,
	prio_get_value,
	NULL, NULL);

int lttng_add_prio_to_ctx(struct lttng_kernel_ctx **ctx)
{
	int ret;

	if (lttng_kernel_find_context(*ctx, ctx_field->event_field->name))
		return -EEXIST;
	ret = lttng_kernel_context_append(ctx, ctx_field);
	wrapper_vmalloc_sync_mappings();
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_add_prio_to_ctx);
