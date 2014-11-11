#undef TRACE_SYSTEM
#define TRACE_SYSTEM power

#if !defined(LTTNG_TRACE_POWER_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_POWER_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/ktime.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
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
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,5,0))
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
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38))
#undef CONFIG_EVENT_POWER_TRACING_DEPRECATED
#define CONFIG_EVENT_POWER_TRACING_DEPRECATED
#define _PWR_EVENT_AVOID_DOUBLE_DEFINING_DEPRECATED
#endif

#ifdef CONFIG_EVENT_POWER_TRACING_DEPRECATED

/*
 * The power events are used for cpuidle & suspend (power_start, power_end)
 *  and for cpufreq (power_frequency)
 */
LTTNG_TRACEPOINT_EVENT_CLASS(power,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
	TP_PROTO(unsigned int type, unsigned int state, unsigned int cpu_id),

	TP_ARGS(type, state, cpu_id),
#else
	TP_PROTO(unsigned int type, unsigned int state),

	TP_ARGS(type, state),
#endif

	TP_FIELDS(
		ctf_integer(u64, type, type)
		ctf_integer(u64, state, state)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
		ctf_integer(u64, cpu_id, cpu_id)
#endif
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(power, power_start,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
	TP_PROTO(unsigned int type, unsigned int state, unsigned int cpu_id),

	TP_ARGS(type, state, cpu_id)
#else
	TP_PROTO(unsigned int type, unsigned int state),

	TP_ARGS(type, state)
#endif
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(power, power_frequency,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
	TP_PROTO(unsigned int type, unsigned int state, unsigned int cpu_id),

	TP_ARGS(type, state, cpu_id)
#else
	TP_PROTO(unsigned int type, unsigned int state),

	TP_ARGS(type, state)
#endif
)

LTTNG_TRACEPOINT_EVENT(power_end,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
	TP_PROTO(unsigned int cpu_id),

	TP_ARGS(cpu_id),
#else
	TP_PROTO(int dummy),

	TP_ARGS(dummy),
#endif

	TP_FIELDS(
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
		ctf_integer(u64, cpu_id, cpu_id)
#else
		ctf_integer(u64, dummy, 0xffff)
#endif
	)
)

/* Deprecated dummy functions must be protected against multi-declartion */
#ifndef _PWR_EVENT_AVOID_DOUBLE_DEFINING_DEPRECATED
#define _PWR_EVENT_AVOID_DOUBLE_DEFINING_DEPRECATED

enum {
	POWER_NONE = 0,
	POWER_CSTATE = 1,
	POWER_PSTATE = 2,
};
#endif /* _PWR_EVENT_AVOID_DOUBLE_DEFINING_DEPRECATED */

#else /* CONFIG_EVENT_POWER_TRACING_DEPRECATED */

#ifndef _PWR_EVENT_AVOID_DOUBLE_DEFINING_DEPRECATED
#define _PWR_EVENT_AVOID_DOUBLE_DEFINING_DEPRECATED
enum {
       POWER_NONE = 0,
       POWER_CSTATE = 1,
       POWER_PSTATE = 2,
};

/* These dummy declaration have to be ripped out when the deprecated
   events get removed */
static inline void trace_power_start(u64 type, u64 state, u64 cpuid) {};
static inline void trace_power_end(u64 cpuid) {};
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
static inline void trace_power_start_rcuidle(u64 type, u64 state, u64 cpuid) {};
static inline void trace_power_end_rcuidle(u64 cpuid) {};
#endif
static inline void trace_power_frequency(u64 type, u64 state, u64 cpuid) {};
#endif /* _PWR_EVENT_AVOID_DOUBLE_DEFINING_DEPRECATED */

#endif /* CONFIG_EVENT_POWER_TRACING_DEPRECATED */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
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
#endif

#endif /* LTTNG_TRACE_POWER_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
