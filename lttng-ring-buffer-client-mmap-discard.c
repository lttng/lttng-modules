/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-ring-buffer-client-discard.c
 *
 * LTTng lib ring buffer client (discard mode).
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <lttng-tracer.h>

#define RING_BUFFER_MODE_TEMPLATE		RING_BUFFER_DISCARD
#define RING_BUFFER_MODE_TEMPLATE_STRING	"discard-mmap"
#define RING_BUFFER_OUTPUT_TEMPLATE		RING_BUFFER_MMAP
#include "lttng-ring-buffer-client.h"
