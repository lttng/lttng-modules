#ifndef _LIB_RING_BUFFER_BACKEND_H
#define _LIB_RING_BUFFER_BACKEND_H

/*
 * lib/ringbuffer/backend.h
 *
 * Ring buffer backend (API).
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Credits to Steven Rostedt for proposing to use an extra-subbuffer owned by
 * the reader in flight recorder mode.
 */

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/uaccess.h>

/* Internal helpers */
#include <wrapper/ringbuffer/backend_internal.h>
#include <wrapper/ringbuffer/frontend_internal.h>

/* Ring buffer backend API */

/* Ring buffer backend access (read/write) */

extern size_t lib_ring_buffer_read(struct lib_ring_buffer_backend *bufb,
				   size_t offset, void *dest, size_t len);

extern int __lib_ring_buffer_copy_to_user(struct lib_ring_buffer_backend *bufb,
					  size_t offset, void __user *dest,
					  size_t len);

extern int lib_ring_buffer_read_cstr(struct lib_ring_buffer_backend *bufb,
				     size_t offset, void *dest, size_t len);

extern unsigned long *
lib_ring_buffer_read_get_pfn(struct lib_ring_buffer_backend *bufb, size_t offset,
			      void ***virt);

/*
 * Return the address where a given offset is located.
 * Should be used to get the current subbuffer header pointer. Given we know
 * it's never on a page boundary, it's safe to write directly to this address,
 * as long as the write is never bigger than a page size.
 */
extern void *
lib_ring_buffer_offset_address(struct lib_ring_buffer_backend *bufb,
			       size_t offset);
extern void *
lib_ring_buffer_read_offset_address(struct lib_ring_buffer_backend *bufb,
				    size_t offset);

/**
 * lib_ring_buffer_write - write data to a buffer backend
 * @config : ring buffer instance configuration
 * @ctx: ring buffer context. (input arguments only)
 * @src : source pointer to copy from
 * @len : length of data to copy
 *
 * This function copies "len" bytes of data from a source pointer to a buffer
 * backend, at the current context offset. This is more or less a buffer
 * backend-specific memcpy() operation. Calls the slow path (_ring_buffer_write)
 * if copy is crossing a page boundary.
 */
static inline
void lib_ring_buffer_write(const struct lib_ring_buffer_config *config,
			   struct lib_ring_buffer_ctx *ctx,
			   const void *src, size_t len)
{
	struct lib_ring_buffer_backend *bufb = &ctx->buf->backend;
	struct channel_backend *chanb = &ctx->chan->backend;
	size_t sbidx, index, pagecpy;
	size_t offset = ctx->buf_offset;
	struct lib_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;

	if (unlikely(!len))
		return;
	offset &= chanb->buf_size - 1;
	sbidx = offset >> chanb->subbuf_size_order;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	pagecpy = min_t(size_t, len, (-offset) & ~PAGE_MASK);
	id = bufb->buf_wsb[sbidx].id;
	sb_bindex = subbuffer_id_get_index(config, id);
	rpages = bufb->array[sb_bindex];
	CHAN_WARN_ON(ctx->chan,
		     config->mode == RING_BUFFER_OVERWRITE
		     && subbuffer_id_is_noref(config, id));
	if (likely(pagecpy == len))
		lib_ring_buffer_do_copy(config,
					rpages->p[index].virt
					    + (offset & ~PAGE_MASK),
					src, len);
	else
		_lib_ring_buffer_write(bufb, offset, src, len, 0);
	ctx->buf_offset += len;
}

/**
 * lib_ring_buffer_memset - write len bytes of c to a buffer backend
 * @config : ring buffer instance configuration
 * @bufb : ring buffer backend
 * @offset : offset within the buffer
 * @c : the byte to copy
 * @len : number of bytes to copy
 *
 * This function writes "len" bytes of "c" to a buffer backend, at a specific
 * offset. This is more or less a buffer backend-specific memset() operation.
 * Calls the slow path (_ring_buffer_memset) if write is crossing a page
 * boundary.
 */
static inline
void lib_ring_buffer_memset(const struct lib_ring_buffer_config *config,
			    struct lib_ring_buffer_ctx *ctx, int c, size_t len)
{

	struct lib_ring_buffer_backend *bufb = &ctx->buf->backend;
	struct channel_backend *chanb = &ctx->chan->backend;
	size_t sbidx, index, pagecpy;
	size_t offset = ctx->buf_offset;
	struct lib_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;

	if (unlikely(!len))
		return;
	offset &= chanb->buf_size - 1;
	sbidx = offset >> chanb->subbuf_size_order;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	pagecpy = min_t(size_t, len, (-offset) & ~PAGE_MASK);
	id = bufb->buf_wsb[sbidx].id;
	sb_bindex = subbuffer_id_get_index(config, id);
	rpages = bufb->array[sb_bindex];
	CHAN_WARN_ON(ctx->chan,
		     config->mode == RING_BUFFER_OVERWRITE
		     && subbuffer_id_is_noref(config, id));
	if (likely(pagecpy == len))
		lib_ring_buffer_do_memset(rpages->p[index].virt
					  + (offset & ~PAGE_MASK),
					  c, len);
	else
		_lib_ring_buffer_memset(bufb, offset, c, len, 0);
	ctx->buf_offset += len;
}

/*
 * Copy up to @len string bytes from @src to @dest. Stop whenever a NULL
 * terminating character is found in @src. Returns the number of bytes
 * copied. Does *not* terminate @dest with NULL terminating character.
 */
static inline
size_t lib_ring_buffer_do_strcpy(const struct lib_ring_buffer_config *config,
		char *dest, const char *src, size_t len)
{
	size_t count;

	for (count = 0; count < len; count++) {
		char c;

		/*
		 * Only read source character once, in case it is
		 * modified concurrently.
		 */
		c = ACCESS_ONCE(src[count]);
		if (!c)
			break;
		lib_ring_buffer_do_copy(config, &dest[count], &c, 1);
	}
	return count;
}

/*
 * Copy up to @len string bytes from @src to @dest. Stop whenever a NULL
 * terminating character is found in @src, or when a fault occurs.
 * Returns the number of bytes copied. Does *not* terminate @dest with
 * NULL terminating character.
 *
 * This function deals with userspace pointers, it should never be called
 * directly without having the src pointer checked with access_ok()
 * previously.
 */
static inline
size_t lib_ring_buffer_do_strcpy_from_user_inatomic(const struct lib_ring_buffer_config *config,
		char *dest, const char __user *src, size_t len)
{
	size_t count;

	for (count = 0; count < len; count++) {
		int ret;
		char c;

		ret = __copy_from_user_inatomic(&c, src + count, 1);
		if (ret || !c)
			break;
		lib_ring_buffer_do_copy(config, &dest[count], &c, 1);
	}
	return count;
}

/**
 * lib_ring_buffer_strcpy - write string data to a buffer backend
 * @config : ring buffer instance configuration
 * @ctx: ring buffer context. (input arguments only)
 * @src : source pointer to copy from
 * @len : length of data to copy
 * @pad : character to use for padding
 *
 * This function copies @len - 1 bytes of string data from a source
 * pointer to a buffer backend, followed by a terminating '\0'
 * character, at the current context offset. This is more or less a
 * buffer backend-specific strncpy() operation. If a terminating '\0'
 * character is found in @src before @len - 1 characters are copied, pad
 * the buffer with @pad characters (e.g. '#'). Calls the slow path
 * (_ring_buffer_strcpy) if copy is crossing a page boundary.
 */
static inline
void lib_ring_buffer_strcpy(const struct lib_ring_buffer_config *config,
			   struct lib_ring_buffer_ctx *ctx,
			   const char *src, size_t len, int pad)
{
	struct lib_ring_buffer_backend *bufb = &ctx->buf->backend;
	struct channel_backend *chanb = &ctx->chan->backend;
	size_t sbidx, index, pagecpy;
	size_t offset = ctx->buf_offset;
	struct lib_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;

	if (unlikely(!len))
		return;
	offset &= chanb->buf_size - 1;
	sbidx = offset >> chanb->subbuf_size_order;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	pagecpy = min_t(size_t, len, (-offset) & ~PAGE_MASK);
	id = bufb->buf_wsb[sbidx].id;
	sb_bindex = subbuffer_id_get_index(config, id);
	rpages = bufb->array[sb_bindex];
	CHAN_WARN_ON(ctx->chan,
		     config->mode == RING_BUFFER_OVERWRITE
		     && subbuffer_id_is_noref(config, id));
	if (likely(pagecpy == len)) {
		size_t count;

		count = lib_ring_buffer_do_strcpy(config,
					rpages->p[index].virt
					    + (offset & ~PAGE_MASK),
					src, len - 1);
		offset += count;
		/* Padding */
		if (unlikely(count < len - 1)) {
			size_t pad_len = len - 1 - count;

			lib_ring_buffer_do_memset(rpages->p[index].virt
						+ (offset & ~PAGE_MASK),
					pad, pad_len);
			offset += pad_len;
		}
		/* Ending '\0' */
		lib_ring_buffer_do_memset(rpages->p[index].virt
					+ (offset & ~PAGE_MASK),
				'\0', 1);
	} else {
		_lib_ring_buffer_strcpy(bufb, offset, src, len, 0, pad);
	}
	ctx->buf_offset += len;
}

/**
 * lib_ring_buffer_copy_from_user_inatomic - write userspace data to a buffer backend
 * @config : ring buffer instance configuration
 * @ctx: ring buffer context. (input arguments only)
 * @src : userspace source pointer to copy from
 * @len : length of data to copy
 *
 * This function copies "len" bytes of data from a userspace pointer to a
 * buffer backend, at the current context offset. This is more or less a buffer
 * backend-specific memcpy() operation. Calls the slow path
 * (_ring_buffer_write_from_user_inatomic) if copy is crossing a page boundary.
 * Disable the page fault handler to ensure we never try to take the mmap_sem.
 */
static inline
void lib_ring_buffer_copy_from_user_inatomic(const struct lib_ring_buffer_config *config,
				    struct lib_ring_buffer_ctx *ctx,
				    const void __user *src, size_t len)
{
	struct lib_ring_buffer_backend *bufb = &ctx->buf->backend;
	struct channel_backend *chanb = &ctx->chan->backend;
	size_t sbidx, index, pagecpy;
	size_t offset = ctx->buf_offset;
	struct lib_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;
	unsigned long ret;
	mm_segment_t old_fs = get_fs();

	if (unlikely(!len))
		return;
	offset &= chanb->buf_size - 1;
	sbidx = offset >> chanb->subbuf_size_order;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	pagecpy = min_t(size_t, len, (-offset) & ~PAGE_MASK);
	id = bufb->buf_wsb[sbidx].id;
	sb_bindex = subbuffer_id_get_index(config, id);
	rpages = bufb->array[sb_bindex];
	CHAN_WARN_ON(ctx->chan,
		     config->mode == RING_BUFFER_OVERWRITE
		     && subbuffer_id_is_noref(config, id));

	set_fs(KERNEL_DS);
	pagefault_disable();
	if (unlikely(!access_ok(VERIFY_READ, src, len)))
		goto fill_buffer;

	if (likely(pagecpy == len)) {
		ret = lib_ring_buffer_do_copy_from_user_inatomic(
			rpages->p[index].virt + (offset & ~PAGE_MASK),
			src, len);
		if (unlikely(ret > 0)) {
			/* Copy failed. */
			goto fill_buffer;
		}
	} else {
		_lib_ring_buffer_copy_from_user_inatomic(bufb, offset, src, len, 0);
	}
	pagefault_enable();
	set_fs(old_fs);
	ctx->buf_offset += len;

	return;

fill_buffer:
	pagefault_enable();
	set_fs(old_fs);
	/*
	 * In the error path we call the slow path version to avoid
	 * the pollution of static inline code.
	 */
	_lib_ring_buffer_memset(bufb, offset, 0, len, 0);
}

/**
 * lib_ring_buffer_strcpy_from_user_inatomic - write userspace string data to a buffer backend
 * @config : ring buffer instance configuration
 * @ctx: ring buffer context (input arguments only)
 * @src : userspace source pointer to copy from
 * @len : length of data to copy
 * @pad : character to use for padding
 *
 * This function copies @len - 1 bytes of string data from a userspace
 * source pointer to a buffer backend, followed by a terminating '\0'
 * character, at the current context offset. This is more or less a
 * buffer backend-specific strncpy() operation. If a terminating '\0'
 * character is found in @src before @len - 1 characters are copied, pad
 * the buffer with @pad characters (e.g. '#'). Calls the slow path
 * (_ring_buffer_strcpy_from_user_inatomic) if copy is crossing a page
 * boundary. Disable the page fault handler to ensure we never try to
 * take the mmap_sem.
 */
static inline
void lib_ring_buffer_strcpy_from_user_inatomic(const struct lib_ring_buffer_config *config,
		struct lib_ring_buffer_ctx *ctx,
		const void __user *src, size_t len, int pad)
{
	struct lib_ring_buffer_backend *bufb = &ctx->buf->backend;
	struct channel_backend *chanb = &ctx->chan->backend;
	size_t sbidx, index, pagecpy;
	size_t offset = ctx->buf_offset;
	struct lib_ring_buffer_backend_pages *rpages;
	unsigned long sb_bindex, id;
	mm_segment_t old_fs = get_fs();

	if (unlikely(!len))
		return;
	offset &= chanb->buf_size - 1;
	sbidx = offset >> chanb->subbuf_size_order;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	pagecpy = min_t(size_t, len, (-offset) & ~PAGE_MASK);
	id = bufb->buf_wsb[sbidx].id;
	sb_bindex = subbuffer_id_get_index(config, id);
	rpages = bufb->array[sb_bindex];
	CHAN_WARN_ON(ctx->chan,
		     config->mode == RING_BUFFER_OVERWRITE
		     && subbuffer_id_is_noref(config, id));

	set_fs(KERNEL_DS);
	pagefault_disable();
	if (unlikely(!access_ok(VERIFY_READ, src, len)))
		goto fill_buffer;

	if (likely(pagecpy == len)) {
		size_t count;

		count = lib_ring_buffer_do_strcpy_from_user_inatomic(config,
					rpages->p[index].virt
					    + (offset & ~PAGE_MASK),
					src, len - 1);
		offset += count;
		/* Padding */
		if (unlikely(count < len - 1)) {
			size_t pad_len = len - 1 - count;

			lib_ring_buffer_do_memset(rpages->p[index].virt
						+ (offset & ~PAGE_MASK),
					pad, pad_len);
			offset += pad_len;
		}
		/* Ending '\0' */
		lib_ring_buffer_do_memset(rpages->p[index].virt
					+ (offset & ~PAGE_MASK),
				'\0', 1);
	} else {
		_lib_ring_buffer_strcpy_from_user_inatomic(bufb, offset, src,
					len, 0, pad);
	}
	pagefault_enable();
	set_fs(old_fs);
	ctx->buf_offset += len;

	return;

fill_buffer:
	pagefault_enable();
	set_fs(old_fs);
	/*
	 * In the error path we call the slow path version to avoid
	 * the pollution of static inline code.
	 */
	_lib_ring_buffer_memset(bufb, offset, pad, len - 1, 0);
	offset += len - 1;
	_lib_ring_buffer_memset(bufb, offset, '\0', 1, 0);
}

/*
 * This accessor counts the number of unread records in a buffer.
 * It only provides a consistent value if no reads not writes are performed
 * concurrently.
 */
static inline
unsigned long lib_ring_buffer_get_records_unread(
				const struct lib_ring_buffer_config *config,
				struct lib_ring_buffer *buf)
{
	struct lib_ring_buffer_backend *bufb = &buf->backend;
	struct lib_ring_buffer_backend_pages *pages;
	unsigned long records_unread = 0, sb_bindex, id;
	unsigned int i;

	for (i = 0; i < bufb->chan->backend.num_subbuf; i++) {
		id = bufb->buf_wsb[i].id;
		sb_bindex = subbuffer_id_get_index(config, id);
		pages = bufb->array[sb_bindex];
		records_unread += v_read(config, &pages->records_unread);
	}
	if (config->mode == RING_BUFFER_OVERWRITE) {
		id = bufb->buf_rsb.id;
		sb_bindex = subbuffer_id_get_index(config, id);
		pages = bufb->array[sb_bindex];
		records_unread += v_read(config, &pages->records_unread);
	}
	return records_unread;
}

#endif /* _LIB_RING_BUFFER_BACKEND_H */
