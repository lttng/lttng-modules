/*
 * probes/lttng-probe-block.c
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng block probes.
 */

#include <linux/module.h>
#include <linux/blktrace_api.h>

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#undef CONFIG_EVENT_TRACING	/* Don't create Ftrace probes */

#include <trace/events/block.h>

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng block probes");
