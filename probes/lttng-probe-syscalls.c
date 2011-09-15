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
//test #define TRACE_INCLUDE_PATH ../instrumentation/syscalls/headers
#define TRACE_INCLUDE_PATH ../instrumentation/events/lttng-module

//test #include "../instrumentation/syscalls/headers/syscalls.h"
#include "../instrumentation/events/lttng-module/syscalls.h"

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng sched probes");
