/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * ringbuffer/config.h
 *
 * Ring buffer configuration header. Note: after declaring the standard inline
 * functions, clients should also include linux/ringbuffer/api.h.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LIB_RING_BUFFER_CONFIG_H
#define _LIB_RING_BUFFER_CONFIG_H

#include <linux/types.h>
#include <linux/percpu.h>
#include <lttng/align.h>
#include <lttng/tracer-core.h>

struct lttng_kernel_ring_buffer;
struct lttng_kernel_ring_buffer_channel;
struct lttng_kernel_ring_buffer_config;
struct lttng_kernel_ring_buffer_ctx;
struct lttng_kernel_ring_buffer_ctx_private;

/*
 * Ring buffer client callbacks. Only used by slow path, never on fast path.
 * For the fast path, record_header_size(), ring_buffer_clock_read() should be
 * provided as inline functions too.  These may simply return 0 if not used by
 * the client.
 */
struct lttng_kernel_ring_buffer_client_cb {
	/* Mandatory callbacks */

	/* A static inline version is also required for fast path */
	u64 (*ring_buffer_clock_read) (struct lttng_kernel_ring_buffer_channel *chan);
	size_t (*record_header_size) (const struct lttng_kernel_ring_buffer_config *config,
				      struct lttng_kernel_ring_buffer_channel *chan, size_t offset,
				      size_t *pre_header_padding,
				      struct lttng_kernel_ring_buffer_ctx *ctx,
				      void *client_ctx);

	/* Slow path only, at subbuffer switch */
	size_t (*subbuffer_header_size) (void);
	void (*buffer_begin) (struct lttng_kernel_ring_buffer *buf, u64 timestamp,
			      unsigned int subbuf_idx);
	void (*buffer_end) (struct lttng_kernel_ring_buffer *buf, u64 timestamp,
			    unsigned int subbuf_idx, unsigned long data_size,
			    const struct lttng_kernel_ring_buffer_ctx *ctx);

	/* Optional callbacks (can be set to NULL) */

	/* Called at buffer creation/finalize */
	int (*buffer_create) (struct lttng_kernel_ring_buffer *buf, void *priv,
			      int cpu, const char *name);
	/*
	 * Clients should guarantee that no new reader handle can be opened
	 * after finalize.
	 */
	void (*buffer_finalize) (struct lttng_kernel_ring_buffer *buf, void *priv, int cpu);

	/*
	 * Extract header length, payload length and timestamp from event
	 * record. Used by buffer iterators. Timestamp is only used by channel
	 * iterator.
	 */
	void (*record_get) (const struct lttng_kernel_ring_buffer_config *config,
			    struct lttng_kernel_ring_buffer_channel *chan, struct lttng_kernel_ring_buffer *buf,
			    size_t offset, size_t *header_len,
			    size_t *payload_len, u64 *timestamp);
};

/*
 * Ring buffer instance configuration.
 *
 * Declare as "static const" within the client object to ensure the inline fast
 * paths can be optimized.
 *
 * alloc/sync pairs:
 *
 * RING_BUFFER_ALLOC_PER_CPU and RING_BUFFER_SYNC_PER_CPU :
 *   Per-cpu buffers with per-cpu synchronization. Tracing must be performed
 *   with preemption disabled (lib_ring_buffer_get_cpu() and
 *   lib_ring_buffer_put_cpu()).
 *
 * RING_BUFFER_ALLOC_PER_CPU and RING_BUFFER_SYNC_PER_CHANNEL :
 *   Per-cpu buffer with per-channel synchronization. Tracing can be performed with
 *   preemption enabled, statistically stays on the local buffers.
 *
 * RING_BUFFER_ALLOC_PER_CHANNEL and RING_BUFFER_SYNC_PER_CPU :
 *   Should only be used for buffers belonging to a single thread or protected
 *   by mutual exclusion by the client. Note that periodical sub-buffer switch
 *   should be disabled in this kind of configuration.
 *
 * RING_BUFFER_ALLOC_PER_CHANNEL and RING_BUFFER_SYNC_PER_CHANNEL :
 *   Per-channel shared buffer with per-channel synchronization.
 *
 * wakeup:
 *
 * RING_BUFFER_WAKEUP_BY_TIMER uses per-cpu timers to poll the
 * buffers and wake up readers if data is ready. Mainly useful for tracers which
 * don't want to call into the wakeup code on the tracing path. Use in
 * combination with "read_timer_interval" channel_create() argument.
 *
 * RING_BUFFER_WAKEUP_BY_WRITER directly wakes up readers when a subbuffer is
 * ready to read. Lower latencies before the reader is woken up. Mainly suitable
 * for drivers. Going through an "irq_work" allows triggering this type of wakeup
 * even from NMI context: the wakeup will be slightly delayed until the next
 * interrupts are handled.
 *
 * RING_BUFFER_WAKEUP_NONE does not perform any wakeup whatsoever. The client
 * has the responsibility to perform wakeups.
 */
struct lttng_kernel_ring_buffer_config {
	enum {
		RING_BUFFER_ALLOC_PER_CPU,
		RING_BUFFER_ALLOC_PER_CHANNEL,
	} alloc;
	enum {
		RING_BUFFER_SYNC_PER_CPU,	/* Wait-free */
		RING_BUFFER_SYNC_PER_CHANNEL,	/* Lock-free */
	} sync;
	enum {
		RING_BUFFER_OVERWRITE,		/* Overwrite when buffer full */
		RING_BUFFER_DISCARD,		/* Discard when buffer full */
	} mode;
	enum {
		RING_BUFFER_SPLICE,
		RING_BUFFER_MMAP,
		RING_BUFFER_READ,		/* TODO */
		RING_BUFFER_ITERATOR,
		RING_BUFFER_NONE,
	} output;
	enum {
		RING_BUFFER_PAGE,
		RING_BUFFER_VMAP,		/* TODO */
		RING_BUFFER_STATIC,		/* TODO */
	} backend;
	enum {
		RING_BUFFER_NO_OOPS_CONSISTENCY,
		RING_BUFFER_OOPS_CONSISTENCY,
	} oops;
	enum {
		RING_BUFFER_IPI_BARRIER,
		RING_BUFFER_NO_IPI_BARRIER,
	} ipi;
	enum {
		RING_BUFFER_WAKEUP_BY_TIMER,	/* wake up performed by timer */
		RING_BUFFER_WAKEUP_BY_WRITER,	/*
						 * writer wakes up reader through
						 * irq_work.
						 */
	} wakeup;
	/*
	 * timestamp_bits: timestamp bits saved at each record.
	 *   0 and 64 disable the timestamp compression scheme.
	 */
	unsigned int timestamp_bits;
	struct lttng_kernel_ring_buffer_client_cb cb;
};

/*
 * ring buffer private context
 *
 * Private context passed to lib_ring_buffer_reserve(), lib_ring_buffer_commit(),
 * lib_ring_buffer_try_discard_reserve(), lib_ring_buffer_align_ctx() and
 * lib_ring_buffer_write().
 *
 * Get struct lttng_kernel_ring_buffer_ctx parent with container_of().
 */

struct lttng_kernel_ring_buffer_ctx_private {
	/* input received by lib_ring_buffer_reserve(). */
	struct lttng_kernel_ring_buffer_channel *chan;			/* ring buffer channel */

	/* output from lib_ring_buffer_reserve() */
	int reserve_cpu;			/* processor id updated by the reserve */
	size_t slot_size;			/* size of the reserved slot */
	unsigned long buf_offset;		/* offset following the record header */
	unsigned long pre_offset;		/*
						 * Initial offset position _before_
						 * the record is written. Positioned
						 * prior to record header alignment
						 * padding.
						 */
	u64 timestamp;				/* time-stamp counter value */
	unsigned int rflags;			/* reservation flags */

	struct lttng_kernel_ring_buffer *buf;	/*
						 * buffer corresponding to processor id
						 * for this channel
						 */
	struct lttng_kernel_ring_buffer_backend_pages *backend_pages;

	/*
	 * Records lost counts are only loaded into these fields before
	 * reserving the last bytes from the ring buffer.
	 */
	unsigned long records_lost_full;
	unsigned long records_lost_wrap;
	unsigned long records_lost_big;
};

/*
 * ring buffer context
 *
 * Context passed to lib_ring_buffer_reserve(), lib_ring_buffer_commit(),
 * lib_ring_buffer_try_discard_reserve(), lib_ring_buffer_align_ctx() and
 * lib_ring_buffer_write().
 */
struct lttng_kernel_ring_buffer_ctx {
	/* Private ring buffer context, set by reserve callback. */
	struct lttng_kernel_ring_buffer_ctx_private priv;

	/* input received by lib_ring_buffer_reserve(), saved here. */
	void *client_priv;		/* Ring buffer client private data */

	size_t data_size;		/* size of payload */
	int largest_align;		/*
					 * alignment of the largest element
					 * in the payload
					 */
	struct lttng_kernel_probe_ctx *probe_ctx;	/* Probe context */
};

/**
 * lib_ring_buffer_ctx_init - initialize ring buffer context
 * @ctx: ring buffer context to initialize
 * @client_priv: client private data
 * @data_size: size of record data payload. It must be greater than 0.
 * @largest_align: largest alignment within data payload types
 */
static inline
void lib_ring_buffer_ctx_init(struct lttng_kernel_ring_buffer_ctx *ctx,
			      void *client_priv,
			      size_t data_size, int largest_align,
			      struct lttng_kernel_probe_ctx *probe_ctx)
{
	ctx->client_priv = client_priv;
	ctx->data_size = data_size;
	ctx->largest_align = largest_align;
	ctx->probe_ctx = probe_ctx;
}

/*
 * Reservation flags.
 *
 * RING_BUFFER_RFLAG_FULL_TIMESTAMP
 *
 * This flag is passed to record_header_size() and to the primitive used to
 * write the record header. It indicates that the full 64-bit time value is
 * needed in the record header. If this flag is not set, the record header needs
 * only to contain "timestamp_bits" bit of time value.
 *
 * Reservation flags can be added by the client, starting from
 * "(RING_BUFFER_FLAGS_END << 0)". It can be used to pass information from
 * record_header_size() to lib_ring_buffer_write_record_header().
 */
#define	RING_BUFFER_RFLAG_FULL_TIMESTAMP	(1U << 0)
#define RING_BUFFER_RFLAG_END			(1U << 1)

#ifndef LTTNG_TRACER_CORE_H
#error "lttng/tracer-core.h is needed for RING_BUFFER_ALIGN define"
#endif

/*
 * We need to define RING_BUFFER_ALIGN_ATTR so it is known early at
 * compile-time. We have to duplicate the "config->align" information and the
 * definition here because config->align is used both in the slow and fast
 * paths, but RING_BUFFER_ALIGN_ATTR is only available for the client code.
 */
#ifdef RING_BUFFER_ALIGN

# define RING_BUFFER_ALIGN_ATTR		/* Default arch alignment */

/*
 * Calculate the offset needed to align the type.
 * size_of_type must be non-zero.
 */
static inline
unsigned int lib_ring_buffer_align(size_t align_drift, size_t size_of_type)
{
	return offset_align(align_drift, size_of_type);
}

#else

# define RING_BUFFER_ALIGN_ATTR __attribute__((packed))

/*
 * Calculate the offset needed to align the type.
 * size_of_type must be non-zero.
 */
static inline
unsigned int lib_ring_buffer_align(size_t align_drift, size_t size_of_type)
{
	return 0;
}

#endif

/**
 * lib_ring_buffer_align_ctx - Align context offset on "alignment"
 * @ctx: ring buffer context.
 */
static inline
void lib_ring_buffer_align_ctx(struct lttng_kernel_ring_buffer_ctx *ctx,
			   size_t alignment)
{
	ctx->priv.buf_offset += lib_ring_buffer_align(ctx->priv.buf_offset,
						 alignment);
}

/*
 * lib_ring_buffer_check_config() returns 0 on success.
 * Used internally to check for valid configurations at channel creation.
 */
static inline
int lib_ring_buffer_check_config(const struct lttng_kernel_ring_buffer_config *config,
			     unsigned int switch_timer_interval,
			     unsigned int read_timer_interval)
{
	if (config->alloc == RING_BUFFER_ALLOC_PER_CHANNEL
	    && config->sync == RING_BUFFER_SYNC_PER_CPU
	    && switch_timer_interval)
		return -EINVAL;
	return 0;
}

#include <ringbuffer/vatomic.h>

#endif /* _LIB_RING_BUFFER_CONFIG_H */
