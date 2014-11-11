#undef TRACE_SYSTEM
#define TRACE_SYSTEM rcu

#if !defined(LTTNG_TRACE_RCU_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_RCU_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/version.h>

/*
 * Tracepoint for start/end markers used for utilization calculations.
 * By convention, the string is of the following forms:
 *
 * "Start <activity>" -- Mark the start of the specified activity,
 *			 such as "context switch".  Nesting is permitted.
 * "End <activity>" -- Mark the end of the specified activity.
 *
 * An "@" character within "<activity>" is a comment character: Data
 * reduction scripts will ignore the "@" and the remainder of the line.
 */
LTTNG_TRACEPOINT_EVENT(rcu_utilization,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *s),
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
	TP_PROTO(char *s),
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

	TP_ARGS(s),

	TP_FIELDS(
		ctf_string(s, s)
	)
)

#ifdef CONFIG_RCU_TRACE

#if defined(CONFIG_TREE_RCU) \
	|| (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0) \
		&& defined(CONFIG_PREEMPT_RCU)) \
	|| defined(CONFIG_TREE_PREEMPT_RCU)

/*
 * Tracepoint for grace-period events: starting and ending a grace
 * period ("start" and "end", respectively), a CPU noting the start
 * of a new grace period or the end of an old grace period ("cpustart"
 * and "cpuend", respectively), a CPU passing through a quiescent
 * state ("cpuqs"), a CPU coming online or going offline ("cpuonl"
 * and "cpuofl", respectively), and a CPU being kicked for being too
 * long in dyntick-idle mode ("kick").
 */
LTTNG_TRACEPOINT_EVENT(rcu_grace_period,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, unsigned long gpnum, const char *gpevent),
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
	TP_PROTO(char *rcuname, unsigned long gpnum, char *gpevent),
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

	TP_ARGS(rcuname, gpnum, gpevent),

	TP_FIELDS(
		ctf_string(rcuname, rcuname)
		ctf_integer(unsigned long, gpnum, gpnum)
		ctf_string(gpevent, gpevent)
	)
)

/*
 * Tracepoint for grace-period-initialization events.  These are
 * distinguished by the type of RCU, the new grace-period number, the
 * rcu_node structure level, the starting and ending CPU covered by the
 * rcu_node structure, and the mask of CPUs that will be waited for.
 * All but the type of RCU are extracted from the rcu_node structure.
 */
LTTNG_TRACEPOINT_EVENT(rcu_grace_period_init,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, unsigned long gpnum, u8 level,
		 int grplo, int grphi, unsigned long qsmask),
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
	TP_PROTO(char *rcuname, unsigned long gpnum, u8 level,
		 int grplo, int grphi, unsigned long qsmask),
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

	TP_ARGS(rcuname, gpnum, level, grplo, grphi, qsmask),

	TP_FIELDS(
		ctf_string(rcuname, rcuname)
		ctf_integer(unsigned long, gpnum, gpnum)
		ctf_integer(u8, level, level)
		ctf_integer(int, grplo, grplo)
		ctf_integer(int, grphi, grphi)
		ctf_integer(unsigned long, qsmask, qsmask)
	)
)

/*
 * Tracepoint for tasks blocking within preemptible-RCU read-side
 * critical sections.  Track the type of RCU (which one day might
 * include SRCU), the grace-period number that the task is blocking
 * (the current or the next), and the task's PID.
 */
LTTNG_TRACEPOINT_EVENT(rcu_preempt_task,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, int pid, unsigned long gpnum),
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
	TP_PROTO(char *rcuname, int pid, unsigned long gpnum),
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

	TP_ARGS(rcuname, pid, gpnum),

	TP_FIELDS(
		ctf_string(rcuname, rcuname)
		ctf_integer(unsigned long, gpnum, gpnum)
		ctf_integer(int, pid, pid)
	)
)

/*
 * Tracepoint for tasks that blocked within a given preemptible-RCU
 * read-side critical section exiting that critical section.  Track the
 * type of RCU (which one day might include SRCU) and the task's PID.
 */
LTTNG_TRACEPOINT_EVENT(rcu_unlock_preempted_task,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, unsigned long gpnum, int pid),
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
	TP_PROTO(char *rcuname, unsigned long gpnum, int pid),
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

	TP_ARGS(rcuname, gpnum, pid),

	TP_FIELDS(
		ctf_string(rcuname, rcuname)
		ctf_integer(unsigned long, gpnum, gpnum)
		ctf_integer(int, pid, pid)
	)
)

/*
 * Tracepoint for quiescent-state-reporting events.  These are
 * distinguished by the type of RCU, the grace-period number, the
 * mask of quiescent lower-level entities, the rcu_node structure level,
 * the starting and ending CPU covered by the rcu_node structure, and
 * whether there are any blocked tasks blocking the current grace period.
 * All but the type of RCU are extracted from the rcu_node structure.
 */
LTTNG_TRACEPOINT_EVENT(rcu_quiescent_state_report,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, unsigned long gpnum,
		 unsigned long mask, unsigned long qsmask,
		 u8 level, int grplo, int grphi, int gp_tasks),
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
	TP_PROTO(char *rcuname, unsigned long gpnum,
		 unsigned long mask, unsigned long qsmask,
		 u8 level, int grplo, int grphi, int gp_tasks),
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

	TP_ARGS(rcuname, gpnum, mask, qsmask, level, grplo, grphi, gp_tasks),

	TP_FIELDS(
		ctf_string(rcuname, rcuname)
		ctf_integer(unsigned long, gpnum, gpnum)
		ctf_integer(unsigned long, mask, mask)
		ctf_integer(unsigned long, qsmask, qsmask)
		ctf_integer(u8, level, level)
		ctf_integer(int, grplo, grplo)
		ctf_integer(int, grphi, grphi)
		ctf_integer(u8, gp_tasks, gp_tasks)
	)
)

/*
 * Tracepoint for quiescent states detected by force_quiescent_state().
 * These trace events include the type of RCU, the grace-period number
 * that was blocked by the CPU, the CPU itself, and the type of quiescent
 * state, which can be "dti" for dyntick-idle mode, "ofl" for CPU offline,
 * or "kick" when kicking a CPU that has been in dyntick-idle mode for
 * too long.
 */
LTTNG_TRACEPOINT_EVENT(rcu_fqs,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, unsigned long gpnum, int cpu, const char *qsevent),
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
	TP_PROTO(char *rcuname, unsigned long gpnum, int cpu, char *qsevent),
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

	TP_ARGS(rcuname, gpnum, cpu, qsevent),

	TP_FIELDS(
		ctf_integer(unsigned long, gpnum, gpnum)
		ctf_integer(int, cpu, cpu)
		ctf_string(rcuname, rcuname)
		ctf_string(qsevent, qsevent)
	)
)

#endif	/*
	 * #if defined(CONFIG_TREE_RCU)
	 *	|| (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)
	 *		&& defined(CONFIG_PREEMPT_RCU))
	 *	|| defined(CONFIG_TREE_PREEMPT_RCU)
	 */

/*
 * Tracepoint for dyntick-idle entry/exit events.  These take a string
 * as argument: "Start" for entering dyntick-idle mode, "End" for
 * leaving it, "--=" for events moving towards idle, and "++=" for events
 * moving away from idle.  "Error on entry: not idle task" and "Error on
 * exit: not idle task" indicate that a non-idle task is erroneously
 * toying with the idle loop.
 *
 * These events also take a pair of numbers, which indicate the nesting
 * depth before and after the event of interest.  Note that task-related
 * events use the upper bits of each number, while interrupt-related
 * events use the lower bits.
 */
LTTNG_TRACEPOINT_EVENT(rcu_dyntick,


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *polarity, long long oldnesting, long long newnesting),

	TP_ARGS(polarity, oldnesting, newnesting),
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
	TP_PROTO(char *polarity, long long oldnesting, long long newnesting),

	TP_ARGS(polarity, oldnesting, newnesting),
#else
	TP_PROTO(char *polarity),

	TP_ARGS(polarity),
#endif

	TP_FIELDS(
		ctf_string(polarity, polarity)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
		ctf_integer(long long, oldnesting, oldnesting)
		ctf_integer(long long, newnesting, newnesting)
#endif
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
/*
 * Tracepoint for RCU preparation for idle, the goal being to get RCU
 * processing done so that the current CPU can shut off its scheduling
 * clock and enter dyntick-idle mode.  One way to accomplish this is
 * to drain all RCU callbacks from this CPU, and the other is to have
 * done everything RCU requires for the current grace period.  In this
 * latter case, the CPU will be awakened at the end of the current grace
 * period in order to process the remainder of its callbacks.
 *
 * These tracepoints take a string as argument:
 *
 *	"No callbacks": Nothing to do, no callbacks on this CPU.
 *	"In holdoff": Nothing to do, holding off after unsuccessful attempt.
 *	"Begin holdoff": Attempt failed, don't retry until next jiffy.
 *	"Dyntick with callbacks": Entering dyntick-idle despite callbacks.
 *	"Dyntick with lazy callbacks": Entering dyntick-idle w/lazy callbacks.
 *	"More callbacks": Still more callbacks, try again to clear them out.
 *	"Callbacks drained": All callbacks processed, off to dyntick idle!
 *	"Timer": Timer fired to cause CPU to continue processing callbacks.
 *	"Demigrate": Timer fired on wrong CPU, woke up correct CPU.
 *	"Cleanup after idle": Idle exited, timer canceled.
 */
LTTNG_TRACEPOINT_EVENT(rcu_prep_idle,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *reason),
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
	TP_PROTO(char *reason),
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

	TP_ARGS(reason),

	TP_FIELDS(
		ctf_string(reason, reason)
	)
)
#endif

/*
 * Tracepoint for the registration of a single RCU callback function.
 * The first argument is the type of RCU, the second argument is
 * a pointer to the RCU callback itself, the third element is the
 * number of lazy callbacks queued, and the fourth element is the
 * total number of callbacks queued.
 */
LTTNG_TRACEPOINT_EVENT(rcu_callback,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, struct rcu_head *rhp, long qlen_lazy,
		 long qlen),

	TP_ARGS(rcuname, rhp, qlen_lazy, qlen),
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
	TP_PROTO(char *rcuname, struct rcu_head *rhp, long qlen_lazy,
		 long qlen),

	TP_ARGS(rcuname, rhp, qlen_lazy, qlen),
#else
	TP_PROTO(char *rcuname, struct rcu_head *rhp, long qlen),

	TP_ARGS(rcuname, rhp, qlen),
#endif

	TP_FIELDS(
		ctf_string(rcuname, rcuname)
		ctf_integer(void *, rhp, rhp)
		ctf_integer(void *, func, rhp->func)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
		ctf_integer(long, qlen_lazy, qlen_lazy)
#endif
		ctf_integer(long, qlen, qlen)
	)
)

/*
 * Tracepoint for the registration of a single RCU callback of the special
 * kfree() form.  The first argument is the RCU type, the second argument
 * is a pointer to the RCU callback, the third argument is the offset
 * of the callback within the enclosing RCU-protected data structure,
 * the fourth argument is the number of lazy callbacks queued, and the
 * fifth argument is the total number of callbacks queued.
 */
LTTNG_TRACEPOINT_EVENT(rcu_kfree_callback,


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, struct rcu_head *rhp, unsigned long offset,
		 long qlen_lazy, long qlen),

	TP_ARGS(rcuname, rhp, offset, qlen_lazy, qlen),
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
	TP_PROTO(char *rcuname, struct rcu_head *rhp, unsigned long offset,
		 long qlen_lazy, long qlen),

	TP_ARGS(rcuname, rhp, offset, qlen_lazy, qlen),
#else
	TP_PROTO(char *rcuname, struct rcu_head *rhp, unsigned long offset,
		 long qlen),

	TP_ARGS(rcuname, rhp, offset, qlen),
#endif

	TP_FIELDS(
		ctf_string(rcuname, rcuname)
		ctf_integer(void *, rhp, rhp)
		ctf_integer(unsigned long, offset, offset)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
		ctf_integer(long, qlen_lazy, qlen_lazy)
#endif
		ctf_integer(long, qlen, qlen)
	)
)

/*
 * Tracepoint for marking the beginning rcu_do_batch, performed to start
 * RCU callback invocation.  The first argument is the RCU flavor,
 * the second is the number of lazy callbacks queued, the third is
 * the total number of callbacks queued, and the fourth argument is
 * the current RCU-callback batch limit.
 */
LTTNG_TRACEPOINT_EVENT(rcu_batch_start,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, long qlen_lazy, long qlen, long blimit),

	TP_ARGS(rcuname, qlen_lazy, qlen, blimit),
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
	TP_PROTO(char *rcuname, long qlen_lazy, long qlen, long blimit),

	TP_ARGS(rcuname, qlen_lazy, qlen, blimit),
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
	TP_PROTO(char *rcuname, long qlen_lazy, long qlen, int blimit),

	TP_ARGS(rcuname, qlen_lazy, qlen, blimit),
#else
	TP_PROTO(char *rcuname, long qlen, int blimit),

	TP_ARGS(rcuname, qlen, blimit),
#endif

	TP_FIELDS(
		ctf_string(rcuname, rcuname)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
		ctf_integer(long, qlen_lazy, qlen_lazy)
#endif
		ctf_integer(long, qlen, qlen)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
		ctf_integer(long, blimit, blimit)
#else
		ctf_integer(int, blimit, blimit)
#endif
	)
)

/*
 * Tracepoint for the invocation of a single RCU callback function.
 * The first argument is the type of RCU, and the second argument is
 * a pointer to the RCU callback itself.
 */
LTTNG_TRACEPOINT_EVENT(rcu_invoke_callback,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, struct rcu_head *rhp),
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
	TP_PROTO(char *rcuname, struct rcu_head *rhp),
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

	TP_ARGS(rcuname, rhp),

	TP_FIELDS(
		ctf_string(rcuname, rcuname)
		ctf_integer(void *, rhp, rhp)
		ctf_integer(void *, func, rhp->func)
	)
)

/*
 * Tracepoint for the invocation of a single RCU callback of the special
 * kfree() form.  The first argument is the RCU flavor, the second
 * argument is a pointer to the RCU callback, and the third argument
 * is the offset of the callback within the enclosing RCU-protected
 * data structure.
 */
LTTNG_TRACEPOINT_EVENT(rcu_invoke_kfree_callback,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, struct rcu_head *rhp, unsigned long offset),
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
	TP_PROTO(char *rcuname, struct rcu_head *rhp, unsigned long offset),
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

	TP_ARGS(rcuname, rhp, offset),

	TP_FIELDS(
		ctf_string(rcuname, rcuname)
		ctf_integer(void *, rhp, rhp)
		ctf_integer(unsigned long, offset, offset)
	)
)

/*
 * Tracepoint for exiting rcu_do_batch after RCU callbacks have been
 * invoked.  The first argument is the name of the RCU flavor,
 * the second argument is number of callbacks actually invoked,
 * the third argument (cb) is whether or not any of the callbacks that
 * were ready to invoke at the beginning of this batch are still
 * queued, the fourth argument (nr) is the return value of need_resched(),
 * the fifth argument (iit) is 1 if the current task is the idle task,
 * and the sixth argument (risk) is the return value from
 * rcu_is_callbacks_kthread().
 */
LTTNG_TRACEPOINT_EVENT(rcu_batch_end,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0))
	TP_PROTO(const char *rcuname, int callbacks_invoked,
		 char cb, char nr, char iit, char risk),

	TP_ARGS(rcuname, callbacks_invoked, cb, nr, iit, risk),
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, int callbacks_invoked,
		 bool cb, bool nr, bool iit, bool risk),

	TP_ARGS(rcuname, callbacks_invoked, cb, nr, iit, risk),
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
	TP_PROTO(char *rcuname, int callbacks_invoked,
		 bool cb, bool nr, bool iit, bool risk),

	TP_ARGS(rcuname, callbacks_invoked, cb, nr, iit, risk),
#else
	TP_PROTO(char *rcuname, int callbacks_invoked),

	TP_ARGS(rcuname, callbacks_invoked),
#endif

	TP_FIELDS(
		ctf_string(rcuname, rcuname)
		ctf_integer(int, callbacks_invoked, callbacks_invoked)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0))
		ctf_integer(char, cb, cb)
		ctf_integer(char, nr, nr)
		ctf_integer(char, iit, iit)
		ctf_integer(char, risk, risk)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
		ctf_integer(bool, cb, cb)
		ctf_integer(bool, nr, nr)
		ctf_integer(bool, iit, iit)
		ctf_integer(bool, risk, risk)
#endif
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
/*
 * Tracepoint for rcutorture readers.  The first argument is the name
 * of the RCU flavor from rcutorture's viewpoint and the second argument
 * is the callback address.
 */
LTTNG_TRACEPOINT_EVENT(rcu_torture_read,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcutorturename, struct rcu_head *rhp,
		 unsigned long secs, unsigned long c_old, unsigned long c),

	TP_ARGS(rcutorturename, rhp, secs, c_old, c),
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
	TP_PROTO(char *rcutorturename, struct rcu_head *rhp,
		 unsigned long secs, unsigned long c_old, unsigned long c),

	TP_ARGS(rcutorturename, rhp, secs, c_old, c),
#else
	TP_PROTO(char *rcutorturename, struct rcu_head *rhp),

	TP_ARGS(rcutorturename, rhp),
#endif

	TP_FIELDS(
		ctf_string(rcutorturename, rcutorturename)
		ctf_integer(struct rcu_head *, rhp, rhp)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
		ctf_integer(unsigned long, secs, secs)
		ctf_integer(unsigned long, c_old, c_old)
		ctf_integer(unsigned long, c, c)
#endif
	)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
/*
 * Tracepoint for _rcu_barrier() execution.  The string "s" describes
 * the _rcu_barrier phase:
 *	"Begin": rcu_barrier_callback() started.
 *	"Check": rcu_barrier_callback() checking for piggybacking.
 *	"EarlyExit": rcu_barrier_callback() piggybacked, thus early exit.
 *	"Inc1": rcu_barrier_callback() piggyback check counter incremented.
 *	"Offline": rcu_barrier_callback() found offline CPU
 *	"OnlineQ": rcu_barrier_callback() found online CPU with callbacks.
 *	"OnlineNQ": rcu_barrier_callback() found online CPU, no callbacks.
 *	"IRQ": An rcu_barrier_callback() callback posted on remote CPU.
 *	"CB": An rcu_barrier_callback() invoked a callback, not the last.
 *	"LastCB": An rcu_barrier_callback() invoked the last callback.
 *	"Inc2": rcu_barrier_callback() piggyback check counter incremented.
 * The "cpu" argument is the CPU or -1 if meaningless, the "cnt" argument
 * is the count of remaining callbacks, and "done" is the piggybacking count.
 */
LTTNG_TRACEPOINT_EVENT(rcu_barrier,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
	TP_PROTO(const char *rcuname, const char *s, int cpu, int cnt, unsigned long done),
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
	TP_PROTO(char *rcuname, char *s, int cpu, int cnt, unsigned long done),
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

	TP_ARGS(rcuname, s, cpu, cnt, done),

	TP_FIELDS(
		ctf_string(rcuname, rcuname)
		ctf_string(s, s)
		ctf_integer(int, cpu, cpu)
		ctf_integer(int, cnt, cnt)
		ctf_integer(unsigned long, done, done)
	)
)
#endif

#else /* #ifdef CONFIG_RCU_TRACE */

#define trace_rcu_grace_period(rcuname, gpnum, gpevent) do { } while (0)
#define trace_rcu_grace_period_init(rcuname, gpnum, level, grplo, grphi, \
				    qsmask) do { } while (0)
#define trace_rcu_preempt_task(rcuname, pid, gpnum) do { } while (0)
#define trace_rcu_unlock_preempted_task(rcuname, gpnum, pid) do { } while (0)
#define trace_rcu_quiescent_state_report(rcuname, gpnum, mask, qsmask, level, \
					 grplo, grphi, gp_tasks) do { } \
	while (0)
#define trace_rcu_fqs(rcuname, gpnum, cpu, qsevent) do { } while (0)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
#define trace_rcu_dyntick(polarity, oldnesting, newnesting) do { } while (0)
#else
#define trace_rcu_dyntick(polarity) do { } while (0)
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
#define trace_rcu_prep_idle(reason) do { } while (0)
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
#define trace_rcu_callback(rcuname, rhp, qlen_lazy, qlen) do { } while (0)
#define trace_rcu_kfree_callback(rcuname, rhp, offset, qlen_lazy, qlen) \
	do { } while (0)
#define trace_rcu_batch_start(rcuname, qlen_lazy, qlen, blimit) \
	do { } while (0)
#else
#define trace_rcu_callback(rcuname, rhp, qlen) do { } while (0)
#define trace_rcu_kfree_callback(rcuname, rhp, offset, qlen) do { } while (0)
#define trace_rcu_batch_start(rcuname, qlen, blimit) do { } while (0)
#endif
#define trace_rcu_invoke_callback(rcuname, rhp) do { } while (0)
#define trace_rcu_invoke_kfree_callback(rcuname, rhp, offset) do { } while (0)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
#define trace_rcu_batch_end(rcuname, callbacks_invoked, cb, nr, iit, risk) \
	do { } while (0)
#else
#define trace_rcu_batch_end(rcuname, callbacks_invoked) do { } while (0)
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
#define trace_rcu_torture_read(rcutorturename, rhp, secs, c_old, c) \
	do { } while (0)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
#define trace_rcu_torture_read(rcutorturename, rhp) do { } while (0)
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
#define trace_rcu_barrier(name, s, cpu, cnt, done) do { } while (0)
#endif
#endif /* #else #ifdef CONFIG_RCU_TRACE */

#endif /* LTTNG_TRACE_RCU_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
