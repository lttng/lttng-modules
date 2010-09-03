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
#include "ltt-relay.h"

size_t ltt_write_event_header_slow(struct ltt_chanbuf_alloc *bufa,
				   struct ltt_chan_alloc *chana,
				   long buf_offset, u16 eID, u32 event_size,
				   u64 tsc, unsigned int rflags)
{
	struct ltt_event_header header;
	u16 small_size;

	switch (rflags) {
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

	header.id_time |= (u32)tsc & LTT_TSC_MASK;
	ltt_relay_write(bufa, chana, buf_offset, &header, sizeof(header));
	buf_offset += sizeof(header);

	switch (rflags) {
	case LTT_RFLAG_ID_SIZE_TSC:
		small_size = (u16)min_t(u32, event_size, LTT_MAX_SMALL_SIZE);
		ltt_relay_write(bufa, chana, buf_offset,
			&eID, sizeof(u16));
		buf_offset += sizeof(u16);
		ltt_relay_write(bufa, chana, buf_offset,
			&small_size, sizeof(u16));
		buf_offset += sizeof(u16);
		if (small_size == LTT_MAX_SMALL_SIZE) {
			ltt_relay_write(bufa, chana, buf_offset,
				&event_size, sizeof(u32));
			buf_offset += sizeof(u32);
		}
		buf_offset += ltt_align(buf_offset, sizeof(u64));
		ltt_relay_write(bufa, chana, buf_offset,
			&tsc, sizeof(u64));
		buf_offset += sizeof(u64);
		break;
	case LTT_RFLAG_ID_SIZE:
		small_size = (u16)min_t(u32, event_size, LTT_MAX_SMALL_SIZE);
		ltt_relay_write(bufa, chana, buf_offset,
			&eID, sizeof(u16));
		buf_offset += sizeof(u16);
		ltt_relay_write(bufa, chana, buf_offset,
			&small_size, sizeof(u16));
		buf_offset += sizeof(u16);
		if (small_size == LTT_MAX_SMALL_SIZE) {
			ltt_relay_write(bufa, chana, buf_offset,
				&event_size, sizeof(u32));
			buf_offset += sizeof(u32);
		}
		break;
	case LTT_RFLAG_ID:
		ltt_relay_write(bufa, chana, buf_offset,
			&eID, sizeof(u16));
		buf_offset += sizeof(u16);
		break;
	}

	return buf_offset;
}
EXPORT_SYMBOL_GPL(ltt_write_event_header_slow);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Linux Trace Toolkit Next Generation Event Header");
