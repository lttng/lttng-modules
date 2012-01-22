#ifndef LTTNG_TRACER_CORE_H
#define LTTNG_TRACER_CORE_H

/*
 * lttng-tracer-core.h
 *
 * Copyright (C) 2005-2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This contains the core definitions for the Linux Trace Toolkit.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/list.h>
#include <linux/percpu.h>

#ifndef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
/* Align data on its natural alignment */
#define RING_BUFFER_ALIGN
#endif

#include "wrapper/ringbuffer/config.h"

struct lttng_session;
struct lttng_channel;
struct lttng_event;

#endif /* LTTNG_TRACER_CORE_H */
