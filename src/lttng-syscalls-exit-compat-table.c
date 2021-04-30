/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-syscalls-exit-compat-table.c
 *
 * LTTng syscall exit compat probes.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright (C) 2021 Francis Deslauriers <francis.deslauriers@efficios.com>
 */

#include <wrapper/tracepoint.h>

#include "lttng-syscalls.h"


#ifdef IA32_NR_syscalls
#define NR_compat_syscalls IA32_NR_syscalls
#else
#define NR_compat_syscalls NR_syscalls
#endif

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TP_MODULE_NOINIT
#define TRACE_INCLUDE_PATH instrumentation/syscalls/headers

#define PARAMS(args...)	args

#define SC_EXIT

#undef sc_in
#define sc_in(...)	__VA_ARGS__
#undef sc_out
#define sc_out(...)
#undef sc_inout
#define sc_inout(...)	__VA_ARGS__
#undef sc_exit
#define sc_exit(...)		__VA_ARGS__

#define LTTNG_TRACEPOINT_TYPE_EXTERN

#include <lttng/events-reset.h>

/* Hijack probe callback for system call compat exit */
#undef TP_PROBE_CB
#define LTTNG_SC_COMPAT
#define TP_PROBE_CB(_template)		&syscall_exit_event_probe
#define SC_LTTNG_TRACEPOINT_EVENT(_name, _proto, _args, _fields) \
	LTTNG_TRACEPOINT_EVENT(compat_syscall_exit_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post) \
	LTTNG_TRACEPOINT_EVENT_CODE(compat_syscall_exit_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_locvar), PARAMS(_code_pre),				\
		PARAMS(_fields), PARAMS(_code_post))
#define SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(_name, _fields) \
	LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(compat_syscall_exit_##_name, PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(_template, _name)		\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(compat_syscall_exit_##_template, compat_syscall_exit_##_name)

#define SC_LTTNG_TRACEPOINT_ENUM(_name, _values) \
	LTTNG_TRACEPOINT_ENUM(_name, PARAMS(_values))
#undef TRACE_SYSTEM
#define TRACE_SYSTEM compat_syscall_exit_integers
#define TRACE_INCLUDE_FILE compat_syscalls_integers
#include <instrumentation/syscalls/headers/syscalls_integers.h>
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#define TRACE_SYSTEM compat_syscall_exit_pointers
#define TRACE_INCLUDE_FILE compat_syscalls_pointers
#include <instrumentation/syscalls/headers/syscalls_pointers.h>
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#undef SC_LTTNG_TRACEPOINT_ENUM
#undef SC_LTTNG_TRACEPOINT_EVENT_CODE
#undef SC_LTTNG_TRACEPOINT_EVENT
#undef SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS
#undef SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#undef TP_PROBE_CB
#undef _TRACE_SYSCALLS_INTEGERS_H
#undef _TRACE_SYSCALLS_POINTERS_H
#undef LTTNG_SC_COMPAT

#define CREATE_SYSCALL_TABLE

#undef sc_exit
#define sc_exit(...)

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_template, _name, _nr, _nrargs)	\
	[ _nr ] = {						\
		.event_func = __event_probe__compat_syscall_exit_##_template, \
		.nrargs = (_nrargs),				\
		.fields = __event_fields___compat_syscall_exit_##_template, \
		.desc = &__event_desc___compat_syscall_exit_##_name, \
	},

/* Event compat syscall exit table */
const struct trace_syscall_entry _compat_sc_exit_table[] = {
#include <instrumentation/syscalls/headers/compat_syscalls_integers.h>
#include <instrumentation/syscalls/headers/compat_syscalls_pointers.h>
};

const struct trace_syscall_table compat_sc_exit_table = {
	.table = _compat_sc_exit_table,
	.len = ARRAY_SIZE(_compat_sc_exit_table),
};

#undef SC_EXIT

#undef CREATE_SYSCALL_TABLE
