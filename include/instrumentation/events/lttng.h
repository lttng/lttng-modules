// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only

#undef TRACE_SYSTEM
#define TRACE_SYSTEM lttng

#if !defined(LTTNG_TRACE_LTTNG_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_LTTNG_H

#include <lttng/tracepoint-event.h>

LTTNG_TRACEPOINT_EVENT(lttng_logger,
	TP_PROTO(const char __user *text, size_t len),
	TP_ARGS(text, len),
	TP_FIELDS(
		ctf_user_sequence_text(char, msg, text, size_t, len)
	)
)

#endif /* LTTNG_TRACE_LTTNG_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
