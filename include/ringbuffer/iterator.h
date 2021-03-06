/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * ringbuffer/iterator.h
 *
 * Ring buffer and channel iterators.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LIB_RING_BUFFER_ITERATOR_H
#define _LIB_RING_BUFFER_ITERATOR_H

#include <ringbuffer/backend.h>
#include <ringbuffer/frontend.h>
#include <ringbuffer/vfs.h>

/*
 * lib_ring_buffer_get_next_record advances the buffer read position to the next
 * record. It returns either the size of the next record, -EAGAIN if there is
 * currently no data available, or -ENODATA if no data is available and buffer
 * is finalized.
 */
extern ssize_t lib_ring_buffer_get_next_record(struct lttng_kernel_ring_buffer_channel *chan,
					       struct lttng_kernel_ring_buffer *buf);

/*
 * Ensure that the current subbuffer is put after client code has read the
 * payload of the current record. Has an effect when the end of subbuffer is
 * reached. It is not required if get_next_record is called successively.
 * However, it should be invoked before returning data to user-space to ensure
 * that the get/put subbuffer state is quiescent.
 */
extern void lib_ring_buffer_put_current_record(struct lttng_kernel_ring_buffer *buf);

/*
 * channel_get_next_record advances the buffer read position to the next record.
 * It returns either the size of the next record, -EAGAIN if there is currently
 * no data available, or -ENODATA if no data is available and buffer is
 * finalized.
 * Returns the current buffer in ret_buf.
 */
extern ssize_t channel_get_next_record(struct lttng_kernel_ring_buffer_channel *chan,
				       struct lttng_kernel_ring_buffer **ret_buf);

/**
 * read_current_record - copy the buffer current record into dest.
 * @buf: ring buffer
 * @dest: destination where the record should be copied
 *
 * dest should be large enough to contain the record. Returns the number of
 * bytes copied.
 */
static inline size_t read_current_record(struct lttng_kernel_ring_buffer *buf, void *dest)
{
	return lib_ring_buffer_read(&buf->backend, buf->iter.read_offset,
				    dest, buf->iter.payload_len);
}

extern int lib_ring_buffer_iterator_open(struct lttng_kernel_ring_buffer *buf);
extern void lib_ring_buffer_iterator_release(struct lttng_kernel_ring_buffer *buf);
extern int channel_iterator_open(struct lttng_kernel_ring_buffer_channel *chan);
extern void channel_iterator_release(struct lttng_kernel_ring_buffer_channel *chan);

extern const struct file_operations channel_payload_file_operations;
extern const struct file_operations lib_ring_buffer_payload_file_operations;

/*
 * Used internally.
 */
int channel_iterator_init(struct lttng_kernel_ring_buffer_channel *chan);
void channel_iterator_unregister_notifiers(struct lttng_kernel_ring_buffer_channel *chan);
void channel_iterator_free(struct lttng_kernel_ring_buffer_channel *chan);
void channel_iterator_reset(struct lttng_kernel_ring_buffer_channel *chan);
void lib_ring_buffer_iterator_reset(struct lttng_kernel_ring_buffer *buf);

#endif /* _LIB_RING_BUFFER_ITERATOR_H */
