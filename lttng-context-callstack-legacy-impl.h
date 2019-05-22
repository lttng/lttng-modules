/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-context-callstack-legacy-impl.h
 *
 * LTTng callstack event context, legacy implementation. Targets
 * kernels and architectures not yet using the stacktrace common
 * infrastructure introduced in the upstream Linux kernel by commit
 * 214d8ca6ee "stacktrace: Provide common infrastructure" (merged in
 * Linux 5.2, then gradually introduced within architectures).
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright (C) 2014 Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#define MAX_ENTRIES 128

enum lttng_cs_ctx_modes {
	CALLSTACK_KERNEL = 0,
	CALLSTACK_USER = 1,
	NR_CALLSTACK_MODES,
};

struct lttng_cs_dispatch {
	struct stack_trace stack_trace;
	unsigned long entries[MAX_ENTRIES];
};

struct lttng_cs {
	struct lttng_cs_dispatch dispatch[RING_BUFFER_MAX_NESTING];
};

struct field_data {
	struct lttng_cs __percpu *cs_percpu;
	enum lttng_cs_ctx_modes mode;
};

struct lttng_cs_type {
	const char *name;
	const char *save_func_name;
	void (*save_func)(struct stack_trace *trace);
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
const char *lttng_cs_ctx_mode_name(enum lttng_cs_ctx_modes mode)
{
	return cs_types[mode].name;
}

static
int init_type(enum lttng_cs_ctx_modes mode)
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
void lttng_cs_set_init(struct lttng_cs __percpu *cs_set)
{
	int cpu, i;

	for_each_possible_cpu(cpu) {
		struct lttng_cs *cs;

		cs = per_cpu_ptr(cs_set, cpu);
		for (i = 0; i < RING_BUFFER_MAX_NESTING; i++) {
			struct lttng_cs_dispatch *dispatch;

			dispatch = &cs->dispatch[i];
			dispatch->stack_trace.entries = dispatch->entries;
			dispatch->stack_trace.max_entries = MAX_ENTRIES;
		}
	}
}

/* Keep track of nesting inside userspace callstack context code */
DEFINE_PER_CPU(int, callstack_user_nesting);

static
struct stack_trace *stack_trace_context(struct lttng_ctx_field *field,
					struct lib_ring_buffer_ctx *ctx)
{
	int buffer_nesting, cs_user_nesting;
	struct lttng_cs *cs;
	struct field_data *fdata = field->priv;

	/*
	 * Do not gather the userspace callstack context when the event was
	 * triggered by the userspace callstack context saving mechanism.
	 */
	cs_user_nesting = per_cpu(callstack_user_nesting, ctx->cpu);

	if (fdata->mode == CALLSTACK_USER && cs_user_nesting >= 1)
		return NULL;

	/*
	 * get_cpu() is not required, preemption is already
	 * disabled while event is written.
	 *
	 * max nesting is checked in lib_ring_buffer_get_cpu().
	 * Check it again as a safety net.
	 */
	cs = per_cpu_ptr(fdata->cs_percpu, ctx->cpu);
	buffer_nesting = per_cpu(lib_ring_buffer_nesting, ctx->cpu) - 1;
	if (buffer_nesting >= RING_BUFFER_MAX_NESTING)
		return NULL;

	return &cs->dispatch[buffer_nesting].stack_trace;
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
	struct stack_trace *trace;
	struct field_data *fdata = field->priv;
	size_t orig_offset = offset;

	/* do not write data if no space is available */
	trace = stack_trace_context(field, ctx);
	if (unlikely(!trace)) {
		offset += lib_ring_buffer_align(offset, lttng_alignof(unsigned int));
		offset += sizeof(unsigned int);
		offset += lib_ring_buffer_align(offset, lttng_alignof(unsigned long));
		return offset - orig_offset;
	}

	/* reset stack trace, no need to clear memory */
	trace->nr_entries = 0;

	if (fdata->mode == CALLSTACK_USER)
		++per_cpu(callstack_user_nesting, ctx->cpu);

	/* do the real work and reserve space */
	cs_types[fdata->mode].save_func(trace);

	if (fdata->mode == CALLSTACK_USER)
		per_cpu(callstack_user_nesting, ctx->cpu)--;

	/*
	 * Remove final ULONG_MAX delimiter. If we cannot find it, add
	 * our own marker to show that the stack is incomplete. This is
	 * more compact for a trace.
	 */
	if (trace->nr_entries > 0
			&& trace->entries[trace->nr_entries - 1] == ULONG_MAX) {
		trace->nr_entries--;
	}
	offset += lib_ring_buffer_align(offset, lttng_alignof(unsigned int));
	offset += sizeof(unsigned int);
	offset += lib_ring_buffer_align(offset, lttng_alignof(unsigned long));
	offset += sizeof(unsigned long) * trace->nr_entries;
	/* Add our own ULONG_MAX delimiter to show incomplete stack. */
	if (trace->nr_entries == trace->max_entries)
		offset += sizeof(unsigned long);
	return offset - orig_offset;
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
