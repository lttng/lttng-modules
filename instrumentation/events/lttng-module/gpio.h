#undef TRACE_SYSTEM
#define TRACE_SYSTEM gpio

#if !defined(LTTNG_TRACE_GPIO_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_GPIO_H

#include "../../../probes/lttng-tracepoint-event.h"

LTTNG_TRACEPOINT_EVENT(gpio_direction,

	TP_PROTO(unsigned gpio, int in, int err),

	TP_ARGS(gpio, in, err),

	TP_STRUCT__entry(
		__field(unsigned, gpio)
		__field(int, in)
		__field(int, err)
	),

	TP_fast_assign(
		tp_assign(gpio, gpio)
		tp_assign(in, in)
		tp_assign(err, err)
	),

	TP_printk("%u %3s (%d)", __entry->gpio,
		__entry->in ? "in" : "out", __entry->err)
)

LTTNG_TRACEPOINT_EVENT(gpio_value,

	TP_PROTO(unsigned gpio, int get, int value),

	TP_ARGS(gpio, get, value),

	TP_STRUCT__entry(
		__field(unsigned, gpio)
		__field(int, get)
		__field(int, value)
	),

	TP_fast_assign(
		tp_assign(gpio, gpio)
		tp_assign(get, get)
		tp_assign(value, value)
	),

	TP_printk("%u %3s %d", __entry->gpio,
		__entry->get ? "get" : "set", __entry->value)
)

#endif /* if !defined(LTTNG_TRACE_GPIO_H) || defined(TRACE_HEADER_MULTI_READ) */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
