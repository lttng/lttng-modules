/*
 * lttng-context-callstack.c
 *
 * LTTng callstack event context.
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright (C) 2014 Francis Giraldeau <francis.giraldeau@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * The callstack context can be added to any kernel
 * event. It records either the kernel or the userspace callstack, up to a
 * max depth. The context is a CTF sequence, such that it uses only the space
 * required for the number of callstack entries.
 *
 * It allocates callstack buffers per-CPU up to 4 interrupt nesting. This
 * nesting limit is the same as defined in the ring buffer. It therefore uses a
 * fixed amount of memory, proportional to the number of CPUs:
 *
 *   size = cpus * nest * depth * sizeof(unsigned long)
 *
 * Which is about 800 bytes per-CPUs on 64-bit host and a depth of 25. The
 * allocation is done at the initialization to avoid memory allocation
 * overhead while tracing, using a shallow stack.
 *
 * The kernel callstack is recovered using save_stack_trace(), and the
 * userspace callstack uses save_stack_trace_user(). They rely on frame
 * pointers. These are usually available for the kernel, but the compiler
 * option -fomit-frame-pointer frequently used in popular Linux distributions
 * may cause the userspace callstack to be unreliable, and is a known
 * limitation of this approach. If frame pointers are not available, it
 * produces no error, but the callstack will be empty. We still provide the
 * feature, because it works well for runtime environments having frame
 * pointers. In the future, unwind support and/or last branch record may
 * provide a solution to this problem.
 *
 * The symbol name resolution is left to the trace reader.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/utsname.h>
#include <linux/stacktrace.h>
#include <linux/spinlock.h>
#include "lttng-events.h"
#include "wrapper/ringbuffer/backend.h"
#include "wrapper/ringbuffer/frontend.h"
#include "wrapper/vmalloc.h"
#include "lttng-tracer.h"

#define MAX_ENTRIES 25

struct lttng_cs_nesting {
	struct stack_trace stack_trace;
	unsigned long entries[MAX_ENTRIES];
};

struct lttng_cs {
	struct lttng_cs_nesting level[RING_BUFFER_MAX_NESTING];
};

struct field_data {
	struct lttng_cs __percpu *cs_percpu;
	int mode;
};

struct lttng_cs_type {
	const char *name;
	const char *save_func_name;
	void (*save_func)(struct stack_trace *trace);
};

enum lttng_cs_ctx_modes {
	CALLSTACK_KERNEL = 0,
	CALLSTACK_USER = 1,
};

static struct lttng_cs_type cs_types[] = {
	{
		.name		= "callstack_kernel",
		.save_func_name	= "save_stack_trace",
		.save_func	= NULL,
	},
	{
		.name		= "callstack_user",
		.save_func_name	= "save_stack_trace_user",
		.save_func	= NULL,
	},
};

static
int init_type(int mode)
{
	unsigned long func;

	if (cs_types[mode].save_func)
		return 0;
	func = kallsyms_lookup_funcptr(cs_types[mode].save_func_name);
	if (!func) {
		printk(KERN_WARNING "LTTng: symbol lookup failed: %s\n",
				cs_types[mode].save_func_name);
		return -EINVAL;
	}
	cs_types[mode].save_func = (void *) func;
	return 0;
}

static
struct stack_trace *stack_trace_context(struct lttng_ctx_field *field,
					struct lib_ring_buffer_ctx *ctx)
{
	int nesting;
	struct lttng_cs *cs;
	struct field_data *fdata = field->priv;

	/*
	 * get_cpu() is not required, preemption is already
	 * disabled while event is written.
	 *
	 * max nesting is checked in lib_ring_buffer_get_cpu().
	 * Check it again as a safety net.
	 */
	cs = per_cpu_ptr(fdata->cs_percpu, ctx->cpu);
	nesting = per_cpu(lib_ring_buffer_nesting, ctx->cpu) - 1;
	if (nesting >= RING_BUFFER_MAX_NESTING) {
		return NULL;
	}
	return &cs->level[nesting].stack_trace;
}

/*
 * In order to reserve the correct size, the callstack is computed. The
 * resulting callstack is saved to be accessed in the record step.
 */
static
size_t lttng_callstack_get_size(size_t offset, struct lttng_ctx_field *field,
				struct lib_ring_buffer_ctx *ctx,
				struct lttng_channel *chan)
{
	size_t size = 0;
	struct stack_trace *trace;
	struct field_data *fdata = field->priv;

	/* do not write data if no space is available */
	trace = stack_trace_context(field, ctx);
	if (unlikely(!trace)) {
		size += lib_ring_buffer_align(offset, lttng_alignof(unsigned int));
		size += sizeof(unsigned int);
		size += lib_ring_buffer_align(offset, lttng_alignof(unsigned long));
		return size;
	}

	/* reset stack trace, no need to clear memory */
	trace->nr_entries = 0;

	/* do the real work and reserve space */
	cs_types[fdata->mode].save_func(trace);
	/*
	 * Remove final ULONG_MAX delimiter. If we cannot find it, add
	 * our own marker to show that the stack is incomplete. This is
	 * more compact for a trace.
	 */
	if (trace->nr_entries > 0
			&& trace->entries[trace->nr_entries - 1] == ULONG_MAX) {
		trace->nr_entries--;
	}
	size += lib_ring_buffer_align(offset, lttng_alignof(unsigned int));
	size += sizeof(unsigned int);
	size += lib_ring_buffer_align(offset, lttng_alignof(unsigned long));
	size += sizeof(unsigned long) * trace->nr_entries;
	/* Add our own ULONG_MAX delimiter to show incomplete stack. */
	if (trace->nr_entries == trace->max_entries)
		size += sizeof(unsigned long);
	return size;
}

static
void lttng_callstack_record(struct lttng_ctx_field *field,
			struct lib_ring_buffer_ctx *ctx,
			struct lttng_channel *chan)
{
	struct stack_trace *trace = stack_trace_context(field, ctx);
	unsigned int nr_seq_entries;

	if (unlikely(!trace)) {
		nr_seq_entries = 0;
		lib_ring_buffer_align_ctx(ctx, lttng_alignof(unsigned int));
		chan->ops->event_write(ctx, &nr_seq_entries, sizeof(unsigned int));
		lib_ring_buffer_align_ctx(ctx, lttng_alignof(unsigned long));
		return;
	}
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(unsigned int));
	nr_seq_entries = trace->nr_entries;
	if (trace->nr_entries == trace->max_entries)
		nr_seq_entries++;
	chan->ops->event_write(ctx, &nr_seq_entries, sizeof(unsigned int));
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(unsigned long));
	chan->ops->event_write(ctx, trace->entries,
			sizeof(unsigned long) * trace->nr_entries);
	/* Add our own ULONG_MAX delimiter to show incomplete stack. */
	if (trace->nr_entries == trace->max_entries) {
		unsigned long delim = ULONG_MAX;

		chan->ops->event_write(ctx, &delim, sizeof(unsigned long));
	}
}

static
void field_data_free(struct field_data *fdata)
{
	if (!fdata)
		return;
	free_percpu(fdata->cs_percpu);
	kfree(fdata);
}

static
struct field_data __percpu *field_data_create(int type)
{
	int cpu, i;
	struct lttng_cs __percpu *cs_set;
	struct field_data *fdata;

	fdata = kzalloc(sizeof(*fdata), GFP_KERNEL);
	if (!fdata)
		return NULL;
	cs_set = alloc_percpu(struct lttng_cs);
	if (!cs_set)
		goto error_alloc;

	fdata->cs_percpu = cs_set;
	for_each_possible_cpu(cpu) {
		struct lttng_cs *cs;

		cs = per_cpu_ptr(cs_set, cpu);
		for (i = 0; i < RING_BUFFER_MAX_NESTING; i++) {
			struct lttng_cs_nesting *level;

			level = &cs->level[i];
			level->stack_trace.entries = level->entries;
			level->stack_trace.max_entries = MAX_ENTRIES;
		}
	}
	fdata->mode = type;
	return fdata;

error_alloc:
	field_data_free(fdata);
	return NULL;
}

static
void lttng_callstack_destroy(struct lttng_ctx_field *field)
{
	struct field_data *fdata = field->priv;

	field_data_free(fdata);
}

static
int __lttng_add_callstack_generic(struct lttng_ctx **ctx, int mode)
{
	const char *ctx_name = cs_types[mode].name;
	struct lttng_ctx_field *field;
	struct field_data *fdata;
	int ret;

	ret = init_type(mode);
	if (ret)
		return ret;
	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, ctx_name)) {
		ret = -EEXIST;
		goto error_find;
	}
	fdata = field_data_create(mode);
	if (!fdata) {
		ret = -ENOMEM;
		goto error_create;
	}

	field->event_field.name = ctx_name;
	field->event_field.type.atype = atype_sequence;
	field->event_field.type.u.sequence.elem_type.atype = atype_integer;
	field->event_field.type.u.sequence.elem_type.u.basic.integer.size = sizeof(unsigned long) * CHAR_BIT;
	field->event_field.type.u.sequence.elem_type.u.basic.integer.alignment = lttng_alignof(long) * CHAR_BIT;
	field->event_field.type.u.sequence.elem_type.u.basic.integer.signedness = lttng_is_signed_type(unsigned long);
	field->event_field.type.u.sequence.elem_type.u.basic.integer.reverse_byte_order = 0;
	field->event_field.type.u.sequence.elem_type.u.basic.integer.base = 16;
	field->event_field.type.u.sequence.elem_type.u.basic.integer.encoding = lttng_encode_none;

	field->event_field.type.u.sequence.length_type.atype = atype_integer;
	field->event_field.type.u.sequence.length_type.u.basic.integer.size = sizeof(unsigned int) * CHAR_BIT;
	field->event_field.type.u.sequence.length_type.u.basic.integer.alignment = lttng_alignof(unsigned int) * CHAR_BIT;
	field->event_field.type.u.sequence.length_type.u.basic.integer.signedness = lttng_is_signed_type(unsigned int);
	field->event_field.type.u.sequence.length_type.u.basic.integer.reverse_byte_order = 0;
	field->event_field.type.u.sequence.length_type.u.basic.integer.base = 10;
	field->event_field.type.u.sequence.length_type.u.basic.integer.encoding = lttng_encode_none;

	field->get_size_arg = lttng_callstack_get_size;
	field->record = lttng_callstack_record;
	field->priv = fdata;
	field->destroy = lttng_callstack_destroy;
	wrapper_vmalloc_sync_all();
	return 0;

error_create:
	field_data_free(fdata);
error_find:
	lttng_remove_context_field(ctx, field);
	return ret;
}

/**
 *	lttng_add_callstack_to_ctx - add callstack event context
 *
 *	@ctx: the lttng_ctx pointer to initialize
 *	@type: the context type
 *
 *	Supported callstack type supported:
 *	LTTNG_KERNEL_CONTEXT_CALLSTACK_KERNEL
 *		Records the callstack of the kernel
 *	LTTNG_KERNEL_CONTEXT_CALLSTACK_USER
 *		Records the callstack of the userspace program (from the kernel)
 *
 * Return 0 for success, or error code.
 */
int lttng_add_callstack_to_ctx(struct lttng_ctx **ctx, int type)
{
	switch (type) {
	case LTTNG_KERNEL_CONTEXT_CALLSTACK_KERNEL:
		return __lttng_add_callstack_generic(ctx, CALLSTACK_KERNEL);
	case LTTNG_KERNEL_CONTEXT_CALLSTACK_USER:
		return __lttng_add_callstack_generic(ctx, CALLSTACK_USER);
	default:
		return -EINVAL;
	}
}
EXPORT_SYMBOL_GPL(lttng_add_callstack_to_ctx);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Francis Giraldeau");
MODULE_DESCRIPTION("Linux Trace Toolkit Callstack Support");
