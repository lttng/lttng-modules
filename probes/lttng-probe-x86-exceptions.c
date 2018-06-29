/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * probes/lttng-probe-x86-exceptions.c
 *
 * LTTng x86 exceptions probes.
 *
 * Copyright (C) 2010-2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <lttng-tracer.h>

/*
 * Create the tracepoint static inlines from the kernel to validate that our
 * trace event macros match the kernel we run on.
 */
#include <asm/trace/exceptions.h>

#include <wrapper/tracepoint.h>

#undef TRACE_INCLUDE_PATH
#undef TRACE_INCLUDE_FILE

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH instrumentation/events/lttng-module/arch/x86

#include <instrumentation/events/lttng-module/arch/x86/exceptions.h>

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng x86 exceptions probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
