/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * ringbuffer/backend_internal.h
 *
 * Ring buffer backend (internal helpers).
 *
 * Copyright (C) 2008-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LIB_RING_BUFFER_BACKEND_INTERNAL_H
#define _LIB_RING_BUFFER_BACKEND_INTERNAL_H

#include <wrapper/compiler.h>
#include <wrapper/inline_memcpy.h>
#include <ringbuffer/config.h>
#include <ringbuffer/backend_types.h>
#include <ringbuffer/frontend_types.h>
#include <linux/string.h>
#include <linux/uaccess.h>

/* Ring buffer backend API presented to the frontend */

/* Ring buffer and channel backend create/free */

int lib_ring_buffer_backend_create(struct lttng_kernel_ring_buffer_backend *bufb,
				   struct channel_backend *chan, int cpu);
void channel_backend_unregister_notifiers(struct channel_backend *chanb);
void lib_ring_buffer_backend_free(struct lttng_kernel_ring_buffer_backend *bufb);
int channel_backend_init(struct channel_backend *chanb,
			 const char *name,
			 const struct lttng_kernel_ring_buffer_config *config,
			 void *priv, size_t subbuf_size,
			 size_t num_subbuf);
void channel_backend_free(struct channel_backend *chanb);

void lib_ring_buffer_backend_reset(struct lttng_kernel_ring_buffer_backend *bufb);
void channel_backend_reset(struct channel_backend *chanb);

int lib_ring_buffer_backend_init(void);
void lib_ring_buffer_backend_exit(void);

extern void _lib_ring_buffer_write(struct lttng_kernel_ring_buffer_backend *bufb,
				   size_t offset, const void *src, size_t len);
extern void _lib_ring_buffer_memset(struct lttng_kernel_ring_buffer_backend *bufb,
				    size_t offset, int c, size_t len);
extern void _lib_ring_buffer_strcpy(struct lttng_kernel_ring_buffer_backend *bufb,
				   size_t offset, const char *src, size_t len, int pad);
extern void _lib_ring_buffer_pstrcpy(struct lttng_kernel_ring_buffer_backend *bufb,
				   size_t offset, const char *src, size_t len, int pad);
extern void _lib_ring_buffer_copy_from_user_inatomic(struct lttng_kernel_ring_buffer_backend *bufb,
					    size_t offset, const void __user *src, size_t len);
extern void _lib_ring_buffer_strcpy_from_user_inatomic(struct lttng_kernel_ring_buffer_backend *bufb,
		size_t offset, const char __user *src, size_t len, int pad);
extern void _lib_ring_buffer_pstrcpy_from_user_inatomic(struct lttng_kernel_ring_buffer_backend *bufb,
				   size_t offset, const char __user *src, size_t len, int pad);

/*
 * Subbuffer ID bits for overwrite mode. Need to fit within a single word to be
 * exchanged atomically.
 *
 * Top half word, except lowest bit, belongs to "offset", which is used to keep
 * to count the produced buffers.  For overwrite mode, this provides the
 * consumer with the capacity to read subbuffers in order, handling the
 * situation where producers would write up to 2^15 buffers (or 2^31 for 64-bit
 * systems) concurrently with a single execution of get_subbuf (between offset
 * sampling and subbuffer ID exchange).
 */

#define HALF_ULONG_BITS		(BITS_PER_LONG >> 1)

#define SB_ID_OFFSET_SHIFT	(HALF_ULONG_BITS + 1)
#define SB_ID_OFFSET_COUNT	(1UL << SB_ID_OFFSET_SHIFT)
#define SB_ID_OFFSET_MASK	(~(SB_ID_OFFSET_COUNT - 1))
/*
 * Lowest bit of top word half belongs to noref. Used only for overwrite mode.
 */
#define SB_ID_NOREF_SHIFT	(SB_ID_OFFSET_SHIFT - 1)
#define SB_ID_NOREF_COUNT	(1UL << SB_ID_NOREF_SHIFT)
#define SB_ID_NOREF_MASK	SB_ID_NOREF_COUNT
/*
 * In overwrite mode: lowest half of word is used for index.
 * Limit of 2^16 subbuffers per buffer on 32-bit, 2^32 on 64-bit.
 * In producer-consumer mode: whole word used for index.
 */
#define SB_ID_INDEX_SHIFT	0
#define SB_ID_INDEX_COUNT	(1UL << SB_ID_INDEX_SHIFT)
#define SB_ID_INDEX_MASK	(SB_ID_NOREF_COUNT - 1)

/*
 * Construct the subbuffer id from offset, index and noref. Use only the index
 * for producer-consumer mode (offset and noref are only used in overwrite
 * mode).
 */
static inline
unsigned long subbuffer_id(const struct lttng_kernel_ring_buffer_config *config,
			   unsigned long offset, unsigned long noref,
			   unsigned long index)
{
	if (config->mode == RING_BUFFER_OVERWRITE)
		return (offset << SB_ID_OFFSET_SHIFT)
		       | (noref << SB_ID_NOREF_SHIFT)
		       | index;
	else
		return index;
}

/*
 * Compare offset with the offset contained within id. Return 1 if the offset
 * bits are identical, else 0.
 */
static inline
int subbuffer_id_compare_offset(const struct lttng_kernel_ring_buffer_config *config,
				unsigned long id, unsigned long offset)
{
	return (id & SB_ID_OFFSET_MASK) == (offset << SB_ID_OFFSET_SHIFT);
}

static inline
unsigned long subbuffer_id_get_index(const struct lttng_kernel_ring_buffer_config *config,
				     unsigned long id)
{
	if (config->mode == RING_BUFFER_OVERWRITE)
		return id & SB_ID_INDEX_MASK;
	else
		return id;
}

static inline
unsigned long subbuffer_id_is_noref(const struct lttng_kernel_ring_buffer_config *config,
				    unsigned long id)
{
	if (config->mode == RING_BUFFER_OVERWRITE)
		return !!(id & SB_ID_NOREF_MASK);
	else
		return 1;
}

/*
 * Only used by reader on subbuffer ID it has exclusive access to. No volatile
 * needed.
 */
static inline
void subbuffer_id_set_noref(const struct lttng_kernel_ring_buffer_config *config,
			    unsigned long *id)
{
	if (config->mode == RING_BUFFER_OVERWRITE)
		*id |= SB_ID_NOREF_MASK;
}

static inline
void subbuffer_id_set_noref_offset(const struct lttng_kernel_ring_buffer_config *config,
				   unsigned long *id, unsigned long offset)
{
	unsigned long tmp;

	if (config->mode == RING_BUFFER_OVERWRITE) {
		tmp = *id;
		tmp &= ~SB_ID_OFFSET_MASK;
		tmp |= offset << SB_ID_OFFSET_SHIFT;
		tmp |= SB_ID_NOREF_MASK;
		/* Volatile store, read concurrently by readers. */
		WRITE_ONCE(*id, tmp);
	}
}

/* No volatile access, since already used locally */
static inline
void subbuffer_id_clear_noref(const struct lttng_kernel_ring_buffer_config *config,
			      unsigned long *id)
{
	if (config->mode == RING_BUFFER_OVERWRITE)
		*id &= ~SB_ID_NOREF_MASK;
}

/*
 * For overwrite mode, cap the number of subbuffers per buffer to:
 * 2^16 on 32-bit architectures
 * 2^32 on 64-bit architectures
 * This is required to fit in the index part of the ID. Return 0 on success,
 * -EPERM on failure.
 */
static inline
int subbuffer_id_check_index(const struct lttng_kernel_ring_buffer_config *config,
			     unsigned long num_subbuf)
{
	if (config->mode == RING_BUFFER_OVERWRITE)
		return (num_subbuf > (1UL << HALF_ULONG_BITS)) ? -EPERM : 0;
	else
		return 0;
}

static inline
void lib_ring_buffer_backend_get_pages(const struct lttng_kernel_ring_buffer_config *config,
			struct lttng_kernel_ring_buffer_ctx *ctx,
			struct lttng_kernel_ring_buffer_backend_pages **backend_pages)
{
	struct lttng_kernel_ring_buffer_backend *bufb = &ctx->priv.buf->backend;
	struct channel_backend *chanb = &ctx->priv.chan->backend;
	size_t sbidx, offset = ctx->priv.buf_offset;
	unsigned long sb_bindex, id;
	struct lttng_kernel_ring_buffer_backend_pages *rpages;

	offset &= chanb->buf_size - 1;
	sbidx = offset >> chanb->subbuf_size_order;
	id = bufb->buf_wsb[sbidx].id;
	sb_bindex = subbuffer_id_get_index(config, id);
	rpages = bufb->array[sb_bindex];
	CHAN_WARN_ON(ctx->priv.chan,
		     config->mode == RING_BUFFER_OVERWRITE
		     && subbuffer_id_is_noref(config, id));
	*backend_pages = rpages;
}

/* Get backend pages from cache. */
static inline
struct lttng_kernel_ring_buffer_backend_pages *
	lib_ring_buffer_get_backend_pages_from_ctx(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer_ctx *ctx)
{
	return ctx->priv.backend_pages;
}

/*
 * The ring buffer can count events recorded and overwritten per buffer,
 * but it is disabled by default due to its performance overhead.
 */
#ifdef LTTNG_RING_BUFFER_COUNT_EVENTS
static inline
void subbuffer_count_record(const struct lttng_kernel_ring_buffer_config *config,
			    struct lttng_kernel_ring_buffer_backend *bufb,
			    unsigned long idx)
{
	unsigned long sb_bindex;

	sb_bindex = subbuffer_id_get_index(config, bufb->buf_wsb[idx].id);
	v_inc(config, &bufb->array[sb_bindex]->records_commit);
}

/*
 * Reader has exclusive subbuffer access for record consumption. No need to
 * perform the decrement atomically.
 */
static inline
void subbuffer_consume_record(const struct lttng_kernel_ring_buffer_config *config,
			      struct lttng_kernel_ring_buffer_backend *bufb)
{
	unsigned long sb_bindex;

	sb_bindex = subbuffer_id_get_index(config, bufb->buf_rsb.id);
	CHAN_WARN_ON(bufb->chan,
		     !v_read(config, &bufb->array[sb_bindex]->records_unread));
	/* Non-atomic decrement protected by exclusive subbuffer access */
	_v_dec(config, &bufb->array[sb_bindex]->records_unread);
	v_inc(config, &bufb->records_read);
}
#else /* LTTNG_RING_BUFFER_COUNT_EVENTS */
static inline
void subbuffer_count_record(const struct lttng_kernel_ring_buffer_config *config,
			    struct lttng_kernel_ring_buffer_backend *bufb,
			    unsigned long idx)
{
}
static inline
void subbuffer_consume_record(const struct lttng_kernel_ring_buffer_config *config,
			      struct lttng_kernel_ring_buffer_backend *bufb)
{
}
#endif /* #else LTTNG_RING_BUFFER_COUNT_EVENTS */

static inline
unsigned long subbuffer_get_records_count(
				const struct lttng_kernel_ring_buffer_config *config,
				struct lttng_kernel_ring_buffer_backend *bufb,
				unsigned long idx)
{
	unsigned long sb_bindex;

	sb_bindex = subbuffer_id_get_index(config, bufb->buf_wsb[idx].id);
	return v_read(config, &bufb->array[sb_bindex]->records_commit);
}

/*
 * Must be executed at subbuffer delivery when the writer has _exclusive_
 * subbuffer access. See lib_ring_buffer_check_deliver() for details.
 * lib_ring_buffer_get_records_count() must be called to get the records
 * count before this function, because it resets the records_commit
 * count.
 */
static inline
unsigned long subbuffer_count_records_overrun(
				const struct lttng_kernel_ring_buffer_config *config,
				struct lttng_kernel_ring_buffer_backend *bufb,
				unsigned long idx)
{
	struct lttng_kernel_ring_buffer_backend_pages *pages;
	unsigned long overruns, sb_bindex;

	sb_bindex = subbuffer_id_get_index(config, bufb->buf_wsb[idx].id);
	pages = bufb->array[sb_bindex];
	overruns = v_read(config, &pages->records_unread);
	v_set(config, &pages->records_unread,
	      v_read(config, &pages->records_commit));
	v_set(config, &pages->records_commit, 0);

	return overruns;
}

static inline
void subbuffer_set_data_size(const struct lttng_kernel_ring_buffer_config *config,
			     struct lttng_kernel_ring_buffer_backend *bufb,
			     unsigned long idx,
			     unsigned long data_size)
{
	struct lttng_kernel_ring_buffer_backend_pages *pages;
	unsigned long sb_bindex;

	sb_bindex = subbuffer_id_get_index(config, bufb->buf_wsb[idx].id);
	pages = bufb->array[sb_bindex];
	pages->data_size = data_size;
}

static inline
unsigned long subbuffer_get_read_data_size(
				const struct lttng_kernel_ring_buffer_config *config,
				struct lttng_kernel_ring_buffer_backend *bufb)
{
	struct lttng_kernel_ring_buffer_backend_pages *pages;
	unsigned long sb_bindex;

	sb_bindex = subbuffer_id_get_index(config, bufb->buf_rsb.id);
	pages = bufb->array[sb_bindex];
	return pages->data_size;
}

static inline
unsigned long subbuffer_get_data_size(
				const struct lttng_kernel_ring_buffer_config *config,
				struct lttng_kernel_ring_buffer_backend *bufb,
				unsigned long idx)
{
	struct lttng_kernel_ring_buffer_backend_pages *pages;
	unsigned long sb_bindex;

	sb_bindex = subbuffer_id_get_index(config, bufb->buf_wsb[idx].id);
	pages = bufb->array[sb_bindex];
	return pages->data_size;
}

static inline
void subbuffer_inc_packet_count(const struct lttng_kernel_ring_buffer_config *config,
				struct lttng_kernel_ring_buffer_backend *bufb,
				unsigned long idx)
{
	/* Concurrently read by _lib_ring_buffer_switch_remote_or_populate_packet(). */
	WRITE_ONCE(bufb->buf_cnt[idx].seq_cnt, bufb->buf_cnt[idx].seq_cnt + 1);
}

/**
 * lib_ring_buffer_clear_noref - Clear the noref subbuffer flag, called by
 *                               writer.
 */
static inline
void lib_ring_buffer_clear_noref(const struct lttng_kernel_ring_buffer_config *config,
				 struct lttng_kernel_ring_buffer_backend *bufb,
				 unsigned long idx)
{
	unsigned long id, new_id;

	if (config->mode != RING_BUFFER_OVERWRITE)
		return;

	/*
	 * Performing a volatile access to read the sb_pages, because we want to
	 * read a coherent version of the pointer and the associated noref flag.
	 */
	id = LTTNG_READ_ONCE(bufb->buf_wsb[idx].id);
	for (;;) {
		/* This check is called on the fast path for each record. */
		if (likely(!subbuffer_id_is_noref(config, id))) {
			/*
			 * Store after load dependency ordering the writes to
			 * the subbuffer after load and test of the noref flag
			 * matches the memory barrier implied by the cmpxchg()
			 * in update_read_sb_index().
			 */
			return;	/* Already writing to this buffer */
		}
		new_id = id;
		subbuffer_id_clear_noref(config, &new_id);
		new_id = cmpxchg(&bufb->buf_wsb[idx].id, id, new_id);
		if (likely(new_id == id))
			break;
		id = new_id;
	}
}

/**
 * lib_ring_buffer_set_noref_offset - Set the noref subbuffer flag and offset,
 *                                    called by writer.
 */
static inline
void lib_ring_buffer_set_noref_offset(const struct lttng_kernel_ring_buffer_config *config,
				      struct lttng_kernel_ring_buffer_backend *bufb,
				      unsigned long idx, unsigned long offset)
{
	if (config->mode != RING_BUFFER_OVERWRITE)
		return;

	/*
	 * Because ring_buffer_set_noref() is only called by a single thread
	 * (the one which updated the cc_sb value), there are no concurrent
	 * updates to take care of: other writers have not updated cc_sb, so
	 * they cannot set the noref flag, and concurrent readers cannot modify
	 * the pointer because the noref flag is not set yet.
	 * The smp_wmb() in ring_buffer_commit() takes care of ordering writes
	 * to the subbuffer before this set noref operation.
	 * subbuffer_set_noref() uses a volatile store to deal with concurrent
	 * readers of the noref flag.
	 */
	CHAN_WARN_ON(bufb->chan,
		     subbuffer_id_is_noref(config, bufb->buf_wsb[idx].id));
	/*
	 * Memory barrier that ensures counter stores are ordered before set
	 * noref and offset.
	 */
	smp_mb();
	subbuffer_id_set_noref_offset(config, &bufb->buf_wsb[idx].id, offset);
}

/**
 * update_read_sb_index - Read-side subbuffer index update.
 */
static inline
int update_read_sb_index(const struct lttng_kernel_ring_buffer_config *config,
			 struct lttng_kernel_ring_buffer_backend *bufb,
			 struct channel_backend *chanb,
			 unsigned long consumed_idx,
			 unsigned long consumed_count)
{
	unsigned long old_id, new_id;

	if (config->mode == RING_BUFFER_OVERWRITE) {
		/*
		 * Exchange the target writer subbuffer with our own unused
		 * subbuffer. No need to use READ_ONCE() here to read the
		 * old_wpage, because the value read will be confirmed by the
		 * following cmpxchg().
		 */
		old_id = bufb->buf_wsb[consumed_idx].id;
		if (unlikely(!subbuffer_id_is_noref(config, old_id)))
			return -EAGAIN;
		/*
		 * Make sure the offset count we are expecting matches the one
		 * indicated by the writer.
		 */
		if (unlikely(!subbuffer_id_compare_offset(config, old_id,
							  consumed_count)))
			return -EAGAIN;
		CHAN_WARN_ON(bufb->chan,
			     !subbuffer_id_is_noref(config, bufb->buf_rsb.id));
		subbuffer_id_set_noref_offset(config, &bufb->buf_rsb.id,
					      consumed_count);
		new_id = cmpxchg(&bufb->buf_wsb[consumed_idx].id, old_id,
				 bufb->buf_rsb.id);
		if (unlikely(old_id != new_id))
			return -EAGAIN;
		bufb->buf_rsb.id = new_id;
	} else {
		/* No page exchange, use the writer page directly */
		bufb->buf_rsb.id = bufb->buf_wsb[consumed_idx].id;
	}
	return 0;
}

static inline __attribute__((always_inline))
void lttng_inline_memcpy(void *dest, const void *src,
		unsigned long len)
{
	switch (len) {
	case 1:
		*(uint8_t *) dest = *(const uint8_t *) src;
		break;
	case 2:
		*(uint16_t *) dest = *(const uint16_t *) src;
		break;
	case 4:
		*(uint32_t *) dest = *(const uint32_t *) src;
		break;
	case 8:
		*(uint64_t *) dest = *(const uint64_t *) src;
		break;
	default:
		inline_memcpy(dest, src, len);
	}
}

/*
 * Use the architecture-specific memcpy implementation for constant-sized
 * inputs, but rely on an inline memcpy for length statically unknown.
 * The function call to memcpy is just way too expensive for a fast path.
 */
#define lib_ring_buffer_do_copy(config, dest, src, len)		\
do {								\
	size_t __len = (len);					\
	if (__builtin_constant_p(len))				\
		memcpy(dest, src, __len);			\
	else							\
		lttng_inline_memcpy(dest, src, __len);		\
} while (0)

/*
 * We use __copy_from_user_inatomic to copy userspace data since we already
 * did the access_ok for the whole range.
 *
 * Return 0 if OK, nonzero on error.
 */
static inline
unsigned long lib_ring_buffer_do_copy_from_user_inatomic(void *dest,
						const void __user *src,
						unsigned long len)
{
	return __copy_from_user_inatomic(dest, src, len);
}

/*
 * write len bytes to dest with c
 */
static inline
void lib_ring_buffer_do_memset(char *dest, int c,
	unsigned long len)
{
	unsigned long i;

	for (i = 0; i < len; i++)
		dest[i] = c;
}

#endif /* _LIB_RING_BUFFER_BACKEND_INTERNAL_H */
