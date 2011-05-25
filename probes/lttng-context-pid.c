/*
 * (C) Copyright	2009-2011 -
 * 		Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng PID context.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include "../ltt-events.h"
#include "../wrapper/ringbuffer/frontend_types.h"
#include "../wrapper/vmalloc.h"
#include "../ltt-tracer.h"

static
void pid_record(struct lttng_ctx_field *field,
		struct lib_ring_buffer_ctx *ctx,
		struct ltt_channel *chan)
{
	pid_t pid;

	pid = current->pid;
	lib_ring_buffer_align_ctx(ctx, ltt_alignof(pid));
	chan->ops->event_write(ctx, &pid, sizeof(pid));
}

int lttng_add_pid_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;
	int ret;

	field = lttng_append_context(ctx);
	if (!field)
		return ret;
	field->name = "pid";
	field->type.atype = atype_integer;
	field->type.u.basic.integer.size = sizeof(pid_t) * CHAR_BIT;
	field->type.u.basic.integer.alignment = ltt_alignof(pid_t) * CHAR_BIT;
	field->type.u.basic.integer.signedness = is_signed_type(pid_t);
	field->type.u.basic.integer.reverse_byte_order = 0;
	field->type.u.basic.integer.base = 10;
	field->type.u.basic.integer.encoding = lttng_encode_none;
	field->callback = pid_record;
	wrapper_vmalloc_sync_all();
	return 0;
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Linux Trace Toolkit Perf Support");
