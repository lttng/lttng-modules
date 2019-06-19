/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * probes/lttng-probe-kallsyms.c
 *
 * LTTng kallsyms probes.
 *
 * Copyright (C) 2019 Geneviève Bastien <gbastien@versatic.net>
 */

#include <linux/module.h>
#include <linux/kallsyms.h>
#include <lttng-events.h>
#include <lttng-tracer.h>

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH instrumentation/events/lttng-module
#define TRACE_INCLUDE_FILE lttng-kallsyms

#include <instrumentation/events/lttng-module/lttng-kallsyms.h>

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Geneviève Bastien <gbastien@versatic.net");
MODULE_DESCRIPTION("LTTng kallsyms probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
