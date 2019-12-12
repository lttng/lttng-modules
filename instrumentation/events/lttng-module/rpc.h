/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM rpc

#if !defined(LTTNG_TRACE_RPC_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_RPC_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/sunrpc/sched.h>
#include <linux/sunrpc/clnt.h>

#ifndef ONCE_LTTNG_RPC_H
#define ONCE_LTTNG_RPC_H

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
static inline
int lttng_get_clid(const struct rpc_task *task)
{
	struct rpc_clnt *tk_client;

	tk_client = task->tk_client;
	if (!tk_client)
		return -1;
	/*
	 * The cl_clid field is always initialized to positive signed
	 * integers. Negative signed integer values are treated as
	 * errors.
	 */
	return (int) tk_client->cl_clid;
}
#endif /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

#endif /* ONCE_LTTNG_RPC_H */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0))
LTTNG_TRACEPOINT_EVENT_CLASS(rpc_task_status,

	TP_PROTO(const struct rpc_task *task),

	TP_ARGS(task),

	TP_FIELDS(
		ctf_integer(unsigned int, task_id, task->tk_pid)
		ctf_integer(int, client_id, lttng_get_clid(task))
		ctf_integer(int, status, task->tk_status)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_status, rpc_call_status,
	TP_PROTO(const struct rpc_task *task),

	TP_ARGS(task)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_status, rpc_bind_status,
	TP_PROTO(const struct rpc_task *task),

	TP_ARGS(task)
)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
LTTNG_TRACEPOINT_EVENT_CLASS(rpc_task_status,

	TP_PROTO(struct rpc_task *task),

	TP_ARGS(task),

	TP_FIELDS(
		ctf_integer(unsigned int, task_id, task->tk_pid)
		ctf_integer(int, client_id, lttng_get_clid(task))
		ctf_integer(int, status, task->tk_status)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_status, rpc_call_status,
	TP_PROTO(struct rpc_task *task),

	TP_ARGS(task)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_status, rpc_bind_status,
	TP_PROTO(struct rpc_task *task),

	TP_ARGS(task)
)
#else
LTTNG_TRACEPOINT_EVENT_CLASS(rpc_task_status,

	TP_PROTO(struct rpc_task *task),

	TP_ARGS(task),

	TP_FIELDS(
		ctf_integer_hex(const struct rpc_task *, task, task)
		ctf_integer_hex(const struct rpc_clnt *, clnt, task->tk_client)
		ctf_integer(int, status, task->tk_status)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_status, rpc_call_status,
	TP_PROTO(struct rpc_task *task),

	TP_ARGS(task)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_status, rpc_bind_status,
	TP_PROTO(struct rpc_task *task),

	TP_ARGS(task)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0))
LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_status, rpc_connect_status,
	TP_PROTO(const struct rpc_task *task),

	TP_ARGS(task)
)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0))
LTTNG_TRACEPOINT_EVENT(rpc_connect_status,
	TP_PROTO(const struct rpc_task *task),

	TP_ARGS(task),

	TP_FIELDS(
		ctf_integer(unsigned int, task_id, task->tk_pid)
		ctf_integer(int, client_id, lttng_get_clid(task))
		ctf_integer(int, status, task->tk_status)
	)
)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
LTTNG_TRACEPOINT_EVENT(rpc_connect_status,
	TP_PROTO(struct rpc_task *task, int status),

	TP_ARGS(task, status),

	TP_FIELDS(
		ctf_integer(unsigned int, task_id, task->tk_pid)
		ctf_integer(int, client_id, lttng_get_clid(task))
		ctf_integer(int, status, status)
	)
)
#else
LTTNG_TRACEPOINT_EVENT(rpc_connect_status,
	TP_PROTO(struct rpc_task *task, int status),

	TP_ARGS(task, status),

	TP_FIELDS(
		ctf_integer_hex(const struct rpc_task *, task, task)
		ctf_integer_hex(const struct rpc_clnt *, clnt, task->tk_client)
		ctf_integer(int, status, status)
	)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0))
LTTNG_TRACEPOINT_EVENT_CLASS(rpc_task_running,

	TP_PROTO(const struct rpc_task *task, const void *action),

	TP_ARGS(task, action),

	TP_FIELDS(
		ctf_integer(unsigned int, task_id, task->tk_pid)
		ctf_integer(int, client_id, lttng_get_clid(task))
		ctf_integer_hex(const void *, action, action)
		ctf_integer(unsigned long, runstate, task->tk_runstate)
		ctf_integer(int, status, task->tk_status)
		ctf_integer(unsigned short, flags, task->tk_flags)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_running, rpc_task_begin,

	TP_PROTO(const struct rpc_task *task, const void *action),

	TP_ARGS(task, action)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_running, rpc_task_run_action,

	TP_PROTO(const struct rpc_task *task, const void *action),

	TP_ARGS(task, action)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_running, rpc_task_complete,

	TP_PROTO(const struct rpc_task *task, const void *action),

	TP_ARGS(task, action)
)

LTTNG_TRACEPOINT_EVENT_CLASS(rpc_task_queued,

	TP_PROTO(const struct rpc_task *task, const struct rpc_wait_queue *q),

	TP_ARGS(task, q),

	TP_FIELDS(
		ctf_integer(unsigned int, task_id, task->tk_pid)
		ctf_integer(int, client_id, lttng_get_clid(task))
		ctf_integer(unsigned long, timeout, task->tk_timeout)
		ctf_integer(unsigned long, runstate, task->tk_runstate)
		ctf_integer(int, status, task->tk_status)
		ctf_integer(unsigned short, flags, task->tk_flags)
		ctf_string(q_name, rpc_qname(q))
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_queued, rpc_task_sleep,

	TP_PROTO(const struct rpc_task *task, const struct rpc_wait_queue *q),

	TP_ARGS(task, q)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_queued, rpc_task_wakeup,

	TP_PROTO(const struct rpc_task *task, const struct rpc_wait_queue *q),

	TP_ARGS(task, q)
)

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
LTTNG_TRACEPOINT_EVENT_CLASS(rpc_task_running,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const void *action),

	TP_ARGS(clnt, task, action),

	TP_FIELDS(
		ctf_integer(unsigned int, task_id, task->tk_pid)
		ctf_integer(int, client_id, lttng_get_clid(task))
		ctf_integer_hex(const void *, action, action)
		ctf_integer(unsigned long, runstate, task->tk_runstate)
		ctf_integer(int, status, task->tk_status)
		ctf_integer(unsigned short, flags, task->tk_flags)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_running, rpc_task_begin,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const void *action),

	TP_ARGS(clnt, task, action)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_running, rpc_task_run_action,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const void *action),

	TP_ARGS(clnt, task, action)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_running, rpc_task_complete,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const void *action),

	TP_ARGS(clnt, task, action)
)

LTTNG_TRACEPOINT_EVENT_CLASS(rpc_task_queued,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const struct rpc_wait_queue *q),

	TP_ARGS(clnt, task, q),

	TP_FIELDS(
		ctf_integer(unsigned int, task_id, task->tk_pid)
		ctf_integer(int, client_id, lttng_get_clid(task))
		ctf_integer(unsigned long, timeout, task->tk_timeout)
		ctf_integer(unsigned long, runstate, task->tk_runstate)
		ctf_integer(int, status, task->tk_status)
		ctf_integer(unsigned short, flags, task->tk_flags)
		ctf_string(q_name, rpc_qname(q))
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_queued, rpc_task_sleep,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const struct rpc_wait_queue *q),

	TP_ARGS(clnt, task, q)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_queued, rpc_task_wakeup,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const struct rpc_wait_queue *q),

	TP_ARGS(clnt, task, q)
)

#else
LTTNG_TRACEPOINT_EVENT_CLASS(rpc_task_running,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const void *action),

	TP_ARGS(clnt, task, action),

	TP_FIELDS(
		ctf_integer_hex(const struct rpc_clnt *, clnt, clnt)
		ctf_integer_hex(const struct rpc_task *, task, task)
		ctf_integer_hex(const void *, action, action)
		ctf_integer(unsigned long, runstate, task->tk_runstate)
		ctf_integer(int, status, task->tk_status)
		ctf_integer(unsigned short, flags, task->tk_flags)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_running, rpc_task_begin,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const void *action),

	TP_ARGS(clnt, task, action)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_running, rpc_task_run_action,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const void *action),

	TP_ARGS(clnt, task, action)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_running, rpc_task_complete,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const void *action),

	TP_ARGS(clnt, task, action)
)

LTTNG_TRACEPOINT_EVENT_CLASS(rpc_task_queued,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const struct rpc_wait_queue *q),

	TP_ARGS(clnt, task, q),

	TP_FIELDS(
		ctf_integer_hex(const struct rpc_clnt *, clnt, clnt)
		ctf_integer_hex(const struct rpc_task *, task, task)
		ctf_integer(unsigned long, timeout, task->tk_timeout)
		ctf_integer(unsigned long, runstate, task->tk_runstate)
		ctf_integer(int, status, task->tk_status)
		ctf_integer(unsigned short, flags, task->tk_flags)
		ctf_string(q_name, rpc_qname(q))
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_queued, rpc_task_sleep,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const struct rpc_wait_queue *q),

	TP_ARGS(clnt, task, q)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(rpc_task_queued, rpc_task_wakeup,

	TP_PROTO(const struct rpc_clnt *clnt, const struct rpc_task *task, const struct rpc_wait_queue *q),

	TP_ARGS(clnt, task, q)
)
#endif

#endif /* LTTNG_TRACE_RPC_H */

#include <probes/define_trace.h>
