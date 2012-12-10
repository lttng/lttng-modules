#undef TRACE_SYSTEM
#define TRACE_SYSTEM workqueue

#if !defined(_TRACE_WORKQUEUE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_WORKQUEUE_H

#include <linux/tracepoint.h>
#include <linux/workqueue.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))

#ifndef _TRACE_WORKQUEUE_DEF_
#define _TRACE_WORKQUEUE_DEF_

struct worker;
struct global_cwq;

/* Have to duplicate all these definitions from kernel/workqueue.c */
/* Extracts only */

enum {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
	NR_WORKER_POOLS		= 2,		/* # worker pools per gcwq */
#endif
	BUSY_WORKER_HASH_ORDER 	= 6,		/* 64 pointers */
	BUSY_WORKER_HASH_SIZE	= 1 << BUSY_WORKER_HASH_ORDER,
};

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
struct worker_pool {
	struct global_cwq	*gcwq;		/* I: the owning gcwq */
	unsigned int		flags;		/* X: flags */

	struct list_head	worklist;	/* L: list of pending works */
	int			nr_workers;	/* L: total number of workers */
	int			nr_idle;	/* L: currently idle ones */

	struct list_head	idle_list;	/* X: list of idle workers */
	struct timer_list	idle_timer;	/* L: worker idle timeout */
	struct timer_list	mayday_timer;	/* L: SOS timer for workers */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0))
	struct mutex		assoc_mutex;	/* protect GCWQ_DISASSOCUATED */
#else
	struct mutex		manager_mutex;	/* mutex manager should hold */
#endif
	struct ida		worker_ida;	/* L: for worker IDs */
};

struct global_cwq {
	spinlock_t		lock;		/* the gcwq lock */
	unsigned int		cpu;		/* I: the associated cpu */
	unsigned int		flags;		/* L: GCWQ_* flags */

	/* workers are chained either in busy_hash or pool idle_list */
	struct hlist_head	busy_hash[BUSY_WORKER_HASH_SIZE];
						/* L: hash of busy workers */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0))
	struct worker_pool	pools[NR_WORKER_POOLS];
						/* normal and highpri pools */
#else
	struct worker_pool	pools[2];	/* normal and highpri pools */

	wait_queue_head_t	rebind_hold;	/* rebind hold wait */
#endif
} ____cacheline_aligned_in_smp;

#else

struct global_cwq {
	spinlock_t		lock;		/* the gcwq lock */
	struct list_head	worklist;	/* L: list of pending works */
	unsigned int		cpu;		/* I: the associated cpu */
	unsigned int		flags;		/* L: GCWQ_* flags */

	int			nr_workers;	/* L: total number of workers */
	int			nr_idle;	/* L: currently idle ones */

	/* workers are chained either in the idle_list or busy_hash */
	struct list_head	idle_list;	/* X: list of idle workers */
	struct hlist_head	busy_hash[BUSY_WORKER_HASH_SIZE];
						/* L: hash of busy workers */

	struct timer_list	idle_timer;	/* L: worker idle timeout */
	struct timer_list	mayday_timer;	/* L: SOS timer for dworkers */

	struct ida		worker_ida;	/* L: for worker IDs */

	struct task_struct	*trustee;	/* L: for gcwq shutdown */
	unsigned int		trustee_state;	/* L: trustee state */
	wait_queue_head_t	trustee_wait;	/* trustee wait */
	struct worker		*first_idle;	/* L: first idle worker */
} ____cacheline_aligned_in_smp;

#endif

struct cpu_workqueue_struct {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
	struct worker_pool	*pool;		/* I: The associated pool */
#else
	struct global_cwq	*gcwq;		/* I: the associated gcwq */
#endif
	struct workqueue_struct	*wq;		/* I: the owning workqueue */
	int			work_color;	/* L: current color */
	int			flush_color;	/* L: flushing color */
	int			nr_in_flight[WORK_NR_COLORS];
						/* L: nr of in_flight works */
	int			nr_active;	/* L: nr of active works */
	int			max_active;	/* L: max active works */
	struct list_head	delayed_works;	/* L: delayed works */
};

#endif

DECLARE_EVENT_CLASS(workqueue_work,

	TP_PROTO(struct work_struct *work),

	TP_ARGS(work),

	TP_STRUCT__entry(
		__field( void *,	work	)
	),

	TP_fast_assign(
		tp_assign(work, work)
	),

	TP_printk("work struct %p", __entry->work)
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
TRACE_EVENT(workqueue_queue_work,

	TP_PROTO(unsigned int req_cpu, struct cpu_workqueue_struct *cwq,
		 struct work_struct *work),

	TP_ARGS(req_cpu, cwq, work),

	TP_STRUCT__entry(
		__field( void *,	work	)
		__field( void *,	function)
		__field( void *,	workqueue)
		__field( unsigned int,	req_cpu	)
		__field( unsigned int,	cpu	)
	),

	TP_fast_assign(
		tp_assign(work, work)
		tp_assign(function, work->func)
		tp_assign(workqueue, cwq->wq)
		tp_assign(req_cpu, req_cpu)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
		tp_assign(cpu, cwq->pool->gcwq->cpu)
#else
		tp_assign(cpu, cwq->gcwq->cpu)
#endif
	),

	TP_printk("work struct=%p function=%pf workqueue=%p req_cpu=%u cpu=%u",
		  __entry->work, __entry->function, __entry->workqueue,
		  __entry->req_cpu, __entry->cpu)
)

/**
 * workqueue_activate_work - called when a work gets activated
 * @work:	pointer to struct work_struct
 *
 * This event occurs when a queued work is put on the active queue,
 * which happens immediately after queueing unless @max_active limit
 * is reached.
 */
DEFINE_EVENT(workqueue_work, workqueue_activate_work,

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
TRACE_EVENT(workqueue_execute_start,

	TP_PROTO(struct work_struct *work),

	TP_ARGS(work),

	TP_STRUCT__entry(
		__field( void *,	work	)
		__field( void *,	function)
	),

	TP_fast_assign(
		tp_assign(work, work)
		tp_assign(function, work->func)
	),

	TP_printk("work struct %p: function %pf", __entry->work, __entry->function)
)

/**
 * workqueue_execute_end - called immediately after the workqueue callback
 * @work:	pointer to struct work_struct
 *
 * Allows to track workqueue execution.
 */
DEFINE_EVENT(workqueue_work, workqueue_execute_end,

	TP_PROTO(struct work_struct *work),

	TP_ARGS(work)
)

#else

DECLARE_EVENT_CLASS(workqueue,

	TP_PROTO(struct task_struct *wq_thread, struct work_struct *work),

	TP_ARGS(wq_thread, work),

	TP_STRUCT__entry(
		__array(char,		thread_comm,	TASK_COMM_LEN)
		__field(pid_t,		thread_pid)
		__field(work_func_t,	func)
	),

	TP_fast_assign(
		tp_memcpy(thread_comm, wq_thread->comm, TASK_COMM_LEN)
		tp_assign(thread_pid, wq_thread->pid)
		tp_assign(func, work->func)
	),

	TP_printk("thread=%s:%d func=%pf", __entry->thread_comm,
		__entry->thread_pid, __entry->func)
)

DEFINE_EVENT(workqueue, workqueue_insertion,

	TP_PROTO(struct task_struct *wq_thread, struct work_struct *work),

	TP_ARGS(wq_thread, work)
)

DEFINE_EVENT(workqueue, workqueue_execution,

	TP_PROTO(struct task_struct *wq_thread, struct work_struct *work),

	TP_ARGS(wq_thread, work)
)

/* Trace the creation of one workqueue thread on a cpu */
TRACE_EVENT(workqueue_creation,

	TP_PROTO(struct task_struct *wq_thread, int cpu),

	TP_ARGS(wq_thread, cpu),

	TP_STRUCT__entry(
		__array(char,	thread_comm,	TASK_COMM_LEN)
		__field(pid_t,	thread_pid)
		__field(int,	cpu)
	),

	TP_fast_assign(
		tp_memcpy(thread_comm, wq_thread->comm, TASK_COMM_LEN)
		tp_assign(thread_pid, wq_thread->pid)
		tp_assign(cpu, cpu)
	),

	TP_printk("thread=%s:%d cpu=%d", __entry->thread_comm,
		__entry->thread_pid, __entry->cpu)
)

TRACE_EVENT(workqueue_destruction,

	TP_PROTO(struct task_struct *wq_thread),

	TP_ARGS(wq_thread),

	TP_STRUCT__entry(
		__array(char,	thread_comm,	TASK_COMM_LEN)
		__field(pid_t,	thread_pid)
	),

	TP_fast_assign(
		tp_memcpy(thread_comm, wq_thread->comm, TASK_COMM_LEN)
		tp_assign(thread_pid, wq_thread->pid)
	),

	TP_printk("thread=%s:%d", __entry->thread_comm, __entry->thread_pid)
)

#endif

#endif /*  _TRACE_WORKQUEUE_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
