/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * ringbuffer/frontend_internal.h
 *
 * Ring Buffer Library Synchronization Header (internal helpers).
 *
 * Copyright (C) 2005-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * See ring_buffer_frontend.c for more information on wait-free algorithms.
 */

#ifndef _LIB_RING_BUFFER_FRONTEND_INTERNAL_H
#define _LIB_RING_BUFFER_FRONTEND_INTERNAL_H

#include <ringbuffer/config.h>
#include <ringbuffer/backend_types.h>
#include <ringbuffer/frontend_types.h>
#include <lttng/abi.h>
#include <lttng/prio_heap.h>	/* For per-CPU read-side iterator */

/* Buffer offset macros */

/* buf_trunc mask selects only the buffer number. */
static inline
unsigned long buf_trunc(unsigned long offset, struct lttng_kernel_ring_buffer_channel *chan)
{
	return offset & ~(chan->backend.buf_size - 1);

}

/* Select the buffer number value (counter). */
static inline
unsigned long buf_trunc_val(unsigned long offset, struct lttng_kernel_ring_buffer_channel *chan)
{
	return buf_trunc(offset, chan) >> chan->backend.buf_size_order;
}

/* buf_offset mask selects only the offset within the current buffer. */
static inline
unsigned long buf_offset(unsigned long offset, struct lttng_kernel_ring_buffer_channel *chan)
{
	return offset & (chan->backend.buf_size - 1);
}

/* subbuf_offset mask selects the offset within the current subbuffer. */
static inline
unsigned long subbuf_offset(unsigned long offset, struct lttng_kernel_ring_buffer_channel *chan)
{
	return offset & (chan->backend.subbuf_size - 1);
}

/* subbuf_trunc mask selects the subbuffer number. */
static inline
unsigned long subbuf_trunc(unsigned long offset, struct lttng_kernel_ring_buffer_channel *chan)
{
	return offset & ~(chan->backend.subbuf_size - 1);
}

/* subbuf_align aligns the offset to the next subbuffer. */
static inline
unsigned long subbuf_align(unsigned long offset, struct lttng_kernel_ring_buffer_channel *chan)
{
	return (offset + chan->backend.subbuf_size)
	       & ~(chan->backend.subbuf_size - 1);
}

/* subbuf_index returns the index of the current subbuffer within the buffer. */
static inline
unsigned long subbuf_index(unsigned long offset, struct lttng_kernel_ring_buffer_channel *chan)
{
	return buf_offset(offset, chan) >> chan->backend.subbuf_size_order;
}

/*
 * Last timestamp comparison functions. Check if the current timestamp
 * overflows timestamp_bits bits from the last timestamp read. When
 * overflows are detected, the full 64-bit timestamp counter should be
 * written in the record header. Reads and writes last_timestamp
 * atomically.
 */

#if (BITS_PER_LONG == 32)
static inline
void save_last_timestamp(const struct lttng_kernel_ring_buffer_config *config,
		   struct lttng_kernel_ring_buffer *buf, u64 timestamp)
{
	if (config->timestamp_bits == 0 || config->timestamp_bits == 64)
		return;

	/*
	 * Ensure the compiler performs this update in a single instruction.
	 */
	v_set(config, &buf->last_timestamp, (unsigned long)(timestamp >> config->timestamp_bits));
}

static inline
int last_timestamp_overflow(const struct lttng_kernel_ring_buffer_config *config,
		      struct lttng_kernel_ring_buffer *buf, u64 timestamp)
{
	unsigned long timestamp_shifted;

	if (config->timestamp_bits == 0 || config->timestamp_bits == 64)
		return 0;

	timestamp_shifted = (unsigned long)(timestamp >> config->timestamp_bits);
	if (unlikely(timestamp_shifted
		     - (unsigned long)v_read(config, &buf->last_timestamp)))
		return 1;
	else
		return 0;
}
#else
static inline
void save_last_timestamp(const struct lttng_kernel_ring_buffer_config *config,
		   struct lttng_kernel_ring_buffer *buf, u64 timestamp)
{
	if (config->timestamp_bits == 0 || config->timestamp_bits == 64)
		return;

	v_set(config, &buf->last_timestamp, (unsigned long)timestamp);
}

static inline
int last_timestamp_overflow(const struct lttng_kernel_ring_buffer_config *config,
		      struct lttng_kernel_ring_buffer *buf, u64 timestamp)
{
	if (config->timestamp_bits == 0 || config->timestamp_bits == 64)
		return 0;

	if (unlikely((timestamp - v_read(config, &buf->last_timestamp))
		     >> config->timestamp_bits))
		return 1;
	else
		return 0;
}
#endif

extern
int lib_ring_buffer_reserve_slow(struct lttng_kernel_ring_buffer_ctx *ctx,
		void *client_ctx);

extern
void lib_ring_buffer_switch_slow(struct lttng_kernel_ring_buffer *buf,
				 enum switch_mode mode);

extern
void lib_ring_buffer_check_deliver_slow(const struct lttng_kernel_ring_buffer_config *config,
				   struct lttng_kernel_ring_buffer *buf,
			           struct lttng_kernel_ring_buffer_channel *chan,
			           unsigned long offset,
				   unsigned long commit_count,
			           unsigned long idx,
				   const struct lttng_kernel_ring_buffer_ctx *ctx);

extern
void lib_ring_buffer_switch_remote(struct lttng_kernel_ring_buffer *buf);
extern
void lib_ring_buffer_switch_remote_empty(struct lttng_kernel_ring_buffer *buf);
extern
int lib_ring_buffer_switch_remote_or_populate_packet(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer *buf,
		void __user *packet,
		u64 *packet_length,
		u64 *packet_length_padded,
		bool *flush_done,
		bool *packet_populated);
extern
void lib_ring_buffer_clear(struct lttng_kernel_ring_buffer *buf);

/* Buffer write helpers */

static inline
void lib_ring_buffer_reserve_push_reader(struct lttng_kernel_ring_buffer *buf,
					 struct lttng_kernel_ring_buffer_channel *chan,
					 unsigned long offset)
{
	unsigned long consumed_old, consumed_new;

	do {
		consumed_old = atomic_long_read(&buf->consumed);
		/*
		 * If buffer is in overwrite mode, push the reader consumed
		 * count if the write position has reached it and we are not
		 * at the first iteration (don't push the reader farther than
		 * the writer). This operation can be done concurrently by many
		 * writers in the same buffer, the writer being at the farthest
		 * write position sub-buffer index in the buffer being the one
		 * which will win this loop.
		 */
		if (unlikely(subbuf_trunc(offset, chan)
			      - subbuf_trunc(consumed_old, chan)
			     >= chan->backend.buf_size))
			consumed_new = subbuf_align(consumed_old, chan);
		else
			return;
	} while (unlikely(atomic_long_cmpxchg(&buf->consumed, consumed_old,
					      consumed_new) != consumed_old));
}

/*
 * Move consumed position to the beginning of subbuffer in which the
 * write offset is. Should only be used on ring buffers that are not
 * actively being written into, because clear_reader does not take into
 * account the commit counters when moving the consumed position, which
 * can make concurrent trace producers or consumers observe consumed
 * position further than the write offset, which breaks ring buffer
 * algorithm guarantees.
 */
static inline
void lib_ring_buffer_clear_reader(struct lttng_kernel_ring_buffer *buf,
				  struct lttng_kernel_ring_buffer_channel *chan)
{
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	unsigned long offset, consumed_old, consumed_new;

	do {
		offset = v_read(config, &buf->offset);
		consumed_old = atomic_long_read(&buf->consumed);
		CHAN_WARN_ON(chan, (long) (subbuf_trunc(offset, chan)
				- subbuf_trunc(consumed_old, chan))
				< 0);
		consumed_new = subbuf_trunc(offset, chan);
	} while (unlikely(atomic_long_cmpxchg(&buf->consumed, consumed_old,
					      consumed_new) != consumed_old));
}

static inline
int lib_ring_buffer_pending_data(const struct lttng_kernel_ring_buffer_config *config,
				 struct lttng_kernel_ring_buffer *buf,
				 struct lttng_kernel_ring_buffer_channel *chan)
{
	return !!subbuf_offset(v_read(config, &buf->offset), chan);
}

static inline
unsigned long lib_ring_buffer_get_data_size(const struct lttng_kernel_ring_buffer_config *config,
					    struct lttng_kernel_ring_buffer *buf,
					    unsigned long idx)
{
	return subbuffer_get_data_size(config, &buf->backend, idx);
}

/*
 * Check if all space reservation in a buffer have been committed. This helps
 * knowing if an execution context is nested (for per-cpu buffers only).
 * This is a very specific ftrace use-case, so we keep this as "internal" API.
 */
static inline
int lib_ring_buffer_reserve_committed(const struct lttng_kernel_ring_buffer_config *config,
				      struct lttng_kernel_ring_buffer *buf,
				      struct lttng_kernel_ring_buffer_channel *chan)
{
	unsigned long offset, idx, commit_count;

	CHAN_WARN_ON(chan, config->alloc != RING_BUFFER_ALLOC_PER_CPU);
	CHAN_WARN_ON(chan, config->sync != RING_BUFFER_SYNC_PER_CPU);

	/*
	 * Read offset and commit count in a loop so they are both read
	 * atomically wrt interrupts. By deal with interrupt concurrency by
	 * restarting both reads if the offset has been pushed. Note that given
	 * we only have to deal with interrupt concurrency here, an interrupt
	 * modifying the commit count will also modify "offset", so it is safe
	 * to only check for offset modifications.
	 */
	do {
		offset = v_read(config, &buf->offset);
		idx = subbuf_index(offset, chan);
		commit_count = v_read(config, &buf->commit_hot[idx].cc);
	} while (offset != v_read(config, &buf->offset));

	return ((buf_trunc(offset, chan) >> chan->backend.num_subbuf_order)
		     - (commit_count & chan->commit_count_mask) == 0);
}

/*
 * Receive end of subbuffer timestamp as parameter. It has been read in the
 * space reservation loop of either reserve or switch, which ensures it
 * progresses monotonically with event records in the buffer. Therefore,
 * it ensures that the end timestamp of a subbuffer is <= begin
 * timestamp of the following subbuffers.
 */
static inline
void lib_ring_buffer_check_deliver(const struct lttng_kernel_ring_buffer_config *config,
				   struct lttng_kernel_ring_buffer *buf,
			           struct lttng_kernel_ring_buffer_channel *chan,
			           unsigned long offset,
				   unsigned long commit_count,
			           unsigned long idx,
				   const struct lttng_kernel_ring_buffer_ctx *ctx)
{
	unsigned long old_commit_count = commit_count
					 - chan->backend.subbuf_size;

	/* Check if all commits have been done */
	if (unlikely((buf_trunc(offset, chan) >> chan->backend.num_subbuf_order)
		     - (old_commit_count & chan->commit_count_mask) == 0))
		lib_ring_buffer_check_deliver_slow(config, buf, chan, offset,
			commit_count, idx, ctx);
}

/*
 * lib_ring_buffer_write_commit_counter
 *
 * For flight recording. must be called after commit.
 * This function increments the subbuffer's commit_seq counter each time the
 * commit count reaches back the reserve offset (modulo subbuffer size). It is
 * useful for crash dump.
 */
static inline
void lib_ring_buffer_write_commit_counter(const struct lttng_kernel_ring_buffer_config *config,
					  struct lttng_kernel_ring_buffer *buf,
				          struct lttng_kernel_ring_buffer_channel *chan,
				          unsigned long buf_offset,
				          unsigned long commit_count,
					  struct commit_counters_hot *cc_hot)
{
	unsigned long commit_seq_old;

	if (config->oops != RING_BUFFER_OOPS_CONSISTENCY)
		return;

	/*
	 * subbuf_offset includes commit_count_mask. We can simply
	 * compare the offsets within the subbuffer without caring about
	 * buffer full/empty mismatch because offset is never zero here
	 * (subbuffer header and record headers have non-zero length).
	 */
	if (unlikely(subbuf_offset(buf_offset - commit_count, chan)))
		return;

	commit_seq_old = v_read(config, &cc_hot->seq);
	if (likely((long) (commit_seq_old - commit_count) < 0))
		v_set(config, &cc_hot->seq, commit_count);
}

extern int lib_ring_buffer_create(struct lttng_kernel_ring_buffer *buf,
				  struct channel_backend *chanb, int cpu);
extern void lib_ring_buffer_free(struct lttng_kernel_ring_buffer *buf);

extern int lib_ring_buffer_packet_initialize(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer *buf,
		void __user *packet,
		u64 begin_timestamp,
		u64 end_timestamp,
		u64 sequence_number,
		u64 *packet_length,
		u64 *packet_length_padded);

/* Keep track of trap nesting inside ring buffer code */
DECLARE_PER_CPU(unsigned int, lib_ring_buffer_nesting);

#endif /* _LIB_RING_BUFFER_FRONTEND_INTERNAL_H */
