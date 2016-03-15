#if !defined(_TRACE_SYSCALLS_UNKNOWN_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SYSCALLS_UNKNOWN_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/syscalls.h>

#define UNKNOWN_SYSCALL_NRARGS	6

#undef TP_PROBE_CB
#define TP_PROBE_CB(_template)          &syscall_entry_probe

LTTNG_TRACEPOINT_EVENT(syscall_entry_unknown,
	TP_PROTO(int id, unsigned long *args),
	TP_ARGS(id, args),
	TP_FIELDS(
		ctf_integer(int, id, id)
		ctf_array(unsigned long, args, args, UNKNOWN_SYSCALL_NRARGS)
	)
)
LTTNG_TRACEPOINT_EVENT(compat_syscall_entry_unknown,
	TP_PROTO(int id, unsigned long *args),
	TP_ARGS(id, args),
	TP_FIELDS(
		ctf_integer(int, id, id)
		ctf_array(unsigned long, args, args, UNKNOWN_SYSCALL_NRARGS)
	)
)

#undef TP_PROBE_CB
#define TP_PROBE_CB(_template)          &syscall_exit_probe

LTTNG_TRACEPOINT_EVENT(syscall_exit_unknown,
	TP_PROTO(int id, long ret, unsigned long *args),
	TP_ARGS(id, ret, args),
	TP_FIELDS(
		ctf_integer(int, id, id)
		ctf_integer(long, ret, ret)
		ctf_array(unsigned long, args, args, UNKNOWN_SYSCALL_NRARGS)
	)
)
LTTNG_TRACEPOINT_EVENT(compat_syscall_exit_unknown,
	TP_PROTO(int id, long ret, unsigned long *args),
	TP_ARGS(id, ret, args),
	TP_FIELDS(
		ctf_integer(int, id, id)
		ctf_integer(long, ret, ret)
		ctf_array(unsigned long, args, args, UNKNOWN_SYSCALL_NRARGS)
	)
)
#endif /*  _TRACE_SYSCALLS_UNKNOWN_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
