/*
 * probes/lttng-probe-kvm.c
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng kvm probes.
 */

#include <linux/module.h>

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#undef CONFIG_EVENT_TRACING	/* Don't create Ftrace probes */

#include <linux/kvm_host.h>
#include <trace/events/kvm.h>

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng kvm probes");
