#if !defined(LTTNG_TRACE_IRQ_VECTORS_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_IRQ_VECTORS_H

#include "../../../../../probes/lttng-tracepoint-event.h"
#include <linux/version.h>

#undef TRACE_SYSTEM
#define TRACE_SYSTEM x86_irq_vectors

LTTNG_TRACEPOINT_EVENT_CLASS(x86_irq_vectors_vector_class,
	TP_PROTO(int vector),
	TP_ARGS(vector),

	TP_FIELDS(
		ctf_integer(int, vector, vector)
	)
)

#define LTTNG_IRQ_VECTOR_TRACEPOINT_EVENT_INSTANCE_MAP(name, map)	\
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(x86_irq_vectors_vector_class,	\
	name##_entry,							\
	map##_entry,							\
	TP_PROTO(int vector),						\
	TP_ARGS(vector)							\
)									\
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(x86_irq_vectors_vector_class,	\
	name##_exit,							\
	map##_exit,							\
	TP_PROTO(int vector),						\
	TP_ARGS(vector)							\
)

/*
 * local_timer - called when entering/exiting a local timer interrupt
 * vector handler
 */
LTTNG_IRQ_VECTOR_TRACEPOINT_EVENT_INSTANCE_MAP(local_timer,
		x86_irq_vectors_local_timer)

/*
 * reschedule - called when entering/exiting a reschedule vector handler
 */
LTTNG_IRQ_VECTOR_TRACEPOINT_EVENT_INSTANCE_MAP(reschedule,
		x86_irq_vectors_reschedule)

/*
 * spurious_apic - called when entering/exiting a spurious apic vector handler
 */
LTTNG_IRQ_VECTOR_TRACEPOINT_EVENT_INSTANCE_MAP(spurious_apic,
		x86_irq_vectors_spurious_apic)

/*
 * error_apic - called when entering/exiting an error apic vector handler
 */
LTTNG_IRQ_VECTOR_TRACEPOINT_EVENT_INSTANCE_MAP(error_apic,
		x86_irq_vectors_error_apic)

/*
 * x86_platform_ipi - called when entering/exiting a x86 platform ipi interrupt
 * vector handler
 */
LTTNG_IRQ_VECTOR_TRACEPOINT_EVENT_INSTANCE_MAP(x86_platform_ipi,
		x86_irq_vectors_ipi)

/*
 * irq_work - called when entering/exiting a irq work interrupt
 * vector handler
 */
LTTNG_IRQ_VECTOR_TRACEPOINT_EVENT_INSTANCE_MAP(irq_work,
		x86_irq_vectors_irq_work)

/*
 * call_function - called when entering/exiting a call function interrupt
 * vector handler
 */
LTTNG_IRQ_VECTOR_TRACEPOINT_EVENT_INSTANCE_MAP(call_function,
		x86_irq_vectors_call_function)

/*
 * call_function_single - called when entering/exiting a call function
 * single interrupt vector handler
 */
LTTNG_IRQ_VECTOR_TRACEPOINT_EVENT_INSTANCE_MAP(call_function_single,
		x86_irq_vectors_call_function_single)

/*
 * threshold_apic - called when entering/exiting a threshold apic interrupt
 * vector handler
 */
LTTNG_IRQ_VECTOR_TRACEPOINT_EVENT_INSTANCE_MAP(threshold_apic,
		x86_irq_vectors_threshold_apic)

/*
 * deferred_error_apic - called when entering/exiting a deferred apic interrupt
 * vector handler
 */
LTTNG_IRQ_VECTOR_TRACEPOINT_EVENT_INSTANCE_MAP(deferred_error_apic,
		x86_irq_vectors_deferred_error_apic)

/*
 * thermal_apic - called when entering/exiting a thermal apic interrupt
 * vector handler
 */
LTTNG_IRQ_VECTOR_TRACEPOINT_EVENT_INSTANCE_MAP(thermal_apic,
		x86_irq_vectors_thermal_apic)

#endif /* LTTNG_TRACE_IRQ_VECTORS_H */

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH ../instrumentation/events/lttng-module/arch/x86
#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_FILE irq_vectors

/* This part must be outside protection */
#include "../../../../../probes/define_trace.h"
