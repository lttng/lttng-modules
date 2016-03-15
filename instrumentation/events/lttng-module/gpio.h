#undef TRACE_SYSTEM
#define TRACE_SYSTEM gpio

#if !defined(LTTNG_TRACE_GPIO_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_GPIO_H

#include <probes/lttng-tracepoint-event.h>

LTTNG_TRACEPOINT_EVENT(gpio_direction,

	TP_PROTO(unsigned gpio, int in, int err),

	TP_ARGS(gpio, in, err),

	TP_FIELDS(
		ctf_integer(unsigned, gpio, gpio)
		ctf_integer(int, in, in)
		ctf_integer(int, err, err)
	)
)

LTTNG_TRACEPOINT_EVENT(gpio_value,

	TP_PROTO(unsigned gpio, int get, int value),

	TP_ARGS(gpio, get, value),

	TP_FIELDS(
		ctf_integer(unsigned, gpio, gpio)
		ctf_integer(int, get, get)
		ctf_integer(int, value, value)
	)
)

#endif /* if !defined(LTTNG_TRACE_GPIO_H) || defined(TRACE_HEADER_MULTI_READ) */

/* This part must be outside protection */
#include <probes/define_trace.h>
