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

#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/anon_inodes.h>
#include <linux/file.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "wrapper/vmalloc.h"	/* for wrapper_vmalloc_sync_all() */
#include "wrapper/ringbuffer/vfs.h"
#include "ltt-debugfs-abi.h"
#include "ltt-events.h"

/*
 * This is LTTng's own personal way to create a system call as an external
 * module. We use ioctl() on /sys/kernel/debug/lttng.
 */

static struct dentry *lttng_dentry;
static const struct file_operations lttng_fops;
static const struct file_operations lttng_session_fops;
static const struct file_operations lttng_channel_fops;
static const struct file_operations lttng_metadata_fops;
static const struct file_operations lttng_event_fops;

enum channel_type {
	PER_CPU_CHANNEL,
	METADATA_CHANNEL,
};

static
int lttng_abi_create_session(void)
{
	struct ltt_session *session;
	struct file *session_file;
	int session_fd, ret;

	session = ltt_session_create();
	if (!session)
		return -ENOMEM;
	session_fd = get_unused_fd();
	if (session_fd < 0) {
		ret = session_fd;
		goto fd_error;
	}
	session_file = anon_inode_getfile("[lttng_session]",
					  &lttng_session_fops,
					  session, O_RDWR);
	if (IS_ERR(session_file)) {
		ret = PTR_ERR(session_file);
		goto file_error;
	}
	session->file = session_file;
	fd_install(session_fd, session_file);
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
 *	@file: the file
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
long lttng_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
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
};

/*
 * We tolerate no failure in this function (if one happens, we print a dmesg
 * error, but cannot return any error, because the channel information is
 * invariant.
 */
static
void lttng_metadata_create_events(struct file *channel_file)
{
	struct ltt_channel *channel = channel_file->private_data;
	char *event_name = "lttng-metadata";
	const struct lttng_event_desc *event_desc;
	struct ltt_event *event;
	int ret;

	event_desc = ltt_event_get(event_name);
	if (!event_desc) {
		ret = -ENOENT;
		goto get_error;
	}
	/*
	 * We tolerate no failure path after event creation. It will stay
	 * invariant for the rest of the session.
	 */
	event = ltt_event_create(channel, event_name, INSTRUM_TRACEPOINTS,
				 event_desc, NULL);
	if (!event) {
		goto create_error;
		ret = -EEXIST;
	}
	return;

create_error:
	ltt_event_put(event_desc);
get_error:
	WARN_ON(1);
	return;		/* not allowed to return error */
}

static
int lttng_abi_create_channel(struct file *session_file,
			     struct lttng_channel __user *uchan_param,
			     enum channel_type channel_type)
{
	struct ltt_session *session = session_file->private_data;
	const struct file_operations *fops;
	const char *transport_name;
	struct ltt_channel *chan;
	struct file *chan_file;
	struct lttng_channel chan_param;
	int chan_fd;
	int ret = 0;

	if (copy_from_user(&chan_param, uchan_param, sizeof(chan_param)))
		return -EFAULT;
	chan_fd = get_unused_fd();
	if (chan_fd < 0) {
		ret = chan_fd;
		goto fd_error;
	}
	chan_file = anon_inode_getfile("[lttng_channel]",
				       &lttng_channel_fops,
				       NULL, O_RDWR);
	if (IS_ERR(chan_file)) {
		ret = PTR_ERR(chan_file);
		goto file_error;
	}
	switch (channel_type) {
	case PER_CPU_CHANNEL:
		transport_name = chan_param.overwrite ?
			"relay-overwrite" : "relay-discard";
		fops = &lttng_channel_fops;
		break;
	case METADATA_CHANNEL:
		transport_name = "relay-metadata";
		fops = &lttng_metadata_fops;
		break;
	default:
		transport_name = "<unknown>";
		break;
	}
	/*
	 * We tolerate no failure path after channel creation. It will stay
	 * invariant for the rest of the session.
	 */
	chan = ltt_channel_create(session, transport_name, NULL,
				  chan_param.subbuf_size,
				  chan_param.num_subbuf,
				  chan_param.switch_timer_interval,
				  chan_param.read_timer_interval);
	if (!chan) {
		ret = -EINVAL;
		goto chan_error;
	}
	chan->file = chan_file;
	chan_file->private_data = chan;
	fd_install(chan_fd, chan_file);
	if (channel_type == METADATA_CHANNEL)
		lttng_metadata_create_events(chan_file);

	/* The channel created holds a reference on the session */
	atomic_long_inc(&session_file->f_count);

	return chan_fd;

chan_error:
	fput(chan_file);
file_error:
	put_unused_fd(chan_fd);
fd_error:
	return ret;
}

/**
 *	lttng_session_ioctl - lttng session fd ioctl
 *
 *	@file: the file
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
long lttng_session_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct ltt_session *session = file->private_data;

	switch (cmd) {
	case LTTNG_CHANNEL:
		return lttng_abi_create_channel(file,
				(struct lttng_channel __user *)arg,
				PER_CPU_CHANNEL);
	case LTTNG_SESSION_START:
		return ltt_session_start(session);
	case LTTNG_SESSION_STOP:
		return ltt_session_stop(session);
	case LTTNG_METADATA:
		return lttng_abi_create_channel(file,
				(struct lttng_channel __user *)arg,
				METADATA_CHANNEL);
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

	if (session)
		ltt_session_destroy(session);
	return 0;
}

static const struct file_operations lttng_session_fops = {
	.release = lttng_session_release,
	.unlocked_ioctl = lttng_session_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_session_ioctl,
#endif
};

static
int lttng_abi_open_stream(struct file *channel_file)
{
	struct ltt_channel *channel = channel_file->private_data;
	struct lib_ring_buffer *buf;
	int stream_fd, ret;
	struct file *stream_file;

	buf = channel->ops->buffer_read_open(channel->chan);
	if (!buf)
		return -ENOENT;

	stream_fd = get_unused_fd();
	if (stream_fd < 0) {
		ret = stream_fd;
		goto fd_error;
	}
	stream_file = anon_inode_getfile("[lttng_stream]",
					 &lib_ring_buffer_file_operations,
					 buf, O_RDWR);
	if (IS_ERR(stream_file)) {
		ret = PTR_ERR(stream_file);
		goto file_error;
	}
	/*
	 * OPEN_FMODE, called within anon_inode_getfile/alloc_file, don't honor
	 * FMODE_LSEEK, FMODE_PREAD nor FMODE_PWRITE. We need to read from this
	 * file descriptor, so we set FMODE_PREAD here.
	 */
	stream_file->f_mode |= FMODE_PREAD;
	fd_install(stream_fd, stream_file);
	/*
	 * The stream holds a reference to the channel within the generic ring
	 * buffer library, so no need to hold a refcount on the channel and
	 * session files here.
	 */
	return stream_fd;

file_error:
	put_unused_fd(stream_fd);
fd_error:
	channel->ops->buffer_read_close(buf);
	return ret;
}

static
int lttng_abi_create_event(struct file *channel_file,
			   struct lttng_event __user *uevent_param)
{
	struct ltt_channel *channel = channel_file->private_data;
	const struct lttng_event_desc *event_desc;
	struct ltt_event *event;
	char *event_name;
	struct lttng_event event_param;
	int event_fd, ret;
	struct file *event_file;

	if (copy_from_user(&event_param, uevent_param, sizeof(event_param)))
		return -EFAULT;
	event_name = kmalloc(PATH_MAX, GFP_KERNEL);
	if (!event_name)
		return -ENOMEM;
	if (strncpy_from_user(event_name, uevent_param->name, PATH_MAX) < 0) {
		ret = -EFAULT;
		goto name_error;
	}
	event_name[PATH_MAX - 1] = '\0';

	event_desc = ltt_event_get(event_name);
	if (!event_desc) {
		ret = -ENOENT;
		goto get_error;
	}
	event_fd = get_unused_fd();
	if (event_fd < 0) {
		ret = event_fd;
		goto fd_error;
	}
	event_file = anon_inode_getfile("[lttng_event]",
					&lttng_event_fops,
					NULL, O_RDWR);
	if (IS_ERR(event_file)) {
		ret = PTR_ERR(event_file);
		goto file_error;
	}
	/*
	 * We tolerate no failure path after event creation. It will stay
	 * invariant for the rest of the session.
	 */
	event = ltt_event_create(channel, event_name, event_param.itype,
				 event_desc, NULL);
	if (!event) {
		goto event_error;
		ret = -EEXIST;
	}
	event_file->private_data = event;
	fd_install(event_fd, event_file);
	/* The event holds a reference on the channel */
	atomic_long_inc(&channel_file->f_count);
	kfree(event_name);
	return event_fd;

event_error:
	fput(event_file);
file_error:
	put_unused_fd(event_fd);
fd_error:
	ltt_event_put(event_desc);
get_error:
name_error:
	kfree(event_name);
	return ret;
}

/**
 *	lttng_channel_ioctl - lttng syscall through ioctl
 *
 *	@file: the file
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
 * Channel and event file descriptors also hold a reference on the session.
 */
static
long lttng_channel_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case LTTNG_STREAM:
		return lttng_abi_open_stream(file);
	case LTTNG_EVENT:
		return lttng_abi_create_event(file, (struct lttng_event __user *)arg);
	default:
		return -ENOIOCTLCMD;
	}
}

/**
 *	lttng_metadata_ioctl - lttng syscall through ioctl
 *
 *	@file: the file
 *	@cmd: the command
 *	@arg: command arg
 *
 *	This ioctl implements lttng commands:
 *      LTTNG_STREAM
 *              Returns an event stream file descriptor or failure.
 *
 * Channel and event file descriptors also hold a reference on the session.
 */
static
long lttng_metadata_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case LTTNG_STREAM:
		return lttng_abi_open_stream(file);
	default:
		return -ENOIOCTLCMD;
	}
}

/* TODO: poll */
#if 0
/**
 *	lttng_channel_poll - lttng stream addition/removal monitoring
 *
 *	@file: the file
 *	@wait: poll table
 */
unsigned int lttng_channel_poll(struct file *file, poll_table *wait)
{
	struct ltt_channel *channel = file->private_data;
	unsigned int mask = 0;

	if (file->f_mode & FMODE_READ) {
		poll_wait_set_exclusive(wait);
		poll_wait(file, &channel->notify_wait, wait);

		/* TODO: identify when the channel is being finalized. */
		if (finalized)
			return POLLHUP;
		else
			return POLLIN | POLLRDNORM;
	}
	return mask;

}
#endif //0

static
int lttng_channel_release(struct inode *inode, struct file *file)
{
	struct ltt_channel *channel = file->private_data;

	if (channel)
		fput(channel->session->file);
	return 0;
}

static const struct file_operations lttng_channel_fops = {
	.release = lttng_channel_release,
/* TODO */
#if 0
	.poll = lttng_channel_poll,
#endif //0
	.unlocked_ioctl = lttng_channel_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_channel_ioctl,
#endif
};

static const struct file_operations lttng_metadata_fops = {
	.release = lttng_channel_release,
	.unlocked_ioctl = lttng_metadata_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_metadata_ioctl,
#endif
};

static
int lttng_event_release(struct inode *inode, struct file *file)
{
	struct ltt_event *event = file->private_data;

	if (event)
		fput(event->chan->file);
	return 0;
}

/* TODO: filter control ioctl */
static const struct file_operations lttng_event_fops = {
	.release = lttng_event_release,
};

int __init ltt_debugfs_abi_init(void)
{
	int ret = 0;

	wrapper_vmalloc_sync_all();
	lttng_dentry = debugfs_create_file("lttng", S_IWUSR, NULL, NULL,
					   &lttng_fops);
	if (IS_ERR(lttng_dentry) || !lttng_dentry) {
		printk(KERN_ERR "Error creating LTTng control file\n");
		ret = -ENOMEM;
		goto error;
	}
error:
	return ret;
}

void __exit ltt_debugfs_abi_exit(void)
{
	debugfs_remove(lttng_dentry);
}
