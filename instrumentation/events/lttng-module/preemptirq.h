/* SPDX-License-Identifier: GPL-2.0 */
#ifdef CONFIG_PREEMPTIRQ_EVENTS

#undef TRACE_SYSTEM
#define TRACE_SYSTEM preemptirq

#if !defined(LTTNG_TRACE_PREEMPTIRQ_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_PREEMPTIRQ_H

#include <linux/ktime.h>
#include <linux/string.h>
#include <asm/sections.h>
#include <probes/lttng-tracepoint-event.h>

/*
 * The preemptirq probe is built when CONFIG_PREEMPTIRQ_EVENTS is defined.
 */

LTTNG_TRACEPOINT_EVENT_CLASS(preemptirq_template,

	TP_PROTO(unsigned long ip, unsigned long parent_ip),

	TP_ARGS(ip, parent_ip),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, caller, ip)
		ctf_integer_hex(unsigned long, parent, parent_ip)
	)
)

/*
 * Tracing of irq enable / disable events is enabled
 *   on >= 4.19 when CONFIG_TRACE_IRQFLAGS is defined.
 *   on previous kernels when CONFIG_PROVE_LOCKING is NOT defined.
 */
#if defined(CONFIG_TRACE_IRQFLAGS)
#define LTTNG_TRACE_IRQ
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(4,19,0) && \
	!defined(CONFIG_PROVE_LOCKING))
#define LTTNG_TRACE_IRQ
#endif

#ifdef LTTNG_TRACE_IRQ
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(preemptirq_template, irq_disable,

	preemptirq_irq_disable,

	TP_PROTO(unsigned long ip, unsigned long parent_ip),

	TP_ARGS(ip, parent_ip)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(preemptirq_template, irq_enable,

	preemptirq_irq_enable,

	TP_PROTO(unsigned long ip, unsigned long parent_ip),

	TP_ARGS(ip, parent_ip)
)
#endif /* LTTNG_TRACE_IRQ */

/*
 * Tracing of preempt enable / disable events is enabled
 *   on >= 4.19 when CONFIG_TRACE_PREEMPT_TOGGLE is defined.
 *   on previous kernels when CONFIG_DEBUG_PREEMPT is defined.
 */
#if defined(CONFIG_TRACE_PREEMPT_TOGGLE)
#define LTTNG_TRACE_PREEMPT
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(4,19,0) && \
	defined(CONFIG_DEBUG_PREEMPT))
#define LTTNG_TRACE_PREEMPT
#endif

#ifdef LTTNG_TRACE_PREEMPT
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(preemptirq_template, preempt_disable,

	preemptirq_preempt_disable,

	TP_PROTO(unsigned long ip, unsigned long parent_ip),

	TP_ARGS(ip, parent_ip)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(preemptirq_template, preempt_enable,

	preemptirq_preempt_enable,

	TP_PROTO(unsigned long ip, unsigned long parent_ip),

	TP_ARGS(ip, parent_ip)
)
#endif /* LTTNG_TRACE_PREEMPT */

#endif /* LTTNG_TRACE_PREEMPTIRQ_H */

/* This part must be outside protection */
#include <probes/define_trace.h>

#endif /* CONFIG_PREEMPTIRQ_EVENTS */
