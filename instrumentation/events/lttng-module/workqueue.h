/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM workqueue

#if !defined(LTTNG_TRACE_WORKQUEUE_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_WORKQUEUE_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/workqueue.h>
#include <linux/version.h>

#ifndef _TRACE_WORKQUEUE_DEF_
#define _TRACE_WORKQUEUE_DEF_

struct worker;
struct global_cwq;

#endif

LTTNG_TRACEPOINT_EVENT_CLASS(workqueue_work,

	TP_PROTO(struct work_struct *work),

	TP_ARGS(work),

	TP_FIELDS(
		ctf_integer_hex(void *, work, work)
	)
)

/**
 * workqueue_queue_work - called when a work gets queued
 * @req_cpu:	the requested cpu
 * @cwq:	pointer to struct cpu_workqueue_struct
 * @work:	pointer to struct work_struct
 *
 * This event occurs when a work is queued immediately or once a
 * delayed work is actually queued on a workqueue (ie: once the delay
 * has been reached).
 */
LTTNG_TRACEPOINT_EVENT(workqueue_queue_work,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
	TP_PROTO(unsigned int req_cpu, struct pool_workqueue *pwq,
		 struct work_struct *work),

	TP_ARGS(req_cpu, pwq, work),
#else
	TP_PROTO(unsigned int req_cpu, struct cpu_workqueue_struct *cwq,
		 struct work_struct *work),

	TP_ARGS(req_cpu, cwq, work),
#endif

	TP_FIELDS(
		ctf_integer_hex(void *, work, work)
		ctf_integer_hex(void *, function, work->func)
		ctf_integer(unsigned int, req_cpu, req_cpu)
	)
)

/**
 * workqueue_activate_work - called when a work gets activated
 * @work:	pointer to struct work_struct
 *
 * This event occurs when a queued work is put on the active queue,
 * which happens immediately after queueing unless @max_active limit
 * is reached.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(workqueue_work, workqueue_activate_work,

	TP_PROTO(struct work_struct *work),

	TP_ARGS(work)
)

/**
 * workqueue_execute_start - called immediately before the workqueue callback
 * @work:	pointer to struct work_struct
 *
 * Allows to track workqueue execution.
 */
LTTNG_TRACEPOINT_EVENT(workqueue_execute_start,

	TP_PROTO(struct work_struct *work),

	TP_ARGS(work),

	TP_FIELDS(
		ctf_integer_hex(void *, work, work)
		ctf_integer_hex(void *, function, work->func)
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0))
/**
 * workqueue_execute_end - called immediately after the workqueue callback
 * @work:	pointer to struct work_struct
 * @function:	pointer to worker function
 *
 * Allows to track workqueue execution.
 */
LTTNG_TRACEPOINT_EVENT(workqueue_execute_end,

	TP_PROTO(struct work_struct *work, work_func_t function),

	TP_ARGS(work, function),

	TP_FIELDS(
		ctf_integer_hex(void *, work, work)
		ctf_integer_hex(void *, function, function)
	)
)
#else
/**
 * workqueue_execute_end - called immediately after the workqueue callback
 * @work:	pointer to struct work_struct
 *
 * Allows to track workqueue execution.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(workqueue_work, workqueue_execute_end,

	TP_PROTO(struct work_struct *work),

	TP_ARGS(work)
)
#endif

#endif /*  LTTNG_TRACE_WORKQUEUE_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
