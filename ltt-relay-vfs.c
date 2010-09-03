/*
 * ltt/ltt-relay-vfs.c
 *
 * (C) Copyright 2009 - Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * LTTng VFS interface.
 *
 * Author:
 *	Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/ltt-channels.h>
#include <asm/atomic.h>

#include "ltt-tracer.h"
#include "ltt-relay.h"
#include "ltt-relay-lockless.h"

/**
 *	ltt_open - open file op for ltt files
 *	@inode: opened inode
 *	@file: opened file
 *
 *	Open implementation. Makes sure only one open instance of a buffer is
 *	done at a given moment.
 */
static int ltt_open(struct inode *inode, struct file *file)
{
	struct ltt_chanbuf *buf = inode->i_private;
	int ret;

	ret = ltt_chanbuf_open_read(buf);
	if (ret)
		goto end;

	file->private_data = buf;
	ret = nonseekable_open(inode, file);
	/*
	 * Let LTTng splice operation must believe that the file descriptor is
	 * seekable. This is a temporary fix to follow new checks added to
	 * splice.c. We should probably do the proper thing and implement a
	 * llseek function eventually, which involves modifying the lttng splice
	 * actors accordingly. TODO
	 */
	file->f_mode |= FMODE_PREAD;
end:
	return ret;
}

/**
 *	ltt_release - release file op for ltt files
 *	@inode: opened inode
 *	@file: opened file
 *
 *	Release implementation.
 */
static int ltt_release(struct inode *inode, struct file *file)
{
	struct ltt_chanbuf *buf = inode->i_private;

	ltt_chanbuf_release_read(buf);

	return 0;
}

/**
 *	ltt_poll - file op for ltt files
 *	@filp: the file
 *	@wait: poll table
 *
 *	Poll implementation.
 */
static unsigned int ltt_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;
	struct inode *inode = filp->f_dentry->d_inode;
	struct ltt_chanbuf *buf = inode->i_private;
	struct ltt_chan *chan = container_of(buf->a.chan, struct ltt_chan, a);

	if (filp->f_mode & FMODE_READ) {
		poll_wait_set_exclusive(wait);
		poll_wait(filp, &buf->read_wait, wait);

		WARN_ON(atomic_long_read(&buf->active_readers) != 1);
		if (SUBBUF_TRUNC(ltt_chanbuf_get_offset(buf), chan)
		  - SUBBUF_TRUNC(ltt_chanbuf_get_consumed(buf), chan)
		  == 0) {
			if (buf->finalized)
				return POLLHUP;
			else
				return 0;
		} else {
			if (SUBBUF_TRUNC(ltt_chanbuf_get_offset(buf), chan)
			  - SUBBUF_TRUNC(ltt_chanbuf_get_consumed(buf), chan)
			  >= chan->a.buf_size)
				return POLLPRI | POLLRDBAND;
			else
				return POLLIN | POLLRDNORM;
		}
	}
	return mask;
}

/**
 *	ltt_ioctl - control on the debugfs file
 *
 *	@inode: the inode
 *	@filp: the file
 *	@cmd: the command
 *	@arg: command arg
 *
 *	This ioctl implements three commands necessary for a minimal
 *	producer/consumer implementation :
 *	RELAY_GET_SB
 *		Get the next sub-buffer that can be read. It never blocks.
 *	RELAY_PUT_SB
 *		Release the currently read sub-buffer. Parameter is the last
 *		put subbuffer (returned by GET_SUBBUF).
 *	RELAY_GET_N_SB
 *		returns the number of sub-buffers in the per cpu channel.
 *	RELAY_GET_SB_SIZE
 *		returns the size of the current sub-buffer.
 *	RELAY_GET_MAX_SB_SIZE
 *		returns the maximum size for sub-buffers.
 */
static
int ltt_ioctl(struct inode *inode, struct file *filp, unsigned int cmd,
	      unsigned long arg)
{
	struct ltt_chanbuf *buf = inode->i_private;
	u32 __user *argp = (u32 __user *)arg;

	switch (cmd) {
	case RELAY_GET_SB:
	{
		unsigned long consumed;
		int ret;

		ret = ltt_chanbuf_get_subbuf(buf, &consumed);
		if (ret)
			return ret;
		else
			return put_user((u32)consumed, argp);
		break;
	}
	case RELAY_PUT_SB:
	{
		u32 uconsumed_old;
		int ret;
		long consumed_old;

		ret = get_user(uconsumed_old, argp);
		if (ret)
			return ret; /* will return -EFAULT */

		consumed_old = ltt_chanbuf_get_consumed(buf);
		consumed_old = consumed_old & (~0xFFFFFFFFL);
		consumed_old = consumed_old | uconsumed_old;
		ret = ltt_chanbuf_put_subbuf(buf, consumed_old);
		if (ret)
			return ret;
		break;
	}
	case RELAY_GET_N_SB:
		return put_user((u32)buf->a.chan->n_sb, argp);
		break;
	case RELAY_GET_SB_SIZE:
		return put_user(get_read_sb_size(buf), argp);
		break;
	case RELAY_GET_MAX_SB_SIZE:
		return put_user((u32)buf->a.chan->sb_size, argp);
		break;
	default:
		return -ENOIOCTLCMD;
	}
	return 0;
}

#ifdef CONFIG_COMPAT
static
long ltt_compat_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	long ret = -ENOIOCTLCMD;

	lock_kernel();
	ret = ltt_ioctl(file->f_dentry->d_inode, file, cmd, arg);
	unlock_kernel();

	return ret;
}
#endif

static const struct file_operations ltt_file_operations = {
	.open = ltt_open,
	.release = ltt_release,
	.poll = ltt_poll,
	.splice_read = ltt_relay_file_splice_read,
	.ioctl = ltt_ioctl,
	.llseek = ltt_relay_no_llseek,
#ifdef CONFIG_COMPAT
	.compat_ioctl = ltt_compat_ioctl,
#endif
};

int ltt_chanbuf_create_file(const char *filename, struct dentry *parent,
			    int mode, struct ltt_chanbuf *buf)
{
	struct ltt_chan *chan = container_of(buf->a.chan, struct ltt_chan, a);
	char *tmpname;
	int ret = 0;

	tmpname = kzalloc(NAME_MAX + 1, GFP_KERNEL);
	if (!tmpname) {
		ret = -ENOMEM;
		goto end;
	}

	snprintf(tmpname, NAME_MAX, "%s%s_%d",
		 chan->overwrite ? LTT_FLIGHT_PREFIX : "",
		 chan->a.filename, buf->a.cpu);

	buf->a.dentry = debugfs_create_file(tmpname, mode, parent, buf,
					    &ltt_file_operations);
	if (!buf->a.dentry) {
		ret = -ENOMEM;
		goto free_name;
	}
free_name:
	kfree(tmpname);
end:
	return ret;
}

int ltt_chanbuf_remove_file(struct ltt_chanbuf *buf)
{
	debugfs_remove(buf->a.dentry);

	return 0;
}
