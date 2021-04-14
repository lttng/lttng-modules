/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-callstack.c
 *
 * LTTng callstack event context.
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright (C) 2014 Francis Giraldeau <francis.giraldeau@gmail.com>
 *
 * The callstack context can be added to any kernel event. It records
 * either the kernel or the userspace callstack, up to a max depth. The
 * context is a CTF sequence, such that it uses only the space required
 * for the number of callstack entries.
 *
 * It allocates callstack buffers per-CPU up to 4 interrupt nesting.
 * This nesting limit is the same as defined in the ring buffer. It
 * therefore uses a fixed amount of memory, proportional to the number
 * of CPUs:
 *
 *   size = cpus * nest * depth * sizeof(unsigned long)
 *
 * Which is 4096 bytes per CPU on 64-bit host and a depth of 128.
 * The allocation is done at the initialization to avoid memory
 * allocation overhead while tracing, using a shallow stack.
 *
 * The kernel callstack is recovered using save_stack_trace(), and the
 * userspace callstack uses save_stack_trace_user(). They rely on frame
 * pointers. These are usually available for the kernel, but the
 * compiler option -fomit-frame-pointer frequently used in popular Linux
 * distributions may cause the userspace callstack to be unreliable, and
 * is a known limitation of this approach. If frame pointers are not
 * available, it produces no error, but the callstack will be empty. We
 * still provide the feature, because it works well for runtime
 * environments having frame pointers. In the future, unwind support
 * and/or last branch record may provide a solution to this problem.
 *
 * The symbol name resolution is left to the trace reader.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/utsname.h>
#include <linux/stacktrace.h>
#include <linux/spinlock.h>
#include <ringbuffer/backend.h>
#include <ringbuffer/frontend.h>
#include <lttng/events.h>
#include <lttng/tracer.h>
#include <lttng/endian.h>
#include "wrapper/vmalloc.h"

#ifdef CONFIG_ARCH_STACKWALK
#include "lttng-context-callstack-stackwalk-impl.h"
#else
#include "lttng-context-callstack-legacy-impl.h"
#endif

#define NR_FIELDS	2

static
void field_data_free(struct field_data *fdata)
{
	if (!fdata)
		return;
	free_percpu(fdata->cs_percpu);
	kfree(fdata);
}

static
struct field_data __percpu *field_data_create(enum lttng_cs_ctx_modes mode)
{
	struct lttng_cs __percpu *cs_set;
	struct field_data *fdata;

	fdata = kzalloc(sizeof(*fdata), GFP_KERNEL);
	if (!fdata)
		return NULL;
	cs_set = alloc_percpu(struct lttng_cs);
	if (!cs_set)
		goto error_alloc;
	lttng_cs_set_init(cs_set);
	fdata->cs_percpu = cs_set;
	fdata->mode = mode;
	return fdata;

error_alloc:
	field_data_free(fdata);
	return NULL;
}

static
void lttng_callstack_sequence_destroy(struct lttng_kernel_ctx_field *field)
{
	struct field_data *fdata = field->priv;

	field_data_free(fdata);
}

static const struct lttng_kernel_event_field *event_fields_kernel[NR_FIELDS] = {
	lttng_kernel_static_event_field("_callstack_kernel_length",
		lttng_kernel_static_type_integer_from_type(unsigned int, __BYTE_ORDER, 10),
		false, false, false),
	lttng_kernel_static_event_field("callstack_kernel",
		lttng_kernel_static_type_sequence("_callstack_kernel_length",
			lttng_kernel_static_type_integer_from_type(unsigned long, __BYTE_ORDER, 16),
			0, none),
		false, false, false),
};

static const struct lttng_kernel_event_field *event_fields_user[NR_FIELDS] = {
	lttng_kernel_static_event_field("_callstack_user_length",
		lttng_kernel_static_type_integer_from_type(unsigned int, __BYTE_ORDER, 10),
		false, false, false),
	lttng_kernel_static_event_field("callstack_user",
		lttng_kernel_static_type_sequence("_callstack_user_length",
			lttng_kernel_static_type_integer_from_type(unsigned long, __BYTE_ORDER, 16),
			0, none),
		false, false, false),
};

const struct lttng_kernel_event_field **lttng_cs_event_fields(enum lttng_cs_ctx_modes mode)
{
	switch (mode) {
	case CALLSTACK_KERNEL:
		return event_fields_kernel;
	case CALLSTACK_USER:
		return event_fields_user;
	default:
		return NULL;
	}
}

static
int __lttng_add_callstack_generic(struct lttng_kernel_ctx **ctx,
		enum lttng_cs_ctx_modes mode)
{
	const struct lttng_kernel_event_field **event_fields;
	struct lttng_kernel_ctx_field ctx_field;
	struct field_data *fdata;
	int ret, i;

	ret = init_type(mode);
	if (ret)
		return ret;
	event_fields = lttng_cs_event_fields(mode);
	if (!event_fields) {
		return -EINVAL;
	}
	for (i = 0; i < NR_FIELDS; i++) {
		if (lttng_kernel_find_context(*ctx, event_fields[i]->name))
			return -EEXIST;
	}
	fdata = field_data_create(mode);
	if (!fdata) {
		ret = -ENOMEM;
		goto error_create;
	}
	memset(&ctx_field, 0, sizeof(ctx_field));
	ctx_field.event_field = event_fields[0];
	ctx_field.get_size_arg = lttng_callstack_length_get_size;
	ctx_field.record = lttng_callstack_length_record;
	ctx_field.priv = fdata;
	ret = lttng_kernel_context_append(ctx, &ctx_field);
	if (ret) {
		ret = -ENOMEM;
		goto error_append0;
	}

	memset(&ctx_field, 0, sizeof(ctx_field));
	ctx_field.event_field = event_fields[1];
	ctx_field.get_size_arg = lttng_callstack_sequence_get_size;
	ctx_field.record = lttng_callstack_sequence_record;
	ctx_field.destroy = lttng_callstack_sequence_destroy;
	ctx_field.priv = fdata;
	ret = lttng_kernel_context_append(ctx, &ctx_field);
	if (ret) {
		ret = -ENOMEM;
		goto error_append1;
	}
	return 0;

error_append1:
	lttng_kernel_context_remove_last(ctx);
error_append0:
	field_data_free(fdata);
error_create:
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
int lttng_add_callstack_to_ctx(struct lttng_kernel_ctx **ctx, int type)
{
	switch (type) {
	case LTTNG_KERNEL_ABI_CONTEXT_CALLSTACK_KERNEL:
		return __lttng_add_callstack_generic(ctx, CALLSTACK_KERNEL);
#ifdef CONFIG_X86
	case LTTNG_KERNEL_ABI_CONTEXT_CALLSTACK_USER:
		return __lttng_add_callstack_generic(ctx, CALLSTACK_USER);
#endif
	default:
		return -EINVAL;
	}
}
EXPORT_SYMBOL_GPL(lttng_add_callstack_to_ctx);
