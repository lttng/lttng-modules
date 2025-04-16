/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only) */
#ifndef _LTTNG_TRACER_H
#define _LTTNG_TRACER_H

/*
 * lttng/tracer.h
 *
 * This contains the definitions for the Linux Trace Toolkit Next
 * Generation tracer.
 *
 * Copyright (C) 2005-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/types.h>
#include <linux/limits.h>
#include <linux/list.h>
#include <linux/cache.h>
#include <linux/timex.h>
#include <linux/wait.h>
#include <asm/atomic.h>
#include <asm/local.h>

#include <wrapper/trace-clock.h>
#include <wrapper/compiler.h>
#include <wrapper/vmalloc.h>
#include <lttng/tracer-core.h>
#include <lttng/events.h>

#define LTTNG_MODULES_MAJOR_VERSION 2
#define LTTNG_MODULES_MINOR_VERSION 14
#define LTTNG_MODULES_PATCHLEVEL_VERSION 0
#define LTTNG_MODULES_EXTRAVERSION "-pre"

#define LTTNG_VERSION_NAME		"Orbitales"
#define LTTNG_VERSION_DESCRIPTION	"This release is named after \"Orbitales\", a bold creation from Noctem Artisans Brasseurs in Québec City. Pitch black and dense—one team member compared the color to used motor oil—this beer offers an intriguing sensory experience. Aged in wood casks, it delivers rich aromas and flavors of blackberry as well as a hint of acidity. There’s no head to speak of, but a whisper of blue cheese funk lingers just enough to keep you curious."

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

/* Number of bytes to log with a read/write event */
#define LTTNG_LOG_RW_SIZE		32L
#define LTTNG_MAX_SMALL_SIZE		0xFFFFU

#ifdef RING_BUFFER_ALIGN
#define lttng_alignof(type)	__alignof__(type)
#else
#define lttng_alignof(type)	1
#endif

/* Tracer properties */
#define CTF_MAGIC_NUMBER		0xC1FC1FC1
#define TSDL_MAGIC_NUMBER		0x75D11D57

/* CTF specification version followed */
#define CTF_SPEC_MAJOR			1
#define CTF_SPEC_MINOR			8

/*
 * Number of milliseconds to retry before failing metadata writes on buffer full
 * condition. (10 seconds)
 */
#define LTTNG_METADATA_TIMEOUT_MSEC	10000

#define LTTNG_RFLAG_EXTENDED		RING_BUFFER_RFLAG_END
#define LTTNG_RFLAG_END			(LTTNG_RFLAG_EXTENDED << 1)

#endif /* _LTTNG_TRACER_H */
