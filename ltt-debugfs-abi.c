/*
 * ltt-debugfs-abi.c
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng debugfs ABI
 *
 * Mimic system calls for:
 * - session creation, returns a file descriptor or failure.
 *   - channel creation, returns a file descriptor or failure.
 *     - Operates on a session file descriptor
 *     - Takes all channel options as parameters.
 *   - stream get, returns a file descriptor or failure.
 *     - Operates on a channel file descriptor.
 *   - stream notifier get, returns a file descriptor or failure.
 *     - Operates on a channel file descriptor.
 *   - event creation, returns a file descriptor or failure.
 *     - Operates on a channel file descriptor
 *     - Takes an event name as parameter
 *     - Takes an instrumentation source as parameter
 *       - e.g. tracepoints, dynamic_probes...
 *     - Takes instrumentation source specific arguments.
 */

#include <linux/debugfs.h>
#include "ltt-events.h"

/*
 * This is LTTng's own personal way to create a system call as an external
 * module. We use ioctl() on /sys/kernel/debug/lttng.
 */

static struct dentry *lttng_dentry;
static const struct file_operations lttng_fops;
static const struct file_operations lttng_session_fops;
static const struct file_operations lttng_channel_fops;

/*
 * LTTng DebugFS ABI structures.
 */

struct lttng_channel {
	int overwrite;			/* 1: overwrite, 0: discard */
	u64 subbuf_size;
	u64 num_subbuf;
	unsigned int switch_timer_interval;
	unsigned int read_timer_interval;
};

struct lttng_event {
	enum instrum_type itype;
	char name[];
};

static
int lttng_abi_create_session(void)
{
	struct ltt_session *session;
	struct file *session_filp;
	int session_fd;

	session = ltt_session_create();
	if (!session)
		return -ENOMEM;
	session_fd = get_unused_fd_flags(O_RDWR);
	if (session_fd < 0) {
		ret = session_fd;
		goto fd_error;
	}
	session_filp = anon_inode_getfile("[lttng_session]",
					  &lttng_session_fops,
					  session, O_RDWR);
	if (IS_ERR(session_filp)) {
		ret = PTR_ERR(session_filp);
		goto file_error;
	}
	fd_install(session_fd, session_filp);
	return session_fd;

file_error:
	put_unused_fd(session_fd);
fd_error:
	ltt_session_destroy(session);
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
 *
 * The returned session will be deleted when its file descriptor is closed.
 */
static
long lttng_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case LTTNG_SESSION:
		return lttng_abi_create_session();
	default:
		return -ENOIOCTLCMD;
	}
}

static const struct file_operations lttng_fops = {
	.unlocked_ioctl = lttng_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_ioctl,
#endif
}

int lttng_abi_create_channel(struct file *session_filp,
			     struct lttng_channel __user *uchan_param)
{
	struct ltt_session *session = session_filp->private_data;
	struct ltt_channel *chan;
	struct file *chan_filp;
	struct lttng_channel chan_param;
	int chan_fd;
	int ret = 0;

	if (copy_from_user(&chan_param, uchan_param, sizeof(chan_param)))
		return -EFAULT;
	chan_fd = get_unused_fd_flags(O_RDWR);
	if (chan_fd < 0) {
		ret = chan_fd;
		goto fd_error;
	}
	chan_filp = anon_inode_getfile("[lttng_channel]",
				       &lttng_channel_fops,
				       NULL, O_RDWR);
	if (IS_ERR(chan_filp)) {
		ret = PTR_ERR(chan_filp);
		goto file_error;
	}
	/*
	 * We tolerate no failure path after channel creation. It will stay
	 * invariant for the rest of the session.
	 */
	chan = ltt_channel_create(session, chan_param->overwrite, NULL,
				  chan_param->subbuf_size,
				  chan_param->num_subbuf,
				  chan_param->switch_timer_interval,
				  chan_param->read_timer_interval);
	if (!chan) {
		ret = -ENOMEM;
		goto chan_error;
	}
	chan_filp->private_data = chan;
	fd_install(chan_fd, chan_filp);
	/* The channel created holds a reference on the session */
	atomic_inc(&session_filp->f_count);

	return chan_fd;

chan_error:
	fput(chan_filp);
file_error:
	put_unused_fd(chan_fd);
fd_error:
	return ret;
}

/**
 *	lttng_session_ioctl - lttng session fd ioctl
 *
 *	@filp: the file
 *	@cmd: the command
 *	@arg: command arg
 *
 *	This ioctl implements lttng commands:
 *	LTTNG_CHANNEL
 *		Returns a LTTng channel file descriptor
 *
 * The returned channel will be deleted when its file descriptor is closed.
 */
static
long lttng_session_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case LTTNG_CHANNEL:
		return lttng_abi_create_channel(filp, (struct lttng_channel __user *)arg);
	default:
		return -ENOIOCTLCMD;
	}
}

/*
 * Called when the last file reference is dropped.
 *
 * Big fat note: channels and events are invariant for the whole session after
 * their creation. So this session destruction also destroys all channel and
 * event structures specific to this session (they are not destroyed when their
 * individual file is released).
 */
static
int lttng_session_release(struct inode *inode, struct file *file)
{
	struct ltt_session *session = file->private_data;
	return ltt_session_destroy(session);
}

static const struct file_operations lttng_session_fops = {
	.release = lttng_session_release,
	.unlocked_ioctl = lttng_session_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_session_ioctl,
#endif
}

static
int lttng_abi_open_stream(struct file *channel_filp)
{
	struct ltt_channel *channel = channel_filp->private_data;
	struct lib_ring_buffer *buf;
	int stream_fd, ret;

	buf = ltt_buffer_read_open(channel->chan);
	if (!buf)
		return -ENOENT;

	stream_fd = get_unused_fd_flags(O_RDWR);
	if (stream_fd < 0) {
		ret = stream_fd;
		goto fd_error;
	}
	stream_filp = anon_inode_getfile("[lttng_stream]",
					 &lib_ring_buffer_file_operations,
					 buf, O_RDWR);
	if (IS_ERR(stream_filp)) {
		ret = PTR_ERR(stream_filp);
		goto file_error;
	}
	fd_install(stream_fd, stream_filp);
	/* The stream holds a reference on the channel */
	atomic_inc(&channel_filp->f_count);
	return stream_fd;

file_error:
	put_unused_fd(stream_fd);
fd_error:
	ltt_buffer_read_close(buf);
	return ret;
}

static
int lttng_abi_create_event(struct file *channel_filp,
			   struct lttng_event __user *uevent_param)
{
	struct ltt_channel *channel = channel_filp->private_data;
	struct ltt_event *event;
	char *event_name;
	struct lttng_event event_param;
	int event_fd, ret;

	if (copy_from_user(&event_param, uevent_param, sizeof(event_param)))
		return -EFAULT;
	event_name = kmalloc(PATH_MAX, GFP_KERNEL);
	if (!event_name)
		return -ENOMEM;
	if (strncpy_from_user(event_name, &uevent_param->name, PATH_MAX)) {
		ret = -EFAULT;
		goto name_error;
	}
	event_name[PATH_MAX - 1] = '\0';
	event_fd = get_unused_fd_flags(O_RDWR);
	if (event_fd < 0) {
		ret = event_fd;
		goto fd_error;
	}
	event_filp = anon_inode_getfile("[lttng_event]",
					&lttng_event_fops,
					NULL, O_RDWR);
	if (IS_ERR(event_filp)) {
		ret = PTR_ERR(event_filp);
		goto file_error;
	}
	/*
	 * We tolerate no failure path after event creation. It will stay
	 * invariant for the rest of the session.
	 */
	event = ltt_event_create(channel, event_param->itype, event_name, NULL);
	if (!event) {
		goto event_error;
		ret = -EEXIST;
	}
	event_filp->private_data = event;
	fd_install(event_fd, event_filp);
	/* The event holds a reference on the channel */
	atomic_inc(&channel_filp->f_count);
	kfree(event_name);
	return event_fd;

event_error:
	fput(event_filp);
file_error:
	put_unused_fd(event_fd);
fd_error:
name_error:
	kfree(event_name);
	return ret;
}

/**
 *	lttng_channel_ioctl - lttng syscall through ioctl
 *
 *	@filp: the file
 *	@cmd: the command
 *	@arg: command arg
 *
 *	This ioctl implements lttng commands:
 *      LTTNG_STREAM
 *              Returns an event stream file descriptor or failure.
 *              (typically, one event stream records events from one CPU)
 *	LTTNG_EVENT
 *		Returns an event file descriptor or failure.
 *
 * The returned session will be deleted when its file descriptor is closed.
 * Channel and event file descriptors also hold a reference on the session.
 */
static
long lttng_channel_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case LTTNG_STREAM:
		return lttng_abi_open_stream(filp);
	case LTTNG_EVENT:
		return lttng_abi_create_event(filp, (struct lttng_event __user *)arg);
	default:
		return -ENOIOCTLCMD;
	}
}

/**
 *	lttng_channel_poll - lttng stream addition/removal monitoring
 *
 *	@filp: the file
 *	@wait: poll table
 */
unsigned int lttng_channel_poll(struct file *filp, poll_table *wait)
{
	struct ltt_channel *channel = filp->private_data;
	unsigned int mask = 0;

	if (filp->f_mode & FMODE_READ) {
		poll_wait_set_exclusive(wait);
		poll_wait(filp, &channel->notify_wait, wait);

		/* TODO: identify when the channel is being finalized. */
		if (finalized)
			return POLLHUP;
		else
			return POLLIN | POLLRDNORM;
	}
	return mask;

}

static const struct file_operations lttng_channel_fops = {
	.release = lttng_channel_release,
	.poll = lttng_channel_poll,
	.unlocked_ioctl = lttng_channel_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_channel_ioctl,
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
