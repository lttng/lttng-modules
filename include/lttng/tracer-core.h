/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/tracer-core.h
 *
 * This contains the core definitions for the Linux Trace Toolkit Next
 * Generation tracer.
 *
 * Copyright (C) 2005-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef LTTNG_TRACER_CORE_H
#define LTTNG_TRACER_CORE_H

#include <linux/list.h>
#include <linux/percpu.h>

#ifndef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
/* Align data on its natural alignment */
#define RING_BUFFER_ALIGN
#endif

#include <ringbuffer/config.h>

struct lttng_session;
struct lttng_channel;
struct lttng_event;

#endif /* LTTNG_TRACER_CORE_H */
