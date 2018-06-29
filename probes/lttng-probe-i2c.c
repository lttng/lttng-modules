/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * probes/lttng-probe-i2c.c
 *
 * LTTng i2c probes.
 *
 * Copyright (C) 2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright (C) 2016 Simon Marchi <simon.marchi@ericsson.com>
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <lttng-tracer.h>

/*
 * Create the tracepoint static inlines from the kernel to validate that our
 * trace event macros match the kernel we run on.
 */
#include <trace/events/i2c.h>

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH instrumentation/events/lttng-module

static int extract_sensitive_payload;
module_param(extract_sensitive_payload, int, 0644);
MODULE_PARM_DESC(extract_sensitive_payload,
		"Whether to extract possibly sensitive data from events (i2c "
		"buffer contents) or not (1 or 0, default: 0).");

#include <instrumentation/events/lttng-module/i2c.h>

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Simon Marchi <simon.marchi@ericsson.com>");
MODULE_DESCRIPTION("LTTng i2c probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
