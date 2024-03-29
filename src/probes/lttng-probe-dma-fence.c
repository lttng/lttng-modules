/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * probes/lttng-probe-dma-fence.c
 *
 * LTTng dma-fence probes.
 *
 * Copyright (C) 2022 Pengutronix, Rouven Czerwinski <entwicklung@pengutronix.de>
 */

#include <linux/dma-fence.h>
/*
 * Create the tracepoint static inlines from the kernel to validate that our
 * trace event macros match the kernel we run on.
 */
#include <trace/events/dma_fence.h>

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH instrumentation/events

#include <instrumentation/events/dma_fence.h>

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Rouven Czerwinski <r.czerwinski@pengutronix.de>");
MODULE_DESCRIPTION("LTTng dma-fence probes");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
