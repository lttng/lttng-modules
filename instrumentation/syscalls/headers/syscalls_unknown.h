#if !defined(_TRACE_SYSCALLS_UNKNOWN_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SYSCALLS_UNKNOWN_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/syscalls.h>

#define UNKNOWN_SYSCALL_NRARGS	6

#undef TP_PROBE_CB
#define TP_PROBE_CB(_template)          &syscall_entry_probe

LTTNG_TRACEPOINT_EVENT(syscall_enter_unknown,
	TP_PROTO(unsigned int id, unsigned long *args),
	TP_ARGS(id, args),
	TP_STRUCT__entry(
		__field(unsigned int, id)
		__array(unsigned long, args, UNKNOWN_SYSCALL_NRARGS)
	),
	TP_fast_assign(
		tp_assign(id, id)
		tp_memcpy(args, args, UNKNOWN_SYSCALL_NRARGS * sizeof(*args))
	),
	TP_printk()
)
LTTNG_TRACEPOINT_EVENT(compat_syscall_enter_unknown,
	TP_PROTO(unsigned int id, unsigned long *args),
	TP_ARGS(id, args),
	TP_STRUCT__entry(
		__field(unsigned int, id)
		__array(unsigned long, args, UNKNOWN_SYSCALL_NRARGS)
	),
	TP_fast_assign(
		tp_assign(id, id)
		tp_memcpy(args, args, UNKNOWN_SYSCALL_NRARGS * sizeof(*args))
	),
	TP_printk()
)

#undef TP_PROBE_CB
#define TP_PROBE_CB(_template)          &syscall_exit_probe

LTTNG_TRACEPOINT_EVENT(syscall_exit_unknown,
	TP_PROTO(unsigned int id, long ret, unsigned long *args),
	TP_ARGS(id, ret, args),
	TP_STRUCT__entry(
		__field(unsigned int, id)
		__field(long, ret)
		__array(unsigned long, args, UNKNOWN_SYSCALL_NRARGS)
	),
	TP_fast_assign(
		tp_assign(id, id)
		tp_assign(ret, ret)
		tp_memcpy(args, args, UNKNOWN_SYSCALL_NRARGS * sizeof(*args))
	),
	TP_printk()
)
LTTNG_TRACEPOINT_EVENT(compat_syscall_exit_unknown,
	TP_PROTO(unsigned int id, long ret, unsigned long *args),
	TP_ARGS(id, ret, args),
	TP_STRUCT__entry(
		__field(unsigned int, id)
		__field(long, ret)
		__array(unsigned long, args, UNKNOWN_SYSCALL_NRARGS)
	),
	TP_fast_assign(
		tp_assign(id, id)
		tp_assign(ret, ret)
		tp_memcpy(args, args, UNKNOWN_SYSCALL_NRARGS * sizeof(*args))
	),
	TP_printk()
)
#endif /*  _TRACE_SYSCALLS_UNKNOWN_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
