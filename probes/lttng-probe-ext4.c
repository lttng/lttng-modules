/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * probes/lttng-probe-ext4.c
 *
 * LTTng ext4 probes.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright (C) 2012 Mentor Graphics Corp.
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <../fs/ext4/ext4.h>
#include <../fs/ext4/mballoc.h>
#include <../fs/ext4/ext4_extents.h>
#include <linux/dcache.h>
#include <lttng-tracer.h>

/*
 * Create the tracepoint static inlines from the kernel to validate that our
 * trace event macros match the kernel we run on.
 */
#include <trace/events/ext4.h>

#include <lttng-kernel-version.h>
#include <wrapper/tracepoint.h>

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH instrumentation/events/lttng-module

#include <instrumentation/events/lttng-module/ext4.h>

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Andrew Gabbasov <andrew_gabbasov@mentor.com>");
MODULE_DESCRIPTION("LTTng ext4 probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
