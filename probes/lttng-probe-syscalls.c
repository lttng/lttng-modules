/*
 * probes/lttng-probe-sched.c
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng sched probes.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>

#ifndef SYSCALL_DETAIL

/*
 * Create the tracepoint static inlines from the kernel to validate that our
 * trace event macros match the kernel we run on.
 */
#include <trace/events/syscalls.h>

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH ../instrumentation/events/lttng-module

#include "../instrumentation/events/lttng-module/syscalls.h"

#else	/* SYSCALL_DETAIL */

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS

#define TRACE_INCLUDE_PATH ../instrumentation/syscalls/headers

#define TRACE_SYSCALL_TABLE(_name, _nr, _nrargs)

#include "../instrumentation/syscalls/headers/syscalls.h"

#undef LTTNG_PACKAGE_BUILD
#undef CREATE_TRACE_POINTS

struct trace_syscall_entry {
	void *func;
	unsigned int nrargs;
};

#define CREATE_SYSCALL_TABLE

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_name, _nr, _nrargs)	\
	[ _nr ] = { .func = __event_probe__##_name, .nrargs = (_nrargs) },

static struct trace_syscall_entry sc_table[] = {
#include "../instrumentation/syscalls/headers/syscalls.h"
};
#undef CREATE_SYSCALL_TABLE

#endif	/* SYSCALL_DETAIL */

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng sched probes");
