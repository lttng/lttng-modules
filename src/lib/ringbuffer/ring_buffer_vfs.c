/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * ring_buffer_vfs.c
 *
 * Ring Buffer VFS file operations.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/compat.h>

#include <ringbuffer/backend.h>
#include <ringbuffer/frontend.h>
#include <ringbuffer/vfs.h>
#include <wrapper/limits.h>
#include <wrapper/poll.h>
#include <lttng/abi.h>
#include <lttng/tracer.h>

static int put_ulong(unsigned long val, unsigned long arg)
{
	return put_user(val, (unsigned long __user *)arg);
}

#ifdef CONFIG_COMPAT
static int compat_put_ulong(compat_ulong_t val, unsigned long arg)
{
	return put_user(val, (compat_ulong_t __user *)compat_ptr(arg));
}
#endif

/*
 * This is not used by anonymous file descriptors. This code is left
 * there if we ever want to implement an inode with open() operation.
 */
int lib_ring_buffer_open(struct inode *inode, struct file *file,
		struct lttng_kernel_ring_buffer *buf)
{
	int ret;

	if (!buf)
		return -EINVAL;

	ret = lib_ring_buffer_open_read(buf);
	if (ret)
		return ret;

	ret = nonseekable_open(inode, file);
	if (ret)
		goto release_read;
	return 0;

release_read:
	lib_ring_buffer_release_read(buf);
	return ret;
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_open);

/**
 *	vfs_lib_ring_buffer_open - ring buffer open file operation
 *	@inode: opened inode
 *	@file: opened file
 *
 *	Open implementation. Makes sure only one open instance of a buffer is
 *	done at a given moment.
 */
static
int vfs_lib_ring_buffer_open(struct inode *inode, struct file *file)
{
	struct lttng_kernel_ring_buffer *buf = inode->i_private;

	file->private_data = buf;
	return lib_ring_buffer_open(inode, file, buf);
}

int lib_ring_buffer_release(struct inode *inode, struct file *file,
		struct lttng_kernel_ring_buffer *buf)
{
	lib_ring_buffer_release_read(buf);

	return 0;
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_release);

/**
 *	vfs_lib_ring_buffer_release - ring buffer release file operation
 *	@inode: opened inode
 *	@file: opened file
 *
 *	Release implementation.
 */
static
int vfs_lib_ring_buffer_release(struct inode *inode, struct file *file)
{
	struct lttng_kernel_ring_buffer *buf = file->private_data;

	return lib_ring_buffer_release(inode, file, buf);
}

unsigned int lib_ring_buffer_poll(struct file *filp, poll_table *wait,
		struct lttng_kernel_ring_buffer *buf)
{
	unsigned int mask = 0;
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	int finalized, disabled;

	if (filp->f_mode & FMODE_READ) {
		poll_wait_set_exclusive(wait);
		poll_wait(filp, &buf->read_wait, wait);

		finalized = lib_ring_buffer_is_finalized(config, buf);
		disabled = lib_ring_buffer_channel_is_disabled(chan);

		/*
		 * lib_ring_buffer_is_finalized() contains a smp_rmb() ordering
		 * finalized load before offsets loads.
		 */
		WARN_ON(atomic_long_read(&buf->active_readers) != 1);
retry:
		if (disabled)
			return POLLERR;

		if (subbuf_trunc(lib_ring_buffer_get_offset(config, buf), chan)
		  - subbuf_trunc(lib_ring_buffer_get_consumed(config, buf), chan)
		  == 0) {
			if (finalized)
				return POLLHUP;
			else {
				/*
				 * The memory barriers
				 * __wait_event()/wake_up_interruptible() take
				 * care of "raw_spin_is_locked" memory ordering.
				 */
				if (raw_spin_is_locked(&buf->raw_tick_nohz_spinlock))
					goto retry;
				else
					return 0;
			}
		} else {
			if (subbuf_trunc(lib_ring_buffer_get_offset(config, buf),
					 chan)
			  - subbuf_trunc(lib_ring_buffer_get_consumed(config, buf),
					 chan)
			  >= chan->backend.buf_size)
				return POLLPRI | POLLRDBAND;
			else
				return POLLIN | POLLRDNORM;
		}
	}
	return mask;
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_poll);

/**
 *	vfs_lib_ring_buffer_poll - ring buffer poll file operation
 *	@filp: the file
 *	@wait: poll table
 *
 *	Poll implementation.
 */
static
unsigned int vfs_lib_ring_buffer_poll(struct file *filp, poll_table *wait)
{
	struct lttng_kernel_ring_buffer *buf = filp->private_data;

	return lib_ring_buffer_poll(filp, wait, buf);
}

static
long _lttng_kernel_abi_ring_buffer_flush_or_populate_packet(struct lttng_kernel_ring_buffer *buf,
		struct lttng_kernel_ring_buffer_channel *chan,
		const struct lttng_kernel_ring_buffer_config *config,
		unsigned long arg)
{
	struct lttng_kernel_abi_ring_buffer_packet_flush_or_populate_packet_args flush_args;
	struct channel_backend *lttng_chan = channel_get_private(buf->backend.chan);
	u64 packet_length, packet_length_padded;
	bool flush_done, packet_populated;
	unsigned long subbuffer_size;
	void __user *packet;
	long ret;

	ret = copy_from_user(&flush_args,
		(struct lttng_kernel_abi_ring_buffer_packet_flush_or_populate_packet_args __user *)arg,
		sizeof(struct lttng_kernel_abi_ring_buffer_packet_flush_or_populate_packet_args));
	if (ret)
		return -EFAULT;

	packet = (void __user *)(unsigned long)flush_args.packet;
	subbuffer_size = lttng_chan->subbuf_size;
	if (!unlikely(lttng_access_ok(VERIFY_WRITE, packet, subbuffer_size)))
		return -EINVAL;

	ret = lib_ring_buffer_switch_remote_or_populate_packet(config, buf, packet,
		&packet_length, &packet_length_padded, &flush_done, &packet_populated);
	if (ret)
		return ret;

	flush_args.flush_done = flush_done;
	flush_args.packet_populated = packet_populated;
	flush_args.packet_length = packet_length;
	flush_args.packet_length_padded = packet_length_padded;
	ret = copy_to_user((void __user *)arg, &flush_args, sizeof(struct lttng_kernel_abi_ring_buffer_packet_flush_or_populate_packet_args));
	return ret;
}

long lib_ring_buffer_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg, struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	if (lib_ring_buffer_channel_is_disabled(chan))
		return -EIO;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT:
		return lib_ring_buffer_snapshot(buf, &buf->cons_snapshot,
					    &buf->prod_snapshot);
	case LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT_SAMPLE_POSITIONS:
		return lib_ring_buffer_snapshot_sample_positions(buf,
				&buf->cons_snapshot, &buf->prod_snapshot);
	case LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT_GET_CONSUMED:
		return put_ulong(buf->cons_snapshot, arg);
	case LTTNG_KERNEL_ABI_RING_BUFFER_SNAPSHOT_GET_PRODUCED:
		return put_ulong(buf->prod_snapshot, arg);
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_SUBBUF:
	{
		unsigned long uconsume;
		long ret;

		ret = get_user(uconsume, (unsigned long __user *) arg);
		if (ret)
			return ret; /* will return -EFAULT */
		ret = lib_ring_buffer_get_subbuf(buf, uconsume);
		if (!ret) {
			/* Set file position to zero at each successful "get" */
			filp->f_pos = 0;
		}
		return ret;
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_PUT_SUBBUF:
		lib_ring_buffer_put_subbuf(buf);
		return 0;

	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF:
	{
		long ret;

		ret = lib_ring_buffer_get_next_subbuf(buf);
		if (!ret) {
			/* Set file position to zero at each successful "get" */
			filp->f_pos = 0;
		}
		return ret;
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_PUT_NEXT_SUBBUF:
		lib_ring_buffer_put_next_subbuf(buf);
		return 0;
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_SUBBUF_SIZE:
		return put_ulong(lib_ring_buffer_get_read_data_size(config, buf),
				 arg);
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_PADDED_SUBBUF_SIZE:
	{
		unsigned long size;

		size = lib_ring_buffer_get_read_data_size(config, buf);
		size = PAGE_ALIGN(size);
		return put_ulong(size, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_MAX_SUBBUF_SIZE:
		return put_ulong(chan->backend.subbuf_size, arg);
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_MMAP_LEN:
	{
		unsigned long mmap_buf_len;

		if (config->output != RING_BUFFER_MMAP)
			return -EINVAL;
		mmap_buf_len = chan->backend.buf_size;
		if (chan->backend.extra_reader_sb)
			mmap_buf_len += chan->backend.subbuf_size;
		if (mmap_buf_len > INT_MAX)
			return -EFBIG;
		return put_ulong(mmap_buf_len, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_MMAP_READ_OFFSET:
	{
		unsigned long sb_bindex;

		if (config->output != RING_BUFFER_MMAP)
			return -EINVAL;
		sb_bindex = subbuffer_id_get_index(config,
						   buf->backend.buf_rsb.id);
		return put_ulong(buf->backend.array[sb_bindex]->mmap_offset,
				 arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH:
		lib_ring_buffer_switch_remote(buf);
		return 0;
	case LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH_EMPTY:
		lib_ring_buffer_switch_remote_empty(buf);
		return 0;
	case LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH_OR_POPULATE_PACKET:
		return _lttng_kernel_abi_ring_buffer_flush_or_populate_packet(buf, chan, config, arg);
	case LTTNG_KERNEL_ABI_RING_BUFFER_CLEAR:
		lib_ring_buffer_clear(buf);
		return 0;
	default:
		return -ENOIOCTLCMD;
	}
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_ioctl);

/**
 *	vfs_lib_ring_buffer_ioctl - control ring buffer reader synchronization
 *
 *	@filp: the file
 *	@cmd: the command
 *	@arg: command arg
 *
 *	This ioctl implements commands necessary for producer/consumer
 *	and flight recorder reader interaction :
 *	LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF
 *		Get the next sub-buffer that can be read. It never blocks.
 *	LTTNG_KERNEL_ABI_RING_BUFFER_PUT_NEXT_SUBBUF
 *		Release the currently read sub-buffer.
 *	LTTNG_KERNEL_ABI_RING_BUFFER_GET_SUBBUF_SIZE
 *		returns the size of the current sub-buffer.
 *	LTTNG_KERNEL_ABI_RING_BUFFER_GET_MAX_SUBBUF_SIZE
 *		returns the maximum size for sub-buffers.
 *	LTTNG_KERNEL_ABI_RING_BUFFER_GET_NUM_SUBBUF
 *		returns the number of reader-visible sub-buffers in the per cpu
 *              channel (for mmap).
 *      LTTNG_KERNEL_ABI_RING_BUFFER_GET_MMAP_READ_OFFSET
 *              returns the offset of the subbuffer belonging to the reader.
 *              Should only be used for mmap clients.
 */
static
long vfs_lib_ring_buffer_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct lttng_kernel_ring_buffer *buf = filp->private_data;

	return lib_ring_buffer_ioctl(filp, cmd, arg, buf);
}

#ifdef CONFIG_COMPAT
long lib_ring_buffer_compat_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg, struct lttng_kernel_ring_buffer *buf)
{
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;

	if (lib_ring_buffer_channel_is_disabled(chan))
		return -EIO;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_SNAPSHOT:
		return lib_ring_buffer_snapshot(buf, &buf->cons_snapshot,
						&buf->prod_snapshot);
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_SNAPSHOT_SAMPLE_POSITIONS:
		return lib_ring_buffer_snapshot_sample_positions(buf,
				&buf->cons_snapshot, &buf->prod_snapshot);
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_SNAPSHOT_GET_CONSUMED:
		return compat_put_ulong(buf->cons_snapshot, arg);
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_SNAPSHOT_GET_PRODUCED:
		return compat_put_ulong(buf->prod_snapshot, arg);
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_SUBBUF:
	{
		__u32 uconsume;
		unsigned long consume;
		long ret;

		ret = get_user(uconsume, (__u32 __user *) arg);
		if (ret)
			return ret; /* will return -EFAULT */
		consume = buf->cons_snapshot;
		consume &= ~0xFFFFFFFFL;
		consume |= uconsume;
		ret = lib_ring_buffer_get_subbuf(buf, consume);
		if (!ret) {
			/* Set file position to zero at each successful "get" */
			filp->f_pos = 0;
		}
		return ret;
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_PUT_SUBBUF:
		lib_ring_buffer_put_subbuf(buf);
		return 0;

	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_NEXT_SUBBUF:
	{
		long ret;

		ret = lib_ring_buffer_get_next_subbuf(buf);
		if (!ret) {
			/* Set file position to zero at each successful "get" */
			filp->f_pos = 0;
		}
		return ret;
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_PUT_NEXT_SUBBUF:
		lib_ring_buffer_put_next_subbuf(buf);
		return 0;
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_SUBBUF_SIZE:
	{
		unsigned long data_size;

		data_size = lib_ring_buffer_get_read_data_size(config, buf);
		if (data_size > UINT_MAX)
			return -EFBIG;
		return compat_put_ulong(data_size, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_PADDED_SUBBUF_SIZE:
	{
		unsigned long size;

		size = lib_ring_buffer_get_read_data_size(config, buf);
		size = PAGE_ALIGN(size);
		if (size > UINT_MAX)
			return -EFBIG;
		return compat_put_ulong(size, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_MAX_SUBBUF_SIZE:
		if (chan->backend.subbuf_size > UINT_MAX)
			return -EFBIG;
		return compat_put_ulong(chan->backend.subbuf_size, arg);
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_MMAP_LEN:
	{
		unsigned long mmap_buf_len;

		if (config->output != RING_BUFFER_MMAP)
			return -EINVAL;
		mmap_buf_len = chan->backend.buf_size;
		if (chan->backend.extra_reader_sb)
			mmap_buf_len += chan->backend.subbuf_size;
		if (mmap_buf_len > UINT_MAX)
			return -EFBIG;
		return compat_put_ulong(mmap_buf_len, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_MMAP_READ_OFFSET:
	{
		unsigned long sb_bindex, read_offset;

		if (config->output != RING_BUFFER_MMAP)
			return -EINVAL;
		sb_bindex = subbuffer_id_get_index(config,
						   buf->backend.buf_rsb.id);
		read_offset = buf->backend.array[sb_bindex]->mmap_offset;
		if (read_offset > UINT_MAX)
			return -EINVAL;
		return compat_put_ulong(read_offset, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_FLUSH:
		lib_ring_buffer_switch_remote(buf);
		return 0;
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_FLUSH_EMPTY:
		lib_ring_buffer_switch_remote_empty(buf);
		return 0;
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_FLUSH_OR_POPULATE_PACKET:
		return _lttng_kernel_abi_ring_buffer_flush_or_populate_packet(buf, chan, config, arg);
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_CLEAR:
		lib_ring_buffer_clear(buf);
		return 0;
	default:
		return -ENOIOCTLCMD;
	}
}
EXPORT_SYMBOL_GPL(lib_ring_buffer_compat_ioctl);

static
long vfs_lib_ring_buffer_compat_ioctl(struct file *filp, unsigned int cmd,
				  unsigned long arg)
{
	struct lttng_kernel_ring_buffer *buf = filp->private_data;

	return lib_ring_buffer_compat_ioctl(filp, cmd, arg, buf);
}
#endif

const struct file_operations lib_ring_buffer_file_operations = {
	.owner = THIS_MODULE,
	.open = vfs_lib_ring_buffer_open,
	.release = vfs_lib_ring_buffer_release,
	.poll = vfs_lib_ring_buffer_poll,
	.splice_read = vfs_lib_ring_buffer_splice_read,
	.mmap = vfs_lib_ring_buffer_mmap,
	.unlocked_ioctl = vfs_lib_ring_buffer_ioctl,
	.llseek = vfs_lib_ring_buffer_no_llseek,
#ifdef CONFIG_COMPAT
	.compat_ioctl = vfs_lib_ring_buffer_compat_ioctl,
#endif
};
EXPORT_SYMBOL_GPL(lib_ring_buffer_file_operations);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng ring buffer library");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
