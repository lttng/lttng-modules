/*
 * ltt/ltt-event-header.c
 *
 * (C) Copyright 2010 - Mathieu Desnoyers (mathieu.desnoyers@efficios.com)
 *
 * LTTng event header.
 *
 * Author:
 *	Mathieu Desnoyers (mathieu.desnoyers@efficios.com)
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include "ltt-tracer.h"

void ltt_write_event_header_slow(const struct lib_ring_buffer_config *config,
				   struct lib_ring_buffer_ctx *ctx,
				   u16 eID, u32 event_size)
{
	struct event_header header;
	u16 small_size;

	switch (ctx->rflags) {
	case LTT_RFLAG_ID_SIZE_TSC:
		header.id_time = 29 << LTT_TSC_BITS;
		break;
	case LTT_RFLAG_ID_SIZE:
		header.id_time = 30 << LTT_TSC_BITS;
		break;
	case LTT_RFLAG_ID:
		header.id_time = 31 << LTT_TSC_BITS;
		break;
	default:
		WARN_ON_ONCE(1);
		header.id_time = 0;
	}

	header.id_time |= (u32)ctx->tsc & LTT_TSC_MASK;
	lib_ring_buffer_write(config, ctx, &header, sizeof(header));

	switch (ctx->rflags) {
	case LTT_RFLAG_ID_SIZE_TSC:
		small_size = (u16)min_t(u32, event_size, LTT_MAX_SMALL_SIZE);
		lib_ring_buffer_write(config, ctx, &eID, sizeof(u16));
		lib_ring_buffer_write(config, ctx, &small_size, sizeof(u16));
		if (small_size == LTT_MAX_SMALL_SIZE)
			lib_ring_buffer_write(config, ctx, &event_size,
					      sizeof(u32));
		lib_ring_buffer_align_ctx(ctx, sizeof(u64));
		lib_ring_buffer_write(config, ctx, &ctx->tsc, sizeof(u64));
		break;
	case LTT_RFLAG_ID_SIZE:
		small_size = (u16)min_t(u32, event_size, LTT_MAX_SMALL_SIZE);
		lib_ring_buffer_write(config, ctx, &eID, sizeof(u16));
		lib_ring_buffer_write(config, ctx, &small_size, sizeof(u16));
		if (small_size == LTT_MAX_SMALL_SIZE)
			lib_ring_buffer_write(config, ctx, &event_size,
					      sizeof(u32));
		break;
	case LTT_RFLAG_ID:
		lib_ring_buffer_write(config, ctx, &eID, sizeof(u16));
		break;
	}
}
EXPORT_SYMBOL_GPL(ltt_write_event_header_slow);
