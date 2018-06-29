/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * probes/lttng-probe-ext3.c
 *
 * LTTng ext3 probes.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright (C) 2012 Mentor Graphics Corp.
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/dcache.h>
#include <linux/version.h>
#include <lttng-tracer.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
#include <../fs/ext3/ext3.h>
#else
#include <linux/ext3_fs_i.h>
#endif

/*
 * Create the tracepoint static inlines from the kernel to validate that our
 * trace event macros match the kernel we run on.
 */
#include <trace/events/ext3.h>

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH instrumentation/events/lttng-module

#include <instrumentation/events/lttng-module/ext3.h>

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Wade Farnsworth <wade_farnsworth@mentor.com>, Paul Woegerer <paul_woegerer@mentor.com>, and Andrew Gabbasov <andrew_gabbasov@mentor.com>");
MODULE_DESCRIPTION("LTTng ext3 probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
