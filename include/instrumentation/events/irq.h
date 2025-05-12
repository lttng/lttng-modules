// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only

#undef TRACE_SYSTEM
#define TRACE_SYSTEM irq

#if !defined(LTTNG_TRACE_IRQ_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_IRQ_H

#include <lttng/tracepoint-event.h>

#ifndef _TRACE_IRQ_DEF_
#define _TRACE_IRQ_DEF_

struct irqaction;
struct softirq_action;

#endif /* _TRACE_IRQ_DEF_ */

/**
 * irq_handler_entry - called immediately before the irq action handler
 * @irq: irq number
 * @action: pointer to struct irqaction
 *
 * The struct irqaction pointed to by @action contains various
 * information about the handler, including the device name,
 * @action->name, and the device id, @action->dev_id. When used in
 * conjunction with the irq_handler_exit tracepoint, we can figure
 * out irq handler latencies.
 */
LTTNG_TRACEPOINT_EVENT(irq_handler_entry,

	TP_PROTO(int irq, struct irqaction *action),

	TP_ARGS(irq, action),

	TP_FIELDS(
		ctf_integer(int, irq, irq)
		ctf_string(name, action->name)
	)
)

/**
 * irq_handler_exit - called immediately after the irq action handler returns
 * @irq: irq number
 * @action: pointer to struct irqaction
 * @ret: return value
 *
 * If the @ret value is set to IRQ_HANDLED, then we know that the corresponding
 * @action->handler scuccessully handled this irq. Otherwise, the irq might be
 * a shared irq line, or the irq was not handled successfully. Can be used in
 * conjunction with the irq_handler_entry to understand irq handler latencies.
 */
LTTNG_TRACEPOINT_EVENT(irq_handler_exit,

	TP_PROTO(int irq, struct irqaction *action, int ret),

	TP_ARGS(irq, action, ret),

	TP_FIELDS(
		ctf_integer(int, irq, irq)
		ctf_integer(int, ret, ret)
	)
)

LTTNG_TRACEPOINT_ENUM(irq_softirq_names,

	TP_ENUM_VALUES(
		ctf_enum_value("HI", HI_SOFTIRQ)
		ctf_enum_value("TIMER", TIMER_SOFTIRQ)
		ctf_enum_value("NET_TX", NET_TX_SOFTIRQ)
		ctf_enum_value("NET_RX", NET_RX_SOFTIRQ)
		ctf_enum_value("BLOCK", BLOCK_SOFTIRQ)
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,5,0))
		ctf_enum_value("IRQ_POLL", IRQ_POLL_SOFTIRQ)
#else
		ctf_enum_value("BLOCK_IOPOLL", BLOCK_IOPOLL_SOFTIRQ)
#endif
		ctf_enum_value("TASKLET", TASKLET_SOFTIRQ)
		ctf_enum_value("SCHED", SCHED_SOFTIRQ)
		ctf_enum_value("HRTIMER", HRTIMER_SOFTIRQ)
		ctf_enum_value("RCU", RCU_SOFTIRQ)
	)
)

#ifdef LTTNG_TRACEPOINT_ENUM_GENERATE
static_assert(ARRAY_SIZE(__enum_values__irq_softirq_names) == NR_SOFTIRQS);
#endif

LTTNG_TRACEPOINT_EVENT_CLASS(irq_softirq,

	TP_PROTO(unsigned int vec_nr),

	TP_ARGS(vec_nr),

	TP_FIELDS(
		ctf_integer(u8, vec, vec_nr)
		ctf_enum(irq_softirq_names, u8, name, vec_nr)
	)
)

/**
 * softirq_entry - called immediately before the softirq handler
 * @vec_nr:  softirq vector number
 *
 * When used in combination with the softirq_exit tracepoint
 * we can determine the softirq handler runtine.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(irq_softirq, softirq_entry,

	irq_softirq_entry,

	TP_PROTO(unsigned int vec_nr),

	TP_ARGS(vec_nr)
)

/**
 * softirq_exit - called immediately after the softirq handler returns
 * @vec_nr:  softirq vector number
 *
 * When used in combination with the softirq_entry tracepoint
 * we can determine the softirq handler runtine.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(irq_softirq, softirq_exit,

	irq_softirq_exit,

	TP_PROTO(unsigned int vec_nr),

	TP_ARGS(vec_nr)
)

/**
 * softirq_raise - called immediately when a softirq is raised
 * @vec_nr:  softirq vector number
 *
 * When used in combination with the softirq_entry tracepoint
 * we can determine the softirq raise to run latency.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(irq_softirq, softirq_raise,

	irq_softirq_raise,

	TP_PROTO(unsigned int vec_nr),

	TP_ARGS(vec_nr)
)

#endif /*  LTTNG_TRACE_IRQ_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
