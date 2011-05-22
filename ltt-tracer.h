/*
 * Copyright (C) 2005,2006,2008 Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * This contains the definitions for the Linux Trace Toolkit tracer.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#ifndef _LTT_TRACER_H
#define _LTT_TRACER_H

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

/* Maximum number of callbacks per marker */
#define LTT_NR_CALLBACKS	10

struct ltt_serialize_closure;

/* Serialization callback */
typedef size_t (*ltt_serialize_cb)(struct lib_ring_buffer *buf,
				   size_t buf_offset,
				   struct ltt_serialize_closure *closure,
				   void *serialize_private,
				   unsigned int stack_pos_ctx,
				   int *largest_align,
				   const char *fmt, va_list *args);

struct ltt_serialize_closure {
	ltt_serialize_cb *callbacks;
	long cb_args[LTT_NR_CALLBACKS];
	unsigned int cb_idx;
};

size_t ltt_serialize_data(struct lib_ring_buffer *buf, size_t buf_offset,
			  struct ltt_serialize_closure *closure,
			  void *serialize_private, unsigned int stack_pos_ctx,
			  int *largest_align, const char *fmt, va_list *args);

enum ltt_channels {
	LTT_CHANNEL_METADATA,
	LTT_CHANNEL_FD_STATE,
	LTT_CHANNEL_GLOBAL_STATE,
	LTT_CHANNEL_IRQ_STATE,
	LTT_CHANNEL_MODULE_STATE,
	LTT_CHANNEL_NETIF_STATE,
	LTT_CHANNEL_SOFTIRQ_STATE,
	LTT_CHANNEL_SWAP_STATE,
	LTT_CHANNEL_SYSCALL_STATE,
	LTT_CHANNEL_TASK_STATE,
	LTT_CHANNEL_VM_STATE,
	LTT_CHANNEL_FS,
	LTT_CHANNEL_INPUT,
	LTT_CHANNEL_IPC,
	LTT_CHANNEL_KERNEL,
	LTT_CHANNEL_MM,
	LTT_CHANNEL_RCU,
	LTT_CHANNEL_DEFAULT,
};

/*
 * Hardcoded event headers
 *
 * event header for a trace with active heartbeat : 27 bits timestamps
 *
 * headers are 32-bits aligned. In order to insure such alignment, a dynamic per
 * trace alignment value must be done.
 *
 * Remember that the C compiler does align each member on the boundary
 * equivalent to their own size.
 *
 * As relay subbuffers are aligned on pages, we are sure that they are 4 and 8
 * bytes aligned, so the buffer header and trace header are aligned.
 *
 * Event headers are aligned depending on the trace alignment option.
 *
 * Note using C structure bitfields for cross-endianness and portability
 * concerns.
 */

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

/*
 * Size reserved for high priority events (interrupts, NMI, BH) at the end of a
 * nearly full buffer. User space won't use this last amount of space when in
 * blocking mode. This space also includes the event header that would be
 * written by this user space event.
 */
#define LTT_RESERVE_CRITICAL		4096

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
