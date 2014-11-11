#undef TRACE_SYSTEM
#define TRACE_SYSTEM workqueue

#if !defined(LTTNG_TRACE_WORKQUEUE_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_WORKQUEUE_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/workqueue.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))

#ifndef _TRACE_WORKQUEUE_DEF_
#define _TRACE_WORKQUEUE_DEF_

struct worker;
struct global_cwq;

#endif

LTTNG_TRACEPOINT_EVENT_CLASS(workqueue_work,

	TP_PROTO(struct work_struct *work),

	TP_ARGS(work),

	TP_FIELDS(
		ctf_integer(void *, work, work)
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
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
		ctf_integer(void *, work, work)
		ctf_integer(void *, function, work->func)
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
#endif

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
		ctf_integer(void *, work, work)
		ctf_integer(void *, function, work->func)
	)
)

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

#else

LTTNG_TRACEPOINT_EVENT_CLASS(workqueue,

	TP_PROTO(struct task_struct *wq_thread, struct work_struct *work),

	TP_ARGS(wq_thread, work),

	TP_FIELDS(
		ctf_array(char, thread_comm, wq_thread->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, thread_pid, wq_thread->pid)
		ctf_integer(work_func_t, func, work->func)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(workqueue, workqueue_insertion,

	TP_PROTO(struct task_struct *wq_thread, struct work_struct *work),

	TP_ARGS(wq_thread, work)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(workqueue, workqueue_execution,

	TP_PROTO(struct task_struct *wq_thread, struct work_struct *work),

	TP_ARGS(wq_thread, work)
)

/* Trace the creation of one workqueue thread on a cpu */
LTTNG_TRACEPOINT_EVENT(workqueue_creation,

	TP_PROTO(struct task_struct *wq_thread, int cpu),

	TP_ARGS(wq_thread, cpu),

	TP_FIELDS(
		ctf_array(char, thread_comm, wq_thread->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, thread_pid, wq_thread->pid)
		ctf_integer(int, cpu, cpu)
	)
)

LTTNG_TRACEPOINT_EVENT(workqueue_destruction,

	TP_PROTO(struct task_struct *wq_thread),

	TP_ARGS(wq_thread),

	TP_FIELDS(
		ctf_array(char, thread_comm, wq_thread->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, thread_pid, wq_thread->pid)
	)
)

#endif

#endif /*  LTTNG_TRACE_WORKQUEUE_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
