/* SPDX-License-Identifier: GPL-2.0-only */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM power

#if !defined(LTTNG_TRACE_POWER_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_POWER_H

#include <lttng/lttng-tracepoint-event.h>
#include <linux/ktime.h>

LTTNG_TRACEPOINT_EVENT_CLASS(power_cpu,

	TP_PROTO(unsigned int state, unsigned int cpu_id),

	TP_ARGS(state, cpu_id),

	TP_FIELDS(
		ctf_integer(u32, state, state)
		ctf_integer(u32, cpu_id, cpu_id)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(power_cpu, cpu_idle,

	power_cpu_idle,

	TP_PROTO(unsigned int state, unsigned int cpu_id),

	TP_ARGS(state, cpu_id)
)

/* This file can get included multiple times, TRACE_HEADER_MULTI_READ at top */
#ifndef _PWR_EVENT_AVOID_DOUBLE_DEFINING
#define _PWR_EVENT_AVOID_DOUBLE_DEFINING

#define PWR_EVENT_EXIT -1
#endif

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(power_cpu, cpu_frequency,

	power_cpu_frequency,

	TP_PROTO(unsigned int frequency, unsigned int cpu_id),

	TP_ARGS(frequency, cpu_id)
)

LTTNG_TRACEPOINT_EVENT_MAP(machine_suspend,

	power_machine_suspend,

	TP_PROTO(unsigned int state),

	TP_ARGS(state),

	TP_FIELDS(
		ctf_integer(u32, state, state)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(power_wakeup_source,

	TP_PROTO(const char *name, unsigned int state),

	TP_ARGS(name, state),

	TP_FIELDS(
		ctf_string(name, name)
		ctf_integer(u64, state, state)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(power_wakeup_source, wakeup_source_activate,

	power_wakeup_source_activate,

	TP_PROTO(const char *name, unsigned int state),

	TP_ARGS(name, state)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(power_wakeup_source, wakeup_source_deactivate,

	power_wakeup_source_deactivate,

	TP_PROTO(const char *name, unsigned int state),

	TP_ARGS(name, state)
)

/*
 * The clock events are used for clock enable/disable and for
 *  clock rate change
 */
LTTNG_TRACEPOINT_EVENT_CLASS(power_clock,

	TP_PROTO(const char *name, unsigned int state, unsigned int cpu_id),

	TP_ARGS(name, state, cpu_id),

	TP_FIELDS(
		ctf_string(name, name)
		ctf_integer(u64, state, state)
		ctf_integer(u64, cpu_id, cpu_id)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(power_clock, clock_enable,

	power_clock_enable,

	TP_PROTO(const char *name, unsigned int state, unsigned int cpu_id),

	TP_ARGS(name, state, cpu_id)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(power_clock, clock_disable,

	power_clock_disable,

	TP_PROTO(const char *name, unsigned int state, unsigned int cpu_id),

	TP_ARGS(name, state, cpu_id)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(power_clock, clock_set_rate,

	power_clock_set_rate,

	TP_PROTO(const char *name, unsigned int state, unsigned int cpu_id),

	TP_ARGS(name, state, cpu_id)
)

/*
 * The power domain events are used for power domains transitions
 */
LTTNG_TRACEPOINT_EVENT_CLASS(power_domain,

	TP_PROTO(const char *name, unsigned int state, unsigned int cpu_id),

	TP_ARGS(name, state, cpu_id),

	TP_FIELDS(
		ctf_string(name, name)
		ctf_integer(u64, state, state)
		ctf_integer(u64, cpu_id, cpu_id)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(power_domain, power_domain_target,

	TP_PROTO(const char *name, unsigned int state, unsigned int cpu_id),

	TP_ARGS(name, state, cpu_id)
)

#endif /* LTTNG_TRACE_POWER_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
