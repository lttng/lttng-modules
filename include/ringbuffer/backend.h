/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * ringbuffer/backend.h
 *
 * Ring buffer backend (API).
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Credits to Steven Rostedt for proposing to use an extra-subbuffer owned by
 * the reader in flight recorder mode.
 */

#ifndef _LIB_RING_BUFFER_BACKEND_H
#define _LIB_RING_BUFFER_BACKEND_H

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <wrapper/uaccess.h>
#include <lttng/probe-user.h>

/* Internal helpers */
#include <ringbuffer/backend_internal.h>
#include <ringbuffer/frontend_internal.h>

/* Ring buffer backend API */

/* Ring buffer backend access (read/write) */

extern size_t lib_ring_buffer_read(struct lttng_kernel_ring_buffer_backend *bufb,
				   size_t offset, void *dest, size_t len);

extern int __lib_ring_buffer_copy_to_user(struct lttng_kernel_ring_buffer_backend *bufb,
					  size_t offset, void __user *dest,
					  size_t len);

extern int lib_ring_buffer_read_cstr(struct lttng_kernel_ring_buffer_backend *bufb,
				     size_t offset, void *dest, size_t len);

extern unsigned long *
lib_ring_buffer_read_get_pfn(struct lttng_kernel_ring_buffer_backend *bufb, size_t offset,
			      void ***virt);

/*
 * Return the address where a given offset is located.
 * Should be used to get the current subbuffer header pointer. Given we know
 * it's never on a page boundary, it's safe to write directly to this address,
 * as long as the write is never bigger than a page size.
 */
extern void *
lib_ring_buffer_offset_address(struct lttng_kernel_ring_buffer_backend *bufb,
			       size_t offset);
extern void *
lib_ring_buffer_read_offset_address(struct lttng_kernel_ring_buffer_backend *bufb,
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
static inline __attribute__((always_inline))
void lib_ring_buffer_write(const struct lttng_kernel_ring_buffer_config *config,
			   struct lttng_kernel_ring_buffer_ctx *ctx,
			   const void *src, size_t len)
{
	struct lttng_kernel_ring_buffer_backend *bufb = &ctx->priv.buf->backend;
	struct channel_backend *chanb = &ctx->priv.chan->backend;
	size_t index, bytes_left_in_page;
	size_t offset = ctx->priv.buf_offset;
	struct lttng_kernel_ring_buffer_backend_pages *backend_pages;

	if (unlikely(!len))
		return;
	backend_pages =
		lib_ring_buffer_get_backend_pages_from_ctx(config, ctx);
	offset &= chanb->buf_size - 1;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	bytes_left_in_page = min_t(size_t, len, (-offset) & ~PAGE_MASK);
	if (likely(bytes_left_in_page == len))
		lib_ring_buffer_do_copy(config,
					backend_pages->p[index].virt
					    + (offset & ~PAGE_MASK),
					src, len);
	else
		_lib_ring_buffer_write(bufb, offset, src, len);
	ctx->priv.buf_offset += len;
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
void lib_ring_buffer_memset(const struct lttng_kernel_ring_buffer_config *config,
			    struct lttng_kernel_ring_buffer_ctx *ctx, int c, size_t len)
{
	struct lttng_kernel_ring_buffer_backend *bufb = &ctx->priv.buf->backend;
	struct channel_backend *chanb = &ctx->priv.chan->backend;
	size_t index, bytes_left_in_page;
	size_t offset = ctx->priv.buf_offset;
	struct lttng_kernel_ring_buffer_backend_pages *backend_pages;

	if (unlikely(!len))
		return;
	backend_pages =
		lib_ring_buffer_get_backend_pages_from_ctx(config, ctx);
	offset &= chanb->buf_size - 1;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	bytes_left_in_page = min_t(size_t, len, (-offset) & ~PAGE_MASK);
	if (likely(bytes_left_in_page == len))
		lib_ring_buffer_do_memset(backend_pages->p[index].virt
					  + (offset & ~PAGE_MASK),
					  c, len);
	else
		_lib_ring_buffer_memset(bufb, offset, c, len);
	ctx->priv.buf_offset += len;
}

/*
 * Copy up to @len string bytes from @src to @dest. Stop whenever a NULL
 * terminating character is found in @src. Returns the number of bytes
 * copied. Does *not* terminate @dest with NULL terminating character.
 */
static inline __attribute__((always_inline))
size_t lib_ring_buffer_do_strcpy(const struct lttng_kernel_ring_buffer_config *config,
		char *dest, const char *src, size_t len)
{
	size_t count;

	for (count = 0; count < len; count++) {
		char c;

		/*
		 * Only read source character once, in case it is
		 * modified concurrently.
		 */
		c = LTTNG_READ_ONCE(src[count]);
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
 * This function deals with userspace pointers. It should be called
 * after having the src pointer checked with access_ok() and with page
 * fault handler disabled.
 */
static inline __attribute__((always_inline))
size_t lib_ring_buffer_do_strcpy_from_user_inatomic(const struct lttng_kernel_ring_buffer_config *config,
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
void lib_ring_buffer_strcpy(const struct lttng_kernel_ring_buffer_config *config,
			   struct lttng_kernel_ring_buffer_ctx *ctx,
			   const char *src, size_t len, int pad)
{
	struct lttng_kernel_ring_buffer_backend *bufb = &ctx->priv.buf->backend;
	struct channel_backend *chanb = &ctx->priv.chan->backend;
	size_t index, bytes_left_in_page;
	size_t offset = ctx->priv.buf_offset;
	struct lttng_kernel_ring_buffer_backend_pages *backend_pages;

	if (unlikely(!len))
		return;
	backend_pages =
		lib_ring_buffer_get_backend_pages_from_ctx(config, ctx);
	offset &= chanb->buf_size - 1;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	bytes_left_in_page = min_t(size_t, len, (-offset) & ~PAGE_MASK);
	if (likely(bytes_left_in_page == len)) {
		size_t count;

		count = lib_ring_buffer_do_strcpy(config,
					backend_pages->p[index].virt
					    + (offset & ~PAGE_MASK),
					src, len - 1);
		offset += count;
		/* Padding */
		if (unlikely(count < len - 1)) {
			size_t pad_len = len - 1 - count;

			lib_ring_buffer_do_memset(backend_pages->p[index].virt
						+ (offset & ~PAGE_MASK),
					pad, pad_len);
			offset += pad_len;
		}
		/* Ending '\0' */
		lib_ring_buffer_do_memset(backend_pages->p[index].virt
					+ (offset & ~PAGE_MASK),
				'\0', 1);
	} else {
		_lib_ring_buffer_strcpy(bufb, offset, src, len, pad);
	}
	ctx->priv.buf_offset += len;
}

/**
 * lib_ring_buffer_pstrcpy - write kernel C-string (input) to a buffer backend P-string
 * @config : ring buffer instance configuration
 * @ctx: ring buffer context. (input arguments only)
 * @src : source pointer to copy from
 * @len : length of data to copy
 * @pad : character to use for padding
 *
 * This function copies up to @len bytes of data from a source pointer
 * to a Pascal String into the buffer backend. If a terminating '\0'
 * character is found in @src before @len characters are copied, pad the
 * buffer with @pad characters (e.g. '\0').
 *
 * The length of the pascal strings in the ring buffer is explicit: it
 * is either the array or sequence length.
 */
static inline
void lib_ring_buffer_pstrcpy(const struct lttng_kernel_ring_buffer_config *config,
			   struct lttng_kernel_ring_buffer_ctx *ctx,
			   const char *src, size_t len, char pad)
	__attribute__((always_inline));
static inline
void lib_ring_buffer_pstrcpy(const struct lttng_kernel_ring_buffer_config *config,
			   struct lttng_kernel_ring_buffer_ctx *ctx,
			   const char *src, size_t len, char pad)
{
	struct lttng_kernel_ring_buffer_backend *bufb = &ctx->priv.buf->backend;
	struct channel_backend *chanb = &ctx->priv.chan->backend;
	size_t index, bytes_left_in_page;
	size_t offset = ctx->priv.buf_offset;
	struct lttng_kernel_ring_buffer_backend_pages *backend_pages;

	if (unlikely(!len))
		return;
	backend_pages =
		lib_ring_buffer_get_backend_pages_from_ctx(config, ctx);
	offset &= chanb->buf_size - 1;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	bytes_left_in_page = min_t(size_t, len, (-offset) & ~PAGE_MASK);
	if (likely(bytes_left_in_page == len)) {
		size_t count;

		count = lib_ring_buffer_do_strcpy(config,
					backend_pages->p[index].virt
					    + (offset & ~PAGE_MASK),
					src, len);
		offset += count;
		/* Padding */
		if (unlikely(count < len)) {
			size_t pad_len = len - count;

			lib_ring_buffer_do_memset(backend_pages->p[index].virt
						+ (offset & ~PAGE_MASK),
					pad, pad_len);
			offset += pad_len;
		}
	} else {
		_lib_ring_buffer_pstrcpy(bufb, offset, src, len, pad);
	}
	ctx->priv.buf_offset += len;
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
static inline __attribute__((always_inline))
void lib_ring_buffer_copy_from_user_inatomic(const struct lttng_kernel_ring_buffer_config *config,
				    struct lttng_kernel_ring_buffer_ctx *ctx,
				    const void __user *src, size_t len)
{
	struct lttng_kernel_ring_buffer_backend *bufb = &ctx->priv.buf->backend;
	struct channel_backend *chanb = &ctx->priv.chan->backend;
	size_t index, bytes_left_in_page;
	size_t offset = ctx->priv.buf_offset;
	struct lttng_kernel_ring_buffer_backend_pages *backend_pages;
	unsigned long ret;

	if (unlikely(!len))
		return;
	backend_pages =
		lib_ring_buffer_get_backend_pages_from_ctx(config, ctx);
	offset &= chanb->buf_size - 1;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	bytes_left_in_page = min_t(size_t, len, (-offset) & ~PAGE_MASK);

	if (unlikely(!lttng_access_ok(VERIFY_READ, src, len)))
		goto fill_buffer;

	pagefault_disable();
	if (likely(bytes_left_in_page == len)) {
		ret = lib_ring_buffer_do_copy_from_user_inatomic(
			backend_pages->p[index].virt + (offset & ~PAGE_MASK),
			src, len);
		if (unlikely(ret > 0)) {
			/* Copy failed. */
			goto fill_buffer_enable_pf;
		}
	} else {
		_lib_ring_buffer_copy_from_user_inatomic(bufb, offset, src, len);
	}
	pagefault_enable();
	ctx->priv.buf_offset += len;

	return;

fill_buffer_enable_pf:
	pagefault_enable();
fill_buffer:
	/*
	 * In the error path we call the slow path version to avoid
	 * the pollution of static inline code.
	 */
	_lib_ring_buffer_memset(bufb, offset, 0, len);
	ctx->priv.buf_offset += len;
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
void lib_ring_buffer_strcpy_from_user_inatomic(const struct lttng_kernel_ring_buffer_config *config,
		struct lttng_kernel_ring_buffer_ctx *ctx,
		const void __user *src, size_t len, int pad)
{
	struct lttng_kernel_ring_buffer_backend *bufb = &ctx->priv.buf->backend;
	struct channel_backend *chanb = &ctx->priv.chan->backend;
	size_t index, bytes_left_in_page;
	size_t offset = ctx->priv.buf_offset;
	struct lttng_kernel_ring_buffer_backend_pages *backend_pages;

	if (unlikely(!len))
		return;
	backend_pages =
		lib_ring_buffer_get_backend_pages_from_ctx(config, ctx);
	offset &= chanb->buf_size - 1;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	bytes_left_in_page = min_t(size_t, len, (-offset) & ~PAGE_MASK);

	if (unlikely(!lttng_access_ok(VERIFY_READ, src, len)))
		goto fill_buffer;

	pagefault_disable();
	if (likely(bytes_left_in_page == len)) {
		size_t count;

		count = lib_ring_buffer_do_strcpy_from_user_inatomic(config,
					backend_pages->p[index].virt
					    + (offset & ~PAGE_MASK),
					src, len - 1);
		offset += count;
		/* Padding */
		if (unlikely(count < len - 1)) {
			size_t pad_len = len - 1 - count;

			lib_ring_buffer_do_memset(backend_pages->p[index].virt
						+ (offset & ~PAGE_MASK),
					pad, pad_len);
			offset += pad_len;
		}
		/* Ending '\0' */
		lib_ring_buffer_do_memset(backend_pages->p[index].virt
					+ (offset & ~PAGE_MASK),
				'\0', 1);
	} else {
		_lib_ring_buffer_strcpy_from_user_inatomic(bufb, offset, src,
					len, pad);
	}
	pagefault_enable();
	ctx->priv.buf_offset += len;

	return;

fill_buffer:
	/*
	 * In the error path we call the slow path version to avoid
	 * the pollution of static inline code.
	 */
	_lib_ring_buffer_memset(bufb, offset, pad, len - 1);
	offset += len - 1;
	_lib_ring_buffer_memset(bufb, offset, '\0', 1);
	ctx->priv.buf_offset += len;
}

/**
 * lib_ring_buffer_pstrcpy_from_user_inatomic - write user-space C-string (input) to a buffer backend P-string
 * @config : ring buffer instance configuration
 * @ctx: ring buffer context. (input arguments only)
 * @src : source pointer to copy from
 * @len : length of data to copy
 * @pad : character to use for padding
 *
 * This function copies up to @len bytes of data from a source pointer
 * to a Pascal String into the buffer backend. If a terminating '\0'
 * character is found in @src before @len characters are copied, pad the
 * buffer with @pad characters (e.g. '\0').
 *
 * The length of the pascal strings in the ring buffer is explicit: it
 * is either the array or sequence length.
 */
static inline
void lib_ring_buffer_pstrcpy_from_user_inatomic(const struct lttng_kernel_ring_buffer_config *config,
			   struct lttng_kernel_ring_buffer_ctx *ctx,
			   const char __user *src, size_t len, char pad)
	__attribute__((always_inline));
static inline
void lib_ring_buffer_pstrcpy_from_user_inatomic(const struct lttng_kernel_ring_buffer_config *config,
			   struct lttng_kernel_ring_buffer_ctx *ctx,
			   const char __user *src, size_t len, char pad)
{
	struct lttng_kernel_ring_buffer_backend *bufb = &ctx->priv.buf->backend;
	struct channel_backend *chanb = &ctx->priv.chan->backend;
	size_t index, bytes_left_in_page;
	size_t offset = ctx->priv.buf_offset;
	struct lttng_kernel_ring_buffer_backend_pages *backend_pages;

	if (unlikely(!len))
		return;
	backend_pages =
		lib_ring_buffer_get_backend_pages_from_ctx(config, ctx);
	offset &= chanb->buf_size - 1;
	index = (offset & (chanb->subbuf_size - 1)) >> PAGE_SHIFT;
	bytes_left_in_page = min_t(size_t, len, (-offset) & ~PAGE_MASK);

	if (unlikely(!lttng_access_ok(VERIFY_READ, src, len)))
		goto fill_buffer;

	pagefault_disable();
	if (likely(bytes_left_in_page == len)) {
		size_t count;

		count = lib_ring_buffer_do_strcpy_from_user_inatomic(config,
					backend_pages->p[index].virt
					    + (offset & ~PAGE_MASK),
					src, len);
		offset += count;
		/* Padding */
		if (unlikely(count < len)) {
			size_t pad_len = len - count;

			lib_ring_buffer_do_memset(backend_pages->p[index].virt
						+ (offset & ~PAGE_MASK),
					pad, pad_len);
			offset += pad_len;
		}
	} else {
		_lib_ring_buffer_pstrcpy_from_user_inatomic(bufb, offset, src, len, pad);
	}
	ctx->priv.buf_offset += len;
	pagefault_enable();

	return;

fill_buffer:
	/*
	 * In the error path we call the slow path version to avoid
	 * the pollution of static inline code.
	 */
	_lib_ring_buffer_memset(bufb, offset, pad, len);
	ctx->priv.buf_offset += len;
}

/*
 * This accessor counts the number of unread records in a buffer.
 * It only provides a consistent value if no reads not writes are performed
 * concurrently.
 */
static inline
unsigned long lib_ring_buffer_get_records_unread(
				const struct lttng_kernel_ring_buffer_config *config,
				struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_backend *bufb = &buf->backend;
	struct lttng_kernel_ring_buffer_backend_pages *pages;
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

/*
 * We use __copy_from_user_inatomic to copy userspace data after
 * checking with access_ok() and disabling page faults.
 *
 * Return 0 if OK, nonzero on error.
 */
static inline
unsigned long lib_ring_buffer_copy_from_user_check_nofault(void *dest,
						const void __user *src,
						unsigned long len)
{
	return lttng_copy_from_user_check_nofault(dest, src, len);
}

#endif /* _LIB_RING_BUFFER_BACKEND_H */
