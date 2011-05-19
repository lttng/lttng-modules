/*
 * (C) Copyright	2009-2011 -
 * 		Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng kprobes integration module.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/kprobes.h>
#include "../ltt-events.h"
#include "../wrapper/ringbuffer/frontend_types.h"
#include "../ltt-tracer.h"

void lttng_kprobes_handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	struct ltt_event *event =
		container_of(p, struct ltt_event, u.kprobe.kp);
	struct ltt_channel *chan = event->chan;
	struct lib_ring_buffer_ctx ctx;
	int ret;
	unsigned long data = (unsigned long) p->addr;

	if (!ACCESS_ONCE(chan->session->active))
		return;
	lib_ring_buffer_ctx_init(&ctx, chan->chan, NULL, sizeof(data),
				 ltt_alignof(data), -1);
	ret = chan->ops->event_reserve(&ctx);
	if (ret < 0)
		return;
	lib_ring_buffer_align_ctx(&ctx, ltt_alignof(data));
	chan->ops->event_write(&ctx, &data, sizeof(data));
	chan->ops->event_commit(&ctx);
}
EXPORT_SYMBOL_GPL(lttng_kprobes_handler_pre);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Linux Trace Toolkit Kprobes Support");
