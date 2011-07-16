#ifndef _LTT_TRACER_H
#define _LTT_TRACER_H

/*
 * ltt-tracer.h
 *
 * Copyright (C) 2005-2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This contains the definitions for the Linux Trace Toolkit tracer.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <stdarg.h>
#include <linux/types.h>
#include <linux/limits.h>
#include <linux/list.h>
#include <linux/cache.h>
#include <linux/timex.h>
#include <linux/wait.h>
#include <asm/atomic.h>
#include <asm/local.h>

#include "wrapper/trace-clock.h"
#include "ltt-tracer-core.h"
#include "ltt-events.h"

#define LTTNG_VERSION 0
#define LTTNG_PATCHLEVEL 9
#define LTTNG_SUBLEVEL 1

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

/* Number of bytes to log with a read/write event */
#define LTT_LOG_RW_SIZE			32L
#define LTT_MAX_SMALL_SIZE		0xFFFFU

#ifdef RING_BUFFER_ALIGN
#define ltt_alignof(type)	__alignof__(type)
#else
#define ltt_alignof(type)	1
#endif

/* Tracer properties */
#define CTF_MAGIC_NUMBER		0xC1FC1FC1
#define TSDL_MAGIC_NUMBER		0x75D11D57
#define CTF_VERSION_MAJOR		0
#define CTF_VERSION_MINOR		1

/*
 * Number of milliseconds to retry before failing metadata writes on buffer full
 * condition. (10 seconds)
 */
#define LTTNG_METADATA_TIMEOUT_MSEC	10000

#define LTT_RFLAG_EXTENDED		RING_BUFFER_RFLAG_END
#define LTT_RFLAG_END			(LTT_RFLAG_EXTENDED << 1)

/* Register and unregister function pointers */

enum ltt_module_function {
	LTT_FUNCTION_RUN_FILTER,
	LTT_FUNCTION_FILTER_CONTROL,
	LTT_FUNCTION_STATEDUMP
};

extern int ltt_module_register(enum ltt_module_function name, void *function,
			       struct module *owner);
extern void ltt_module_unregister(enum ltt_module_function name);

/* Exported control function */

void ltt_core_register(int (*function)(u8, void *));

void ltt_core_unregister(void);

extern
void ltt_statedump_register_kprobes_dump(void (*callback)(void *call_data));
extern
void ltt_statedump_unregister_kprobes_dump(void (*callback)(void *call_data));

extern void ltt_dump_softirq_vec(void *call_data);

#ifdef CONFIG_HAVE_LTT_DUMP_TABLES
extern void ltt_dump_sys_call_table(void *call_data);
extern void ltt_dump_idt_table(void *call_data);
#else
static inline void ltt_dump_sys_call_table(void *call_data)
{
}

static inline void ltt_dump_idt_table(void *call_data)
{
}
#endif

#endif /* _LTT_TRACER_H */
