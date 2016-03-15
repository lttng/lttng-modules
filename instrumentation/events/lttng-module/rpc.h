#undef TRACE_SYSTEM
#define TRACE_SYSTEM rpc

#if !defined(LTTNG_TRACE_RPC_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_RPC_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/sunrpc/sched.h>
#include <linux/sunrpc/clnt.h>

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

LTTNG_TRACEPOINT_EVENT(rpc_connect_status,
	TP_PROTO(struct rpc_task *task, int status),

	TP_ARGS(task, status),

	TP_FIELDS(
		ctf_integer_hex(const struct rpc_task *, task, task)
		ctf_integer_hex(const struct rpc_clnt *, clnt, task->tk_client)
		ctf_integer(int, status, status)
	)
)

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

#endif /* LTTNG_TRACE_RPC_H */

#include <probes/define_trace.h>
