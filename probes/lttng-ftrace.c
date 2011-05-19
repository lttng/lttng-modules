/*
 * (C) Copyright	2009-2011 -
 * 		Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng function tracer integration module.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/ftrace.h>
#include <linux/slab.h>
#include "../ltt-events.h"
#include "../wrapper/ringbuffer/frontend_types.h"
#include "../ltt-tracer.h"

static
int lttng_ftrace_handler(unsigned long ip, unsigned long parent_ip, void **data)
{
	struct ltt_event *event = *data;
	struct ltt_channel *chan = event->chan;
	struct lib_ring_buffer_ctx ctx;
	struct {
		unsigned long ip;
		unsigned long parent_ip;
	} payload;
	int ret;

	if (!ACCESS_ONCE(chan->session->active))
		return 0;
	lib_ring_buffer_ctx_init(&ctx, chan->chan, NULL,
				 sizeof(payload), ltt_alignof(payload), -1);
	ret = chan->ops->event_reserve(&ctx);
	if (ret < 0)
		return 0;
	payload.ip = ip;
	payload.parent_ip = parent_ip;
	lib_ring_buffer_align_ctx(&ctx, ltt_alignof(payload));
	chan->ops->event_write(&ctx, &payload, sizeof(payload));
	chan->ops->event_commit(&ctx);
	return 0;
}

/*
 * Create event description
 */
static
int lttng_create_ftrace_event(const char *name, struct ltt_event *event)
{
	struct lttng_event_field *fields;
	struct lttng_event_desc *desc;
	int ret;

	desc = kzalloc(sizeof(*event->desc), GFP_KERNEL);
	if (!desc)
		return -ENOMEM;
	desc->name = kstrdup(name, GFP_KERNEL);
	if (!desc->name) {
		ret = -ENOMEM;
		goto error_str;
	}
	desc->nr_fields = 2;
	desc->fields = fields =
		kzalloc(2 * sizeof(struct lttng_event_field), GFP_KERNEL);
	fields[0].name = "ip";
	fields[0].type.atype = atype_integer;
	fields[0].type.u.basic.integer.size = sizeof(unsigned long);
	fields[0].type.u.basic.integer.alignment = ltt_alignof(unsigned long);
	fields[0].type.u.basic.integer.signedness = 0;
	fields[0].type.u.basic.integer.reverse_byte_order = 0;
	fields[0].type.u.basic.integer.base = 16;
	fields[0].type.u.basic.integer.encoding = lttng_encode_none;

	fields[1].name = "parent_ip";
	fields[1].type.atype = atype_integer;
	fields[1].type.u.basic.integer.size = sizeof(unsigned long);
	fields[1].type.u.basic.integer.alignment = ltt_alignof(unsigned long);
	fields[1].type.u.basic.integer.signedness = 0;
	fields[1].type.u.basic.integer.reverse_byte_order = 0;
	fields[1].type.u.basic.integer.base = 16;
	fields[1].type.u.basic.integer.encoding = lttng_encode_none;

	event->desc = desc;

	return 0;

error_str:
	kfree(desc);
	return ret;
}

static
struct ftrace_probe_ops lttng_ftrace_ops = {
	.func = lttng_ftrace_handler,
};

int lttng_ftrace_register(const char *name,
			  const char *symbol_name,
			  struct ltt_event *event)
{
	int ret;

	ret = lttng_create_ftrace_event(name, event);
	if (ret)
		goto error;

	event->u.ftrace.symbol_name = kstrdup(name, GFP_KERNEL);
	if (!event->u.ftrace.symbol_name)
		goto name_error;

	ret = register_ftrace_function_probe(symbol_name,
			&lttng_ftrace_ops, event);
	if (ret)
		goto register_error;
	return 0;

register_error:
	kfree(event->u.ftrace.symbol_name);
name_error:
	kfree(event->desc->name);
	kfree(event->desc);
error:
	return ret;
}
EXPORT_SYMBOL_GPL(lttng_ftrace_register);

void lttng_ftrace_unregister(struct ltt_event *event)
{
	unregister_ftrace_function_probe(event->u.ftrace.symbol_name,
			&lttng_ftrace_ops, event);
	kfree(event->u.ftrace.symbol_name);
	kfree(event->desc->name);
	kfree(event->desc);
}
EXPORT_SYMBOL_GPL(lttng_ftrace_unregister);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Linux Trace Toolkit Ftrace Support");
