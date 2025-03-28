/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * ringbuffer/vfs.h
 *
 * Wait-free ring buffer VFS file operations.
 *
 * Copyright (C) 2005-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LIB_RING_BUFFER_VFS_H
#define _LIB_RING_BUFFER_VFS_H

#include <linux/fs.h>
#include <linux/poll.h>

/* VFS API */

extern const struct file_operations lib_ring_buffer_file_operations;

/*
 * Internal file operations.
 */

struct lttng_kernel_ring_buffer;

int lib_ring_buffer_open(struct inode *inode, struct file *file,
		struct lttng_kernel_ring_buffer *buf);
int lib_ring_buffer_release(struct inode *inode, struct file *file,
		struct lttng_kernel_ring_buffer *buf);
unsigned int lib_ring_buffer_poll(struct file *filp, poll_table *wait,
		struct lttng_kernel_ring_buffer *buf);
ssize_t lib_ring_buffer_splice_read(struct file *in, loff_t *ppos,
		struct pipe_inode_info *pipe, size_t len,
		unsigned int flags, struct lttng_kernel_ring_buffer *buf);
int lib_ring_buffer_mmap(struct file *filp, struct vm_area_struct *vma,
		struct lttng_kernel_ring_buffer *buf);

/* Ring Buffer ioctl() and ioctl numbers */
long lib_ring_buffer_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg, struct lttng_kernel_ring_buffer *buf);
#ifdef CONFIG_COMPAT
long lib_ring_buffer_compat_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg, struct lttng_kernel_ring_buffer *buf);
#endif

ssize_t vfs_lib_ring_buffer_file_splice_read(struct file *in, loff_t *ppos,
		struct pipe_inode_info *pipe, size_t len, unsigned int flags);
loff_t vfs_lib_ring_buffer_no_llseek(struct file *file, loff_t offset,
		int origin);
int vfs_lib_ring_buffer_mmap(struct file *filp, struct vm_area_struct *vma);
ssize_t vfs_lib_ring_buffer_splice_read(struct file *in, loff_t *ppos,
		struct pipe_inode_info *pipe, size_t len,
		unsigned int flags);

/*
 * Use LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF / LTTNG_KERNEL_ABI_RING_BUFFER_PUT_NEXT_SUBBUF to read and
 * consume sub-buffers sequentially.
 *
 * Reading sub-buffers without consuming them can be performed with:
 *
 * LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT
 * LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT_GET_CONSUMED
 * LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT_GET_PRODUCED
 *
 * to get the offset range to consume, and then by passing each sub-buffer
 * offset to RING_BUFFER_GET_SUBBUF, read the sub-buffer, and then release it
 * with RING_BUFFER_PUT_SUBBUF.
 *
 * Note that the "snapshot" API can be used to read the sub-buffer in reverse
 * order, which is useful for flight recorder snapshots.
 */

/* Get a snapshot of the current ring buffer producer and consumer positions */
#define LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT			_IO(0xF6, 0x00)
/* Get the consumer position (iteration start) */
#define LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT_GET_CONSUMED	_IOR(0xF6, 0x01, unsigned long)
/* Get the producer position (iteration end) */
#define LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT_GET_PRODUCED	_IOR(0xF6, 0x02, unsigned long)
/* Get exclusive read access to the specified sub-buffer position */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_SUBBUF			_IOW(0xF6, 0x03, unsigned long)
/* Release exclusive sub-buffer access */
#define LTTNG_KERNEL_ABI_RING_BUFFER_PUT_SUBBUF			_IO(0xF6, 0x04)

/* Get exclusive read access to the next sub-buffer that can be read. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF		_IO(0xF6, 0x05)
/* Release exclusive sub-buffer access, move consumer forward. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_PUT_NEXT_SUBBUF		_IO(0xF6, 0x06)
/* returns the size of the current sub-buffer, without padding (for mmap). */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_SUBBUF_SIZE		_IOR(0xF6, 0x07, unsigned long)
/* returns the size of the current sub-buffer, with padding (for splice). */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_PADDED_SUBBUF_SIZE	_IOR(0xF6, 0x08, unsigned long)
/* returns the maximum size for sub-buffers. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_MAX_SUBBUF_SIZE	_IOR(0xF6, 0x09, unsigned long)
/* returns the length to mmap. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_MMAP_LEN		_IOR(0xF6, 0x0A, unsigned long)
/* returns the offset of the subbuffer belonging to the mmap reader. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_MMAP_READ_OFFSET	_IOR(0xF6, 0x0B, unsigned long)
/* Flush the current sub-buffer, if non-empty. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH			_IO(0xF6, 0x0C)
/* Get the current version of the metadata cache (after a get_next). */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_METADATA_VERSION	_IOR(0xF6, 0x0D, uint64_t)
/*
 * Get a snapshot of the current ring buffer producer and consumer positions,
 * regardless of whether or not the two positions are contained within the same
 * sub-buffer.
 */
#define LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT_SAMPLE_POSITIONS	_IO(0xF6, 0x0E)
/* Flush the current sub-buffer, even if empty. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH_EMPTY		_IO(0xF6, 0x0F)
/*
 * Reset the position of what has been consumed from the metadata cache to 0
 * so it can be read again.
 */
#define LTTNG_KERNEL_ABI_RING_BUFFER_METADATA_CACHE_DUMP	_IO(0xF6, 0x10)
/* Clear ring buffer content. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_CLEAR			_IO(0xF6, 0x11)
/*
 * Get next metadata subbuffer, returning a flag indicating whether the
 * metadata is guaranteed to be in a consistent state at the end of this
 * sub-buffer (can be parsed).
 */
#define LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF_METADATA_CHECK	_IOR(0xF6, 0x12, uint32_t)
/*
 * Flush the current sub-buffer or populate a packet.
 *
 * Argument is a pointer to a struct lttng_kernel_abi_ring_buffer_packet_flush_or_populate_packet_args
 * which uses the fields as input and output. The region of memory pointed
 * at by the `packet` member should be at least large enough to fit
 * a sub-buffer for the stream.
 */
#define LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH_OR_POPULATE_PACKET	_IOWR(0xF6, 0x13, \
	struct lttng_kernel_abi_ring_buffer_packet_flush_or_populate_packet_args)

#ifdef CONFIG_COMPAT
/* Get a snapshot of the current ring buffer producer and consumer positions */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_SNAPSHOT		LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT
/* Get the consumer position (iteration start) */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_SNAPSHOT_GET_CONSUMED \
	_IOR(0xF6, 0x01, compat_ulong_t)
/* Get the producer position (iteration end) */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_SNAPSHOT_GET_PRODUCED \
	_IOR(0xF6, 0x02, compat_ulong_t)
/* Get exclusive read access to the specified sub-buffer position */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_SUBBUF		_IOW(0xF6, 0x03, compat_ulong_t)
/* Release exclusive sub-buffer access */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_PUT_SUBBUF		LTTNG_KERNEL_ABI_RING_BUFFER_PUT_SUBBUF

/* Get exclusive read access to the next sub-buffer that can be read. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_NEXT_SUBBUF	LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF
/* Release exclusive sub-buffer access, move consumer forward. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_PUT_NEXT_SUBBUF	LTTNG_KERNEL_ABI_RING_BUFFER_PUT_NEXT_SUBBUF
/* returns the size of the current sub-buffer, without padding (for mmap). */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_SUBBUF_SIZE	_IOR(0xF6, 0x07, compat_ulong_t)
/* returns the size of the current sub-buffer, with padding (for splice). */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_PADDED_SUBBUF_SIZE \
	_IOR(0xF6, 0x08, compat_ulong_t)
/* returns the maximum size for sub-buffers. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_MAX_SUBBUF_SIZE	_IOR(0xF6, 0x09, compat_ulong_t)
/* returns the length to mmap. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_MMAP_LEN		_IOR(0xF6, 0x0A, compat_ulong_t)
/* returns the offset of the subbuffer belonging to the mmap reader. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_MMAP_READ_OFFSET	_IOR(0xF6, 0x0B, compat_ulong_t)
/* Flush the current sub-buffer, if non-empty. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_FLUSH		LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH
/* Get the current version of the metadata cache (after a get_next). */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_METADATA_VERSION	LTTNG_KERNEL_ABI_RING_BUFFER_GET_METADATA_VERSION
/*
 * Get a snapshot of the current ring buffer producer and consumer positions,
 * regardless of whether or not the two positions are contained within the same
 * sub-buffer.
 */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_SNAPSHOT_SAMPLE_POSITIONS	\
	LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT_SAMPLE_POSITIONS
/* Flush the current sub-buffer, even if empty. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_FLUSH_EMPTY			\
	LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH_EMPTY
/* Clear ring buffer content. */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_CLEAR			\
	LTTNG_KERNEL_ABI_RING_BUFFER_CLEAR
/*
 * Get next metadata subbuffer, returning a flag indicating whether the
 * metadata is guaranteed to be in a consistent state at the end of this
 * sub-buffer (can be parsed).
 */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_NEXT_SUBBUF_METADATA_CHECK \
	LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF_METADATA_CHECK
/*
 * Flush the current sub-buffer or populate a packet.
 *
 * Argument is a pointer to a struct lttng_kernel_abi_ring_buffer_packet_flush_or_populate_packet_args
 * which uses the fields as input and output. The region of memory pointed
 * at by the `packet` member should be at least large enough to fit
 * a sub-buffer for the stream.
 */
#define LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_FLUSH_OR_POPULATE_PACKET	LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH_OR_POPULATE_PACKET
#endif /* CONFIG_COMPAT */

#endif /* _LIB_LTTNG_KERNEL_ABI_RING_BUFFER_VFS_H */
