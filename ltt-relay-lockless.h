#ifndef _LTT_LTT_RELAY_LOCKLESS_H
#define _LTT_LTT_RELAY_LOCKLESS_H

/*
 * ltt/ltt-relay-lockless.h
 *
 * (C) Copyright 2005-2008 - Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * LTTng lockless buffer space management (reader/writer).
 *
 * Author:
 *	Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * Inspired from LTT :
 *  Karim Yaghmour (karim@opersys.com)
 *  Tom Zanussi (zanussi@us.ibm.com)
 *  Bob Wisniewski (bob@watson.ibm.com)
 * And from K42 :
 *  Bob Wisniewski (bob@watson.ibm.com)
 *
 * Changelog:
 *  08/10/08, Cleanup.
 *  19/10/05, Complete lockless mechanism.
 *  27/05/05, Modular redesign and rewrite.
 *
 * Userspace reader semantic :
 * while (poll fd != POLLHUP) {
 *   - ioctl RELAY_GET_SUBBUF_SIZE
 *   while (1) {
 *     - ioctl GET_SUBBUF
 *     - splice 1 subbuffer worth of data to a pipe
 *     - splice the data from pipe to disk/network
 *     - ioctl PUT_SUBBUF, check error value
 *       if err val < 0, previous subbuffer was corrupted.
 *   }
 * }
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/cache.h>
#include <linux/time.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/rcupdate.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/bitops.h>
#include <linux/fs.h>
#include <linux/smp_lock.h>
#include <linux/debugfs.h>
#include <linux/stat.h>
#include <linux/cpu.h>
#include <linux/pipe_fs_i.h>
#include <linux/splice.h>
#include <asm/atomic.h>
#include <asm/local.h>

#include "ltt-tracer.h"
#include "ltt-relay.h"

#if 0
#define printk_dbg(fmt, args...) printk(fmt, args)
#else
#define printk_dbg(fmt, args...)
#endif

struct commit_counters {
	local_t cc;
	local_t cc_sb;			/* Incremented _once_ at sb switch */
	local_t events;			/* Event count */
};

/* LTTng lockless logging buffer info */
struct ltt_chanbuf {
	struct ltt_chanbuf_alloc a;	/* Parent. First field. */
	/* First 32 bytes cache-hot cacheline */
	local_t offset;			/* Current offset in the buffer */
	struct commit_counters *commit_count;
					/* Commit count per sub-buffer */
	atomic_long_t consumed;		/*
					 * Current offset in the buffer
					 * standard atomic access (shared)
					 */
	unsigned long last_tsc;		/*
					 * Last timestamp written in the buffer.
					 */
	/* End of first 32 bytes cacheline */
#ifdef CONFIG_LTT_VMCORE
	local_t *commit_seq;		/* Consecutive commits */
#endif
	atomic_long_t active_readers;	/*
					 * Active readers count
					 * standard atomic access (shared)
					 */
	local_t events_lost;
	local_t corrupted_subbuffers;
	spinlock_t full_lock;		/*
					 * buffer full condition spinlock, only
					 * for userspace tracing blocking mode
					 * synchronization with reader.
					 */
	wait_queue_head_t write_wait;	/*
					 * Wait queue for blocking user space
					 * writers
					 */
	wait_queue_head_t read_wait;	/* reader wait queue */
	unsigned int finalized;		/* buffer has been finalized */
	struct timer_list switch_timer;	/* timer for periodical switch */
};

/*
 * A switch is done during tracing or as a final flush after tracing (so it
 * won't write in the new sub-buffer).
 */
enum force_switch_mode { FORCE_ACTIVE, FORCE_FLUSH };

extern
int ltt_reserve_slot_lockless_slow(struct ltt_chan *chan,
				   struct ltt_trace *trace, size_t data_size,
				   int largest_align, int cpu,
				   struct ltt_chanbuf **ret_buf,
				   size_t *slot_size, long *buf_offset,
				   u64 *tsc, unsigned int *rflags);

extern void ltt_force_switch_lockless_slow(struct ltt_chanbuf *buf,
					   enum force_switch_mode mode);

/*
 * Last TSC comparison functions. Check if the current TSC overflows
 * LTT_TSC_BITS bits from the last TSC read. Reads and writes last_tsc
 * atomically.
 */

#if (BITS_PER_LONG == 32)
static __inline__ void save_last_tsc(struct ltt_chanbuf *buf, u64 tsc)
{
	buf->last_tsc = (unsigned long)(tsc >> LTT_TSC_BITS);
}

static __inline__ int last_tsc_overflow(struct ltt_chanbuf *buf, u64 tsc)
{
	unsigned long tsc_shifted = (unsigned long)(tsc >> LTT_TSC_BITS);

	if (unlikely((tsc_shifted - buf->last_tsc)))
		return 1;
	else
		return 0;
}
#else
static __inline__ void save_last_tsc(struct ltt_chanbuf *buf, u64 tsc)
{
	buf->last_tsc = (unsigned long)tsc;
}

static __inline__ int last_tsc_overflow(struct ltt_chanbuf *buf, u64 tsc)
{
	if (unlikely((tsc - buf->last_tsc) >> LTT_TSC_BITS))
		return 1;
	else
		return 0;
}
#endif

extern
int ltt_chanbuf_create(struct ltt_chanbuf *buf, struct ltt_chan_alloc *chana,
		       int cpu);
extern void ltt_chanbuf_free(struct ltt_chanbuf *buf);
extern int ltt_chan_create(const char *base_filename, struct ltt_chan *chan,
			   struct dentry *parent, size_t sb_size, size_t n_sb,
			   int overwrite, struct ltt_trace *trace);
extern void ltt_chan_free(struct kref *kref);
extern void ltt_chan_remove_files(struct ltt_chan *chan);

/* Buffer access operations */

extern int ltt_chanbuf_open_read(struct ltt_chanbuf *buf);
extern void ltt_chanbuf_release_read(struct ltt_chanbuf *buf);
extern int ltt_chanbuf_get_subbuf(struct ltt_chanbuf *buf,
				  unsigned long *consumed);
extern int ltt_chanbuf_put_subbuf(struct ltt_chanbuf *buf,
				  unsigned long consumed);
extern void ltt_chan_start_switch_timer(struct ltt_chan *chan);
extern void ltt_chan_stop_switch_timer(struct ltt_chan *chan);

extern int ltt_relay_init(void);
extern void ltt_relay_exit(void);

static __inline__
unsigned long ltt_chanbuf_get_offset(struct ltt_chanbuf *buf)
{
	return local_read(&buf->offset);
}

static __inline__
unsigned long ltt_chanbuf_get_consumed(struct ltt_chanbuf *buf)
{
	return atomic_long_read(&buf->consumed);
}

static __inline__
int ltt_chanbuf_is_finalized(struct ltt_chanbuf *buf)
{
	return buf->finalized;
}

static __inline__
void ltt_reserve_push_reader(struct ltt_chanbuf *buf, struct ltt_chan *chan,
			     long offset)
{
	long consumed_old, consumed_new;

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
		 * If the buffer is not in overwrite mode, pushing the reader
		 * only happens if a sub-buffer is corrupted.
		 */
		if (unlikely((SUBBUF_TRUNC(offset, chan)
			      - SUBBUF_TRUNC(consumed_old, chan))
			     >= chan->a.buf_size))
			consumed_new = SUBBUF_ALIGN(consumed_old, chan);
		else
			return;
	} while (unlikely(atomic_long_cmpxchg(&buf->consumed, consumed_old,
					      consumed_new) != consumed_old));
}

#ifdef CONFIG_LTT_VMCORE
static __inline__
void ltt_vmcore_check_deliver(struct ltt_chanbuf *buf, long commit_count,
			      long idx)
{
	local_set(&buf->commit_seq[idx], commit_count);
}
#else
static __inline__
void ltt_vmcore_check_deliver(struct ltt_chanbuf *buf, long commit_count,
			      long idx)
{
}
#endif

static __inline__
void ltt_check_deliver(struct ltt_chanbuf *buf, struct ltt_chan *chan,
		       long offset, long commit_count, long idx)
{
	long old_commit_count = commit_count - chan->a.sb_size;

	/* Check if all commits have been done */
	if (unlikely((BUFFER_TRUNC(offset, chan) >> chan->a.n_sb_order)
		     - (old_commit_count & chan->commit_count_mask) == 0)) {
		/*
		 * If we succeeded in updating the cc_sb, we are delivering
		 * the subbuffer. Deals with concurrent updates of the "cc"
		 * value without adding a add_return atomic operation to the
		 * fast path.
		 */
		if (likely(local_cmpxchg(&buf->commit_count[idx].cc_sb,
					 old_commit_count, commit_count)
			   == old_commit_count)) {
			/*
			 * Set noref flag for this subbuffer.
			 */
			ltt_set_noref_flag(&buf->a, idx);
			ltt_vmcore_check_deliver(buf, commit_count, idx);
		}
	}
}


static __inline__
int ltt_poll_deliver(struct ltt_chanbuf *buf, struct ltt_chan *chan)
{
	long consumed_old, consumed_idx, commit_count, write_offset;

	consumed_old = atomic_long_read(&buf->consumed);
	consumed_idx = SUBBUF_INDEX(consumed_old, chan);
	commit_count = local_read(&buf->commit_count[consumed_idx].cc_sb);
	/*
	 * No memory barrier here, since we are only interested
	 * in a statistically correct polling result. The next poll will
	 * get the data is we are racing. The mb() that ensures correct
	 * memory order is in get_subbuf.
	 */
	write_offset = local_read(&buf->offset);

	/*
	 * Check that the subbuffer we are trying to consume has been
	 * already fully committed.
	 */

	if (((commit_count - chan->a.sb_size)
	     & chan->commit_count_mask)
	    - (BUFFER_TRUNC(consumed_old, chan)
	       >> chan->a.n_sb_order)
	    != 0)
		return 0;

	/*
	 * Check that we are not about to read the same subbuffer in
	 * which the writer head is.
	 */
	if ((SUBBUF_TRUNC(write_offset, chan)
	   - SUBBUF_TRUNC(consumed_old, chan))
	   == 0)
		return 0;

	return 1;

}

static __inline__
u32 get_read_sb_size(struct ltt_chanbuf *buf)
{
	struct ltt_subbuffer_header *header =
		(struct ltt_subbuffer_header *)
			ltt_relay_read_offset_address(&buf->a, 0);
	return header->sb_size;
}

/*
 * returns 0 if reserve ok, or 1 if the slow path must be taken.
 */
static __inline__
int ltt_relay_try_reserve(struct ltt_chanbuf *buf, struct ltt_chan *chan,
			  size_t data_size, u64 *tsc, unsigned int *rflags,
			  int largest_align, long *o_begin, long *o_end,
			  long *o_old, size_t *before_hdr_pad, size_t *size)
{
	*o_begin = local_read(&buf->offset);
	*o_old = *o_begin;

	*tsc = trace_clock_read64();

#ifdef CONFIG_LTT_VMCORE
	prefetch(&buf->commit_count[SUBBUF_INDEX(*o_begin, chan)]);
	prefetch(&buf->commit_seq[SUBBUF_INDEX(*o_begin, chan)]);
#else
	prefetchw(&buf->commit_count[SUBBUF_INDEX(*o_begin, chan)]);
#endif
	if (last_tsc_overflow(buf, *tsc))
		*rflags = LTT_RFLAG_ID_SIZE_TSC;

	if (unlikely(SUBBUF_OFFSET(*o_begin, chan) == 0))
		return 1;

	*size = ltt_get_header_size(chan, *o_begin, data_size, before_hdr_pad,
				    *rflags);
	*size += ltt_align(*o_begin + *size, largest_align) + data_size;
	if (unlikely((SUBBUF_OFFSET(*o_begin, chan) + *size) > chan->a.sb_size))
		return 1;

	/*
	 * Event fits in the current buffer and we are not on a switch
	 * boundary. It's safe to write.
	 */
	*o_end = *o_begin + *size;

	if (unlikely((SUBBUF_OFFSET(*o_end, chan)) == 0))
		/*
		 * The offset_end will fall at the very beginning of the next
		 * subbuffer.
		 */
		return 1;

	return 0;
}

static __inline__
int ltt_reserve_slot(struct ltt_chan *chan,
		     struct ltt_trace *trace, size_t data_size,
		     int largest_align, int cpu,
		     struct ltt_chanbuf **ret_buf,
		     size_t *slot_size, long *buf_offset, u64 *tsc,
		     unsigned int *rflags)
{
	struct ltt_chanbuf *buf = *ret_buf = per_cpu_ptr(chan->a.buf, cpu);
	long o_begin, o_end, o_old;
	size_t before_hdr_pad;

	/*
	 * Perform retryable operations.
	 */
	if (unlikely(__get_cpu_var(ltt_nesting) > 4)) {
		local_inc(&buf->events_lost);
		return -EPERM;
	}

	if (unlikely(ltt_relay_try_reserve(buf, chan, data_size, tsc, rflags,
					   largest_align, &o_begin, &o_end,
					   &o_old, &before_hdr_pad, slot_size)))
		goto slow_path;

	if (unlikely(local_cmpxchg(&buf->offset, o_old, o_end) != o_old))
		goto slow_path;

	/*
	 * Atomically update last_tsc. This update races against concurrent
	 * atomic updates, but the race will always cause supplementary full TSC
	 * events, never the opposite (missing a full TSC event when it would be
	 * needed).
	 */
	save_last_tsc(buf, *tsc);

	/*
	 * Push the reader if necessary
	 */
	ltt_reserve_push_reader(buf, chan, o_end - 1);

	/*
	 * Clear noref flag for this subbuffer.
	 */
	ltt_clear_noref_flag(&buf->a, SUBBUF_INDEX(o_end - 1, chan));

	*buf_offset = o_begin + before_hdr_pad;
	return 0;
slow_path:
	return ltt_reserve_slot_lockless_slow(chan, trace, data_size,
					      largest_align, cpu, ret_buf,
					      slot_size, buf_offset, tsc,
					      rflags);
}

/*
 * Force a sub-buffer switch for a per-cpu buffer. This operation is
 * completely reentrant : can be called while tracing is active with
 * absolutely no lock held.
 *
 * Note, however, that as a local_cmpxchg is used for some atomic
 * operations, this function must be called from the CPU which owns the buffer
 * for a ACTIVE flush.
 */
static __inline__
void ltt_force_switch(struct ltt_chanbuf *buf, enum force_switch_mode mode)
{
	return ltt_force_switch_lockless_slow(buf, mode);
}

/*
 * for flight recording. must be called after relay_commit.
 * This function increments the subbuffer's commit_seq counter each time the
 * commit count reaches back the reserve offset (module subbuffer size). It is
 * useful for crash dump.
 */
#ifdef CONFIG_LTT_VMCORE
static __inline__
void ltt_write_commit_counter(struct ltt_chanbuf *buf, struct ltt_chan *chan,
			      long idx, long buf_offset, long commit_count,
			      size_t data_size)
{
	long offset;
	long commit_seq_old;

	offset = buf_offset + data_size;

	/*
	 * SUBBUF_OFFSET includes commit_count_mask. We can simply
	 * compare the offsets within the subbuffer without caring about
	 * buffer full/empty mismatch because offset is never zero here
	 * (subbuffer header and event headers have non-zero length).
	 */
	if (unlikely(SUBBUF_OFFSET(offset - commit_count, chan)))
		return;

	commit_seq_old = local_read(&buf->commit_seq[idx]);
	while (commit_seq_old < commit_count)
		commit_seq_old = local_cmpxchg(&buf->commit_seq[idx],
					 commit_seq_old, commit_count);
}
#else
static __inline__
void ltt_write_commit_counter(struct ltt_chanbuf *buf, struct ltt_chan *chan,
			      long idx, long buf_offset, long commit_count,
			      size_t data_size)
{
}
#endif

/*
 * Atomic unordered slot commit. Increments the commit count in the
 * specified sub-buffer, and delivers it if necessary.
 *
 * Parameters:
 *
 * @buf: buffer.
 * @chan: channel.
 * @buf_offset : offset following the event header.
 * @data_size : size of the event data.
 * @slot_size : size of the reserved slot.
 */
static __inline__
void ltt_commit_slot(struct ltt_chanbuf *buf, struct ltt_chan *chan,
		     long buf_offset, size_t data_size, size_t slot_size)
{
	long offset_end = buf_offset;
	long endidx = SUBBUF_INDEX(offset_end - 1, chan);
	long commit_count;

#ifdef LTT_NO_IPI_BARRIER
	smp_wmb();
#else
	/*
	 * Must write slot data before incrementing commit count.
	 * This compiler barrier is upgraded into a smp_mb() by the IPI
	 * sent by get_subbuf().
	 */
	barrier();
#endif
	local_add(slot_size, &buf->commit_count[endidx].cc);
	local_inc(&buf->commit_count[endidx].events);
	/*
	 * commit count read can race with concurrent OOO commit count updates.
	 * This is only needed for ltt_check_deliver (for non-polling delivery
	 * only) and for ltt_write_commit_counter. The race can only cause the
	 * counter to be read with the same value more than once, which could
	 * cause :
	 * - Multiple delivery for the same sub-buffer (which is handled
	 *   gracefully by the reader code) if the value is for a full
	 *   sub-buffer. It's important that we can never miss a sub-buffer
	 *   delivery. Re-reading the value after the local_add ensures this.
	 * - Reading a commit_count with a higher value that what was actually
	 *   added to it for the ltt_write_commit_counter call (again caused by
	 *   a concurrent committer). It does not matter, because this function
	 *   is interested in the fact that the commit count reaches back the
	 *   reserve offset for a specific sub-buffer, which is completely
	 *   independent of the order.
	 */
	commit_count = local_read(&buf->commit_count[endidx].cc);

	ltt_check_deliver(buf, chan, offset_end - 1, commit_count, endidx);
	/*
	 * Update data_size for each commit. It's needed only for extracting
	 * ltt buffers from vmcore, after crash.
	 */
	ltt_write_commit_counter(buf, chan, endidx, buf_offset,
				 commit_count, data_size);
}

#endif //_LTT_LTT_RELAY_LOCKLESS_H
