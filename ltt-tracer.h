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
#include <linux/trace-clock.h>
#include <asm/atomic.h>
#include <asm/local.h>

#include "ltt-tracer-core.h"
#include "ltt-events.h"

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

#if 0
size_t ltt_serialize_printf(struct lib_ring_buffer *buf, unsigned long buf_offset,
			    size_t *msg_size, char *output, size_t outlen,
			    const char *fmt);

/*
 * Unique ID assigned to each registered probe.
 */
enum marker_id {
	MARKER_ID_SET_MARKER_ID = 0,	/* Static IDs available (range 0-7) */
	MARKER_ID_SET_MARKER_FORMAT,
	MARKER_ID_COMPACT,		/* Compact IDs (range: 8-127)	    */
	MARKER_ID_DYNAMIC,		/* Dynamic IDs (range: 128-65535)   */
};

/* static ids 0-1 reserved for internal use. */
#define MARKER_CORE_IDS		2
static __inline__ enum marker_id marker_id_type(uint16_t id)
{
	if (id < MARKER_CORE_IDS)
		return (enum marker_id)id;
	else
		return MARKER_ID_DYNAMIC;
}

struct user_dbg_data {
	unsigned long avail_size;
	unsigned long write;
	unsigned long read;
};

enum trace_mode { LTT_TRACE_NORMAL, LTT_TRACE_FLIGHT, LTT_TRACE_HYBRID };

#define CHANNEL_FLAG_ENABLE	(1U<<0)
#define CHANNEL_FLAG_OVERWRITE	(1U<<1)
#endif //0

#if 0
/* Per-trace information - each trace/flight recorder represented by one */
struct ltt_trace {
	/* First 32 bytes cache-hot cacheline */
	struct list_head list;
	struct ltt_chan **channels;
	unsigned int nr_channels;
	int active;
	/* Second 32 bytes cache-hot cacheline */
	struct ltt_trace_ops *ops;
	u32 freq_scale;
	u64 start_freq;
	u64 start_tsc;
	unsigned long long start_monotonic;
	struct timeval		start_time;
	struct ltt_channel_setting *settings;
	struct {
		struct dentry			*trace_root;
		struct dentry			*ascii_root;
	} dentry;
	struct kref kref; /* Each channel has a kref of the trace struct */
	struct ltt_transport *transport;
	struct kref ltt_transport_kref;
	wait_queue_head_t kref_wq; /* Place for ltt_trace_destroy to sleep */
	char trace_name[NAME_MAX];
} ____cacheline_aligned;
#endif //0

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

#define LTT_RESERVED_EVENTS	3
#define LTT_EVENT_BITS		5
#define LTT_FREE_EVENTS		((1 << LTT_EVENT_BITS) - LTT_RESERVED_EVENTS)
#define LTT_TSC_BITS		27
#define LTT_TSC_MASK		((1 << LTT_TSC_BITS) - 1)

struct event_header {
	u32 id_time;		/* 5 bits event id (MSB); 27 bits time (LSB) */
};

/* Reservation flags */
#define	LTT_RFLAG_ID			(1 << 0)
#define	LTT_RFLAG_ID_SIZE		(1 << 1)
#define	LTT_RFLAG_ID_SIZE_TSC		(1 << 2)

#define LTT_MAX_SMALL_SIZE		0xFFFFU

/*
 * We use asm/timex.h : cpu_khz/HZ variable in here : we might have to deal
 * specifically with CPU frequency scaling someday, so using an interpolation
 * between the start and end of buffer values is not flexible enough. Using an
 * immediate frequency value permits to calculate directly the times for parts
 * of a buffer that would be before a frequency change.
 *
 * Keep the natural field alignment for _each field_ within this structure if
 * you ever add/remove a field from this header. Packed attribute is not used
 * because gcc generates poor code on at least powerpc and mips. Don't ever
 * let gcc add padding between the structure elements.
 */
struct packet_header {
	uint32_t magic;			/*
					 * Trace magic number.
					 * contains endianness information.
					 */
	uint8_t trace_uuid[16];
	uint32_t stream_id;
	uint64_t timestamp_begin;	/* Cycle count at subbuffer start */
	uint64_t timestamp_end;	/* Cycle count at subbuffer end */
	uint32_t content_size;		/* Size of data in subbuffer */
	uint32_t packet_size;		/* Subbuffer size (include padding) */
	uint32_t events_lost;		/*
					 * Events lost in this subbuffer since
					 * the beginning of the trace.
					 * (may overflow)
					 */
	/* TODO: move to metadata */
#if 0
	uint8_t major_version;
	uint8_t minor_version;
	uint8_t arch_size;		/* Architecture pointer size */
	uint8_t alignment;		/* LTT data alignment */
	uint64_t start_time_sec;	/* NTP-corrected start time */
	uint64_t start_time_usec;
	uint64_t start_freq;		/*
					 * Frequency at trace start,
					 * used all along the trace.
					 */
	uint32_t freq_scale;		/* Frequency scaling (divisor) */
#endif //0
	uint8_t header_end[0];		/* End of header */
};

static inline notrace u64 lib_ring_buffer_clock_read(struct channel *chan)
{
	return trace_clock_read64();
}

/*
 * record_header_size - Calculate the header size and padding necessary.
 * @config: ring buffer instance configuration
 * @chan: channel
 * @offset: offset in the write buffer
 * @data_size: size of the payload
 * @pre_header_padding: padding to add before the header (output)
 * @rflags: reservation flags
 * @ctx: reservation context
 *
 * Returns the event header size (including padding).
 *
 * Important note :
 * The event header must be 32-bits. The total offset calculated here :
 *
 * Alignment of header struct on 32 bits (min arch size, header size)
 * + sizeof(header struct)  (32-bits)
 * + (opt) u16 (ext. event id)
 * + (opt) u16 (event_size)
 *             (if event_size == LTT_MAX_SMALL_SIZE, has ext. event size)
 * + (opt) u32 (ext. event size)
 * + (opt) u64 full TSC (aligned on min(64-bits, arch size))
 *
 * The payload must itself determine its own alignment from the biggest type it
 * contains.
 */
static __inline__
unsigned char record_header_size(const struct lib_ring_buffer_config *config,
				 struct channel *chan, size_t offset,
				 size_t data_size, size_t *pre_header_padding,
				 unsigned int rflags,
				 struct lib_ring_buffer_ctx *ctx)
{
	size_t orig_offset = offset;
	size_t padding;

	BUILD_BUG_ON(sizeof(struct event_header) != sizeof(u32));

	padding = lib_ring_buffer_align(offset,
					sizeof(struct event_header));
	offset += padding;
	offset += sizeof(struct event_header);

	if (unlikely(rflags)) {
		switch (rflags) {
		case LTT_RFLAG_ID_SIZE_TSC:
			offset += sizeof(u16) + sizeof(u16);
			if (data_size >= LTT_MAX_SMALL_SIZE)
				offset += sizeof(u32);
			offset += ltt_align(offset, sizeof(u64));
			offset += sizeof(u64);
			break;
		case LTT_RFLAG_ID_SIZE:
			offset += sizeof(u16) + sizeof(u16);
			if (data_size >= LTT_MAX_SMALL_SIZE)
				offset += sizeof(u32);
			break;
		case LTT_RFLAG_ID:
			offset += sizeof(u16);
			break;
		}
	}

	*pre_header_padding = padding;
	return offset - orig_offset;
}

#include <linux/ringbuffer/api.h>

extern
void ltt_write_event_header_slow(const struct lib_ring_buffer_config *config,
				 struct lib_ring_buffer_ctx *ctx,
				 u16 eID, u32 event_size);

/*
 * ltt_write_event_header
 *
 * Writes the event header to the offset (already aligned on 32-bits).
 *
 * @config: ring buffer instance configuration
 * @ctx: reservation context
 * @eID : event ID
 * @event_size : size of the event, excluding the event header.
 */
static __inline__
void ltt_write_event_header(const struct lib_ring_buffer_config *config,
			    struct lib_ring_buffer_ctx *ctx,
			    u16 eID, u32 event_size)
{
	struct event_header header;

	if (unlikely(ctx->rflags))
		goto slow_path;

	header.id_time = eID << LTT_TSC_BITS;
	header.id_time |= (u32)ctx->tsc & LTT_TSC_MASK;
	lib_ring_buffer_write(config, ctx, &header, sizeof(header));

slow_path:
	ltt_write_event_header_slow(config, ctx, eID, event_size);
}

#if 0
/*
 * ltt_read_event_header
 * buf_offset must aligned on 32 bits
 */
static __inline__
size_t ltt_read_event_header(struct ltt_chanbuf_alloc *bufa, long buf_offset,
			     u64 *tsc, u32 *event_size, u16 *eID,
			     unsigned int *rflags)
{
	struct ltt_event_header header;
	u16 small_size;

	ltt_relay_read(bufa, buf_offset, &header, sizeof(header));
	buf_offset += sizeof(header);

	*event_size = INT_MAX;
	*eID = header.id_time >> LTT_TSC_BITS;
	*tsc = header.id_time & LTT_TSC_MASK;

	switch (*eID) {
	case 29:
		*rflags = LTT_RFLAG_ID_SIZE_TSC;
		ltt_relay_read(bufa, buf_offset, eID, sizeof(u16));
		buf_offset += sizeof(u16);
		ltt_relay_read(bufa, buf_offset, &small_size, sizeof(u16));
		buf_offset += sizeof(u16);
		if (small_size == LTT_MAX_SMALL_SIZE) {
			ltt_relay_read(bufa, buf_offset, event_size,
					sizeof(u32));
			buf_offset += sizeof(u32);
		} else
			*event_size = small_size;
		buf_offset += ltt_align(buf_offset, sizeof(u64));
		ltt_relay_read(bufa, buf_offset, tsc, sizeof(u64));
		buf_offset += sizeof(u64);
		break;
	case 30:
		*rflags = LTT_RFLAG_ID_SIZE;
		ltt_relay_read(bufa, buf_offset, eID, sizeof(u16));
		buf_offset += sizeof(u16);
		ltt_relay_read(bufa, buf_offset, &small_size, sizeof(u16));
		buf_offset += sizeof(u16);
		if (small_size == LTT_MAX_SMALL_SIZE) {
			ltt_relay_read(bufa, buf_offset, event_size,
					sizeof(u32));
			buf_offset += sizeof(u32);
		} else
			*event_size = small_size;
		break;
	case 31:
		*rflags = LTT_RFLAG_ID;
		ltt_relay_read(bufa, buf_offset, eID, sizeof(u16));
		buf_offset += sizeof(u16);
		break;
	default:
		*rflags = 0;
		break;
	}

	return buf_offset;
}
#endif //0

/* Tracer properties */
#define CTF_MAGIC_NUMBER		0xC1FC1FC1
#define LTT_TRACER_VERSION_MAJOR	3
#define LTT_TRACER_VERSION_MINOR	0

/**
 * ltt_write_trace_header - Write trace header
 * @priv: Private data (struct trace)
 * @header: Memory address where the information must be written to
 */
static __inline__
void write_trace_header(const struct lib_ring_buffer_config *config,
			struct packet_header *header)
{
	header->magic = CTF_MAGIC_NUMBER;
#if 0
	/* TODO: move start time to metadata */
	header->major_version = LTT_TRACER_VERSION_MAJOR;
	header->minor_version = LTT_TRACER_VERSION_MINOR;
	header->arch_size = sizeof(void *);
	header->alignment = lib_ring_buffer_get_alignment(config);
	header->start_time_sec = ltt_chan->session->start_time.tv_sec;
	header->start_time_usec = ltt_chan->session->start_time.tv_usec;
	header->start_freq = ltt_chan->session->start_freq;
	header->freq_scale = ltt_chan->session->freq_scale;
#endif //0
}

/*
 * Size reserved for high priority events (interrupts, NMI, BH) at the end of a
 * nearly full buffer. User space won't use this last amount of space when in
 * blocking mode. This space also includes the event header that would be
 * written by this user space event.
 */
#define LTT_RESERVE_CRITICAL		4096

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
