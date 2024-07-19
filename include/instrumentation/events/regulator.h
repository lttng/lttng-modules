// SPDX-FileCopyrightText: 2012 Paul Woegerer <paul_woegerer@mentor.com>
//
// SPDX-License-Identifier: GPL-2.0-only

#undef TRACE_SYSTEM
#define TRACE_SYSTEM regulator

#if !defined(LTTNG_TRACE_REGULATOR_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_REGULATOR_H

#include <lttng/tracepoint-event.h>
#include <linux/ktime.h>

/*
 * Events which just log themselves and the regulator name for enable/disable
 * type tracking.
 */
LTTNG_TRACEPOINT_EVENT_CLASS(regulator_basic,

	TP_PROTO(const char *name),

	TP_ARGS(name),

	TP_FIELDS(
		ctf_string(name, name)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regulator_basic, regulator_enable,

	TP_PROTO(const char *name),

	TP_ARGS(name)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regulator_basic, regulator_enable_delay,

	TP_PROTO(const char *name),

	TP_ARGS(name)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regulator_basic, regulator_enable_complete,

	TP_PROTO(const char *name),

	TP_ARGS(name)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regulator_basic, regulator_disable,

	TP_PROTO(const char *name),

	TP_ARGS(name)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regulator_basic, regulator_disable_complete,

	TP_PROTO(const char *name),

	TP_ARGS(name)

)

/*
 * Events that take a range of numerical values, mostly for voltages
 * and so on.
 */
LTTNG_TRACEPOINT_EVENT_CLASS(regulator_range,

	TP_PROTO(const char *name, int min, int max),

	TP_ARGS(name, min, max),

	TP_FIELDS(
		ctf_string(name, name)
		ctf_integer(int, min, min)
		ctf_integer(int, max, max)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regulator_range, regulator_set_voltage,

	TP_PROTO(const char *name, int min, int max),

	TP_ARGS(name, min, max)

)


/*
 * Events that take a single value, mostly for readback and refcounts.
 */
LTTNG_TRACEPOINT_EVENT_CLASS(regulator_value,

	TP_PROTO(const char *name, unsigned int val),

	TP_ARGS(name, val),

	TP_FIELDS(
		ctf_string(name, name)
		ctf_integer(unsigned int, val, val)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(regulator_value, regulator_set_voltage_complete,

	TP_PROTO(const char *name, unsigned int value),

	TP_ARGS(name, value)

)

#endif /* _TRACE_POWER_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
