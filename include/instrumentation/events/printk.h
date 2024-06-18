// SPDX-FileCopyrightText: 2012 Andrew Gabbasov <andrew_gabbasov@mentor.com>
//
// SPDX-License-Identifier: GPL-2.0-only

#undef TRACE_SYSTEM
#define TRACE_SYSTEM printk

#if !defined(LTTNG_TRACE_PRINTK_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_PRINTK_H

#include <lttng/tracepoint-event.h>
#include <lttng/kernel-version.h>

LTTNG_TRACEPOINT_EVENT_MAP(console,

	printk_console,

	TP_PROTO(const char *text, size_t len),

	TP_ARGS(text, len),

	TP_FIELDS(
		ctf_sequence_text(char, msg, text, size_t, len)
	)
)

#endif /* LTTNG_TRACE_PRINTK_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
