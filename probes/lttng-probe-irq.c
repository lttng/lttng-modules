/*
 * probes/lttng-probe-irq.c
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng irq probes.
 */

#include <linux/module.h>
#include <linux/interrupt.h>

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#undef CONFIG_EVENT_TRACING	/* Don't create Ftrace probes */

#include <trace/events/irq.h>

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng irq probes");
