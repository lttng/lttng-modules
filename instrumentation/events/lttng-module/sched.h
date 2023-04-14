/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM sched

#if !defined(LTTNG_TRACE_SCHED_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_SCHED_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/sched.h>
#include <linux/pid_namespace.h>
#include <linux/binfmts.h>
#include <lttng-kernel-version.h>
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,9,0))
#include <linux/sched/rt.h>
#endif
#include <wrapper/namespace.h>

#define LTTNG_MAX_PID_NS_LEVEL 32

#ifndef _TRACE_SCHED_DEF_
#define _TRACE_SCHED_DEF_

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,18,0) \
	|| LTTNG_RHEL_KERNEL_RANGE(5,14,0,162,0,0, 5,15,0,0,0,0))

static inline long __trace_sched_switch_state(bool preempt,
		unsigned int prev_state,
		struct task_struct *p)
{
        unsigned int state;

#ifdef CONFIG_SCHED_DEBUG
        BUG_ON(p != current);
#endif /* CONFIG_SCHED_DEBUG */

        /*
         * Preemption ignores task state, therefore preempted tasks are always
         * RUNNING (we will not have dequeued if state != RUNNING).
         */
        if (preempt)
                return TASK_REPORT_MAX;

        /*
         * task_state_index() uses fls() and returns a value from 0-8 range.
         * Decrement it by 1 (except TASK_RUNNING state i.e 0) before using
         * it for left shift operation to get the correct task->state
         * mapping.
         */
	state = __task_state_index(prev_state, p->exit_state);

        return state ? (1 << (state - 1)) : state;
}

#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0))

static inline long __trace_sched_switch_state(bool preempt, struct task_struct *p)
{
        unsigned int state;

#ifdef CONFIG_SCHED_DEBUG
        BUG_ON(p != current);
#endif /* CONFIG_SCHED_DEBUG */

        /*
         * Preemption ignores task state, therefore preempted tasks are always
         * RUNNING (we will not have dequeued if state != RUNNING).
         */
        if (preempt)
                return TASK_REPORT_MAX;

        /*
         * task_state_index() uses fls() and returns a value from 0-8 range.
         * Decrement it by 1 (except TASK_RUNNING state i.e 0) before using
         * it for left shift operation to get the correct task->state
         * mapping.
         */
        state = task_state_index(p);

        return state ? (1 << (state - 1)) : state;
}

#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,14,0))

static inline long __trace_sched_switch_state(bool preempt, struct task_struct *p)
{
        unsigned int state;

#ifdef CONFIG_SCHED_DEBUG
        BUG_ON(p != current);
#endif /* CONFIG_SCHED_DEBUG */

        /*
         * Preemption ignores task state, therefore preempted tasks are always
         * RUNNING (we will not have dequeued if state != RUNNING).
         */
        if (preempt)
                return TASK_REPORT_MAX;

        /*
         * __get_task_state() uses fls() and returns a value from 0-8 range.
         * Decrement it by 1 (except TASK_RUNNING state i.e 0) before using
         * it for left shift operation to get the correct task->state
         * mapping.
         */
        state = __get_task_state(p);

        return state ? (1 << (state - 1)) : state;
}

#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,4,0))

static inline long __trace_sched_switch_state(bool preempt, struct task_struct *p)
{
#ifdef CONFIG_SCHED_DEBUG
	BUG_ON(p != current);
#endif /* CONFIG_SCHED_DEBUG */
	/*
	 * Preemption ignores task state, therefore preempted tasks are always RUNNING
	 * (we will not have dequeued if state != RUNNING).
	 */
	return preempt ? TASK_RUNNING | TASK_STATE_MAX : p->state;
}

#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,19,0))

static inline long __trace_sched_switch_state(struct task_struct *p)
{
	long state = p->state;

#ifdef CONFIG_PREEMPT
#ifdef CONFIG_SCHED_DEBUG
	BUG_ON(p != current);
#endif /* CONFIG_SCHED_DEBUG */
	/*
	 * For all intents and purposes a preempted task is a running task.
	 */
	if (preempt_count() & PREEMPT_ACTIVE)
		state = TASK_RUNNING | TASK_STATE_MAX;
#endif	/* CONFIG_PREEMPT */

	return state;
}

#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,13,0))

static inline long __trace_sched_switch_state(struct task_struct *p)
{
	long state = p->state;

#ifdef CONFIG_PREEMPT
	/*
	 * For all intents and purposes a preempted task is a running task.
	 */
	if (task_preempt_count(p) & PREEMPT_ACTIVE)
		state = TASK_RUNNING | TASK_STATE_MAX;
#endif

	return state;
}

#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,2,0))

static inline long __trace_sched_switch_state(struct task_struct *p)
{
	long state = p->state;

#ifdef CONFIG_PREEMPT
	/*
	 * For all intents and purposes a preempted task is a running task.
	 */
	if (task_thread_info(p)->preempt_count & PREEMPT_ACTIVE)
		state = TASK_RUNNING | TASK_STATE_MAX;
#endif

	return state;
}

#else

static inline long __trace_sched_switch_state(struct task_struct *p)
{
	long state = p->state;

#ifdef CONFIG_PREEMPT
	/*
	 * For all intents and purposes a preempted task is a running task.
	 */
	if (task_thread_info(p)->preempt_count & PREEMPT_ACTIVE)
		state = TASK_RUNNING;
#endif

	return state;
}

#endif

#endif /* _TRACE_SCHED_DEF_ */

#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM
/*
 * Enumeration of the task state bitmask.
 * Only bit flags are enumerated here, not composition of states.
 */
LTTNG_TRACEPOINT_ENUM(task_state,
	TP_ENUM_VALUES(
		ctf_enum_value("TASK_RUNNING", TASK_RUNNING)
		ctf_enum_value("TASK_INTERRUPTIBLE", TASK_INTERRUPTIBLE)
		ctf_enum_value("TASK_UNINTERRUPTIBLE", TASK_UNINTERRUPTIBLE)
		ctf_enum_value("TASK_STOPPED", __TASK_STOPPED)
		ctf_enum_value("TASK_TRACED", __TASK_TRACED)
		ctf_enum_value("EXIT_DEAD", EXIT_DEAD)
		ctf_enum_value("EXIT_ZOMBIE", EXIT_ZOMBIE)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,9,0))
		ctf_enum_value("TASK_PARKED", TASK_PARKED)
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,9,0)) */

		ctf_enum_value("TASK_DEAD", TASK_DEAD)
		ctf_enum_value("TASK_WAKEKILL", TASK_WAKEKILL)
		ctf_enum_value("TASK_WAKING", TASK_WAKING)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,2,0))
		ctf_enum_value("TASK_NOLOAD", TASK_NOLOAD)
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,2,0)) */

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,8,0))
		ctf_enum_value("TASK_NEW", TASK_NEW)
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,8,0)) */

		ctf_enum_value("TASK_STATE_MAX", TASK_STATE_MAX)
	)
)
#endif /* CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM */

/*
 * Tracepoint for calling kthread_stop, performed to end a kthread:
 */
LTTNG_TRACEPOINT_EVENT(sched_kthread_stop,

	TP_PROTO(struct task_struct *t),

	TP_ARGS(t),

	TP_FIELDS(
		ctf_array_text(char, comm, t->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, tid, t->pid)
	)
)

/*
 * Tracepoint for the return value of the kthread stopping:
 */
LTTNG_TRACEPOINT_EVENT(sched_kthread_stop_ret,

	TP_PROTO(int ret),

	TP_ARGS(ret),

	TP_FIELDS(
		ctf_integer(int, ret, ret)
	)
)

/*
 * Tracepoint for waking up a task:
 */
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,3,0) || \
	LTTNG_RT_KERNEL_RANGE(4,1,10,11, 4,2,0,0) || \
	LTTNG_RT_KERNEL_RANGE(3,18,27,26, 3,19,0,0) || \
	LTTNG_RT_KERNEL_RANGE(3,14,61,63, 3,15,0,0) || \
	LTTNG_RT_KERNEL_RANGE(3,12,54,73, 3,13,0,0) || \
	LTTNG_RT_KERNEL_RANGE(3,10,97,106, 3,11,0,0) || \
	LTTNG_RT_KERNEL_RANGE(3,4,110,139, 3,5,0,0) || \
	LTTNG_RT_KERNEL_RANGE(3,2,77,111, 3,3,0,0))
LTTNG_TRACEPOINT_EVENT_CLASS(sched_wakeup_template,

	TP_PROTO(struct task_struct *p),

	TP_ARGS(p),

	TP_FIELDS(
		ctf_array_text(char, comm, p->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(int, prio, p->prio - MAX_RT_PRIO)
		ctf_integer(int, target_cpu, task_cpu(p))
	)
)
#else /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,3,0)) */
LTTNG_TRACEPOINT_EVENT_CLASS(sched_wakeup_template,

	TP_PROTO(struct task_struct *p, int success),

	TP_ARGS(p, success),

	TP_FIELDS(
		ctf_array_text(char, comm, p->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(int, prio, p->prio - MAX_RT_PRIO)
		ctf_integer(int, success, success)
		ctf_integer(int, target_cpu, task_cpu(p))
	)
)
#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,3,0)) */

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,3,0) || \
	LTTNG_RT_KERNEL_RANGE(4,1,10,11, 4,2,0,0) || \
	LTTNG_RT_KERNEL_RANGE(3,18,27,26, 3,19,0,0) || \
	LTTNG_RT_KERNEL_RANGE(3,14,61,63, 3,15,0,0) || \
	LTTNG_RT_KERNEL_RANGE(3,12,54,73, 3,13,0,0) || \
	LTTNG_RT_KERNEL_RANGE(3,10,97,106, 3,11,0,0) || \
	LTTNG_RT_KERNEL_RANGE(3,4,110,139, 3,5,0,0) || \
	LTTNG_RT_KERNEL_RANGE(3,2,77,111, 3,3,0,0))

/*
 * Tracepoint called when waking a task; this tracepoint is guaranteed to be
 * called from the waking context.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(sched_wakeup_template, sched_waking,
	     TP_PROTO(struct task_struct *p),
	     TP_ARGS(p))

/*
 * Tracepoint called when the task is actually woken; p->state == TASK_RUNNNG.
 * It it not always called from the waking context.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(sched_wakeup_template, sched_wakeup,
	     TP_PROTO(struct task_struct *p),
	     TP_ARGS(p))

/*
 * Tracepoint for waking up a new task:
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(sched_wakeup_template, sched_wakeup_new,
	     TP_PROTO(struct task_struct *p),
	     TP_ARGS(p))

#else

LTTNG_TRACEPOINT_EVENT_INSTANCE(sched_wakeup_template, sched_wakeup,
	     TP_PROTO(struct task_struct *p, int success),
	     TP_ARGS(p, success))

/*
 * Tracepoint for waking up a new task:
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(sched_wakeup_template, sched_wakeup_new,
	     TP_PROTO(struct task_struct *p, int success),
	     TP_ARGS(p, success))

#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,3,0)) */

/*
 * Tracepoint for task switches, performed by the scheduler:
 */

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,18,0) \
	|| LTTNG_RHEL_KERNEL_RANGE(5,14,0,162,0,0, 5,15,0,0,0,0))
LTTNG_TRACEPOINT_EVENT(sched_switch,

	TP_PROTO(bool preempt,
		struct task_struct *prev,
		struct task_struct *next,
		unsigned int prev_state),

	TP_ARGS(preempt, prev, next, prev_state),

	TP_FIELDS(
		ctf_array_text(char, prev_comm,	prev->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, prev_tid, prev->pid)
		ctf_integer(int, prev_prio, prev->prio - MAX_RT_PRIO)
#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM
		ctf_enum(task_state, long, prev_state, __trace_sched_switch_state(preempt, prev_state, prev))
#else
		ctf_integer(long, prev_state, __trace_sched_switch_state(preempt, prev_state, prev))
#endif
		ctf_array_text(char, next_comm, next->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, next_tid, next->pid)
		ctf_integer(int, next_prio, next->prio - MAX_RT_PRIO)
	)
)

#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,4,0))

LTTNG_TRACEPOINT_EVENT(sched_switch,

	TP_PROTO(bool preempt,
		 struct task_struct *prev,
		 struct task_struct *next),

	TP_ARGS(preempt, prev, next),

	TP_FIELDS(
		ctf_array_text(char, prev_comm,	prev->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, prev_tid, prev->pid)
		ctf_integer(int, prev_prio, prev->prio - MAX_RT_PRIO)
#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM
		ctf_enum(task_state, long, prev_state, __trace_sched_switch_state(preempt, prev))
#else
		ctf_integer(long, prev_state, __trace_sched_switch_state(preempt, prev))
#endif
		ctf_array_text(char, next_comm, next->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, next_tid, next->pid)
		ctf_integer(int, next_prio, next->prio - MAX_RT_PRIO)
	)
)

#else

LTTNG_TRACEPOINT_EVENT(sched_switch,

	TP_PROTO(struct task_struct *prev,
		 struct task_struct *next),

	TP_ARGS(prev, next),

	TP_FIELDS(
		ctf_array_text(char, prev_comm,	prev->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, prev_tid, prev->pid)
		ctf_integer(int, prev_prio, prev->prio - MAX_RT_PRIO)
#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM
		ctf_enum(task_state, long, prev_state, __trace_sched_switch_state(prev))
#else
		ctf_integer(long, prev_state, __trace_sched_switch_state(prev))
#endif
		ctf_array_text(char, next_comm, next->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, next_tid, next->pid)
		ctf_integer(int, next_prio, next->prio - MAX_RT_PRIO)
	)
)
#endif

/*
 * Tracepoint for a task being migrated:
 */
LTTNG_TRACEPOINT_EVENT(sched_migrate_task,

	TP_PROTO(struct task_struct *p, int dest_cpu),

	TP_ARGS(p, dest_cpu),

	TP_FIELDS(
		ctf_array_text(char, comm, p->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(int, prio, p->prio - MAX_RT_PRIO)
		ctf_integer(int, orig_cpu, task_cpu(p))
		ctf_integer(int, dest_cpu, dest_cpu)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(sched_process_template,

	TP_PROTO(struct task_struct *p),

	TP_ARGS(p),

	TP_FIELDS(
		ctf_array_text(char, comm, p->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(int, prio, p->prio - MAX_RT_PRIO)
	)
)

/*
 * Tracepoint for freeing a task:
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(sched_process_template, sched_process_free,
	     TP_PROTO(struct task_struct *p),
	     TP_ARGS(p))


/*
 * Tracepoint for a task exiting:
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(sched_process_template, sched_process_exit,
	     TP_PROTO(struct task_struct *p),
	     TP_ARGS(p))

/*
 * Tracepoint for waiting on task to unschedule:
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(sched_process_template, sched_wait_task,
	TP_PROTO(struct task_struct *p),
	TP_ARGS(p))

/*
 * Tracepoint for a waiting task:
 */
LTTNG_TRACEPOINT_EVENT(sched_process_wait,

	TP_PROTO(struct pid *pid),

	TP_ARGS(pid),

	TP_FIELDS(
		ctf_array_text(char, comm, current->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, tid, pid_nr(pid))
		ctf_integer(int, prio, current->prio - MAX_RT_PRIO)
	)
)

/*
 * Tracepoint for do_fork.
 * Saving both TID and PID information, especially for the child, allows
 * trace analyzers to distinguish between creation of a new process and
 * creation of a new thread. Newly created processes will have child_tid
 * == child_pid, while creation of a thread yields to child_tid !=
 * child_pid.
 */
LTTNG_TRACEPOINT_EVENT_CODE(sched_process_fork,

	TP_PROTO(struct task_struct *parent, struct task_struct *child),

	TP_ARGS(parent, child),

	TP_locvar(
		pid_t vtids[LTTNG_MAX_PID_NS_LEVEL];
		unsigned int ns_level;
	),

	TP_code_pre(
		if (child) {
			struct pid *child_pid;
			unsigned int i;

			child_pid = task_pid(child);
			tp_locvar->ns_level =
				min_t(unsigned int, child_pid->level + 1,
					LTTNG_MAX_PID_NS_LEVEL);
			for (i = 0; i < tp_locvar->ns_level; i++)
				tp_locvar->vtids[i] = child_pid->numbers[i].nr;
		}
	),

	TP_FIELDS(
		ctf_array_text(char, parent_comm, parent->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, parent_tid, parent->pid)
		ctf_integer(pid_t, parent_pid, parent->tgid)
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
		ctf_integer(unsigned int, parent_ns_inum,
			({
				unsigned int parent_ns_inum = 0;

				if (parent) {
					struct pid_namespace *pid_ns;

					pid_ns = task_active_pid_ns(parent);
					if (pid_ns)
						parent_ns_inum =
							pid_ns->lttng_ns_inum;
				}
				parent_ns_inum;
			}))
#endif
		ctf_array_text(char, child_comm, child->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, child_tid, child->pid)
		ctf_sequence(pid_t, vtids, tp_locvar->vtids, u8, tp_locvar->ns_level)
		ctf_integer(pid_t, child_pid, child->tgid)
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))
		ctf_integer(unsigned int, child_ns_inum,
			({
				unsigned int child_ns_inum = 0;

				if (child) {
					struct pid_namespace *pid_ns;

					pid_ns = task_active_pid_ns(child);
					if (pid_ns)
						child_ns_inum =
							pid_ns->lttng_ns_inum;
				}
				child_ns_inum;
			}))
#endif
	),

	TP_code_post()
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,4,0))
/*
 * Tracepoint for exec:
 */
LTTNG_TRACEPOINT_EVENT(sched_process_exec,

	TP_PROTO(struct task_struct *p, pid_t old_pid,
		 struct linux_binprm *bprm),

	TP_ARGS(p, old_pid, bprm),

	TP_FIELDS(
		ctf_string(filename, bprm->filename)
		ctf_integer(pid_t, tid, p->pid)
		ctf_integer(pid_t, old_tid, old_pid)
	)
)
#endif

/*
 * XXX the below sched_stat tracepoints only apply to SCHED_OTHER/BATCH/IDLE
 *     adding sched_stat support to SCHED_FIFO/RR would be welcome.
 */
LTTNG_TRACEPOINT_EVENT_CLASS(sched_stat_template,

	TP_PROTO(struct task_struct *tsk, u64 delay),

	TP_ARGS(tsk, delay),

	TP_FIELDS(
		ctf_array_text(char, comm, tsk->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, tid, tsk->pid)
		ctf_integer(u64, delay, delay)
	)
)


/*
 * Tracepoint for accounting wait time (time the task is runnable
 * but not actually running due to scheduler contention).
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(sched_stat_template, sched_stat_wait,
	     TP_PROTO(struct task_struct *tsk, u64 delay),
	     TP_ARGS(tsk, delay))

/*
 * Tracepoint for accounting sleep time (time the task is not runnable,
 * including iowait, see below).
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(sched_stat_template, sched_stat_sleep,
	     TP_PROTO(struct task_struct *tsk, u64 delay),
	     TP_ARGS(tsk, delay))

/*
 * Tracepoint for accounting iowait time (time the task is not runnable
 * due to waiting on IO to complete).
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(sched_stat_template, sched_stat_iowait,
	     TP_PROTO(struct task_struct *tsk, u64 delay),
	     TP_ARGS(tsk, delay))

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,3,0))
/*
 * Tracepoint for accounting blocked time (time the task is in uninterruptible).
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(sched_stat_template, sched_stat_blocked,
	     TP_PROTO(struct task_struct *tsk, u64 delay),
	     TP_ARGS(tsk, delay))
#endif

/*
 * Tracepoint for accounting runtime (time the task is executing
 * on a CPU).
 */
LTTNG_TRACEPOINT_EVENT(sched_stat_runtime,

	TP_PROTO(struct task_struct *tsk, u64 runtime, u64 vruntime),

	TP_ARGS(tsk, runtime, vruntime),

	TP_FIELDS(
		ctf_array_text(char, comm, tsk->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, tid, tsk->pid)
		ctf_integer(u64, runtime, runtime)
		ctf_integer(u64, vruntime, vruntime)
	)
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,12,0) || \
	LTTNG_RT_KERNEL_RANGE(4,9,27,18, 4,10,0,0) || \
	LTTNG_RT_KERNEL_RANGE(4,11,5,1, 4,12,0,0))
/*
 * Tracepoint for showing priority inheritance modifying a tasks
 * priority.
 */
LTTNG_TRACEPOINT_EVENT(sched_pi_setprio,

	TP_PROTO(struct task_struct *tsk, struct task_struct *pi_task),

	TP_ARGS(tsk, pi_task),

	TP_FIELDS(
		ctf_array_text(char, comm, tsk->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, tid, tsk->pid)
		ctf_integer(int, oldprio, tsk->prio - MAX_RT_PRIO)
		ctf_integer(int, newprio, pi_task ? pi_task->prio - MAX_RT_PRIO : tsk->prio - MAX_RT_PRIO)
	)
)
#else
/*
 * Tracepoint for showing priority inheritance modifying a tasks
 * priority.
 */
LTTNG_TRACEPOINT_EVENT(sched_pi_setprio,

	TP_PROTO(struct task_struct *tsk, int newprio),

	TP_ARGS(tsk, newprio),

	TP_FIELDS(
		ctf_array_text(char, comm, tsk->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, tid, tsk->pid)
		ctf_integer(int, oldprio, tsk->prio - MAX_RT_PRIO)
		ctf_integer(int, newprio, newprio - MAX_RT_PRIO)
	)
)
#endif

#endif /* LTTNG_TRACE_SCHED_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
