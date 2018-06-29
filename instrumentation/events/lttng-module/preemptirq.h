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


LTTNG_TRACEPOINT_EVENT_CLASS(preemptirq_template,

	TP_PROTO(unsigned long ip, unsigned long parent_ip),

	TP_ARGS(ip, parent_ip),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, caller, ip)
		ctf_integer_hex(unsigned long, parent, parent_ip)
	)
)

#ifndef CONFIG_PROVE_LOCKING
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
#endif /* !CONFIG_PROVE_LOCKING */

#ifdef CONFIG_DEBUG_PREEMPT
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
#endif /* CONFIG_DEBUG_PREEMPT */

#endif /* LTTNG_TRACE_PREEMPTIRQ_H */

/* This part must be outside protection */
#include <probes/define_trace.h>

#endif /* CONFIG_PREEMPTIRQ_EVENTS */
