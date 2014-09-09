#undef TRACE_SYSTEM
#define TRACE_SYSTEM lttng

#if !defined(LTTNG_TRACE_LTTNG_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_LTTNG_H

#include "../../../probes/lttng-tracepoint-event.h"

LTTNG_TRACEPOINT_EVENT(lttng_logger,
	TP_PROTO(const char __user *text, size_t len),
	TP_ARGS(text, len),
	TP_STRUCT__entry(
		__dynamic_array_text(char, msg, len)
	),
	TP_fast_assign(
		tp_memcpy_dyn_from_user(msg, text)
	),
	TP_printk("")
)

#endif /* LTTNG_TRACE_LTTNG_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
