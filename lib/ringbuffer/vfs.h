/* SPDX-License-Identifier: (GPL-2.0 OR LGPL-2.1)
 *
 * lib/ringbuffer/vfs.h
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

struct lib_ring_buffer;

int lib_ring_buffer_open(struct inode *inode, struct file *file,
		struct lib_ring_buffer *buf);
int lib_ring_buffer_release(struct inode *inode, struct file *file,
		struct lib_ring_buffer *buf);
unsigned int lib_ring_buffer_poll(struct file *filp, poll_table *wait,
		struct lib_ring_buffer *buf);
ssize_t lib_ring_buffer_splice_read(struct file *in, loff_t *ppos,
		struct pipe_inode_info *pipe, size_t len,
		unsigned int flags, struct lib_ring_buffer *buf);
int lib_ring_buffer_mmap(struct file *filp, struct vm_area_struct *vma,
		struct lib_ring_buffer *buf);

/* Ring Buffer ioctl() and ioctl numbers */
long lib_ring_buffer_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg, struct lib_ring_buffer *buf);
#ifdef CONFIG_COMPAT
long lib_ring_buffer_compat_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg, struct lib_ring_buffer *buf);
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
 * Use RING_BUFFER_GET_NEXT_SUBBUF / RING_BUFFER_PUT_NEXT_SUBBUF to read and
 * consume sub-buffers sequentially.
 *
 * Reading sub-buffers without consuming them can be performed with:
 *
 * RING_BUFFER_SNAPSHOT
 * RING_BUFFER_SNAPSHOT_GET_CONSUMED
 * RING_BUFFER_SNAPSHOT_GET_PRODUCED
 *
 * to get the offset range to consume, and then by passing each sub-buffer
 * offset to RING_BUFFER_GET_SUBBUF, read the sub-buffer, and then release it
 * with RING_BUFFER_PUT_SUBBUF.
 *
 * Note that the "snapshot" API can be used to read the sub-buffer in reverse
 * order, which is useful for flight recorder snapshots.
 */

/* Get a snapshot of the current ring buffer producer and consumer positions */
#define RING_BUFFER_SNAPSHOT			_IO(0xF6, 0x00)
/* Get the consumer position (iteration start) */
#define RING_BUFFER_SNAPSHOT_GET_CONSUMED	_IOR(0xF6, 0x01, unsigned long)
/* Get the producer position (iteration end) */
#define RING_BUFFER_SNAPSHOT_GET_PRODUCED	_IOR(0xF6, 0x02, unsigned long)
/* Get exclusive read access to the specified sub-buffer position */
#define RING_BUFFER_GET_SUBBUF			_IOW(0xF6, 0x03, unsigned long)
/* Release exclusive sub-buffer access */
#define RING_BUFFER_PUT_SUBBUF			_IO(0xF6, 0x04)

/* Get exclusive read access to the next sub-buffer that can be read. */
#define RING_BUFFER_GET_NEXT_SUBBUF		_IO(0xF6, 0x05)
/* Release exclusive sub-buffer access, move consumer forward. */
#define RING_BUFFER_PUT_NEXT_SUBBUF		_IO(0xF6, 0x06)
/* returns the size of the current sub-buffer, without padding (for mmap). */
#define RING_BUFFER_GET_SUBBUF_SIZE		_IOR(0xF6, 0x07, unsigned long)
/* returns the size of the current sub-buffer, with padding (for splice). */
#define RING_BUFFER_GET_PADDED_SUBBUF_SIZE	_IOR(0xF6, 0x08, unsigned long)
/* returns the maximum size for sub-buffers. */
#define RING_BUFFER_GET_MAX_SUBBUF_SIZE		_IOR(0xF6, 0x09, unsigned long)
/* returns the length to mmap. */
#define RING_BUFFER_GET_MMAP_LEN		_IOR(0xF6, 0x0A, unsigned long)
/* returns the offset of the subbuffer belonging to the mmap reader. */
#define RING_BUFFER_GET_MMAP_READ_OFFSET	_IOR(0xF6, 0x0B, unsigned long)
/* Flush the current sub-buffer, if non-empty. */
#define RING_BUFFER_FLUSH			_IO(0xF6, 0x0C)
/* Get the current version of the metadata cache (after a get_next). */
#define RING_BUFFER_GET_METADATA_VERSION	_IOR(0xF6, 0x0D, uint64_t)
/*
 * Get a snapshot of the current ring buffer producer and consumer positions,
 * regardless of whether or not the two positions are contained within the same
 * sub-buffer.
 */
#define RING_BUFFER_SNAPSHOT_SAMPLE_POSITIONS	_IO(0xF6, 0x0E)
/* Flush the current sub-buffer, even if empty. */
#define RING_BUFFER_FLUSH_EMPTY			_IO(0xF6, 0x0F)
/*
 * Reset the position of what has been consumed from the metadata cache to 0
 * so it can be read again.
 */
#define RING_BUFFER_METADATA_CACHE_DUMP		_IO(0xF6, 0x10)
/*
 * Get next metadata subbuffer, returning a flag indicating whether the
 * metadata is guaranteed to be in a consistent state at the end of this
 * sub-buffer (can be parsed).
 */
#define RING_BUFFER_GET_NEXT_SUBBUF_METADATA_CHECK	_IOR(0xF6, 0x12, uint32_t)

#ifdef CONFIG_COMPAT
/* Get a snapshot of the current ring buffer producer and consumer positions */
#define RING_BUFFER_COMPAT_SNAPSHOT		RING_BUFFER_SNAPSHOT
/* Get the consumer position (iteration start) */
#define RING_BUFFER_COMPAT_SNAPSHOT_GET_CONSUMED \
	_IOR(0xF6, 0x01, compat_ulong_t)
/* Get the producer position (iteration end) */
#define RING_BUFFER_COMPAT_SNAPSHOT_GET_PRODUCED \
	_IOR(0xF6, 0x02, compat_ulong_t)
/* Get exclusive read access to the specified sub-buffer position */
#define RING_BUFFER_COMPAT_GET_SUBBUF		_IOW(0xF6, 0x03, compat_ulong_t)
/* Release exclusive sub-buffer access */
#define RING_BUFFER_COMPAT_PUT_SUBBUF		RING_BUFFER_PUT_SUBBUF

/* Get exclusive read access to the next sub-buffer that can be read. */
#define RING_BUFFER_COMPAT_GET_NEXT_SUBBUF	RING_BUFFER_GET_NEXT_SUBBUF
/* Release exclusive sub-buffer access, move consumer forward. */
#define RING_BUFFER_COMPAT_PUT_NEXT_SUBBUF	RING_BUFFER_PUT_NEXT_SUBBUF
/* returns the size of the current sub-buffer, without padding (for mmap). */
#define RING_BUFFER_COMPAT_GET_SUBBUF_SIZE	_IOR(0xF6, 0x07, compat_ulong_t)
/* returns the size of the current sub-buffer, with padding (for splice). */
#define RING_BUFFER_COMPAT_GET_PADDED_SUBBUF_SIZE \
	_IOR(0xF6, 0x08, compat_ulong_t)
/* returns the maximum size for sub-buffers. */
#define RING_BUFFER_COMPAT_GET_MAX_SUBBUF_SIZE	_IOR(0xF6, 0x09, compat_ulong_t)
/* returns the length to mmap. */
#define RING_BUFFER_COMPAT_GET_MMAP_LEN		_IOR(0xF6, 0x0A, compat_ulong_t)
/* returns the offset of the subbuffer belonging to the mmap reader. */
#define RING_BUFFER_COMPAT_GET_MMAP_READ_OFFSET	_IOR(0xF6, 0x0B, compat_ulong_t)
/* Flush the current sub-buffer, if non-empty. */
#define RING_BUFFER_COMPAT_FLUSH		RING_BUFFER_FLUSH
/* Get the current version of the metadata cache (after a get_next). */
#define RING_BUFFER_COMPAT_GET_METADATA_VERSION	RING_BUFFER_GET_METADATA_VERSION
/*
 * Get a snapshot of the current ring buffer producer and consumer positions,
 * regardless of whether or not the two positions are contained within the same
 * sub-buffer.
 */
#define RING_BUFFER_COMPAT_SNAPSHOT_SAMPLE_POSITIONS	\
	RING_BUFFER_SNAPSHOT_SAMPLE_POSITIONS
/* Flush the current sub-buffer, even if empty. */
#define RING_BUFFER_COMPAT_FLUSH_EMPTY			\
	RING_BUFFER_FLUSH_EMPTY
/*
 * Get next metadata subbuffer, returning a flag indicating whether the
 * metadata is guaranteed to be in a consistent state at the end of this
 * sub-buffer (can be parsed).
 */
#define RING_BUFFER_COMPAT_GET_NEXT_SUBBUF_METADATA_CHECK \
	RING_BUFFER_GET_NEXT_SUBBUF_METADATA_CHECK
#endif /* CONFIG_COMPAT */

#endif /* _LIB_RING_BUFFER_VFS_H */
