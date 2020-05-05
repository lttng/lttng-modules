/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
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
#include "lttng-events.h"
#include "wrapper/ringbuffer/backend.h"
#include "wrapper/ringbuffer/frontend.h"
#include "wrapper/vmalloc.h"
#include "lttng-tracer.h"

#ifdef CONFIG_ARCH_STACKWALK
#include "lttng-context-callstack-stackwalk-impl.h"
#else
#include "lttng-context-callstack-legacy-impl.h"
#endif

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
void lttng_callstack_destroy(struct lttng_ctx_field *field)
{
	struct field_data *fdata = field->priv;

	field_data_free(fdata);
}

static
int __lttng_add_callstack_generic(struct lttng_ctx **ctx,
		enum lttng_cs_ctx_modes mode)
{
	const char *ctx_name = lttng_cs_ctx_mode_name(mode);
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
	wrapper_vmalloc_sync_mappings();
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
#ifdef CONFIG_X86
	case LTTNG_KERNEL_CONTEXT_CALLSTACK_USER:
		return __lttng_add_callstack_generic(ctx, CALLSTACK_USER);
#endif
	default:
		return -EINVAL;
	}
}
EXPORT_SYMBOL_GPL(lttng_add_callstack_to_ctx);
