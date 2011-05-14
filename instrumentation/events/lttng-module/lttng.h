#undef TRACE_SYSTEM
#define TRACE_SYSTEM lttng

#if !defined(_TRACE_LTTNG_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_LTTNG_H

#include <linux/tracepoint.h>

TRACE_EVENT(lttng_metadata,

	TP_PROTO(const char *str),

	TP_ARGS(str),

	TP_STRUCT__entry(
		__string(	string,		str	)
	),

	TP_fast_assign(
		tp_strcpy(string, str)
	),

	TP_printk("")
)

#endif /*  _TRACE_LTTNG_H */

/* This part must be outside protection */
#include "define_trace.h"
