/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-syscalls-enum.c
 *
 * LTTng syscall enum types
 *
 * Copyright (C) 2021 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright (C) 2021 Francis Deslauriers <francis.deslauriers@efficios.com>
 */

#include <lttng/events.h>

#include "lttng-syscalls.h"

#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TP_MODULE_NOINIT
#define TRACE_INCLUDE_PATH instrumentation/syscalls/headers

#define PARAMS(args...)	args

#undef sc_exit
#define sc_exit(...)
#undef sc_in
#define sc_in(...)	__VA_ARGS__
#undef sc_out
#define sc_out(...)
#undef sc_inout
#define sc_inout(...)	__VA_ARGS__

#define TP_PROBE_CB(_template)
#define SC_LTTNG_TRACEPOINT_EVENT(_name, _proto, _args, _fields)
#define SC_LTTNG_TRACEPOINT_EVENT_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post)
#define SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(_name, _fields)
#define SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(_template, _name)

#define LTTNG_TRACEPOINT_TYPE_DEFINE_EXPORT

#define SC_LTTNG_TRACEPOINT_ENUM(_name, _values) \
	LTTNG_TRACEPOINT_ENUM(_name, PARAMS(_values))

#undef TRACE_SYSTEM
#define TRACE_SYSTEM syscall_entry_integers
#define TRACE_INCLUDE_FILE syscalls_integers
#include <instrumentation/syscalls/headers/syscalls_integers.h>
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#define TRACE_SYSTEM syscall_entry_pointers
#define TRACE_INCLUDE_FILE syscalls_pointers
#include <instrumentation/syscalls/headers/syscalls_pointers.h>
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#undef SC_LTTNG_TRACEPOINT_ENUM
