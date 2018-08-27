/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-abi.c
 *
 * LTTng ABI
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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
#include <linux/proc_fs.h>
#include <linux/anon_inodes.h>
#include <linux/file.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <wrapper/vmalloc.h>	/* for wrapper_vmalloc_sync_all() */
#include <wrapper/ringbuffer/vfs.h>
#include <wrapper/ringbuffer/backend.h>
#include <wrapper/ringbuffer/frontend.h>
#include <wrapper/poll.h>
#include <wrapper/file.h>
#include <wrapper/kref.h>
#include <lttng-string-utils.h>
#include <lttng-abi.h>
#include <lttng-abi-old.h>
#include <lttng-events.h>
#include <lttng-tracer.h>
#include <lttng-tp-mempool.h>
#include <lib/ringbuffer/frontend_types.h>

/*
 * This is LTTng's own personal way to create a system call as an external
 * module. We use ioctl() on /proc/lttng.
 */

static struct proc_dir_entry *lttng_proc_dentry;
static const struct file_operations lttng_fops;
static const struct file_operations lttng_session_fops;
static const struct file_operations lttng_channel_fops;
static const struct file_operations lttng_metadata_fops;
static const struct file_operations lttng_event_fops;
static struct file_operations lttng_stream_ring_buffer_file_operations;

static int put_u64(uint64_t val, unsigned long arg);

/*
 * Teardown management: opened file descriptors keep a refcount on the module,
 * so it can only exit when all file descriptors are closed.
 */

static
int lttng_abi_create_session(void)
{
	struct lttng_session *session;
	struct file *session_file;
	int session_fd, ret;

	session = lttng_session_create();
	if (!session)
		return -ENOMEM;
	session_fd = lttng_get_unused_fd();
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
	lttng_session_destroy(session);
	return ret;
}

static
int lttng_abi_tracepoint_list(void)
{
	struct file *tracepoint_list_file;
	int file_fd, ret;

	file_fd = lttng_get_unused_fd();
	if (file_fd < 0) {
		ret = file_fd;
		goto fd_error;
	}

	tracepoint_list_file = anon_inode_getfile("[lttng_tracepoint_list]",
					  &lttng_tracepoint_list_fops,
					  NULL, O_RDWR);
	if (IS_ERR(tracepoint_list_file)) {
		ret = PTR_ERR(tracepoint_list_file);
		goto file_error;
	}
	ret = lttng_tracepoint_list_fops.open(NULL, tracepoint_list_file);
	if (ret < 0)
		goto open_error;
	fd_install(file_fd, tracepoint_list_file);
	return file_fd;

open_error:
	fput(tracepoint_list_file);
file_error:
	put_unused_fd(file_fd);
fd_error:
	return ret;
}

#ifndef CONFIG_HAVE_SYSCALL_TRACEPOINTS
static inline
int lttng_abi_syscall_list(void)
{
	return -ENOSYS;
}
#else
static
int lttng_abi_syscall_list(void)
{
	struct file *syscall_list_file;
	int file_fd, ret;

	file_fd = lttng_get_unused_fd();
	if (file_fd < 0) {
		ret = file_fd;
		goto fd_error;
	}

	syscall_list_file = anon_inode_getfile("[lttng_syscall_list]",
					  &lttng_syscall_list_fops,
					  NULL, O_RDWR);
	if (IS_ERR(syscall_list_file)) {
		ret = PTR_ERR(syscall_list_file);
		goto file_error;
	}
	ret = lttng_syscall_list_fops.open(NULL, syscall_list_file);
	if (ret < 0)
		goto open_error;
	fd_install(file_fd, syscall_list_file);
	return file_fd;

open_error:
	fput(syscall_list_file);
file_error:
	put_unused_fd(file_fd);
fd_error:
	return ret;
}
#endif

static
void lttng_abi_tracer_version(struct lttng_kernel_tracer_version *v)
{
	v->major = LTTNG_MODULES_MAJOR_VERSION;
	v->minor = LTTNG_MODULES_MINOR_VERSION;
	v->patchlevel = LTTNG_MODULES_PATCHLEVEL_VERSION;
}

static
void lttng_abi_tracer_abi_version(struct lttng_kernel_tracer_abi_version *v)
{
	v->major = LTTNG_MODULES_ABI_MAJOR_VERSION;
	v->minor = LTTNG_MODULES_ABI_MINOR_VERSION;
}

static
long lttng_abi_add_context(struct file *file,
	struct lttng_kernel_context *context_param,
	struct lttng_ctx **ctx, struct lttng_session *session)
{

	if (session->been_active)
		return -EPERM;

	switch (context_param->ctx) {
	case LTTNG_KERNEL_CONTEXT_PID:
		return lttng_add_pid_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_PRIO:
		return lttng_add_prio_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_NICE:
		return lttng_add_nice_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_VPID:
		return lttng_add_vpid_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_TID:
		return lttng_add_tid_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_VTID:
		return lttng_add_vtid_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_PPID:
		return lttng_add_ppid_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_VPPID:
		return lttng_add_vppid_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_PERF_COUNTER:
		context_param->u.perf_counter.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		return lttng_add_perf_counter_to_ctx(context_param->u.perf_counter.type,
				context_param->u.perf_counter.config,
				context_param->u.perf_counter.name,
				ctx);
	case LTTNG_KERNEL_CONTEXT_PROCNAME:
		return lttng_add_procname_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_HOSTNAME:
		return lttng_add_hostname_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_CPU_ID:
		return lttng_add_cpu_id_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_INTERRUPTIBLE:
		return lttng_add_interruptible_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_NEED_RESCHEDULE:
		return lttng_add_need_reschedule_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_PREEMPTIBLE:
		return lttng_add_preemptible_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_MIGRATABLE:
		return lttng_add_migratable_to_ctx(ctx);
	case LTTNG_KERNEL_CONTEXT_CALLSTACK_KERNEL:
	case LTTNG_KERNEL_CONTEXT_CALLSTACK_USER:
		return lttng_add_callstack_to_ctx(ctx, context_param->ctx);
	default:
		return -EINVAL;
	}
}

/**
 *	lttng_ioctl - lttng syscall through ioctl
 *
 *	@file: the file
 *	@cmd: the command
 *	@arg: command arg
 *
 *	This ioctl implements lttng commands:
 *	LTTNG_KERNEL_SESSION
 *		Returns a LTTng trace session file descriptor
 *	LTTNG_KERNEL_TRACER_VERSION
 *		Returns the LTTng kernel tracer version
 *	LTTNG_KERNEL_TRACEPOINT_LIST
 *		Returns a file descriptor listing available tracepoints
 *	LTTNG_KERNEL_WAIT_QUIESCENT
 *		Returns after all previously running probes have completed
 *	LTTNG_KERNEL_TRACER_ABI_VERSION
 *		Returns the LTTng kernel tracer ABI version
 *
 * The returned session will be deleted when its file descriptor is closed.
 */
static
long lttng_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case LTTNG_KERNEL_OLD_SESSION:
	case LTTNG_KERNEL_SESSION:
		return lttng_abi_create_session();
	case LTTNG_KERNEL_OLD_TRACER_VERSION:
	{
		struct lttng_kernel_tracer_version v;
		struct lttng_kernel_old_tracer_version oldv;
		struct lttng_kernel_old_tracer_version *uversion =
			(struct lttng_kernel_old_tracer_version __user *) arg;

		lttng_abi_tracer_version(&v);
		oldv.major = v.major;
		oldv.minor = v.minor;
		oldv.patchlevel = v.patchlevel;

		if (copy_to_user(uversion, &oldv, sizeof(oldv)))
			return -EFAULT;
		return 0;
	}
	case LTTNG_KERNEL_TRACER_VERSION:
	{
		struct lttng_kernel_tracer_version version;
		struct lttng_kernel_tracer_version *uversion =
			(struct lttng_kernel_tracer_version __user *) arg;

		lttng_abi_tracer_version(&version);

		if (copy_to_user(uversion, &version, sizeof(version)))
			return -EFAULT;
		return 0;
	}
	case LTTNG_KERNEL_TRACER_ABI_VERSION:
	{
		struct lttng_kernel_tracer_abi_version version;
		struct lttng_kernel_tracer_abi_version *uversion =
			(struct lttng_kernel_tracer_abi_version __user *) arg;

		lttng_abi_tracer_abi_version(&version);

		if (copy_to_user(uversion, &version, sizeof(version)))
			return -EFAULT;
		return 0;
	}
	case LTTNG_KERNEL_OLD_TRACEPOINT_LIST:
	case LTTNG_KERNEL_TRACEPOINT_LIST:
		return lttng_abi_tracepoint_list();
	case LTTNG_KERNEL_SYSCALL_LIST:
		return lttng_abi_syscall_list();
	case LTTNG_KERNEL_OLD_WAIT_QUIESCENT:
	case LTTNG_KERNEL_WAIT_QUIESCENT:
		synchronize_trace();
		return 0;
	case LTTNG_KERNEL_OLD_CALIBRATE:
	{
		struct lttng_kernel_old_calibrate __user *ucalibrate =
			(struct lttng_kernel_old_calibrate __user *) arg;
		struct lttng_kernel_old_calibrate old_calibrate;
		struct lttng_kernel_calibrate calibrate;
		int ret;

		if (copy_from_user(&old_calibrate, ucalibrate, sizeof(old_calibrate)))
			return -EFAULT;
		calibrate.type = old_calibrate.type;
		ret = lttng_calibrate(&calibrate);
		if (copy_to_user(ucalibrate, &old_calibrate, sizeof(old_calibrate)))
			return -EFAULT;
		return ret;
	}
	case LTTNG_KERNEL_CALIBRATE:
	{
		struct lttng_kernel_calibrate __user *ucalibrate =
			(struct lttng_kernel_calibrate __user *) arg;
		struct lttng_kernel_calibrate calibrate;
		int ret;

		if (copy_from_user(&calibrate, ucalibrate, sizeof(calibrate)))
			return -EFAULT;
		ret = lttng_calibrate(&calibrate);
		if (copy_to_user(ucalibrate, &calibrate, sizeof(calibrate)))
			return -EFAULT;
		return ret;
	}
	default:
		return -ENOIOCTLCMD;
	}
}

static const struct file_operations lttng_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = lttng_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_ioctl,
#endif
};

static
int lttng_abi_create_channel(struct file *session_file,
			     struct lttng_kernel_channel *chan_param,
			     enum channel_type channel_type)
{
	struct lttng_session *session = session_file->private_data;
	const struct file_operations *fops = NULL;
	const char *transport_name;
	struct lttng_channel *chan;
	struct file *chan_file;
	int chan_fd;
	int ret = 0;

	chan_fd = lttng_get_unused_fd();
	if (chan_fd < 0) {
		ret = chan_fd;
		goto fd_error;
	}
	switch (channel_type) {
	case PER_CPU_CHANNEL:
		fops = &lttng_channel_fops;
		break;
	case METADATA_CHANNEL:
		fops = &lttng_metadata_fops;
		break;
	}

	chan_file = anon_inode_getfile("[lttng_channel]",
				       fops,
				       NULL, O_RDWR);
	if (IS_ERR(chan_file)) {
		ret = PTR_ERR(chan_file);
		goto file_error;
	}
	switch (channel_type) {
	case PER_CPU_CHANNEL:
		if (chan_param->output == LTTNG_KERNEL_SPLICE) {
			transport_name = chan_param->overwrite ?
				"relay-overwrite" : "relay-discard";
		} else if (chan_param->output == LTTNG_KERNEL_MMAP) {
			transport_name = chan_param->overwrite ?
				"relay-overwrite-mmap" : "relay-discard-mmap";
		} else {
			return -EINVAL;
		}
		break;
	case METADATA_CHANNEL:
		if (chan_param->output == LTTNG_KERNEL_SPLICE)
			transport_name = "relay-metadata";
		else if (chan_param->output == LTTNG_KERNEL_MMAP)
			transport_name = "relay-metadata-mmap";
		else
			return -EINVAL;
		break;
	default:
		transport_name = "<unknown>";
		break;
	}
	if (atomic_long_add_unless(&session_file->f_count,
		1, INT_MAX) == INT_MAX) {
		goto refcount_error;
	}
	/*
	 * We tolerate no failure path after channel creation. It will stay
	 * invariant for the rest of the session.
	 */
	chan = lttng_channel_create(session, transport_name, NULL,
				  chan_param->subbuf_size,
				  chan_param->num_subbuf,
				  chan_param->switch_timer_interval,
				  chan_param->read_timer_interval,
				  channel_type);
	if (!chan) {
		ret = -EINVAL;
		goto chan_error;
	}
	chan->file = chan_file;
	chan_file->private_data = chan;
	fd_install(chan_fd, chan_file);

	return chan_fd;

chan_error:
	atomic_long_dec(&session_file->f_count);
refcount_error:
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
 *	LTTNG_KERNEL_CHANNEL
 *		Returns a LTTng channel file descriptor
 *	LTTNG_KERNEL_ENABLE
 *		Enables tracing for a session (weak enable)
 *	LTTNG_KERNEL_DISABLE
 *		Disables tracing for a session (strong disable)
 *	LTTNG_KERNEL_METADATA
 *		Returns a LTTng metadata file descriptor
 *	LTTNG_KERNEL_SESSION_TRACK_PID
 *		Add PID to session tracker
 *	LTTNG_KERNEL_SESSION_UNTRACK_PID
 *		Remove PID from session tracker
 *
 * The returned channel will be deleted when its file descriptor is closed.
 */
static
long lttng_session_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct lttng_session *session = file->private_data;

	switch (cmd) {
	case LTTNG_KERNEL_OLD_CHANNEL:
	{
		struct lttng_kernel_channel chan_param;
		struct lttng_kernel_old_channel old_chan_param;

		if (copy_from_user(&old_chan_param,
				(struct lttng_kernel_old_channel __user *) arg,
				sizeof(struct lttng_kernel_old_channel)))
			return -EFAULT;
		chan_param.overwrite = old_chan_param.overwrite;
		chan_param.subbuf_size = old_chan_param.subbuf_size;
		chan_param.num_subbuf = old_chan_param.num_subbuf;
		chan_param.switch_timer_interval = old_chan_param.switch_timer_interval;
		chan_param.read_timer_interval = old_chan_param.read_timer_interval;
		chan_param.output = old_chan_param.output;

		return lttng_abi_create_channel(file, &chan_param,
				PER_CPU_CHANNEL);
	}
	case LTTNG_KERNEL_CHANNEL:
	{
		struct lttng_kernel_channel chan_param;

		if (copy_from_user(&chan_param,
				(struct lttng_kernel_channel __user *) arg,
				sizeof(struct lttng_kernel_channel)))
			return -EFAULT;
		return lttng_abi_create_channel(file, &chan_param,
				PER_CPU_CHANNEL);
	}
	case LTTNG_KERNEL_OLD_SESSION_START:
	case LTTNG_KERNEL_OLD_ENABLE:
	case LTTNG_KERNEL_SESSION_START:
	case LTTNG_KERNEL_ENABLE:
		return lttng_session_enable(session);
	case LTTNG_KERNEL_OLD_SESSION_STOP:
	case LTTNG_KERNEL_OLD_DISABLE:
	case LTTNG_KERNEL_SESSION_STOP:
	case LTTNG_KERNEL_DISABLE:
		return lttng_session_disable(session);
	case LTTNG_KERNEL_OLD_METADATA:
	{
		struct lttng_kernel_channel chan_param;
		struct lttng_kernel_old_channel old_chan_param;

		if (copy_from_user(&old_chan_param,
				(struct lttng_kernel_old_channel __user *) arg,
				sizeof(struct lttng_kernel_old_channel)))
			return -EFAULT;
		chan_param.overwrite = old_chan_param.overwrite;
		chan_param.subbuf_size = old_chan_param.subbuf_size;
		chan_param.num_subbuf = old_chan_param.num_subbuf;
		chan_param.switch_timer_interval = old_chan_param.switch_timer_interval;
		chan_param.read_timer_interval = old_chan_param.read_timer_interval;
		chan_param.output = old_chan_param.output;

		return lttng_abi_create_channel(file, &chan_param,
				METADATA_CHANNEL);
	}
	case LTTNG_KERNEL_METADATA:
	{
		struct lttng_kernel_channel chan_param;

		if (copy_from_user(&chan_param,
					(struct lttng_kernel_channel __user *) arg,
					sizeof(struct lttng_kernel_channel)))
			return -EFAULT;
		return lttng_abi_create_channel(file, &chan_param,
				METADATA_CHANNEL);
	}
	case LTTNG_KERNEL_SESSION_TRACK_PID:
		return lttng_session_track_pid(session, (int) arg);
	case LTTNG_KERNEL_SESSION_UNTRACK_PID:
		return lttng_session_untrack_pid(session, (int) arg);
	case LTTNG_KERNEL_SESSION_LIST_TRACKER_PIDS:
		return lttng_session_list_tracker_pids(session);
	case LTTNG_KERNEL_SESSION_METADATA_REGEN:
		return lttng_session_metadata_regenerate(session);
	case LTTNG_KERNEL_SESSION_STATEDUMP:
		return lttng_session_statedump(session);
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
	struct lttng_session *session = file->private_data;

	if (session)
		lttng_session_destroy(session);
	return 0;
}

static const struct file_operations lttng_session_fops = {
	.owner = THIS_MODULE,
	.release = lttng_session_release,
	.unlocked_ioctl = lttng_session_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_session_ioctl,
#endif
};

/**
 *	lttng_metadata_ring_buffer_poll - LTTng ring buffer poll file operation
 *	@filp: the file
 *	@wait: poll table
 *
 *	Handles the poll operations for the metadata channels.
 */
static
unsigned int lttng_metadata_ring_buffer_poll(struct file *filp,
		poll_table *wait)
{
	struct lttng_metadata_stream *stream = filp->private_data;
	struct lib_ring_buffer *buf = stream->priv;
	int finalized;
	unsigned int mask = 0;

	if (filp->f_mode & FMODE_READ) {
		poll_wait_set_exclusive(wait);
		poll_wait(filp, &stream->read_wait, wait);

		finalized = stream->finalized;

		/*
		 * lib_ring_buffer_is_finalized() contains a smp_rmb()
		 * ordering finalized load before offsets loads.
		 */
		WARN_ON(atomic_long_read(&buf->active_readers) != 1);

		if (finalized)
			mask |= POLLHUP;

		mutex_lock(&stream->metadata_cache->lock);
		if (stream->metadata_cache->metadata_written >
				stream->metadata_out)
			mask |= POLLIN;
		mutex_unlock(&stream->metadata_cache->lock);
	}

	return mask;
}

static
void lttng_metadata_ring_buffer_ioctl_put_next_subbuf(struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	struct lttng_metadata_stream *stream = filp->private_data;

	stream->metadata_out = stream->metadata_in;
}

/*
 * Reset the counter of how much metadata has been consumed to 0. That way,
 * the consumer receives the content of the metadata cache unchanged. This is
 * different from the metadata_regenerate where the offset from epoch is
 * resampled, here we want the exact same content as the last time the metadata
 * was generated. This command is only possible if all the metadata written
 * in the cache has been output to the metadata stream to avoid corrupting the
 * metadata file.
 *
 * Return 0 on success, a negative value on error.
 */
static
int lttng_metadata_cache_dump(struct lttng_metadata_stream *stream)
{
	int ret;
	struct lttng_metadata_cache *cache = stream->metadata_cache;

	mutex_lock(&cache->lock);
	if (stream->metadata_out != cache->metadata_written) {
		ret = -EBUSY;
		goto end;
	}
	stream->metadata_out = 0;
	stream->metadata_in = 0;
	wake_up_interruptible(&stream->read_wait);
	ret = 0;

end:
	mutex_unlock(&cache->lock);
	return ret;
}

static
long lttng_metadata_ring_buffer_ioctl(struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	int ret;
	struct lttng_metadata_stream *stream = filp->private_data;
	struct lib_ring_buffer *buf = stream->priv;

	switch (cmd) {
	case RING_BUFFER_GET_NEXT_SUBBUF:
	{
		struct lttng_metadata_stream *stream = filp->private_data;
		struct lib_ring_buffer *buf = stream->priv;
		struct channel *chan = buf->backend.chan;

		ret = lttng_metadata_output_channel(stream, chan);
		if (ret > 0) {
			lib_ring_buffer_switch_slow(buf, SWITCH_ACTIVE);
			ret = 0;
		} else if (ret < 0)
			goto err;
		break;
	}
	case RING_BUFFER_GET_SUBBUF:
	{
		/*
		 * Random access is not allowed for metadata channel.
		 */
		return -ENOSYS;
	}
	case RING_BUFFER_FLUSH_EMPTY:	/* Fall-through. */
	case RING_BUFFER_FLUSH:
	{
		struct lttng_metadata_stream *stream = filp->private_data;
		struct lib_ring_buffer *buf = stream->priv;
		struct channel *chan = buf->backend.chan;

		/*
		 * Before doing the actual ring buffer flush, write up to one
		 * packet of metadata in the ring buffer.
		 */
		ret = lttng_metadata_output_channel(stream, chan);
		if (ret < 0)
			goto err;
		break;
	}
	case RING_BUFFER_GET_METADATA_VERSION:
	{
		struct lttng_metadata_stream *stream = filp->private_data;

		return put_u64(stream->version, arg);
	}
	case RING_BUFFER_METADATA_CACHE_DUMP:
	{
		struct lttng_metadata_stream *stream = filp->private_data;

		return lttng_metadata_cache_dump(stream);
	}
	default:
		break;
	}
	/* PUT_SUBBUF is the one from lib ring buffer, unmodified. */

	/* Performing lib ring buffer ioctl after our own. */
	ret = lib_ring_buffer_ioctl(filp, cmd, arg, buf);
	if (ret < 0)
		goto err;

	switch (cmd) {
	case RING_BUFFER_PUT_NEXT_SUBBUF:
	{
		lttng_metadata_ring_buffer_ioctl_put_next_subbuf(filp,
				cmd, arg);
		break;
	}
	default:
		break;
	}
err:
	return ret;
}

#ifdef CONFIG_COMPAT
static
long lttng_metadata_ring_buffer_compat_ioctl(struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	int ret;
	struct lttng_metadata_stream *stream = filp->private_data;
	struct lib_ring_buffer *buf = stream->priv;

	switch (cmd) {
	case RING_BUFFER_GET_NEXT_SUBBUF:
	{
		struct lttng_metadata_stream *stream = filp->private_data;
		struct lib_ring_buffer *buf = stream->priv;
		struct channel *chan = buf->backend.chan;

		ret = lttng_metadata_output_channel(stream, chan);
		if (ret > 0) {
			lib_ring_buffer_switch_slow(buf, SWITCH_ACTIVE);
			ret = 0;
		} else if (ret < 0)
			goto err;
		break;
	}
	case RING_BUFFER_GET_SUBBUF:
	{
		/*
		 * Random access is not allowed for metadata channel.
		 */
		return -ENOSYS;
	}
	case RING_BUFFER_FLUSH_EMPTY:	/* Fall-through. */
	case RING_BUFFER_FLUSH:
	{
		struct lttng_metadata_stream *stream = filp->private_data;
		struct lib_ring_buffer *buf = stream->priv;
		struct channel *chan = buf->backend.chan;

		/*
		 * Before doing the actual ring buffer flush, write up to one
		 * packet of metadata in the ring buffer.
		 */
		ret = lttng_metadata_output_channel(stream, chan);
		if (ret < 0)
			goto err;
		break;
	}
	case RING_BUFFER_GET_METADATA_VERSION:
	{
		struct lttng_metadata_stream *stream = filp->private_data;

		return put_u64(stream->version, arg);
	}
	case RING_BUFFER_METADATA_CACHE_DUMP:
	{
		struct lttng_metadata_stream *stream = filp->private_data;

		return lttng_metadata_cache_dump(stream);
	}
	default:
		break;
	}
	/* PUT_SUBBUF is the one from lib ring buffer, unmodified. */

	/* Performing lib ring buffer ioctl after our own. */
	ret = lib_ring_buffer_compat_ioctl(filp, cmd, arg, buf);
	if (ret < 0)
		goto err;

	switch (cmd) {
	case RING_BUFFER_PUT_NEXT_SUBBUF:
	{
		lttng_metadata_ring_buffer_ioctl_put_next_subbuf(filp,
				cmd, arg);
		break;
	}
	default:
		break;
	}
err:
	return ret;
}
#endif

/*
 * This is not used by anonymous file descriptors. This code is left
 * there if we ever want to implement an inode with open() operation.
 */
static
int lttng_metadata_ring_buffer_open(struct inode *inode, struct file *file)
{
	struct lttng_metadata_stream *stream = inode->i_private;
	struct lib_ring_buffer *buf = stream->priv;

	file->private_data = buf;
	/*
	 * Since life-time of metadata cache differs from that of
	 * session, we need to keep our own reference on the transport.
	 */
	if (!try_module_get(stream->transport->owner)) {
		printk(KERN_WARNING "LTT : Can't lock transport module.\n");
		return -EBUSY;
	}
	return lib_ring_buffer_open(inode, file, buf);
}

static
int lttng_metadata_ring_buffer_release(struct inode *inode, struct file *file)
{
	struct lttng_metadata_stream *stream = file->private_data;
	struct lib_ring_buffer *buf = stream->priv;

	kref_put(&stream->metadata_cache->refcount, metadata_cache_destroy);
	module_put(stream->transport->owner);
	return lib_ring_buffer_release(inode, file, buf);
}

static
ssize_t lttng_metadata_ring_buffer_splice_read(struct file *in, loff_t *ppos,
		struct pipe_inode_info *pipe, size_t len,
		unsigned int flags)
{
	struct lttng_metadata_stream *stream = in->private_data;
	struct lib_ring_buffer *buf = stream->priv;

	return lib_ring_buffer_splice_read(in, ppos, pipe, len,
			flags, buf);
}

static
int lttng_metadata_ring_buffer_mmap(struct file *filp,
		struct vm_area_struct *vma)
{
	struct lttng_metadata_stream *stream = filp->private_data;
	struct lib_ring_buffer *buf = stream->priv;

	return lib_ring_buffer_mmap(filp, vma, buf);
}

static
const struct file_operations lttng_metadata_ring_buffer_file_operations = {
	.owner = THIS_MODULE,
	.open = lttng_metadata_ring_buffer_open,
	.release = lttng_metadata_ring_buffer_release,
	.poll = lttng_metadata_ring_buffer_poll,
	.splice_read = lttng_metadata_ring_buffer_splice_read,
	.mmap = lttng_metadata_ring_buffer_mmap,
	.unlocked_ioctl = lttng_metadata_ring_buffer_ioctl,
	.llseek = vfs_lib_ring_buffer_no_llseek,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_metadata_ring_buffer_compat_ioctl,
#endif
};

static
int lttng_abi_create_stream_fd(struct file *channel_file, void *stream_priv,
		const struct file_operations *fops)
{
	int stream_fd, ret;
	struct file *stream_file;

	stream_fd = lttng_get_unused_fd();
	if (stream_fd < 0) {
		ret = stream_fd;
		goto fd_error;
	}
	stream_file = anon_inode_getfile("[lttng_stream]", fops,
			stream_priv, O_RDWR);
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
	return ret;
}

static
int lttng_abi_open_stream(struct file *channel_file)
{
	struct lttng_channel *channel = channel_file->private_data;
	struct lib_ring_buffer *buf;
	int ret;
	void *stream_priv;

	buf = channel->ops->buffer_read_open(channel->chan);
	if (!buf)
		return -ENOENT;

	stream_priv = buf;
	ret = lttng_abi_create_stream_fd(channel_file, stream_priv,
			&lttng_stream_ring_buffer_file_operations);
	if (ret < 0)
		goto fd_error;

	return ret;

fd_error:
	channel->ops->buffer_read_close(buf);
	return ret;
}

static
int lttng_abi_open_metadata_stream(struct file *channel_file)
{
	struct lttng_channel *channel = channel_file->private_data;
	struct lttng_session *session = channel->session;
	struct lib_ring_buffer *buf;
	int ret;
	struct lttng_metadata_stream *metadata_stream;
	void *stream_priv;

	buf = channel->ops->buffer_read_open(channel->chan);
	if (!buf)
		return -ENOENT;

	metadata_stream = kzalloc(sizeof(struct lttng_metadata_stream),
			GFP_KERNEL);
	if (!metadata_stream) {
		ret = -ENOMEM;
		goto nomem;
	}
	metadata_stream->metadata_cache = session->metadata_cache;
	init_waitqueue_head(&metadata_stream->read_wait);
	metadata_stream->priv = buf;
	stream_priv = metadata_stream;
	metadata_stream->transport = channel->transport;

	/*
	 * Since life-time of metadata cache differs from that of
	 * session, we need to keep our own reference on the transport.
	 */
	if (!try_module_get(metadata_stream->transport->owner)) {
		printk(KERN_WARNING "LTT : Can't lock transport module.\n");
		ret = -EINVAL;
		goto notransport;
	}

	if (!lttng_kref_get(&session->metadata_cache->refcount)) {
		ret = -EOVERFLOW;
		goto kref_error;
	}

	ret = lttng_abi_create_stream_fd(channel_file, stream_priv,
			&lttng_metadata_ring_buffer_file_operations);
	if (ret < 0)
		goto fd_error;

	list_add(&metadata_stream->list,
		&session->metadata_cache->metadata_stream);
	return ret;

fd_error:
	kref_put(&session->metadata_cache->refcount, metadata_cache_destroy);
kref_error:
	module_put(metadata_stream->transport->owner);
notransport:
	kfree(metadata_stream);
nomem:
	channel->ops->buffer_read_close(buf);
	return ret;
}

static
int lttng_abi_create_event(struct file *channel_file,
			   struct lttng_kernel_event *event_param)
{
	struct lttng_channel *channel = channel_file->private_data;
	int event_fd, ret;
	struct file *event_file;
	void *priv;

	event_param->name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
	switch (event_param->instrumentation) {
	case LTTNG_KERNEL_KRETPROBE:
		event_param->u.kretprobe.symbol_name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		break;
	case LTTNG_KERNEL_KPROBE:
		event_param->u.kprobe.symbol_name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		break;
	case LTTNG_KERNEL_FUNCTION:
		event_param->u.ftrace.symbol_name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		break;
	default:
		break;
	}
	event_fd = lttng_get_unused_fd();
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
	/* The event holds a reference on the channel */
	if (atomic_long_add_unless(&channel_file->f_count,
		1, INT_MAX) == INT_MAX) {
		ret = -EOVERFLOW;
		goto refcount_error;
	}
	if (event_param->instrumentation == LTTNG_KERNEL_TRACEPOINT
			|| event_param->instrumentation == LTTNG_KERNEL_SYSCALL) {
		struct lttng_enabler *enabler;

		if (strutils_is_star_glob_pattern(event_param->name)) {
			/*
			 * If the event name is a star globbing pattern,
			 * we create the special star globbing enabler.
			 */
			enabler = lttng_enabler_create(LTTNG_ENABLER_STAR_GLOB,
				event_param, channel);
		} else {
			enabler = lttng_enabler_create(LTTNG_ENABLER_NAME,
				event_param, channel);
		}
		priv = enabler;
	} else {
		struct lttng_event *event;

		/*
		 * We tolerate no failure path after event creation. It
		 * will stay invariant for the rest of the session.
		 */
		event = lttng_event_create(channel, event_param,
				NULL, NULL,
				event_param->instrumentation);
		WARN_ON_ONCE(!event);
		if (IS_ERR(event)) {
			ret = PTR_ERR(event);
			goto event_error;
		}
		priv = event;
	}
	event_file->private_data = priv;
	fd_install(event_fd, event_file);
	return event_fd;

event_error:
	atomic_long_dec(&channel_file->f_count);
refcount_error:
	fput(event_file);
file_error:
	put_unused_fd(event_fd);
fd_error:
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
 *      LTTNG_KERNEL_STREAM
 *              Returns an event stream file descriptor or failure.
 *              (typically, one event stream records events from one CPU)
 *	LTTNG_KERNEL_EVENT
 *		Returns an event file descriptor or failure.
 *	LTTNG_KERNEL_CONTEXT
 *		Prepend a context field to each event in the channel
 *	LTTNG_KERNEL_ENABLE
 *		Enable recording for events in this channel (weak enable)
 *	LTTNG_KERNEL_DISABLE
 *		Disable recording for events in this channel (strong disable)
 *
 * Channel and event file descriptors also hold a reference on the session.
 */
static
long lttng_channel_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct lttng_channel *channel = file->private_data;

	switch (cmd) {
	case LTTNG_KERNEL_OLD_STREAM:
	case LTTNG_KERNEL_STREAM:
		return lttng_abi_open_stream(file);
	case LTTNG_KERNEL_OLD_EVENT:
	{
		struct lttng_kernel_event *uevent_param;
		struct lttng_kernel_old_event *old_uevent_param;
		int ret;

		uevent_param = kmalloc(sizeof(struct lttng_kernel_event),
				GFP_KERNEL);
		if (!uevent_param) {
			ret = -ENOMEM;
			goto old_event_end;
		}
		old_uevent_param = kmalloc(
				sizeof(struct lttng_kernel_old_event),
				GFP_KERNEL);
		if (!old_uevent_param) {
			ret = -ENOMEM;
			goto old_event_error_free_param;
		}
		if (copy_from_user(old_uevent_param,
				(struct lttng_kernel_old_event __user *) arg,
				sizeof(struct lttng_kernel_old_event))) {
			ret = -EFAULT;
			goto old_event_error_free_old_param;
		}

		memcpy(uevent_param->name, old_uevent_param->name,
				sizeof(uevent_param->name));
		uevent_param->instrumentation =
			old_uevent_param->instrumentation;

		switch (old_uevent_param->instrumentation) {
		case LTTNG_KERNEL_KPROBE:
			uevent_param->u.kprobe.addr =
				old_uevent_param->u.kprobe.addr;
			uevent_param->u.kprobe.offset =
				old_uevent_param->u.kprobe.offset;
			memcpy(uevent_param->u.kprobe.symbol_name,
				old_uevent_param->u.kprobe.symbol_name,
				sizeof(uevent_param->u.kprobe.symbol_name));
			break;
		case LTTNG_KERNEL_KRETPROBE:
			uevent_param->u.kretprobe.addr =
				old_uevent_param->u.kretprobe.addr;
			uevent_param->u.kretprobe.offset =
				old_uevent_param->u.kretprobe.offset;
			memcpy(uevent_param->u.kretprobe.symbol_name,
				old_uevent_param->u.kretprobe.symbol_name,
				sizeof(uevent_param->u.kretprobe.symbol_name));
			break;
		case LTTNG_KERNEL_FUNCTION:
			memcpy(uevent_param->u.ftrace.symbol_name,
					old_uevent_param->u.ftrace.symbol_name,
					sizeof(uevent_param->u.ftrace.symbol_name));
			break;
		default:
			break;
		}
		ret = lttng_abi_create_event(file, uevent_param);

old_event_error_free_old_param:
		kfree(old_uevent_param);
old_event_error_free_param:
		kfree(uevent_param);
old_event_end:
		return ret;
	}
	case LTTNG_KERNEL_EVENT:
	{
		struct lttng_kernel_event uevent_param;

		if (copy_from_user(&uevent_param,
				(struct lttng_kernel_event __user *) arg,
				sizeof(uevent_param)))
			return -EFAULT;
		return lttng_abi_create_event(file, &uevent_param);
	}
	case LTTNG_KERNEL_OLD_CONTEXT:
	{
		struct lttng_kernel_context *ucontext_param;
		struct lttng_kernel_old_context *old_ucontext_param;
		int ret;

		ucontext_param = kmalloc(sizeof(struct lttng_kernel_context),
				GFP_KERNEL);
		if (!ucontext_param) {
			ret = -ENOMEM;
			goto old_ctx_end;
		}
		old_ucontext_param = kmalloc(sizeof(struct lttng_kernel_old_context),
				GFP_KERNEL);
		if (!old_ucontext_param) {
			ret = -ENOMEM;
			goto old_ctx_error_free_param;
		}

		if (copy_from_user(old_ucontext_param,
				(struct lttng_kernel_old_context __user *) arg,
				sizeof(struct lttng_kernel_old_context))) {
			ret = -EFAULT;
			goto old_ctx_error_free_old_param;
		}
		ucontext_param->ctx = old_ucontext_param->ctx;
		memcpy(ucontext_param->padding, old_ucontext_param->padding,
				sizeof(ucontext_param->padding));
		/* only type that uses the union */
		if (old_ucontext_param->ctx == LTTNG_KERNEL_CONTEXT_PERF_COUNTER) {
			ucontext_param->u.perf_counter.type =
				old_ucontext_param->u.perf_counter.type;
			ucontext_param->u.perf_counter.config =
				old_ucontext_param->u.perf_counter.config;
			memcpy(ucontext_param->u.perf_counter.name,
				old_ucontext_param->u.perf_counter.name,
				sizeof(ucontext_param->u.perf_counter.name));
		}

		ret = lttng_abi_add_context(file,
				ucontext_param,
				&channel->ctx, channel->session);

old_ctx_error_free_old_param:
		kfree(old_ucontext_param);
old_ctx_error_free_param:
		kfree(ucontext_param);
old_ctx_end:
		return ret;
	}
	case LTTNG_KERNEL_CONTEXT:
	{
		struct lttng_kernel_context ucontext_param;

		if (copy_from_user(&ucontext_param,
				(struct lttng_kernel_context __user *) arg,
				sizeof(ucontext_param)))
			return -EFAULT;
		return lttng_abi_add_context(file,
				&ucontext_param,
				&channel->ctx, channel->session);
	}
	case LTTNG_KERNEL_OLD_ENABLE:
	case LTTNG_KERNEL_ENABLE:
		return lttng_channel_enable(channel);
	case LTTNG_KERNEL_OLD_DISABLE:
	case LTTNG_KERNEL_DISABLE:
		return lttng_channel_disable(channel);
	case LTTNG_KERNEL_SYSCALL_MASK:
		return lttng_channel_syscall_mask(channel,
			(struct lttng_kernel_syscall_mask __user *) arg);
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
 *      LTTNG_KERNEL_STREAM
 *              Returns an event stream file descriptor or failure.
 *
 * Channel and event file descriptors also hold a reference on the session.
 */
static
long lttng_metadata_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case LTTNG_KERNEL_OLD_STREAM:
	case LTTNG_KERNEL_STREAM:
		return lttng_abi_open_metadata_stream(file);
	default:
		return -ENOIOCTLCMD;
	}
}

/**
 *	lttng_channel_poll - lttng stream addition/removal monitoring
 *
 *	@file: the file
 *	@wait: poll table
 */
unsigned int lttng_channel_poll(struct file *file, poll_table *wait)
{
	struct lttng_channel *channel = file->private_data;
	unsigned int mask = 0;

	if (file->f_mode & FMODE_READ) {
		poll_wait_set_exclusive(wait);
		poll_wait(file, channel->ops->get_hp_wait_queue(channel->chan),
			  wait);

		if (channel->ops->is_disabled(channel->chan))
			return POLLERR;
		if (channel->ops->is_finalized(channel->chan))
			return POLLHUP;
		if (channel->ops->buffer_has_read_closed_stream(channel->chan))
			return POLLIN | POLLRDNORM;
		return 0;
	}
	return mask;

}

static
int lttng_channel_release(struct inode *inode, struct file *file)
{
	struct lttng_channel *channel = file->private_data;

	if (channel)
		fput(channel->session->file);
	return 0;
}

static
int lttng_metadata_channel_release(struct inode *inode, struct file *file)
{
	struct lttng_channel *channel = file->private_data;

	if (channel) {
		fput(channel->session->file);
		lttng_metadata_channel_destroy(channel);
	}

	return 0;
}

static const struct file_operations lttng_channel_fops = {
	.owner = THIS_MODULE,
	.release = lttng_channel_release,
	.poll = lttng_channel_poll,
	.unlocked_ioctl = lttng_channel_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_channel_ioctl,
#endif
};

static const struct file_operations lttng_metadata_fops = {
	.owner = THIS_MODULE,
	.release = lttng_metadata_channel_release,
	.unlocked_ioctl = lttng_metadata_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_metadata_ioctl,
#endif
};

/**
 *	lttng_event_ioctl - lttng syscall through ioctl
 *
 *	@file: the file
 *	@cmd: the command
 *	@arg: command arg
 *
 *	This ioctl implements lttng commands:
 *	LTTNG_KERNEL_CONTEXT
 *		Prepend a context field to each record of this event
 *	LTTNG_KERNEL_ENABLE
 *		Enable recording for this event (weak enable)
 *	LTTNG_KERNEL_DISABLE
 *		Disable recording for this event (strong disable)
 */
static
long lttng_event_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct lttng_event *event;
	struct lttng_enabler *enabler;
	enum lttng_event_type *evtype = file->private_data;

	switch (cmd) {
	case LTTNG_KERNEL_OLD_CONTEXT:
	{
		/* Not implemented */
		return -ENOSYS;
	}
	case LTTNG_KERNEL_CONTEXT:
	{
		/* Not implemented */
		return -ENOSYS;
	}
	case LTTNG_KERNEL_OLD_ENABLE:
	case LTTNG_KERNEL_ENABLE:
		switch (*evtype) {
		case LTTNG_TYPE_EVENT:
			event = file->private_data;
			return lttng_event_enable(event);
		case LTTNG_TYPE_ENABLER:
			enabler = file->private_data;
			return lttng_enabler_enable(enabler);
		default:
			WARN_ON_ONCE(1);
			return -ENOSYS;
		}
	case LTTNG_KERNEL_OLD_DISABLE:
	case LTTNG_KERNEL_DISABLE:
		switch (*evtype) {
		case LTTNG_TYPE_EVENT:
			event = file->private_data;
			return lttng_event_disable(event);
		case LTTNG_TYPE_ENABLER:
			enabler = file->private_data;
			return lttng_enabler_disable(enabler);
		default:
			WARN_ON_ONCE(1);
			return -ENOSYS;
		}
	case LTTNG_KERNEL_FILTER:
		switch (*evtype) {
		case LTTNG_TYPE_EVENT:
			return -EINVAL;
		case LTTNG_TYPE_ENABLER:
		{
			enabler = file->private_data;
			return lttng_enabler_attach_bytecode(enabler,
				(struct lttng_kernel_filter_bytecode __user *) arg);
		}

		}
	case LTTNG_KERNEL_ADD_CALLSITE:
		switch (*evtype) {
		case LTTNG_TYPE_EVENT:
			event = file->private_data;
			return lttng_event_add_callsite(event,
				(struct lttng_kernel_event_callsite __user *) arg);
		case LTTNG_TYPE_ENABLER:
			return -EINVAL;
		}
	default:
		return -ENOIOCTLCMD;
	}
}

static
int lttng_event_release(struct inode *inode, struct file *file)
{
	struct lttng_event *event;
	struct lttng_enabler *enabler;
	enum lttng_event_type *evtype = file->private_data;

	if (!evtype)
		return 0;

	switch (*evtype) {
	case LTTNG_TYPE_EVENT:
		event = file->private_data;
		if (event)
			fput(event->chan->file);
		break;
	case LTTNG_TYPE_ENABLER:
		enabler = file->private_data;
		if (enabler)
			fput(enabler->chan->file);
		break;
	default:
		WARN_ON_ONCE(1);
		break;
	}

	return 0;
}

/* TODO: filter control ioctl */
static const struct file_operations lttng_event_fops = {
	.owner = THIS_MODULE,
	.release = lttng_event_release,
	.unlocked_ioctl = lttng_event_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_event_ioctl,
#endif
};

static int put_u64(uint64_t val, unsigned long arg)
{
	return put_user(val, (uint64_t __user *) arg);
}

static long lttng_stream_ring_buffer_ioctl(struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	struct lib_ring_buffer *buf = filp->private_data;
	struct channel *chan = buf->backend.chan;
	const struct lib_ring_buffer_config *config = &chan->backend.config;
	const struct lttng_channel_ops *ops = chan->backend.priv_ops;
	int ret;

	if (atomic_read(&chan->record_disabled))
		return -EIO;

	switch (cmd) {
	case LTTNG_RING_BUFFER_GET_TIMESTAMP_BEGIN:
	{
		uint64_t ts;

		ret = ops->timestamp_begin(config, buf, &ts);
		if (ret < 0)
			goto error;
		return put_u64(ts, arg);
	}
	case LTTNG_RING_BUFFER_GET_TIMESTAMP_END:
	{
		uint64_t ts;

		ret = ops->timestamp_end(config, buf, &ts);
		if (ret < 0)
			goto error;
		return put_u64(ts, arg);
	}
	case LTTNG_RING_BUFFER_GET_EVENTS_DISCARDED:
	{
		uint64_t ed;

		ret = ops->events_discarded(config, buf, &ed);
		if (ret < 0)
			goto error;
		return put_u64(ed, arg);
	}
	case LTTNG_RING_BUFFER_GET_CONTENT_SIZE:
	{
		uint64_t cs;

		ret = ops->content_size(config, buf, &cs);
		if (ret < 0)
			goto error;
		return put_u64(cs, arg);
	}
	case LTTNG_RING_BUFFER_GET_PACKET_SIZE:
	{
		uint64_t ps;

		ret = ops->packet_size(config, buf, &ps);
		if (ret < 0)
			goto error;
		return put_u64(ps, arg);
	}
	case LTTNG_RING_BUFFER_GET_STREAM_ID:
	{
		uint64_t si;

		ret = ops->stream_id(config, buf, &si);
		if (ret < 0)
			goto error;
		return put_u64(si, arg);
	}
	case LTTNG_RING_BUFFER_GET_CURRENT_TIMESTAMP:
	{
		uint64_t ts;

		ret = ops->current_timestamp(config, buf, &ts);
		if (ret < 0)
			goto error;
		return put_u64(ts, arg);
	}
	case LTTNG_RING_BUFFER_GET_SEQ_NUM:
	{
		uint64_t seq;

		ret = ops->sequence_number(config, buf, &seq);
		if (ret < 0)
			goto error;
		return put_u64(seq, arg);
	}
	case LTTNG_RING_BUFFER_INSTANCE_ID:
	{
		uint64_t id;

		ret = ops->instance_id(config, buf, &id);
		if (ret < 0)
			goto error;
		return put_u64(id, arg);
	}
	default:
		return lib_ring_buffer_file_operations.unlocked_ioctl(filp,
				cmd, arg);
	}

error:
	return -ENOSYS;
}

#ifdef CONFIG_COMPAT
static long lttng_stream_ring_buffer_compat_ioctl(struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	struct lib_ring_buffer *buf = filp->private_data;
	struct channel *chan = buf->backend.chan;
	const struct lib_ring_buffer_config *config = &chan->backend.config;
	const struct lttng_channel_ops *ops = chan->backend.priv_ops;
	int ret;

	if (atomic_read(&chan->record_disabled))
		return -EIO;

	switch (cmd) {
	case LTTNG_RING_BUFFER_COMPAT_GET_TIMESTAMP_BEGIN:
	{
		uint64_t ts;

		ret = ops->timestamp_begin(config, buf, &ts);
		if (ret < 0)
			goto error;
		return put_u64(ts, arg);
	}
	case LTTNG_RING_BUFFER_COMPAT_GET_TIMESTAMP_END:
	{
		uint64_t ts;

		ret = ops->timestamp_end(config, buf, &ts);
		if (ret < 0)
			goto error;
		return put_u64(ts, arg);
	}
	case LTTNG_RING_BUFFER_COMPAT_GET_EVENTS_DISCARDED:
	{
		uint64_t ed;

		ret = ops->events_discarded(config, buf, &ed);
		if (ret < 0)
			goto error;
		return put_u64(ed, arg);
	}
	case LTTNG_RING_BUFFER_COMPAT_GET_CONTENT_SIZE:
	{
		uint64_t cs;

		ret = ops->content_size(config, buf, &cs);
		if (ret < 0)
			goto error;
		return put_u64(cs, arg);
	}
	case LTTNG_RING_BUFFER_COMPAT_GET_PACKET_SIZE:
	{
		uint64_t ps;

		ret = ops->packet_size(config, buf, &ps);
		if (ret < 0)
			goto error;
		return put_u64(ps, arg);
	}
	case LTTNG_RING_BUFFER_COMPAT_GET_STREAM_ID:
	{
		uint64_t si;

		ret = ops->stream_id(config, buf, &si);
		if (ret < 0)
			goto error;
		return put_u64(si, arg);
	}
	case LTTNG_RING_BUFFER_GET_CURRENT_TIMESTAMP:
	{
		uint64_t ts;

		ret = ops->current_timestamp(config, buf, &ts);
		if (ret < 0)
			goto error;
		return put_u64(ts, arg);
	}
	case LTTNG_RING_BUFFER_COMPAT_GET_SEQ_NUM:
	{
		uint64_t seq;

		ret = ops->sequence_number(config, buf, &seq);
		if (ret < 0)
			goto error;
		return put_u64(seq, arg);
	}
	case LTTNG_RING_BUFFER_COMPAT_INSTANCE_ID:
	{
		uint64_t id;

		ret = ops->instance_id(config, buf, &id);
		if (ret < 0)
			goto error;
		return put_u64(id, arg);
	}
	default:
		return lib_ring_buffer_file_operations.compat_ioctl(filp,
				cmd, arg);
	}

error:
	return -ENOSYS;
}
#endif /* CONFIG_COMPAT */

static void lttng_stream_override_ring_buffer_fops(void)
{
	lttng_stream_ring_buffer_file_operations.owner = THIS_MODULE;
	lttng_stream_ring_buffer_file_operations.open =
		lib_ring_buffer_file_operations.open;
	lttng_stream_ring_buffer_file_operations.release =
		lib_ring_buffer_file_operations.release;
	lttng_stream_ring_buffer_file_operations.poll =
		lib_ring_buffer_file_operations.poll;
	lttng_stream_ring_buffer_file_operations.splice_read =
		lib_ring_buffer_file_operations.splice_read;
	lttng_stream_ring_buffer_file_operations.mmap =
		lib_ring_buffer_file_operations.mmap;
	lttng_stream_ring_buffer_file_operations.unlocked_ioctl =
		lttng_stream_ring_buffer_ioctl;
	lttng_stream_ring_buffer_file_operations.llseek =
		lib_ring_buffer_file_operations.llseek;
#ifdef CONFIG_COMPAT
	lttng_stream_ring_buffer_file_operations.compat_ioctl =
		lttng_stream_ring_buffer_compat_ioctl;
#endif
}

int __init lttng_abi_init(void)
{
	int ret = 0;

	wrapper_vmalloc_sync_all();
	lttng_clock_ref();

	ret = lttng_tp_mempool_init();
	if (ret) {
		goto error;
	}

	lttng_proc_dentry = proc_create_data("lttng", S_IRUSR | S_IWUSR, NULL,
					&lttng_fops, NULL);

	if (!lttng_proc_dentry) {
		printk(KERN_ERR "Error creating LTTng control file\n");
		ret = -ENOMEM;
		goto error;
	}
	lttng_stream_override_ring_buffer_fops();
	return 0;

error:
	lttng_tp_mempool_destroy();
	lttng_clock_unref();
	return ret;
}

/* No __exit annotation because used by init error path too. */
void lttng_abi_exit(void)
{
	lttng_tp_mempool_destroy();
	lttng_clock_unref();
	if (lttng_proc_dentry)
		remove_proc_entry("lttng", NULL);
}
