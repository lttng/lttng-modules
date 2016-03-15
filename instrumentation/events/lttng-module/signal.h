#undef TRACE_SYSTEM
#define TRACE_SYSTEM signal

#if !defined(LTTNG_TRACE_SIGNAL_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_SIGNAL_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/version.h>

#ifndef _TRACE_SIGNAL_DEF
#define _TRACE_SIGNAL_DEF
#include <linux/signal.h>
#include <linux/sched.h>
#undef LTTNG_FIELDS_SIGINFO
#define LTTNG_FIELDS_SIGINFO(info)				\
		ctf_integer(int, errno,				\
			(info == SEND_SIG_NOINFO || info == SEND_SIG_FORCED || info == SEND_SIG_PRIV) ? \
			0 :					\
			info->si_errno)				\
		ctf_integer(int, code,				\
			(info == SEND_SIG_NOINFO || info == SEND_SIG_FORCED) ? \
			SI_USER : 				\
			((info == SEND_SIG_PRIV) ? SI_KERNEL : info->si_code))
#endif /* _TRACE_SIGNAL_DEF */

/**
 * signal_generate - called when a signal is generated
 * @sig: signal number
 * @info: pointer to struct siginfo
 * @task: pointer to struct task_struct
 *
 * Current process sends a 'sig' signal to 'task' process with
 * 'info' siginfo. If 'info' is SEND_SIG_NOINFO or SEND_SIG_PRIV,
 * 'info' is not a pointer and you can't access its field. Instead,
 * SEND_SIG_NOINFO means that si_code is SI_USER, and SEND_SIG_PRIV
 * means that si_code is SI_KERNEL.
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
LTTNG_TRACEPOINT_EVENT(signal_generate,

	TP_PROTO(int sig, struct siginfo *info, struct task_struct *task),

	TP_ARGS(sig, info, task),

	TP_FIELDS(
		ctf_integer(int, sig, sig)
		LTTNG_FIELDS_SIGINFO(info)
		ctf_array_text(char, comm, task->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, pid, task->pid)
	)
)
#else
LTTNG_TRACEPOINT_EVENT(signal_generate,

	TP_PROTO(int sig, struct siginfo *info, struct task_struct *task,
			int group, int result),

	TP_ARGS(sig, info, task, group, result),

	TP_FIELDS(
		ctf_integer(int, sig, sig)
		LTTNG_FIELDS_SIGINFO(info)
		ctf_array_text(char, comm, task->comm, TASK_COMM_LEN)
		ctf_integer(pid_t, pid, task->pid)
		ctf_integer(int, group, group)
		ctf_integer(int, result, result)
	)
)
#endif

/**
 * signal_deliver - called when a signal is delivered
 * @sig: signal number
 * @info: pointer to struct siginfo
 * @ka: pointer to struct k_sigaction
 *
 * A 'sig' signal is delivered to current process with 'info' siginfo,
 * and it will be handled by 'ka'. ka->sa.sa_handler can be SIG_IGN or
 * SIG_DFL.
 * Note that some signals reported by signal_generate tracepoint can be
 * lost, ignored or modified (by debugger) before hitting this tracepoint.
 * This means, this can show which signals are actually delivered, but
 * matching generated signals and delivered signals may not be correct.
 */
LTTNG_TRACEPOINT_EVENT(signal_deliver,

	TP_PROTO(int sig, struct siginfo *info, struct k_sigaction *ka),

	TP_ARGS(sig, info, ka),

	TP_FIELDS(
		ctf_integer(int, sig, sig)
		LTTNG_FIELDS_SIGINFO(info)
		ctf_integer(unsigned long, sa_handler, (unsigned long) ka->sa.sa_handler)
		ctf_integer(unsigned long, sa_flags, ka->sa.sa_flags)
	)
)

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
LTTNG_TRACEPOINT_EVENT_CLASS(signal_queue_overflow,

	TP_PROTO(int sig, int group, struct siginfo *info),

	TP_ARGS(sig, group, info),

	TP_FIELDS(
		ctf_integer(int, sig, sig)
		ctf_integer(int, group, group)
		LTTNG_FIELDS_SIGINFO(info)
	)
)

/**
 * signal_overflow_fail - called when signal queue is overflow
 * @sig: signal number
 * @group: signal to process group or not (bool)
 * @info: pointer to struct siginfo
 *
 * Kernel fails to generate 'sig' signal with 'info' siginfo, because
 * siginfo queue is overflow, and the signal is dropped.
 * 'group' is not 0 if the signal will be sent to a process group.
 * 'sig' is always one of RT signals.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(signal_queue_overflow, signal_overflow_fail,

	TP_PROTO(int sig, int group, struct siginfo *info),

	TP_ARGS(sig, group, info)
)

/**
 * signal_lose_info - called when siginfo is lost
 * @sig: signal number
 * @group: signal to process group or not (bool)
 * @info: pointer to struct siginfo
 *
 * Kernel generates 'sig' signal but loses 'info' siginfo, because siginfo
 * queue is overflow.
 * 'group' is not 0 if the signal will be sent to a process group.
 * 'sig' is always one of non-RT signals.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(signal_queue_overflow, signal_lose_info,

	TP_PROTO(int sig, int group, struct siginfo *info),

	TP_ARGS(sig, group, info)
)
#endif

#endif /* LTTNG_TRACE_SIGNAL_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
