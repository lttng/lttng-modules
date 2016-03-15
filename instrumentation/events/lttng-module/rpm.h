
#undef TRACE_SYSTEM
#define TRACE_SYSTEM rpm

#if !defined(LTTNG_TRACE_RUNTIME_POWER_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_RUNTIME_POWER_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/ktime.h>

#ifndef _TRACE_RPM_DEF_
#define _TRACE_RPM_DEF_
struct device;
#endif

/*
 * The rpm_internal events are used for tracing some important
 * runtime pm internal functions.
 */
LTTNG_TRACEPOINT_EVENT_CLASS(rpm_internal,

	TP_PROTO(struct device *dev, int flags),

	TP_ARGS(dev, flags),

	TP_FIELDS(
		ctf_string(name, dev_name(dev))
		ctf_integer(int, flags, flags)
		ctf_integer(int, usage_count, atomic_read(&dev->power.usage_count))
		ctf_integer(int, disable_depth, dev->power.disable_depth)
		ctf_integer(int, runtime_auto, dev->power.runtime_auto)
		ctf_integer(int, request_pending, dev->power.request_pending)
		ctf_integer(int, irq_safe, dev->power.irq_safe)
		ctf_integer(int, child_count, atomic_read(&dev->power.child_count))
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpm_internal, rpm_suspend,

	TP_PROTO(struct device *dev, int flags),

	TP_ARGS(dev, flags)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpm_internal, rpm_resume,

	TP_PROTO(struct device *dev, int flags),

	TP_ARGS(dev, flags)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpm_internal, rpm_idle,

	TP_PROTO(struct device *dev, int flags),

	TP_ARGS(dev, flags)
)

LTTNG_TRACEPOINT_EVENT(rpm_return_int,
	TP_PROTO(struct device *dev, unsigned long ip, int ret),
	TP_ARGS(dev, ip, ret),

	TP_FIELDS(
		ctf_string(name, dev_name(dev))
		ctf_integer(unsigned long, ip, ip)
		ctf_integer(int, ret, ret)
	)
)

#endif /* LTTNG_TRACE_RUNTIME_POWER_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
