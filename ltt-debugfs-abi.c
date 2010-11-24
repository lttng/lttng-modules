/*
 * ltt-debugfs-abi.c
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng debugfs ABI
 *
 * Mimic system calls for:
 * - session creation, returns a file descriptor or failure.
 * - channel creation, returns a file descriptor or failure.
 *   - Takes a session file descriptor parameter
 *   - Takes all channel options as parameters.
 * - event creation, returns a file descriptor or failure.
 *   - Takes an event name as parameter
 *   - Takes an instrumentation source as parameter
 *     - e.g. tracepoints, dynamic_probes...
 *   - Takes instrumentation source specific arguments.
 */

#include <linux/debugfs.h>

/*
 * This is LTTng's own personal way to create a system call as an external
 * module. We use ioctl() on /sys/kernel/debug/lttng.
 */

static struct dentry *lttng_dentry;

/*
 * LTTng DebugFS ABI structures.
 */

struct lttng_channel {
	int session;			/* Session file descriptor */
	int overwrite;			/* 1: overwrite, 0: discard */
	u64 subbuf_size;
	u64 num_subbuf;
	unsigned int switch_timer_interval;
	unsigned int read_timer_interval;
};

struct lttng_event {
	int channel;			/* Channel file descriptor */
	enum instrum_type itype;
	char name[];
};

int lttng_abi_create_session(void)
{
	struct ltt_session *session;
	struct file *session_file;
	int session_fd;

	session = ltt_session_create()
	if (!session)
		return -ENOMEM;
	session_fd = get_unused_fd_flags(O_RDWR);
	if (session_fd < 0) {
		ret = session_fd;
		goto fd_error;
	}
	session_file = anon_inode_getfile("[lttng_session]",
					  &lttng_fops,
					  session, O_RDWR);
	if (IS_ERR(session_file)) {
		ret = PTR_ERR(session_file);
		goto file_error;
	}
	return session_fd;

file_error:
	put_unused_fd(session_fd);
fd_error:
	ltt_session_destroy(session);
	return ret;
}

int lttng_abi_create_channel(struct lttng_channel __user *uchan_param)
{
	struct ltt_channel *chan;
	struct file *chan_file;
	struct lttng_channel chan_param;
	int chan_fd;

	if (copy_from_user(&chan_param, ucham_param, sizeof(chan_param)))
		return -EFAULT;
	/* TODO: fetch session pointer from file descriptor */
	chan = ltt_channel_create();
	if (!chan)
		return -ENOMEM;
	chan_fd = get_unused_fd_flags(O_RDWR);
	if (chan_fd < 0) {
		ret = chan_fd;
		goto fd_error;
	}
	chan_file = anon_inode_getfile("[lttng_channel]",
				       &lttng_fops,
				       chan, O_RDWR);
	if (IS_ERR(chan_file)) {
		ret = PTR_ERR(chan_file);
		goto file_error;
	}
	return chan_fd;

file_error:
	put_unused_fd(chan_fd);
fd_error:
	ltt_channel_destroy(chan);
	return ret;
}

/**
 *	lttng_ioctl - lttng syscall through ioctl
 *
 *	@filp: the file
 *	@cmd: the command
 *	@arg: command arg
 *
 *	This ioctl implements lttng commands:
 *	LTTNG_SESSION
 *		Returns a LTTng trace session file descriptor
 *	LTTNG_CHANNEL
 *		Returns a LTTng channel file descriptor
 *	LTTNG_EVENT
 *		Returns a file descriptor or failure.
 *
 * The returned session will be deleted when its file descriptor is closed.
 * Channel and event file descriptors also hold a reference on the session.
 */
long lttng_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct inode *inode = filp->f_dentry->d_inode;

	switch (cmd) {
	case LTTNG_SESSION:
		return lttng_abi_create_session();
	case LTTNG_CHANNEL:
		return lttng_abi_create_channel((struct lttng_channel __user *)arg);
	case LTTNG_EVENT:
		return lttng_abi_create_event((struct lttng_event __user *)arg);
	default:
		return -ENOIOCTLCMD;
	}
}

#ifdef CONFIG_COMPAT
long lttng_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct inode *inode = filp->f_dentry->d_inode;

	switch (cmd) {
	case LTTNG_SESSION:
		return lttng_abi_create_session();
	case LTTNG_CHANNEL:
		return lttng_abi_create_channel((struct lttng_channel __user *)arg);
	case LTTNG_EVENT:
		return lttng_abi_create_event((struct lttng_event __user *)arg);
	default:
		return -ENOIOCTLCMD;
	}
}
#endif

const struct file_operations lttng_file_operations = {
	.unlocked_ioctl = lttng_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_compat_ioctl,
#endif
}

static int __init ltt_debugfs_abi_init(void)
{
	int ret = 0;

	lttng_dentry = debugfs_create_file("lttng", NULL);
	if (IS_ERR(lttng_dentry) || !lttng_dentry)
		printk(KERN_ERR "Error creating LTTng control file\n");
		ret = -ENOMEM;
		goto error;
	}
error:
	return ret;
}

static void __exit ltt_debugfs_abi_exit(void)
{
	debugfs_remote(lttng_dentry);
}
