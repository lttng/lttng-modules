/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
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

#include <asm/barrier.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/anon_inodes.h>
#include <linux/file.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <wrapper/vmalloc.h>	/* for wrapper_vmalloc_sync_mappings() */
#include <ringbuffer/vfs.h>
#include <ringbuffer/backend.h>
#include <ringbuffer/frontend.h>
#include <wrapper/compiler_attributes.h>
#include <wrapper/file_ref.h>
#include <wrapper/poll.h>
#include <wrapper/kref.h>
#include <wrapper/uaccess.h>
#include <lttng/string-utils.h>
#include <lttng/abi.h>
#include <lttng/abi-old.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <lttng/tracer.h>
#include <lttng/tp-mempool.h>
#include <ringbuffer/frontend_types.h>
#include <ringbuffer/iterator.h>

/*
 * This is LTTng's own personal way to create a system call as an external
 * module. We use ioctl() on /proc/lttng.
 */

static struct proc_dir_entry *lttng_proc_dentry;

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,6,0))
static const struct proc_ops lttng_proc_ops;
#else
static const struct file_operations lttng_proc_ops;
#endif

static const struct file_operations lttng_session_fops;
static const struct file_operations lttng_event_notifier_group_fops;
static const struct file_operations lttng_channel_fops;
static const struct file_operations lttng_metadata_fops;
static const struct file_operations lttng_event_session_fops;
static const struct file_operations lttng_event_session_enabler_fops;
static struct file_operations lttng_stream_ring_buffer_file_operations;

static int put_u64(uint64_t val, unsigned long arg);
static int put_u32(uint32_t val, unsigned long arg);

static
int lttng_abi_create_event_counter_enabler(struct file *channel_file,
			struct lttng_kernel_counter_event *counter_event);
static
long lttng_abi_session_create_counter(
		struct lttng_kernel_session *session,
		const struct lttng_kernel_counter_conf *counter_conf);

static int validate_zeroed_padding(char *p, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++) {
		if (p[i])
			return -1;
	}
	return 0;
}

/*
 * Teardown management: opened file descriptors keep a refcount on the module,
 * so it can only exit when all file descriptors are closed.
 */

static
int lttng_abi_create_session(void)
{
	struct lttng_kernel_session *session;
	struct file *session_file;
	int session_fd, ret;

	session = lttng_session_create();
	if (!session)
		return -ENOMEM;
	session_fd = get_unused_fd_flags(0);
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
	session->priv->file = session_file;
	fd_install(session_fd, session_file);
	return session_fd;

file_error:
	put_unused_fd(session_fd);
fd_error:
	lttng_session_destroy(session);
	return ret;
}

static
void event_notifier_send_notification_work_wakeup(struct irq_work *entry)
{
	struct lttng_event_notifier_group *event_notifier_group =
			container_of(entry, struct lttng_event_notifier_group,
					wakeup_pending);
	wake_up_interruptible(&event_notifier_group->read_wait);
}

static
int lttng_abi_create_event_notifier_group(void)
{
	struct lttng_event_notifier_group *event_notifier_group;
	struct file *event_notifier_group_file;
	int event_notifier_group_fd, ret;

	event_notifier_group = lttng_event_notifier_group_create();
	if (!event_notifier_group)
		return -ENOMEM;

	event_notifier_group_fd = get_unused_fd_flags(0);
	if (event_notifier_group_fd < 0) {
		ret = event_notifier_group_fd;
		goto fd_error;
	}
	event_notifier_group_file = anon_inode_getfile("[lttng_event_notifier_group]",
					  &lttng_event_notifier_group_fops,
					  event_notifier_group, O_RDWR);
	if (IS_ERR(event_notifier_group_file)) {
		ret = PTR_ERR(event_notifier_group_file);
		goto file_error;
	}

	event_notifier_group->file = event_notifier_group_file;
	init_waitqueue_head(&event_notifier_group->read_wait);
	init_irq_work(&event_notifier_group->wakeup_pending,
		      event_notifier_send_notification_work_wakeup);
	fd_install(event_notifier_group_fd, event_notifier_group_file);
	return event_notifier_group_fd;

file_error:
	put_unused_fd(event_notifier_group_fd);
fd_error:
	lttng_event_notifier_group_destroy(event_notifier_group);
	return ret;
}

static
int lttng_abi_tracepoint_list(void)
{
	struct file *tracepoint_list_file;
	int file_fd, ret;

	file_fd = get_unused_fd_flags(0);
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

	file_fd = get_unused_fd_flags(0);
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
void lttng_abi_tracer_version(struct lttng_kernel_abi_tracer_version *v)
{
	v->major = LTTNG_MODULES_MAJOR_VERSION;
	v->minor = LTTNG_MODULES_MINOR_VERSION;
	v->patchlevel = LTTNG_MODULES_PATCHLEVEL_VERSION;
}

static
void lttng_abi_tracer_abi_version(struct lttng_kernel_abi_tracer_abi_version *v)
{
	v->major = LTTNG_KERNEL_ABI_MAJOR_VERSION;
	v->minor = LTTNG_KERNEL_ABI_MINOR_VERSION;
}

static
long lttng_abi_add_context(struct file *file,
	struct lttng_kernel_abi_context *context_param,
	struct lttng_kernel_ctx **ctx, struct lttng_kernel_session *session)
{

	if (session->priv->been_active)
		return -EPERM;

	switch (context_param->ctx) {
	case LTTNG_KERNEL_ABI_CONTEXT_PID:
		return lttng_add_pid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_PRIO:
		return lttng_add_prio_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_NICE:
		return lttng_add_nice_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_VPID:
		return lttng_add_vpid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_TID:
		return lttng_add_tid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_VTID:
		return lttng_add_vtid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_PPID:
		return lttng_add_ppid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_VPPID:
		return lttng_add_vppid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_PERF_COUNTER:
		context_param->u.perf_counter.name[LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1] = '\0';
		return lttng_add_perf_counter_to_ctx(context_param->u.perf_counter.type,
				context_param->u.perf_counter.config,
				context_param->u.perf_counter.name,
				ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_PROCNAME:
		return lttng_add_procname_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_HOSTNAME:
		return lttng_add_hostname_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_CPU_ID:
		return lttng_add_cpu_id_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_INTERRUPTIBLE:
		return lttng_add_interruptible_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_NEED_RESCHEDULE:
		return lttng_add_need_reschedule_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_PREEMPTIBLE:
		return lttng_add_preemptible_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_MIGRATABLE:
		return lttng_add_migratable_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_CALLSTACK_KERNEL:
	case LTTNG_KERNEL_ABI_CONTEXT_CALLSTACK_USER:
		return lttng_add_callstack_to_ctx(ctx, context_param->ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_CGROUP_NS:
		return lttng_add_cgroup_ns_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_IPC_NS:
		return lttng_add_ipc_ns_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_MNT_NS:
		return lttng_add_mnt_ns_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_NET_NS:
		return lttng_add_net_ns_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_PID_NS:
		return lttng_add_pid_ns_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_USER_NS:
		return lttng_add_user_ns_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_UTS_NS:
		return lttng_add_uts_ns_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_UID:
		return lttng_add_uid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_EUID:
		return lttng_add_euid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_SUID:
		return lttng_add_suid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_GID:
		return lttng_add_gid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_EGID:
		return lttng_add_egid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_SGID:
		return lttng_add_sgid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_VUID:
		return lttng_add_vuid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_VEUID:
		return lttng_add_veuid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_VSUID:
		return lttng_add_vsuid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_VGID:
		return lttng_add_vgid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_VEGID:
		return lttng_add_vegid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_VSGID:
		return lttng_add_vsgid_to_ctx(ctx);
	case LTTNG_KERNEL_ABI_CONTEXT_TIME_NS:
		return lttng_add_time_ns_to_ctx(ctx);
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
 *	LTTNG_KERNEL_ABI_SESSION
 *		Returns a LTTng trace session file descriptor
 *	LTTNG_KERNEL_ABI_TRACER_VERSION
 *		Returns the LTTng kernel tracer version
 *	LTTNG_KERNEL_ABI_TRACEPOINT_LIST
 *		Returns a file descriptor listing available tracepoints
 *	LTTNG_KERNEL_ABI_WAIT_QUIESCENT
 *		Returns after all previously running probes have completed
 *	LTTNG_KERNEL_ABI_TRACER_ABI_VERSION
 *		Returns the LTTng kernel tracer ABI version
 *	LTTNG_KERNEL_ABI_EVENT_NOTIFIER_GROUP_CREATE
 *		Returns a LTTng event notifier group file descriptor
 *
 * The returned session will be deleted when its file descriptor is closed.
 */
static
long lttng_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case LTTNG_KERNEL_ABI_OLD_SESSION:
	case LTTNG_KERNEL_ABI_SESSION:
		return lttng_abi_create_session();
	case LTTNG_KERNEL_ABI_EVENT_NOTIFIER_GROUP_CREATE:
		return lttng_abi_create_event_notifier_group();
	case LTTNG_KERNEL_ABI_OLD_TRACER_VERSION:
	{
		struct lttng_kernel_abi_tracer_version v;
		struct lttng_kernel_abi_old_tracer_version oldv;
		struct lttng_kernel_abi_old_tracer_version *uversion =
			(struct lttng_kernel_abi_old_tracer_version __user *) arg;

		lttng_abi_tracer_version(&v);
		oldv.major = v.major;
		oldv.minor = v.minor;
		oldv.patchlevel = v.patchlevel;

		if (copy_to_user(uversion, &oldv, sizeof(oldv)))
			return -EFAULT;
		return 0;
	}
	case LTTNG_KERNEL_ABI_TRACER_VERSION:
	{
		struct lttng_kernel_abi_tracer_version version;
		struct lttng_kernel_abi_tracer_version *uversion =
			(struct lttng_kernel_abi_tracer_version __user *) arg;

		lttng_abi_tracer_version(&version);

		if (copy_to_user(uversion, &version, sizeof(version)))
			return -EFAULT;
		return 0;
	}
	case LTTNG_KERNEL_ABI_TRACER_ABI_VERSION:
	{
		struct lttng_kernel_abi_tracer_abi_version version;
		struct lttng_kernel_abi_tracer_abi_version *uversion =
			(struct lttng_kernel_abi_tracer_abi_version __user *) arg;

		lttng_abi_tracer_abi_version(&version);

		if (copy_to_user(uversion, &version, sizeof(version)))
			return -EFAULT;
		return 0;
	}
	case LTTNG_KERNEL_ABI_OLD_TRACEPOINT_LIST:
	case LTTNG_KERNEL_ABI_TRACEPOINT_LIST:
		return lttng_abi_tracepoint_list();
	case LTTNG_KERNEL_ABI_SYSCALL_LIST:
		return lttng_abi_syscall_list();
	case LTTNG_KERNEL_ABI_OLD_WAIT_QUIESCENT:
	case LTTNG_KERNEL_ABI_WAIT_QUIESCENT:
		synchronize_trace();
		return 0;
	case LTTNG_KERNEL_ABI_OLD_CALIBRATE:
	{
		struct lttng_kernel_abi_old_calibrate __user *ucalibrate =
			(struct lttng_kernel_abi_old_calibrate __user *) arg;
		struct lttng_kernel_abi_old_calibrate old_calibrate;
		struct lttng_kernel_abi_calibrate calibrate;
		int ret;

		if (copy_from_user(&old_calibrate, ucalibrate, sizeof(old_calibrate)))
			return -EFAULT;
		calibrate.type = old_calibrate.type;
		ret = lttng_calibrate(&calibrate);
		if (copy_to_user(ucalibrate, &old_calibrate, sizeof(old_calibrate)))
			return -EFAULT;
		return ret;
	}
	case LTTNG_KERNEL_ABI_CALIBRATE:
	{
		struct lttng_kernel_abi_calibrate __user *ucalibrate =
			(struct lttng_kernel_abi_calibrate __user *) arg;
		struct lttng_kernel_abi_calibrate calibrate;
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

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,6,0))
static const struct proc_ops lttng_proc_ops = {
	.proc_open = nonseekable_open,
	.proc_ioctl = lttng_ioctl,
#ifdef CONFIG_COMPAT
	.proc_compat_ioctl = lttng_ioctl,
#endif /* CONFIG_COMPAT */
};
#else
static const struct file_operations lttng_proc_ops = {
	.owner = THIS_MODULE,
	.open = nonseekable_open,
	.unlocked_ioctl = lttng_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_ioctl,
#endif /* CONFIG_COMPAT */
};
#endif

static
int lttng_abi_create_channel(struct file *session_file,
			     struct lttng_kernel_abi_channel *chan_param,
			     enum channel_type channel_type)
{
	struct lttng_kernel_session *session = session_file->private_data;
	const struct file_operations *fops = NULL;
	const char *transport_name;
	struct lttng_kernel_channel_buffer *chan;
	struct file *chan_file;
	int chan_fd;
	int ret = 0;

	chan_fd = get_unused_fd_flags(0);
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
		if (chan_param->output == LTTNG_KERNEL_ABI_SPLICE) {
			transport_name = chan_param->overwrite ?
				"relay-overwrite" : "relay-discard";
		} else if (chan_param->output == LTTNG_KERNEL_ABI_MMAP) {
			transport_name = chan_param->overwrite ?
				"relay-overwrite-mmap" : "relay-discard-mmap";
		} else {
			return -EINVAL;
		}
		break;
	case METADATA_CHANNEL:
		if (chan_param->output == LTTNG_KERNEL_ABI_SPLICE)
			transport_name = "relay-metadata";
		else if (chan_param->output == LTTNG_KERNEL_ABI_MMAP)
			transport_name = "relay-metadata-mmap";
		else
			return -EINVAL;
		break;
	default:
		transport_name = "<unknown>";
		break;
	}
	if (!lttng_file_ref_get(session_file)) {
		ret = -EOVERFLOW;
		goto refcount_error;
	}
	/*
	 * We tolerate no failure path after channel creation. It will stay
	 * invariant for the rest of the session.
	 */
	chan = lttng_channel_buffer_create(session, transport_name, NULL,
				  chan_param->subbuf_size,
				  chan_param->num_subbuf,
				  chan_param->switch_timer_interval,
				  chan_param->read_timer_interval,
				  channel_type);
	if (!chan) {
		ret = -EINVAL;
		goto chan_error;
	}
	chan->priv->parent.file = chan_file;
	chan_file->private_data = chan;
	fd_install(chan_fd, chan_file);

	return chan_fd;

chan_error:
	if (!lttng_file_ref_put(session_file)) {
		ret = -EINVAL;
	}
refcount_error:
	fput(chan_file);
file_error:
	put_unused_fd(chan_fd);
fd_error:
	return ret;
}

static
int lttng_abi_session_set_name(struct lttng_kernel_session *session,
		struct lttng_kernel_abi_session_name *name)
{
	size_t len;

	len = strnlen(name->name, LTTNG_KERNEL_ABI_SESSION_NAME_LEN);

	if (len == LTTNG_KERNEL_ABI_SESSION_NAME_LEN) {
		/* Name is too long/malformed */
		return -EINVAL;
	}

	strcpy(session->priv->name, name->name);
	return 0;
}

static
int lttng_abi_session_set_creation_time(struct lttng_kernel_session *session,
		struct lttng_kernel_abi_session_creation_time *time)
{
	size_t len;

	len = strnlen(time->iso8601, LTTNG_KERNEL_ABI_SESSION_CREATION_TIME_ISO8601_LEN);

	if (len == LTTNG_KERNEL_ABI_SESSION_CREATION_TIME_ISO8601_LEN) {
		/* Time is too long/malformed */
		return -EINVAL;
	}

	strcpy(session->priv->creation_time, time->iso8601);
	return 0;
}

static
int lttng_abi_session_set_output_format(struct lttng_kernel_session *session,
		enum lttng_kernel_abi_session_output_format format)
{
	session->priv->output_format = format;
	return 0;
}

static
int lttng_counter_release(struct inode *inode, struct file *file)
{
	struct lttng_kernel_channel_counter *counter = file->private_data;

	if (counter) {
		/*
		 * Do not destroy the counter itself. Wait of the owner
		 * (event_notifier group) to be destroyed.
		 */
		fput(counter->priv->owner);
	}

	return 0;
}

static
int copy_user_event_param_ext(struct lttng_kernel_abi_event_ext *event_param_ext,
			struct lttng_kernel_abi_event *event_param)
{
	struct lttng_kernel_abi_event_ext __user *uevent_ext =
		(struct lttng_kernel_abi_event_ext __user *) (unsigned long) event_param->event_ext;
	uint32_t len;
	int ret;

	/* Use zeroed defaults if extension parameters are not set. */
	if (!uevent_ext)
		return 0;
	ret = get_user(len, &uevent_ext->len);
	if (ret)
		return ret;
	if (len > PAGE_SIZE)
		return -E2BIG;
	ret = lttng_copy_struct_from_user(event_param_ext, sizeof(*event_param_ext), uevent_ext, len);
	if (ret)
		return ret;
	/* Ensure that len is consistent with the initial get_user(). */
	event_param_ext->len = len;

	/* Validate that we know of all flags and enum values used. */
	switch (event_param_ext->match_check) {
	case LTTNG_KERNEL_ABI_MATCH_DEFAULT:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_MATCH_IMMEDIATE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_MATCH_LAZY:
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static
int user_event_param_ext_get_match_check(const struct lttng_kernel_abi_event_ext *event_param_ext,
		enum lttng_kernel_abi_match_check *_match_check)
{
	enum lttng_kernel_abi_match_check match_check = LTTNG_KERNEL_ABI_MATCH_DEFAULT;

	if (event_param_ext->len < offsetofend(struct lttng_kernel_abi_event_ext, match_check))
		goto end;
	match_check = event_param_ext->match_check;
end:
	*_match_check = match_check;
	return 0;
}

static
int create_counter_key_from_abi_dimensions(struct lttng_kernel_counter_key **_counter_key,
		uint32_t nr_dimensions, void __user *ptr)
{
	struct lttng_kernel_abi_counter_key_dimension __user *udimension;
	struct lttng_kernel_abi_counter_key_dimension kdimension = {};
	struct lttng_kernel_counter_key *counter_key = NULL;
	struct lttng_key_token *token_array = NULL;
	uint32_t i;
	int ret = 0;

	if (!nr_dimensions || nr_dimensions > LTTNG_KERNEL_COUNTER_MAX_DIMENSIONS)
		return -EINVAL;
	counter_key = kzalloc(sizeof(*counter_key), GFP_KERNEL);
	if (!counter_key)
		return -ENOMEM;
	counter_key->nr_dimensions = nr_dimensions;
	/* Only a single dimension is supported. */
	if (WARN_ON_ONCE(nr_dimensions != 1)) {
		ret = -EINVAL;
		goto error;
	}
	udimension = (struct lttng_kernel_abi_counter_key_dimension __user *)ptr;
	ret = lttng_copy_struct_from_user(&kdimension, sizeof(kdimension), udimension, sizeof(*udimension));
	if (ret)
		goto error;
	if (kdimension.len > PAGE_SIZE) {
		ret = -E2BIG;
		goto error;
	}
	switch (kdimension.key_type) {
	case LTTNG_KERNEL_ABI_KEY_TYPE_TOKENS:
	{
		struct lttng_kernel_abi_counter_key_dimension_tokens kdimension_tokens = {};
		struct lttng_kernel_abi_counter_key_dimension_tokens __user *udimension_tokens;
		struct lttng_kernel_abi_key_token __user *utoken;
		uint32_t nr_tokens;

		udimension_tokens = (struct lttng_kernel_abi_counter_key_dimension_tokens __user *)ptr;
		ret = lttng_copy_struct_from_user(&kdimension_tokens, sizeof(kdimension_tokens),
				udimension_tokens, kdimension.len);
		if (ret)
			goto error;
		nr_tokens = kdimension_tokens.nr_key_tokens;
		if (nr_tokens > LTTNG_KERNEL_COUNTER_MAX_TOKENS) {
			ret = -EINVAL;
			goto error;
		}
		token_array = kzalloc(nr_tokens * sizeof(*token_array), GFP_KERNEL);
		if (!token_array) {
			ret = -ENOMEM;
			goto error;
		}
		counter_key->dimension_array[0].token_array = token_array;
		counter_key->dimension_array[0].nr_key_tokens = nr_tokens;
		utoken = (struct lttng_kernel_abi_key_token __user *)((unsigned long)ptr + kdimension.len);
		for (i = 0; i < nr_tokens; i++) {
			struct lttng_kernel_abi_key_token ktoken = {};
			struct lttng_key_token *key_token = &token_array[i];
			uint32_t token_len = 0;

			ret = lttng_copy_struct_from_user(&ktoken, sizeof(ktoken), utoken, sizeof(*utoken));
			if (ret)
				goto error;
			token_len += ktoken.len;
			switch (ktoken.type) {
			case LTTNG_KERNEL_ABI_KEY_TOKEN_STRING:
			{
				struct lttng_kernel_abi_key_token_string __user *utoken_string;
				struct lttng_kernel_abi_key_token_string *ktoken_string;
				char __user *string_ptr;
				size_t string_len;

				utoken_string = (struct lttng_kernel_abi_key_token_string __user *) utoken;
				ret = lttng_copy_struct_from_user(&ktoken_string, sizeof(ktoken_string),
						utoken_string, ktoken.len);
				if (ret)
					goto error;
				string_ptr = (char __user *) ((unsigned long)utoken_string + ktoken.len);
				string_len = ktoken_string->string_len;
				key_token->type = LTTNG_KEY_TOKEN_STRING;
				if (!string_len || string_len > PAGE_SIZE) {
					ret = -EINVAL;
					goto error;
				}
				key_token->str = kzalloc(string_len, GFP_KERNEL);
				if (!key_token->str) {
					ret = -ENOMEM;
					goto error;
				}
				ret = copy_from_user(key_token->str, string_ptr, string_len);
				if (ret)
					goto error;
				if (key_token->str[string_len - 1] != '\0') {
					ret = -EINVAL;
					goto error;
				}
				token_len += string_len;
				break;
			}
			case LTTNG_KERNEL_ABI_KEY_TOKEN_EVENT_NAME:
				key_token->type = LTTNG_KEY_TOKEN_EVENT_NAME;
				break;

			case LTTNG_KERNEL_ABI_KEY_TOKEN_PROVIDER_NAME:
				lttng_fallthrough;
			default:
				ret = -EINVAL;
				goto error;
			}
			utoken = (struct lttng_kernel_abi_key_token __user *)((unsigned long)utoken + token_len);
		}

		break;
	}
	case LTTNG_KERNEL_ABI_KEY_TYPE_INTEGER:
	default:
		ret = -EINVAL;
		goto error;
	}
	*_counter_key = counter_key;
	return 0;

error:
	destroy_counter_key(counter_key);
	return ret;
}

int create_counter_key_from_kernel(struct lttng_kernel_counter_key **_new_key,
		const struct lttng_kernel_counter_key *src_key)
{
	struct lttng_kernel_counter_key *new_key;
	int i, ret = 0;

	new_key = kzalloc(sizeof(*new_key), GFP_KERNEL);
	if (!new_key)
		return -ENOMEM;
	new_key->nr_dimensions = src_key->nr_dimensions;
	for (i = 0; i < src_key->nr_dimensions; i++) {
		struct lttng_kernel_counter_key_dimension *new_dimension = &new_key->dimension_array[i];
		const struct lttng_kernel_counter_key_dimension *src_dimension = &src_key->dimension_array[i];
		uint32_t nr_tokens = src_dimension->nr_key_tokens;
		int j;

		new_dimension->nr_key_tokens = nr_tokens;
		new_dimension->token_array = kzalloc(nr_tokens * sizeof(*new_dimension->token_array), GFP_KERNEL);
		if (!new_dimension->token_array) {
			ret = -ENOMEM;
			goto error;
		}
		for (j = 0; j < nr_tokens; j++) {
			struct lttng_key_token *new_key_token = &new_dimension->token_array[j];
			struct lttng_key_token *src_key_token = &src_dimension->token_array[j];

			switch (src_key_token->type) {
			case LTTNG_KEY_TOKEN_STRING:
				new_key_token->type = LTTNG_KEY_TOKEN_STRING;
				new_key_token->str = kstrdup(src_key_token->str, GFP_KERNEL);
				if (!new_key_token->str) {
					ret = -ENOMEM;
					goto error;
				}
				break;
			case LTTNG_KEY_TOKEN_EVENT_NAME:
				new_key_token->type = LTTNG_KEY_TOKEN_EVENT_NAME;
				break;

			default:
				ret = -EINVAL;
				goto error;
			}
		}
	}
	*_new_key = new_key;
	return 0;

error:
	destroy_counter_key(new_key);
	return ret;
}

void destroy_counter_key(struct lttng_kernel_counter_key *counter_key)
{
	int i;

	if (!counter_key)
		return;
	for (i = 0; i < counter_key->nr_dimensions; i++) {
		struct lttng_kernel_counter_key_dimension *dimension = &counter_key->dimension_array[i];
		uint32_t nr_tokens = dimension->nr_key_tokens;
		int j;

		for (j = 0; j < nr_tokens; j++) {
			struct lttng_key_token *key_token = &dimension->token_array[j];

			switch (key_token->type) {
			case LTTNG_KEY_TOKEN_STRING:
				kfree(key_token->str);
				break;

			case LTTNG_KEY_TOKEN_EVENT_NAME:
				lttng_fallthrough;
			case LTTNG_KEY_TOKEN_UNKNOWN:
				break;

			default:
				WARN_ON_ONCE(1);
			}
		}
		kfree(dimension->token_array);
	}
	kfree(counter_key);
}

static
long lttng_counter_ioctl_abi_old_counter_read(struct lttng_kernel_channel_counter *counter,
		unsigned int cmd, unsigned long arg)
{
	size_t indexes[LTTNG_KERNEL_ABI_OLD_COUNTER_DIMENSION_MAX] = {};
	struct lttng_kernel_abi_old_counter_read local_counter_read;
	struct lttng_kernel_abi_old_counter_read __user *ucounter_read =
			(struct lttng_kernel_abi_old_counter_read __user *) arg;
	bool overflow, underflow;
	int64_t value;
	int32_t cpu;
	int ret, i;

	if (copy_from_user(&local_counter_read, ucounter_read,
				sizeof(local_counter_read)))
		return -EFAULT;
	if (validate_zeroed_padding(local_counter_read.padding,
			sizeof(local_counter_read.padding)))
		return -EINVAL;
	if (local_counter_read.index.number_dimensions > LTTNG_KERNEL_ABI_OLD_COUNTER_DIMENSION_MAX)
		return -EINVAL;
	/* Cast all indexes into size_t. */
	for (i = 0; i < local_counter_read.index.number_dimensions; i++)
		indexes[i] = local_counter_read.index.dimension_indexes[i];
	cpu = local_counter_read.cpu;

	ret = lttng_kernel_counter_read(counter, indexes, cpu, &value, &overflow, &underflow);
	if (ret)
		return ret;
	local_counter_read.value.value = value;
	local_counter_read.value.overflow = overflow;
	local_counter_read.value.underflow = underflow;

	if (copy_to_user(&ucounter_read->value, &local_counter_read.value,
				sizeof(local_counter_read.value)))
		return -EFAULT;

	return 0;
}

static
long lttng_counter_ioctl_abi_old_counter_aggregate(struct lttng_kernel_channel_counter *counter,
		unsigned int cmd, unsigned long arg)
{
	size_t indexes[LTTNG_KERNEL_ABI_OLD_COUNTER_DIMENSION_MAX] = {};
	struct lttng_kernel_abi_old_counter_aggregate local_counter_aggregate;
	struct lttng_kernel_abi_old_counter_aggregate __user *ucounter_aggregate =
			(struct lttng_kernel_abi_old_counter_aggregate __user *) arg;
	bool overflow, underflow;
	int64_t value;
	int ret, i;

	if (copy_from_user(&local_counter_aggregate, ucounter_aggregate,
				sizeof(local_counter_aggregate)))
		return -EFAULT;
	if (validate_zeroed_padding(local_counter_aggregate.padding,
			sizeof(local_counter_aggregate.padding)))
		return -EINVAL;
	if (local_counter_aggregate.index.number_dimensions > LTTNG_KERNEL_ABI_OLD_COUNTER_DIMENSION_MAX)
		return -EINVAL;
	/* Cast all indexes into size_t. */
	for (i = 0; i < local_counter_aggregate.index.number_dimensions; i++)
		indexes[i] = local_counter_aggregate.index.dimension_indexes[i];

	ret = lttng_kernel_counter_aggregate(counter, indexes, &value, &overflow, &underflow);
	if (ret)
		return ret;
	local_counter_aggregate.value.value = value;
	local_counter_aggregate.value.overflow = overflow;
	local_counter_aggregate.value.underflow = underflow;

	if (copy_to_user(&ucounter_aggregate->value, &local_counter_aggregate.value,
				sizeof(local_counter_aggregate.value)))
		return -EFAULT;

	return 0;
}

static
long lttng_counter_ioctl_abi_old_counter_clear(struct lttng_kernel_channel_counter *counter,
		unsigned int cmd, unsigned long arg)
{
	size_t indexes[LTTNG_KERNEL_ABI_OLD_COUNTER_DIMENSION_MAX] = {};
	struct lttng_kernel_abi_old_counter_clear local_counter_clear;
	struct lttng_kernel_abi_old_counter_clear __user *ucounter_clear =
			(struct lttng_kernel_abi_old_counter_clear __user *) arg;
	int i;

	if (copy_from_user(&local_counter_clear, ucounter_clear,
				sizeof(local_counter_clear)))
		return -EFAULT;
	if (validate_zeroed_padding(local_counter_clear.padding,
			sizeof(local_counter_clear.padding)))
		return -EINVAL;
	if (local_counter_clear.index.number_dimensions > LTTNG_KERNEL_ABI_OLD_COUNTER_DIMENSION_MAX)
		return -EINVAL;
	/* Cast all indexes into size_t. */
	for (i = 0; i < local_counter_clear.index.number_dimensions; i++)
		indexes[i] = local_counter_clear.index.dimension_indexes[i];
	return lttng_kernel_counter_clear(counter, indexes);
}

#ifdef CONFIG_LTTNG_EXPERIMENTAL_COUNTER
static
long lttng_counter_ioctl_abi_counter_read(struct lttng_kernel_channel_counter *counter,
		unsigned int cmd, unsigned long arg)
{
	size_t indexes[LTTNG_KERNEL_COUNTER_MAX_DIMENSIONS] = {};
	struct lttng_kernel_abi_counter_read kcounter_read = {};
	struct lttng_kernel_abi_counter_read __user *ucounter_read =
			(struct lttng_kernel_abi_counter_read __user *) arg;
	uint32_t len, number_dimensions;
	bool overflow, underflow;
	int64_t value;
	int32_t cpu;
	int ret, i;

	ret = get_user(len, &ucounter_read->len);
	if (ret)
		return ret;
	if (len > PAGE_SIZE)
		return -E2BIG;
	if (len < offsetofend(struct lttng_kernel_abi_counter_read, value))
		return -EINVAL;
	ret = lttng_copy_struct_from_user(&kcounter_read, sizeof(kcounter_read),
			ucounter_read, len);
	if (ret)
		return ret;
	number_dimensions = kcounter_read.index.number_dimensions;
	if (!number_dimensions || number_dimensions > LTTNG_KERNEL_COUNTER_MAX_DIMENSIONS)
		return -EINVAL;
	/* Cast all indexes into size_t. */
	for (i = 0; i < number_dimensions; i++) {
		uint64_t __user *ptr = ((uint64_t __user *)(unsigned long)kcounter_read.index.ptr) + i;
		uint64_t index;

		ret = get_user(index, ptr);
		if (ret)
			return ret;
		indexes[i] = index;
	}
	cpu = kcounter_read.cpu;
	ret = lttng_kernel_counter_read(counter, indexes, cpu, &value, &overflow, &underflow);
	if (ret)
		return ret;
	kcounter_read.value.value = value;
	kcounter_read.value.flags |= underflow ? LTTNG_KERNEL_ABI_COUNTER_VALUE_FLAG_UNDERFLOW : 0;
	kcounter_read.value.flags |= overflow ? LTTNG_KERNEL_ABI_COUNTER_VALUE_FLAG_OVERFLOW : 0;

	if (copy_to_user(&ucounter_read->value, &kcounter_read.value, sizeof(kcounter_read.value)))
		return -EFAULT;

	return 0;
}

static
long lttng_counter_ioctl_abi_counter_aggregate(struct lttng_kernel_channel_counter *counter,
		unsigned int cmd, unsigned long arg)
{
	size_t indexes[LTTNG_KERNEL_COUNTER_MAX_DIMENSIONS] = {};
	struct lttng_kernel_abi_counter_aggregate kcounter_aggregate = {};
	struct lttng_kernel_abi_counter_aggregate __user *ucounter_aggregate =
			(struct lttng_kernel_abi_counter_aggregate __user *) arg;
	uint32_t len, number_dimensions;
	bool overflow, underflow;
	int64_t value;
	int ret, i;

	ret = get_user(len, &ucounter_aggregate->len);
	if (ret)
		return ret;
	if (len > PAGE_SIZE)
		return -E2BIG;
	if (len < offsetofend(struct lttng_kernel_abi_counter_aggregate, value))
		return -EINVAL;
	ret = lttng_copy_struct_from_user(&kcounter_aggregate, sizeof(kcounter_aggregate),
			ucounter_aggregate, len);
	if (ret)
		return ret;
	number_dimensions = kcounter_aggregate.index.number_dimensions;
	if (!number_dimensions || number_dimensions > LTTNG_KERNEL_COUNTER_MAX_DIMENSIONS)
		return -EINVAL;
	/* Cast all indexes into size_t. */
	for (i = 0; i < number_dimensions; i++) {
		uint64_t __user *ptr = ((uint64_t __user *)(unsigned long)kcounter_aggregate.index.ptr) + i;
		uint64_t index;

		ret = get_user(index, ptr);
		if (ret)
			return ret;
		indexes[i] = index;
	}
	ret = lttng_kernel_counter_aggregate(counter, indexes, &value, &overflow, &underflow);
	if (ret)
		return ret;
	kcounter_aggregate.value.value = value;
	kcounter_aggregate.value.flags |= underflow ? LTTNG_KERNEL_ABI_COUNTER_VALUE_FLAG_UNDERFLOW : 0;
	kcounter_aggregate.value.flags |= overflow ? LTTNG_KERNEL_ABI_COUNTER_VALUE_FLAG_OVERFLOW : 0;

	if (copy_to_user(&ucounter_aggregate->value, &kcounter_aggregate.value, sizeof(kcounter_aggregate.value)))
		return -EFAULT;

	return 0;
}

static
long lttng_counter_ioctl_abi_counter_clear(struct lttng_kernel_channel_counter *counter,
		unsigned int cmd, unsigned long arg)
{
	size_t indexes[LTTNG_KERNEL_COUNTER_MAX_DIMENSIONS] = {};
	struct lttng_kernel_abi_counter_clear kcounter_clear = {};
	struct lttng_kernel_abi_counter_clear __user *ucounter_clear =
			(struct lttng_kernel_abi_counter_clear __user *) arg;
	uint32_t len, number_dimensions;
	int ret, i;

	ret = get_user(len, &ucounter_clear->len);
	if (ret)
		return ret;
	if (len > PAGE_SIZE)
		return -E2BIG;
	if (len < offsetofend(struct lttng_kernel_abi_counter_clear, index))
		return -EINVAL;
	ret = lttng_copy_struct_from_user(&kcounter_clear, sizeof(kcounter_clear),
			ucounter_clear, len);
	if (ret)
		return ret;
	number_dimensions = kcounter_clear.index.number_dimensions;
	if (!number_dimensions || number_dimensions > LTTNG_KERNEL_COUNTER_MAX_DIMENSIONS)
		return -EINVAL;
	/* Cast all indexes into size_t. */
	for (i = 0; i < number_dimensions; i++) {
		uint64_t __user *ptr = ((uint64_t __user *)(unsigned long)kcounter_clear.index.ptr) + i;
		uint64_t index;

		ret = get_user(index, ptr);
		if (ret)
			return ret;
		indexes[i] = index;
	}
	return lttng_kernel_counter_clear(counter, indexes);
}
#endif	/* CONFIG_LTTNG_EXPERIMENTAL_COUNTER */

static
long lttng_counter_ioctl_abi_counter_event(struct file *file,
		struct lttng_kernel_channel_counter *counter,
		unsigned int cmd, unsigned long arg)
{
	struct lttng_kernel_abi_counter_event __user *ucounter_event =
			(struct lttng_kernel_abi_counter_event __user *) arg;
	struct lttng_kernel_abi_counter_event kcounter_event = {};
	struct lttng_kernel_counter_event *counter_event;
	uint32_t len;
	int ret;

	ret = get_user(len, &ucounter_event->len);
	if (ret)
		return ret;
	if (len > PAGE_SIZE)
		return -E2BIG;
	if (len < offsetofend(struct lttng_kernel_abi_counter_event, number_key_dimensions))
		return -EINVAL;
	counter_event = kzalloc(sizeof(*counter_event), GFP_KERNEL);
	if (!counter_event)
		return -ENOMEM;
	ret = lttng_copy_struct_from_user(&kcounter_event, sizeof(kcounter_event),
			ucounter_event, len);
	if (ret)
		goto end_counter_event;
	memcpy(&counter_event->event_param, &kcounter_event.event, sizeof(counter_event->event_param));
	ret = copy_user_event_param_ext(&counter_event->event_param_ext, &kcounter_event.event);
	if (ret)
		goto end_counter_event;
	switch (kcounter_event.action) {
	case LTTNG_KERNEL_ABI_COUNTER_ACTION_INCREMENT:
		/* No specific data for this action. */
		break;
	default:
		ret = -EINVAL;
		goto end_counter_event;
	}
	ret = create_counter_key_from_abi_dimensions(&counter_event->counter_key,
			kcounter_event.number_key_dimensions,
			(void __user *) arg + len);
	if (ret)
		goto end_counter_event;
	ret = lttng_abi_create_event_counter_enabler(file, counter_event);
	destroy_counter_key(counter_event->counter_key);
end_counter_event:
	kfree(counter_event);
	return ret;
}

static
long lttng_counter_ioctl_abi_counter_map_descriptor(struct lttng_kernel_channel_counter *counter,
		unsigned int cmd, unsigned long arg)
{
	struct lttng_kernel_abi_counter_map_descriptor __user *udescriptor =
		(struct lttng_kernel_abi_counter_map_descriptor __user *) arg;
	struct lttng_kernel_abi_counter_map_descriptor kdescriptor = {};
	struct lttng_counter_map_descriptor *descriptor;
	char key[LTTNG_KERNEL_COUNTER_KEY_LEN] = {};
	uint64_t array_indexes[1];
	size_t key_strlen;
	uint32_t len;
	int ret;

	ret = get_user(len, &udescriptor->len);
	if (ret)
		return ret;
	if (len > PAGE_SIZE)
		return -E2BIG;
	if (len < offsetofend(struct lttng_kernel_abi_counter_map_descriptor, array_indexes_len))
		return -EINVAL;
	ret = lttng_copy_struct_from_user(&kdescriptor, sizeof(kdescriptor), udescriptor, len);
	if (ret)
		return ret;
	mutex_lock(&counter->priv->map.lock);
	if (kdescriptor.descriptor_index >= counter->priv->map.nr_descriptors) {
		ret = -EOVERFLOW;
		goto map_descriptor_error_unlock;
	}
	if (kdescriptor.array_indexes_len < 1) {
		ret = -EINVAL;
		goto map_descriptor_error_unlock;
	}
	kdescriptor.array_indexes_len = 1;
	descriptor = &counter->priv->map.descriptors[kdescriptor.descriptor_index];
	kdescriptor.dimension = 0;
	kdescriptor.user_token = descriptor->user_token;
	memcpy(&key, descriptor->key, LTTNG_KERNEL_COUNTER_KEY_LEN);
	array_indexes[0] = descriptor->array_index;
	mutex_unlock(&counter->priv->map.lock);

	key_strlen = strlen(key) + 1;
	if (kdescriptor.key_string_len < key_strlen)
		return -ENOSPC;
	kdescriptor.key_string_len = key_strlen;
	if (copy_to_user((char __user *)(unsigned long)kdescriptor.key_string, key, key_strlen))
		return -EFAULT;
	if (copy_to_user((uint64_t __user *)(unsigned long)kdescriptor.array_indexes, array_indexes, sizeof(uint64_t)))
		return -EFAULT;
	if (copy_to_user(udescriptor, &kdescriptor, min(sizeof(kdescriptor), (size_t)len)))
		return -EFAULT;
	return 0;

map_descriptor_error_unlock:
	mutex_unlock(&counter->priv->map.lock);
	return ret;
}

static
long lttng_counter_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct lttng_kernel_channel_counter *counter = file->private_data;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_OLD_COUNTER_READ:
		return lttng_counter_ioctl_abi_old_counter_read(counter, cmd, arg);
	case LTTNG_KERNEL_ABI_OLD_COUNTER_AGGREGATE:
		return lttng_counter_ioctl_abi_old_counter_aggregate(counter, cmd, arg);
	case LTTNG_KERNEL_ABI_OLD_COUNTER_CLEAR:
		return lttng_counter_ioctl_abi_old_counter_clear(counter, cmd, arg);
#ifdef CONFIG_LTTNG_EXPERIMENTAL_COUNTER
	case LTTNG_KERNEL_ABI_COUNTER_READ:
		return lttng_counter_ioctl_abi_counter_read(counter, cmd, arg);
	case LTTNG_KERNEL_ABI_COUNTER_AGGREGATE:
		return lttng_counter_ioctl_abi_counter_aggregate(counter, cmd, arg);
	case LTTNG_KERNEL_ABI_COUNTER_CLEAR:
		return lttng_counter_ioctl_abi_counter_clear(counter, cmd, arg);
#endif	/* CONFIG_LTTNG_EXPERIMENTAL_COUNTER */
	case LTTNG_KERNEL_ABI_COUNTER_EVENT:
		return lttng_counter_ioctl_abi_counter_event(file, counter, cmd, arg);
	case LTTNG_KERNEL_ABI_ENABLE:
		return lttng_channel_enable(&counter->parent);
	case LTTNG_KERNEL_ABI_DISABLE:
		return lttng_channel_disable(&counter->parent);
	case LTTNG_KERNEL_ABI_SYSCALL_MASK:
		return lttng_syscall_table_get_active_mask(&counter->priv->parent.syscall_table,
			(struct lttng_kernel_abi_syscall_mask __user *) arg);
	case LTTNG_KERNEL_ABI_COUNTER_MAP_NR_DESCRIPTORS:
	{
		uint64_t __user *user_nr_descriptors = (uint64_t __user *) arg;
		uint64_t nr_descriptors;

		mutex_lock(&counter->priv->map.lock);
		nr_descriptors = counter->priv->map.nr_descriptors;
		mutex_unlock(&counter->priv->map.lock);
		return put_user(nr_descriptors, user_nr_descriptors);
	}
	case LTTNG_KERNEL_ABI_COUNTER_MAP_DESCRIPTOR:
		return lttng_counter_ioctl_abi_counter_map_descriptor(counter, cmd, arg);
	default:
		return -ENOSYS;
	}
}

static const struct file_operations lttng_counter_fops = {
	.owner = THIS_MODULE,
	.release = lttng_counter_release,
	.unlocked_ioctl = lttng_counter_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_counter_ioctl,
#endif
};


static
enum tracker_type get_tracker_type(struct lttng_kernel_abi_tracker_args *tracker)
{
	switch (tracker->type) {
	case LTTNG_KERNEL_ABI_TRACKER_PID:
		return TRACKER_PID;
	case LTTNG_KERNEL_ABI_TRACKER_VPID:
		return TRACKER_VPID;
	case LTTNG_KERNEL_ABI_TRACKER_UID:
		return TRACKER_UID;
	case LTTNG_KERNEL_ABI_TRACKER_VUID:
		return TRACKER_VUID;
	case LTTNG_KERNEL_ABI_TRACKER_GID:
		return TRACKER_GID;
	case LTTNG_KERNEL_ABI_TRACKER_VGID:
		return TRACKER_VGID;
	default:
		return TRACKER_UNKNOWN;
	}
}

static
int lttng_abi_copy_user_old_counter_conf(struct lttng_kernel_counter_conf *counter_conf,
		struct lttng_kernel_abi_old_counter_conf __user *old_ucounter_conf)
{
	struct lttng_kernel_abi_old_counter_conf old_kcounter_conf;
	struct lttng_kernel_counter_dimension *dimension;
	int ret;

	memset(counter_conf, 0, sizeof(*counter_conf));
	ret = copy_from_user(&old_kcounter_conf, old_ucounter_conf,
			sizeof(old_kcounter_conf));
	if (ret)
		return ret;
	if (!old_kcounter_conf.number_dimensions ||
			old_kcounter_conf.number_dimensions > LTTNG_KERNEL_COUNTER_MAX_DIMENSIONS)
		return -EINVAL;
	switch (old_kcounter_conf.arithmetic) {
	case LTTNG_KERNEL_ABI_COUNTER_ARITHMETIC_MODULAR:
		counter_conf->arithmetic = LTTNG_KERNEL_COUNTER_ARITHMETIC_MODULAR;
		break;
	default:
		return -EINVAL;
	}
	switch (old_kcounter_conf.bitness) {
	case LTTNG_KERNEL_ABI_COUNTER_BITNESS_32:
		counter_conf->bitness = LTTNG_KERNEL_COUNTER_BITNESS_32;
		break;
	case LTTNG_KERNEL_ABI_COUNTER_BITNESS_64:
		counter_conf->bitness = LTTNG_KERNEL_COUNTER_BITNESS_64;
		break;
	default:
		return -EINVAL;
	}
	counter_conf->global_sum_step = old_kcounter_conf.global_sum_step;
	counter_conf->flags |= old_kcounter_conf.coalesce_hits ?
					LTTNG_KERNEL_COUNTER_CONF_FLAG_COALESCE_HITS : 0;
	dimension = &counter_conf->dimension_array[0];
	dimension->flags |= old_kcounter_conf.dimensions[0].has_underflow ?
					LTTNG_KERNEL_COUNTER_DIMENSION_FLAG_UNDERFLOW : 0;
	dimension->flags |= old_kcounter_conf.dimensions[0].has_overflow ?
					LTTNG_KERNEL_COUNTER_DIMENSION_FLAG_OVERFLOW : 0;
	dimension->size = old_kcounter_conf.dimensions[0].size;
	dimension->underflow_index = old_kcounter_conf.dimensions[0].underflow_index;
	dimension->overflow_index = old_kcounter_conf.dimensions[0].overflow_index;
	return 0;
}

static
int lttng_abi_copy_user_counter_conf(struct lttng_kernel_counter_conf *counter_conf,
		struct lttng_kernel_abi_counter_conf __user *ucounter_conf)
{
	uint32_t len, number_dimensions;
	struct lttng_kernel_abi_counter_conf kcounter_conf = {};
	struct lttng_kernel_counter_dimension *dimension;
	struct lttng_kernel_abi_counter_dimension kdimension = {};
	struct lttng_kernel_abi_counter_dimension __user *udimension;
	int ret;

	memset(counter_conf, 0, sizeof(*counter_conf));
	ret = get_user(len, &ucounter_conf->len);
	if (ret)
		return ret;
	if (len < offsetofend(struct lttng_kernel_abi_counter_conf, dimension_array))
		return -EINVAL;
	if (len > PAGE_SIZE)
		return -EINVAL;

	ret = lttng_copy_struct_from_user(&kcounter_conf, sizeof(kcounter_conf), ucounter_conf, len);
	if (ret)
		return ret;

	/* Validate flags and enumerations */
	switch (kcounter_conf.arithmetic) {
	case LTTNG_KERNEL_ABI_COUNTER_ARITHMETIC_MODULAR:
		counter_conf->arithmetic = LTTNG_KERNEL_COUNTER_ARITHMETIC_MODULAR;
		break;
	default:
		return -EINVAL;
	}
	switch (kcounter_conf.bitness) {
	case LTTNG_KERNEL_ABI_COUNTER_BITNESS_32:
		counter_conf->bitness = LTTNG_KERNEL_COUNTER_BITNESS_32;
		break;
	case LTTNG_KERNEL_ABI_COUNTER_BITNESS_64:
		counter_conf->bitness = LTTNG_KERNEL_COUNTER_BITNESS_64;
		break;
	default:
		return -EINVAL;
	}
	if (kcounter_conf.flags & ~LTTNG_KERNEL_ABI_COUNTER_CONF_FLAG_COALESCE_HITS)
		return -EINVAL;
	counter_conf->global_sum_step = kcounter_conf.global_sum_step;
	counter_conf->flags |= (kcounter_conf.flags & LTTNG_KERNEL_ABI_COUNTER_CONF_FLAG_COALESCE_HITS) ?
					LTTNG_KERNEL_COUNTER_CONF_FLAG_COALESCE_HITS : 0;

	number_dimensions = kcounter_conf.dimension_array.number_dimensions;
	if (!number_dimensions || number_dimensions > LTTNG_KERNEL_COUNTER_MAX_DIMENSIONS)
		return -EINVAL;
	dimension = &counter_conf->dimension_array[0];
	len = kcounter_conf.dimension_array.elem_len;
	if (len > PAGE_SIZE)
		return -E2BIG;
	if (len < offsetofend(struct lttng_kernel_abi_counter_dimension, overflow_index))
		return -EINVAL;
	udimension = (struct lttng_kernel_abi_counter_dimension __user *)(unsigned long)kcounter_conf.dimension_array.ptr;
	ret = lttng_copy_struct_from_user(&kdimension, sizeof(kdimension), udimension, len);
	if (ret)
		return ret;

	/* Validate flags */
	if (kdimension.flags & ~(LTTNG_KERNEL_ABI_COUNTER_DIMENSION_FLAG_UNDERFLOW |
					LTTNG_KERNEL_ABI_COUNTER_DIMENSION_FLAG_OVERFLOW))
		return -EINVAL;

	dimension->flags |= (kdimension.flags & LTTNG_KERNEL_ABI_COUNTER_DIMENSION_FLAG_UNDERFLOW) ?
					LTTNG_KERNEL_COUNTER_DIMENSION_FLAG_UNDERFLOW : 0;
	dimension->flags |= (kdimension.flags & LTTNG_KERNEL_ABI_COUNTER_DIMENSION_FLAG_OVERFLOW) ?
					LTTNG_KERNEL_COUNTER_DIMENSION_FLAG_OVERFLOW : 0;
	dimension->size = kdimension.size;
	dimension->underflow_index = kdimension.underflow_index;
	dimension->overflow_index = kdimension.overflow_index;
	return 0;
}

/**
 *	lttng_session_ioctl - lttng session fd ioctl
 *
 *	@file: the file
 *	@cmd: the command
 *	@arg: command arg
 *
 *	This ioctl implements lttng commands:
 *	LTTNG_KERNEL_ABI_CHANNEL
 *		Returns a LTTng channel file descriptor
 *	LTTNG_KERNEL_ABI_ENABLE
 *		Enables tracing for a session (weak enable)
 *	LTTNG_KERNEL_ABI_DISABLE
 *		Disables tracing for a session (strong disable)
 *	LTTNG_KERNEL_ABI_METADATA
 *		Returns a LTTng metadata file descriptor
 *	LTTNG_KERNEL_ABI_SESSION_TRACK_PID
 *		Add PID to session PID tracker
 *	LTTNG_KERNEL_ABI_SESSION_UNTRACK_PID
 *		Remove PID from session PID tracker
 *	LTTNG_KERNEL_ABI_SESSION_TRACK_ID
 *		Add ID to tracker
 *	LTTNG_KERNEL_ABI_SESSION_UNTRACK_ID
 *		Remove ID from tracker
 *
 * The returned channel will be deleted when its file descriptor is closed.
 */
static
long lttng_session_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct lttng_kernel_session *session = file->private_data;
	struct lttng_kernel_abi_channel chan_param;
	struct lttng_kernel_abi_old_channel old_chan_param;
	int ret;

	/*
	 * Handle backward compatibility. OLD commands have wrong
	 * directions, replace them by the correct direction.
	 */
	switch (cmd) {
	case LTTNG_KERNEL_ABI_OLD_SESSION_TRACK_PID:
		cmd = LTTNG_KERNEL_ABI_SESSION_TRACK_PID;
		break;
	case LTTNG_KERNEL_ABI_OLD_SESSION_UNTRACK_PID:
		cmd = LTTNG_KERNEL_ABI_SESSION_UNTRACK_PID;
		break;
	case LTTNG_KERNEL_ABI_OLD_SESSION_TRACK_ID:
		cmd = LTTNG_KERNEL_ABI_SESSION_TRACK_ID;
		break;
	case LTTNG_KERNEL_ABI_OLD_SESSION_UNTRACK_ID:
		cmd = LTTNG_KERNEL_ABI_SESSION_UNTRACK_ID;
		break;
	case LTTNG_KERNEL_ABI_OLD_SESSION_LIST_TRACKER_IDS:
		cmd = LTTNG_KERNEL_ABI_SESSION_LIST_TRACKER_IDS;
		break;
	case LTTNG_KERNEL_ABI_OLD_SESSION_SET_NAME:
		cmd = LTTNG_KERNEL_ABI_SESSION_SET_NAME;
		break;
	case LTTNG_KERNEL_ABI_OLD_SESSION_SET_CREATION_TIME:
		cmd = LTTNG_KERNEL_ABI_SESSION_SET_CREATION_TIME;
		break;
	default:
		/* Nothing to do. */
		break;
	}

	switch (cmd) {
	case LTTNG_KERNEL_ABI_OLD_CHANNEL:
	{
		if (copy_from_user(&old_chan_param,
				(struct lttng_kernel_abi_old_channel __user *) arg,
				sizeof(struct lttng_kernel_abi_old_channel)))
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
	case LTTNG_KERNEL_ABI_CHANNEL:
	{
		if (copy_from_user(&chan_param,
				(struct lttng_kernel_abi_channel __user *) arg,
				sizeof(struct lttng_kernel_abi_channel)))
			return -EFAULT;
		return lttng_abi_create_channel(file, &chan_param,
				PER_CPU_CHANNEL);
	}
	case LTTNG_KERNEL_ABI_OLD_SESSION_START:
	case LTTNG_KERNEL_ABI_OLD_ENABLE:
	case LTTNG_KERNEL_ABI_SESSION_START:
	case LTTNG_KERNEL_ABI_ENABLE:
		return lttng_session_enable(session);
	case LTTNG_KERNEL_ABI_OLD_SESSION_STOP:
	case LTTNG_KERNEL_ABI_OLD_DISABLE:
	case LTTNG_KERNEL_ABI_SESSION_STOP:
	case LTTNG_KERNEL_ABI_DISABLE:
		return lttng_session_disable(session);
	case LTTNG_KERNEL_ABI_OLD_METADATA:
	{
		if (copy_from_user(&old_chan_param,
				(struct lttng_kernel_abi_old_channel __user *) arg,
				sizeof(struct lttng_kernel_abi_old_channel)))
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
	case LTTNG_KERNEL_ABI_METADATA:
	{
		if (copy_from_user(&chan_param,
					(struct lttng_kernel_abi_channel __user *) arg,
					sizeof(struct lttng_kernel_abi_channel)))
			return -EFAULT;
		return lttng_abi_create_channel(file, &chan_param,
				METADATA_CHANNEL);
	}
	case LTTNG_KERNEL_ABI_SESSION_TRACK_PID:
		return lttng_session_track_id(session, TRACKER_PID, (int) arg);
	case LTTNG_KERNEL_ABI_SESSION_UNTRACK_PID:
		return lttng_session_untrack_id(session, TRACKER_PID, (int) arg);
	case LTTNG_KERNEL_ABI_SESSION_TRACK_ID:
	{
		struct lttng_kernel_abi_tracker_args tracker;
		enum tracker_type tracker_type;

		if (copy_from_user(&tracker,
				(struct lttng_kernel_abi_tracker_args __user *) arg,
				sizeof(struct lttng_kernel_abi_tracker_args)))
			return -EFAULT;
		tracker_type = get_tracker_type(&tracker);
		if (tracker_type == TRACKER_UNKNOWN)
			return -EINVAL;
		return lttng_session_track_id(session, tracker_type, tracker.id);
	}
	case LTTNG_KERNEL_ABI_SESSION_UNTRACK_ID:
	{
		struct lttng_kernel_abi_tracker_args tracker;
		enum tracker_type tracker_type;

		if (copy_from_user(&tracker,
				(struct lttng_kernel_abi_tracker_args __user *) arg,
				sizeof(struct lttng_kernel_abi_tracker_args)))
			return -EFAULT;
		tracker_type = get_tracker_type(&tracker);
		if (tracker_type == TRACKER_UNKNOWN)
			return -EINVAL;
		return lttng_session_untrack_id(session, tracker_type,
				tracker.id);
	}
	case LTTNG_KERNEL_ABI_SESSION_LIST_TRACKER_PIDS:
		return lttng_session_list_tracker_ids(session, TRACKER_PID);
	case LTTNG_KERNEL_ABI_SESSION_LIST_TRACKER_IDS:
	{
		struct lttng_kernel_abi_tracker_args tracker;
		enum tracker_type tracker_type;

		if (copy_from_user(&tracker,
				(struct lttng_kernel_abi_tracker_args __user *) arg,
				sizeof(struct lttng_kernel_abi_tracker_args)))
			return -EFAULT;
		tracker_type = get_tracker_type(&tracker);
		if (tracker_type == TRACKER_UNKNOWN)
			return -EINVAL;
		return lttng_session_list_tracker_ids(session, tracker_type);
	}
	case LTTNG_KERNEL_ABI_SESSION_METADATA_REGEN:
		return lttng_session_metadata_regenerate(session);
	case LTTNG_KERNEL_ABI_SESSION_STATEDUMP:
		return lttng_session_statedump(session);
	case LTTNG_KERNEL_ABI_SESSION_SET_NAME:
	{
		struct lttng_kernel_abi_session_name name;

		if (copy_from_user(&name,
				(struct lttng_kernel_abi_session_name __user *) arg,
				sizeof(struct lttng_kernel_abi_session_name)))
			return -EFAULT;
		return lttng_abi_session_set_name(session, &name);
	}
	case LTTNG_KERNEL_ABI_SESSION_SET_CREATION_TIME:
	{
		struct lttng_kernel_abi_session_creation_time time;

		if (copy_from_user(&time,
				(struct lttng_kernel_abi_session_creation_time __user *) arg,
				sizeof(struct lttng_kernel_abi_session_creation_time)))
			return -EFAULT;
		return lttng_abi_session_set_creation_time(session, &time);
	}
	case LTTNG_KERNEL_ABI_SESSION_SET_OUTPUT_FORMAT:
	{
		return lttng_abi_session_set_output_format(session,
				(enum lttng_kernel_abi_session_output_format) arg);
	}
	case LTTNG_KERNEL_ABI_COUNTER:
	{
		struct lttng_kernel_counter_conf counter_conf;

		ret = lttng_abi_copy_user_counter_conf(&counter_conf,
				(struct lttng_kernel_abi_counter_conf __user *) arg);
		if (ret)
			return ret;
		return lttng_abi_session_create_counter(session, &counter_conf);
	}
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
	struct lttng_kernel_session *session = file->private_data;

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

/*
 * When encountering empty buffer, flush current sub-buffer if non-empty
 * and retry (if new data available to read after flush).
 */
static
ssize_t lttng_event_notifier_group_notif_read(struct file *filp, char __user *user_buf,
		size_t count, loff_t *ppos)
{
	struct lttng_event_notifier_group *event_notifier_group = filp->private_data;
	struct lttng_kernel_ring_buffer_channel *chan = event_notifier_group->chan;
	struct lttng_kernel_ring_buffer *buf = event_notifier_group->buf;
	ssize_t read_count = 0, len;
	size_t read_offset;

	might_sleep();
	if (!lttng_access_ok(VERIFY_WRITE, user_buf, count))
		return -EFAULT;

	/* Finish copy of previous record */
	if (*ppos != 0) {
		if (count != 0) {
			len = chan->iter.len_left;
			read_offset = *ppos;
			goto skip_get_next;
		}
	}

	while (read_count < count) {
		size_t copy_len, space_left;

		len = lib_ring_buffer_get_next_record(chan, buf);
len_test:
		if (len < 0) {
			/*
			 * Check if buffer is finalized (end of file).
			 */
			if (len == -ENODATA) {
				/* A 0 read_count will tell about end of file */
				goto nodata;
			}
			if (filp->f_flags & O_NONBLOCK) {
				if (!read_count)
					read_count = -EAGAIN;
				goto nodata;
			} else {
				int error;

				/*
				 * No data available at the moment, return what
				 * we got.
				 */
				if (read_count)
					goto nodata;

				/*
				 * Wait for returned len to be >= 0 or -ENODATA.
				 */
				error = wait_event_interruptible(
					  event_notifier_group->read_wait,
					  ((len = lib_ring_buffer_get_next_record(
						  chan, buf)), len != -EAGAIN));
				CHAN_WARN_ON(chan, len == -EBUSY);
				if (error) {
					read_count = error;
					goto nodata;
				}
				CHAN_WARN_ON(chan, len < 0 && len != -ENODATA);
				goto len_test;
			}
		}
		read_offset = buf->iter.read_offset;
skip_get_next:
		space_left = count - read_count;
		if (len <= space_left) {
			copy_len = len;
			chan->iter.len_left = 0;
			*ppos = 0;
		} else {
			copy_len = space_left;
			chan->iter.len_left = len - copy_len;
			*ppos = read_offset + copy_len;
		}
		if (__lib_ring_buffer_copy_to_user(&buf->backend, read_offset,
					       &user_buf[read_count],
					       copy_len)) {
			/*
			 * Leave the len_left and ppos values at their current
			 * state, as we currently have a valid event to read.
			 */
			return -EFAULT;
		}
		read_count += copy_len;
	}
	goto put_record;

nodata:
	*ppos = 0;
	chan->iter.len_left = 0;

put_record:
	if (*ppos == 0)
		lib_ring_buffer_put_current_record(buf);
	return read_count;
}

/*
 * If the ring buffer is non empty (even just a partial subbuffer), return that
 * there is data available. Perform a ring buffer flush if we encounter a
 * non-empty ring buffer which does not have any consumeable subbuffer available.
 */
static
unsigned int lttng_event_notifier_group_notif_poll(struct file *filp,
		poll_table *wait)
{
	unsigned int mask = 0;
	struct lttng_event_notifier_group *event_notifier_group = filp->private_data;
	struct lttng_kernel_ring_buffer_channel *chan = event_notifier_group->chan;
	struct lttng_kernel_ring_buffer *buf = event_notifier_group->buf;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	int finalized, disabled;
	unsigned long consumed, offset;
	size_t subbuffer_header_size = config->cb.subbuffer_header_size();

	if (filp->f_mode & FMODE_READ) {
		poll_wait_set_exclusive(wait);
		poll_wait(filp, &event_notifier_group->read_wait, wait);

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

		offset = lib_ring_buffer_get_offset(config, buf);
		consumed = lib_ring_buffer_get_consumed(config, buf);

		/*
		 * If there is no buffer available to consume.
		 */
		if (subbuf_trunc(offset, chan) - subbuf_trunc(consumed, chan) == 0) {
			/*
			 * If there is a non-empty subbuffer, flush and try again.
			 */
			if (subbuf_offset(offset, chan) > subbuffer_header_size) {
				lib_ring_buffer_switch_remote(buf);
				goto retry;
			}

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
			if (subbuf_trunc(offset, chan) - subbuf_trunc(consumed, chan)
					>= chan->backend.buf_size)
				return POLLPRI | POLLRDBAND;
			else
				return POLLIN | POLLRDNORM;
		}
	}

	return mask;
}

/**
 *	lttng_event_notifier_group_notif_open - event_notifier ring buffer open file operation
 *	@inode: opened inode
 *	@file: opened file
 *
 *	Open implementation. Makes sure only one open instance of a buffer is
 *	done at a given moment.
 */
static int lttng_event_notifier_group_notif_open(struct inode *inode, struct file *file)
{
	struct lttng_event_notifier_group *event_notifier_group = inode->i_private;
	struct lttng_kernel_ring_buffer *buf = event_notifier_group->buf;

	file->private_data = event_notifier_group;
	return lib_ring_buffer_open(inode, file, buf);
}

/**
 *	lttng_event_notifier_group_notif_release - event_notifier ring buffer release file operation
 *	@inode: opened inode
 *	@file: opened file
 *
 *	Release implementation.
 */
static int lttng_event_notifier_group_notif_release(struct inode *inode, struct file *file)
{
	struct lttng_event_notifier_group *event_notifier_group = file->private_data;
	struct lttng_kernel_ring_buffer *buf = event_notifier_group->buf;
	int ret;

	ret = lib_ring_buffer_release(inode, file, buf);
	if (ret)
		return ret;
	fput(event_notifier_group->file);
	return 0;
}

static const struct file_operations lttng_event_notifier_group_notif_fops = {
	.owner = THIS_MODULE,
	.open = lttng_event_notifier_group_notif_open,
	.release = lttng_event_notifier_group_notif_release,
	.read = lttng_event_notifier_group_notif_read,
	.poll = lttng_event_notifier_group_notif_poll,
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
	struct lttng_kernel_ring_buffer *buf = stream->priv;
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
	struct lttng_kernel_ring_buffer *buf = stream->priv;
	unsigned int rb_cmd;
	bool coherent;

	if (cmd == LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF_METADATA_CHECK)
		rb_cmd = LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF;
	else
		rb_cmd = cmd;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF:
	{
		struct lttng_metadata_stream *stream = filp->private_data;
		struct lttng_kernel_ring_buffer *buf = stream->priv;
		struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;

		ret = lttng_metadata_output_channel(stream, chan, NULL);
		if (ret > 0) {
			lib_ring_buffer_switch_slow(buf, SWITCH_ACTIVE);
			ret = 0;
		} else if (ret < 0)
			goto err;
		break;
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_SUBBUF:
	{
		/*
		 * Random access is not allowed for metadata channel.
		 */
		return -ENOSYS;
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH_EMPTY:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH:
	{
		struct lttng_metadata_stream *stream = filp->private_data;
		struct lttng_kernel_ring_buffer *buf = stream->priv;
		struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;

		/*
		 * Before doing the actual ring buffer flush, write up to one
		 * packet of metadata in the ring buffer.
		 */
		ret = lttng_metadata_output_channel(stream, chan, NULL);
		if (ret < 0)
			goto err;
		break;
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_METADATA_VERSION:
	{
		struct lttng_metadata_stream *stream = filp->private_data;

		return put_u64(stream->version, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_METADATA_CACHE_DUMP:
	{
		struct lttng_metadata_stream *stream = filp->private_data;

		return lttng_metadata_cache_dump(stream);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF_METADATA_CHECK:
	{
		struct lttng_metadata_stream *stream = filp->private_data;
		struct lttng_kernel_ring_buffer *buf = stream->priv;
		struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;

		ret = lttng_metadata_output_channel(stream, chan, &coherent);
		if (ret > 0) {
			lib_ring_buffer_switch_slow(buf, SWITCH_ACTIVE);
			ret = 0;
		} else if (ret < 0) {
			goto err;
		}
		break;
	}
	default:
		break;
	}
	/* PUT_SUBBUF is the one from lib ring buffer, unmodified. */

	/* Performing lib ring buffer ioctl after our own. */
	ret = lib_ring_buffer_ioctl(filp, rb_cmd, arg, buf);
	if (ret < 0)
		goto err;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_RING_BUFFER_PUT_NEXT_SUBBUF:
	{
		lttng_metadata_ring_buffer_ioctl_put_next_subbuf(filp,
				cmd, arg);
		break;
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF_METADATA_CHECK:
	{
		return put_u32(coherent, arg);
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
	struct lttng_kernel_ring_buffer *buf = stream->priv;
	unsigned int rb_cmd;
	bool coherent;

	if (cmd == LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF_METADATA_CHECK)
		rb_cmd = LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF;
	else
		rb_cmd = cmd;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF:
	{
		struct lttng_metadata_stream *stream = filp->private_data;
		struct lttng_kernel_ring_buffer *buf = stream->priv;
		struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;

		ret = lttng_metadata_output_channel(stream, chan, NULL);
		if (ret > 0) {
			lib_ring_buffer_switch_slow(buf, SWITCH_ACTIVE);
			ret = 0;
		} else if (ret < 0)
			goto err;
		break;
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_SUBBUF:
	{
		/*
		 * Random access is not allowed for metadata channel.
		 */
		return -ENOSYS;
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH_EMPTY:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_RING_BUFFER_FLUSH:
	{
		struct lttng_metadata_stream *stream = filp->private_data;
		struct lttng_kernel_ring_buffer *buf = stream->priv;
		struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;

		/*
		 * Before doing the actual ring buffer flush, write up to one
		 * packet of metadata in the ring buffer.
		 */
		ret = lttng_metadata_output_channel(stream, chan, NULL);
		if (ret < 0)
			goto err;
		break;
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_METADATA_VERSION:
	{
		struct lttng_metadata_stream *stream = filp->private_data;

		return put_u64(stream->version, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_METADATA_CACHE_DUMP:
	{
		struct lttng_metadata_stream *stream = filp->private_data;

		return lttng_metadata_cache_dump(stream);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF_METADATA_CHECK:
	{
		struct lttng_metadata_stream *stream = filp->private_data;
		struct lttng_kernel_ring_buffer *buf = stream->priv;
		struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;

		ret = lttng_metadata_output_channel(stream, chan, &coherent);
		if (ret > 0) {
			lib_ring_buffer_switch_slow(buf, SWITCH_ACTIVE);
			ret = 0;
		} else if (ret < 0) {
			goto err;
		}
		break;
	}
	default:
		break;
	}
	/* PUT_SUBBUF is the one from lib ring buffer, unmodified. */

	/* Performing lib ring buffer ioctl after our own. */
	ret = lib_ring_buffer_compat_ioctl(filp, rb_cmd, arg, buf);
	if (ret < 0)
		goto err;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_RING_BUFFER_PUT_NEXT_SUBBUF:
	{
		lttng_metadata_ring_buffer_ioctl_put_next_subbuf(filp,
				cmd, arg);
		break;
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_NEXT_SUBBUF_METADATA_CHECK:
	{
		return put_u32(coherent, arg);
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
	struct lttng_kernel_ring_buffer *buf = stream->priv;

	file->private_data = buf;
	/*
	 * Since life-time of metadata cache differs from that of
	 * session, we need to keep our own reference on the transport.
	 */
	if (!try_module_get(stream->transport->owner)) {
		printk(KERN_WARNING "LTTng: Can't lock transport module.\n");
		return -EBUSY;
	}
	return lib_ring_buffer_open(inode, file, buf);
}

static
int lttng_metadata_ring_buffer_release(struct inode *inode, struct file *file)
{
	struct lttng_metadata_stream *stream = file->private_data;
	struct lttng_kernel_ring_buffer *buf = stream->priv;

	mutex_lock(&stream->metadata_cache->lock);
	list_del(&stream->node);
	mutex_unlock(&stream->metadata_cache->lock);
	kref_put(&stream->metadata_cache->refcount, metadata_cache_destroy);
	module_put(stream->transport->owner);
	kfree(stream);
	return lib_ring_buffer_release(inode, file, buf);
}

static
ssize_t lttng_metadata_ring_buffer_splice_read(struct file *in, loff_t *ppos,
		struct pipe_inode_info *pipe, size_t len,
		unsigned int flags)
{
	struct lttng_metadata_stream *stream = in->private_data;
	struct lttng_kernel_ring_buffer *buf = stream->priv;

	return lib_ring_buffer_splice_read(in, ppos, pipe, len,
			flags, buf);
}

static
int lttng_metadata_ring_buffer_mmap(struct file *filp,
		struct vm_area_struct *vma)
{
	struct lttng_metadata_stream *stream = filp->private_data;
	struct lttng_kernel_ring_buffer *buf = stream->priv;

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
		const struct file_operations *fops, const char *name)
{
	int stream_fd, ret;
	struct file *stream_file;

	stream_fd = get_unused_fd_flags(0);
	if (stream_fd < 0) {
		ret = stream_fd;
		goto fd_error;
	}
	stream_file = anon_inode_getfile(name, fops, stream_priv, O_RDWR);
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
	struct lttng_kernel_channel_buffer *channel = channel_file->private_data;
	struct lttng_kernel_ring_buffer *buf;
	int ret;
	void *stream_priv;

	buf = channel->ops->priv->buffer_read_open(channel->priv->rb_chan);
	if (!buf)
		return -ENOENT;

	stream_priv = buf;
	ret = lttng_abi_create_stream_fd(channel_file, stream_priv,
			&lttng_stream_ring_buffer_file_operations,
			"[lttng_stream]");
	if (ret < 0)
		goto fd_error;

	return ret;

fd_error:
	channel->ops->priv->buffer_read_close(buf);
	return ret;
}

static
int lttng_abi_open_metadata_stream(struct file *channel_file)
{
	struct lttng_kernel_channel_buffer *channel = channel_file->private_data;
	struct lttng_kernel_session *session = channel->parent.session;
	struct lttng_kernel_ring_buffer *buf;
	int ret;
	struct lttng_metadata_stream *metadata_stream;
	void *stream_priv;

	buf = channel->ops->priv->buffer_read_open(channel->priv->rb_chan);
	if (!buf)
		return -ENOENT;

	metadata_stream = kzalloc(sizeof(struct lttng_metadata_stream),
			GFP_KERNEL);
	if (!metadata_stream) {
		ret = -ENOMEM;
		goto nomem;
	}
	metadata_stream->metadata_cache = session->priv->metadata_cache;
	init_waitqueue_head(&metadata_stream->read_wait);
	metadata_stream->priv = buf;
	stream_priv = metadata_stream;
	metadata_stream->transport = channel->priv->transport;
	/* Initial state is an empty metadata, considered as incoherent. */
	metadata_stream->coherent = false;

	/*
	 * Since life-time of metadata cache differs from that of
	 * session, we need to keep our own reference on the transport.
	 */
	if (!try_module_get(metadata_stream->transport->owner)) {
		printk(KERN_WARNING "LTTng: Can't lock transport module.\n");
		ret = -EINVAL;
		goto notransport;
	}

	if (!lttng_kref_get(&session->priv->metadata_cache->refcount)) {
		ret = -EOVERFLOW;
		goto kref_error;
	}

	ret = lttng_abi_create_stream_fd(channel_file, stream_priv,
			&lttng_metadata_ring_buffer_file_operations,
			"[lttng_metadata_stream]");
	if (ret < 0)
		goto fd_error;

	mutex_lock(&session->priv->metadata_cache->lock);
	list_add(&metadata_stream->node,
		&session->priv->metadata_cache->metadata_stream);
	mutex_unlock(&session->priv->metadata_cache->lock);
	return ret;

fd_error:
	kref_put(&session->priv->metadata_cache->refcount, metadata_cache_destroy);
kref_error:
	module_put(metadata_stream->transport->owner);
notransport:
	kfree(metadata_stream);
nomem:
	channel->ops->priv->buffer_read_close(buf);
	return ret;
}

static
int lttng_abi_open_event_notifier_group_stream(struct file *notif_file)
{
	struct lttng_event_notifier_group *event_notifier_group = notif_file->private_data;
	struct lttng_kernel_ring_buffer_channel *chan = event_notifier_group->chan;
	struct lttng_kernel_ring_buffer *buf;
	int ret;
	void *stream_priv;

	buf = event_notifier_group->ops->priv->buffer_read_open(chan);
	if (!buf)
		return -ENOENT;

	/* The event_notifier notification fd holds a reference on the event_notifier group */
	if (!lttng_file_ref_get(notif_file)) {
		ret = -EOVERFLOW;
		goto refcount_error;
	}
	event_notifier_group->buf = buf;
	stream_priv = event_notifier_group;
	ret = lttng_abi_create_stream_fd(notif_file, stream_priv,
			&lttng_event_notifier_group_notif_fops,
			"[lttng_event_notifier_stream]");
	if (ret < 0)
		goto fd_error;

	return ret;

fd_error:
	if (!lttng_file_ref_put(notif_file)) {
		/* Don't change return code */
	}
refcount_error:
	event_notifier_group->ops->priv->buffer_read_close(buf);
	return ret;
}

static
int lttng_abi_validate_event_param(struct lttng_kernel_abi_event *event_param,
		struct lttng_kernel_abi_event_ext *event_param_ext)
{
	enum lttng_kernel_abi_match_check match_check;

	if (user_event_param_ext_get_match_check(event_param_ext, &match_check))
		return -EINVAL;

	/* Limit ABI to implemented features. */
	switch (event_param->instrumentation) {
	case LTTNG_KERNEL_ABI_SYSCALL:
		switch (event_param->u.syscall.entryexit) {
		case LTTNG_KERNEL_ABI_SYSCALL_ENTRYEXIT:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_SYSCALL_ENTRY:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_SYSCALL_EXIT:
			break;
		default:
			return -EINVAL;
		}
		switch (event_param->u.syscall.abi) {
		case LTTNG_KERNEL_ABI_SYSCALL_ABI_ALL:
			break;
		default:
			return -EINVAL;
		}
		switch (event_param->u.syscall.match) {
		case LTTNG_KERNEL_ABI_SYSCALL_MATCH_NAME:
			break;
		default:
			return -EINVAL;
		}
		switch (match_check) {
		case LTTNG_KERNEL_ABI_MATCH_DEFAULT:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_MATCH_LAZY:
			break;
		default:
			return -EINVAL;
		}
		break;

	case LTTNG_KERNEL_ABI_KRETPROBE:
		switch (event_param->u.kretprobe.entryexit) {
		case LTTNG_KERNEL_ABI_KRETPROBE_ENTRYEXIT:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_KRETPROBE_ENTRY:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_KRETPROBE_EXIT:
			break;
		default:
			return -EINVAL;
		}
		switch (match_check) {
		case LTTNG_KERNEL_ABI_MATCH_DEFAULT:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_MATCH_IMMEDIATE:
			break;
		default:
			return -EINVAL;
		}
		break;

	case LTTNG_KERNEL_ABI_TRACEPOINT:
		switch (match_check) {
		case LTTNG_KERNEL_ABI_MATCH_DEFAULT:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_MATCH_LAZY:
			break;
		default:
			return -EINVAL;
		}
		break;

	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_UPROBE:
		switch (match_check) {
		case LTTNG_KERNEL_ABI_MATCH_DEFAULT:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_MATCH_IMMEDIATE:
			break;
		default:
			return -EINVAL;
		}
		break;

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_NOOP:
		lttng_fallthrough;
	default:
		return -EINVAL;
	}
	return 0;
}

static
int lttng_abi_validate_event_match(struct lttng_kernel_abi_event *event_param,
		struct lttng_kernel_abi_event_ext *event_param_ext)
{
	enum lttng_kernel_abi_match_check match_check;
	int ret;

	if (user_event_param_ext_get_match_check(event_param_ext, &match_check))
		return -EINVAL;

	/* Validate match */
	if (match_check == LTTNG_KERNEL_ABI_MATCH_DEFAULT) {
		switch (event_param->instrumentation) {
		case LTTNG_KERNEL_ABI_TRACEPOINT:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_SYSCALL:
			match_check = LTTNG_KERNEL_ABI_MATCH_LAZY;
			break;
		case LTTNG_KERNEL_ABI_KPROBE:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_KRETPROBE:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_UPROBE:
			match_check = LTTNG_KERNEL_ABI_MATCH_IMMEDIATE;
			break;

		case LTTNG_KERNEL_ABI_FUNCTION:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_NOOP:
			lttng_fallthrough;
		default:
			return -EINVAL;
		}
	}

	if (match_check == LTTNG_KERNEL_ABI_MATCH_IMMEDIATE) {
		switch (event_param->instrumentation) {
			break;
		case LTTNG_KERNEL_ABI_KPROBE:
			ret = lttng_kprobes_match_check(event_param->u.kprobe.symbol_name,
					event_param->u.kprobe.offset,
					event_param->u.kprobe.addr);
			if (ret)
				return ret;
			break;
		case LTTNG_KERNEL_ABI_KRETPROBE:
			ret = lttng_kretprobes_match_check(event_param->u.kretprobe.symbol_name,
					event_param->u.kretprobe.offset,
					event_param->u.kretprobe.addr);
			if (ret)
				return ret;
			break;
		case LTTNG_KERNEL_ABI_UPROBE:
			/*
			 * uprobes are immediately created, which includes match checking.
			 */
			break;

		case LTTNG_KERNEL_ABI_TRACEPOINT:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_SYSCALL:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_FUNCTION:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_NOOP:
			lttng_fallthrough;
		default:
			return -EINVAL;
		}
	}
	return 0;
}

static
int lttng_abi_create_event_recorder_enabler(struct file *channel_file,
			   struct lttng_kernel_abi_event *event_param,
			   struct lttng_kernel_abi_event_ext *event_param_ext)
{
	const struct file_operations *fops;
	struct lttng_kernel_channel_buffer *channel = channel_file->private_data;
	int event_fd, ret;
	struct file *event_file;
	void *priv;

	event_param->name[LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1] = '\0';
	switch (event_param->instrumentation) {
	case LTTNG_KERNEL_ABI_KRETPROBE:
		event_param->u.kretprobe.symbol_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1] = '\0';
		break;
	case LTTNG_KERNEL_ABI_KPROBE:
		event_param->u.kprobe.symbol_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1] = '\0';
		break;
	case LTTNG_KERNEL_ABI_FUNCTION:
		WARN_ON_ONCE(1);
		/* Not implemented. */
		break;
	default:
		break;
	}

	switch (event_param->instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_SYSCALL:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KRETPROBE:
		fops = &lttng_event_session_enabler_fops;
		break;

	case LTTNG_KERNEL_ABI_UPROBE:
		fops = &lttng_event_session_fops;
		break;

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_NOOP:
		lttng_fallthrough;
	default:
		return -EINVAL;
	}

	event_fd = get_unused_fd_flags(0);
	if (event_fd < 0) {
		ret = event_fd;
		goto fd_error;
	}
	event_file = anon_inode_getfile("[lttng_event_recorder]",
					fops, NULL, O_RDWR);
	if (IS_ERR(event_file)) {
		ret = PTR_ERR(event_file);
		goto file_error;
	}
	/* The event holds a reference on the channel */
	if (!lttng_file_ref_get(channel_file)) {
		ret = -EOVERFLOW;
		goto refcount_error;
	}
	ret = lttng_abi_validate_event_param(event_param, event_param_ext);
	if (ret)
		goto event_error;
	ret = lttng_abi_validate_event_match(event_param, event_param_ext);
	if (ret)
		goto event_error;

	switch (event_param->instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_SYSCALL:
	{
		struct lttng_event_recorder_enabler *event_enabler;

		if (strutils_is_star_glob_pattern(event_param->name)) {
			/*
			 * If the event name is a star globbing pattern,
			 * we create the special star globbing enabler.
			 */
			event_enabler = lttng_event_recorder_enabler_create(LTTNG_ENABLER_FORMAT_STAR_GLOB,
				event_param, channel);
		} else {
			event_enabler = lttng_event_recorder_enabler_create(LTTNG_ENABLER_FORMAT_NAME,
				event_param, channel);
		}
		if (event_enabler)
			lttng_event_enabler_session_add(channel->parent.session, &event_enabler->parent);
		priv = event_enabler;
		break;
	}

	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KRETPROBE:
	{
		struct lttng_event_recorder_enabler *event_enabler;

		event_enabler = lttng_event_recorder_enabler_create(LTTNG_ENABLER_FORMAT_NAME,
				event_param, channel);
		if (event_enabler)
			lttng_event_enabler_session_add(channel->parent.session, &event_enabler->parent);
		priv = event_enabler;
		break;
	}

	case LTTNG_KERNEL_ABI_UPROBE:
	{
		struct lttng_kernel_event_common *event;
		struct lttng_event_recorder_enabler *event_enabler;

		event_enabler = lttng_event_recorder_enabler_create(LTTNG_ENABLER_FORMAT_NAME,
				event_param, channel);
		if (!event_enabler) {
			ret = -ENOMEM;
			goto event_error;
		}
		/*
		 * We tolerate no failure path after event creation. It
		 * will stay invariant for the rest of the session.
		 */
		event = lttng_kernel_event_create(&event_enabler->parent.parent, NULL, NULL);
		lttng_event_enabler_destroy(&event_enabler->parent.parent);
		if (IS_ERR(event)) {
			ret = PTR_ERR(event);
			goto event_error;
		}
		priv = event;
		break;
	}

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_NOOP:
		lttng_fallthrough;
	default:
		ret = -EINVAL;
		goto event_error;
	}
	event_file->private_data = priv;
	fd_install(event_fd, event_file);
	return event_fd;

event_error:
	if (!lttng_file_ref_put(channel_file)) {
		ret = -EINVAL;
	}
refcount_error:
	fput(event_file);
file_error:
	put_unused_fd(event_fd);
fd_error:
	return ret;
}

static
int lttng_abi_create_event_counter_enabler(struct file *channel_file,
			struct lttng_kernel_counter_event *counter_event)
{
	const struct file_operations *fops;
	struct lttng_kernel_channel_counter *channel = channel_file->private_data;
	struct lttng_kernel_abi_event *event_param = &counter_event->event_param;
	struct lttng_kernel_abi_event_ext *event_param_ext = &counter_event->event_param_ext;
	struct lttng_kernel_counter_key *counter_key = counter_event->counter_key;
	int event_fd, ret;
	struct file *event_file;
	void *priv;

	event_param->name[LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1] = '\0';
	switch (event_param->instrumentation) {
	case LTTNG_KERNEL_ABI_KRETPROBE:
		event_param->u.kretprobe.symbol_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1] = '\0';
		break;
	case LTTNG_KERNEL_ABI_KPROBE:
		event_param->u.kprobe.symbol_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1] = '\0';
		break;
	case LTTNG_KERNEL_ABI_FUNCTION:
		WARN_ON_ONCE(1);
		/* Not implemented. */
		break;
	default:
		break;
	}

	switch (event_param->instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_SYSCALL:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KRETPROBE:
		fops = &lttng_event_session_enabler_fops;
		break;

	case LTTNG_KERNEL_ABI_UPROBE:
		fops = &lttng_event_session_fops;
		break;

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_NOOP:
		lttng_fallthrough;
	default:
		return -EINVAL;
	}

	event_fd = get_unused_fd_flags(0);
	if (event_fd < 0) {
		ret = event_fd;
		goto fd_error;
	}
	event_file = anon_inode_getfile("[lttng_event_counter]",
					fops, NULL, O_RDWR);
	if (IS_ERR(event_file)) {
		ret = PTR_ERR(event_file);
		goto file_error;
	}
	/* The event holds a reference on the channel */
	if (!lttng_file_ref_get(channel_file)) {
		ret = -EOVERFLOW;
		goto refcount_error;
	}
	ret = lttng_abi_validate_event_param(event_param, event_param_ext);
	if (ret)
		goto event_error;
	ret = lttng_abi_validate_event_match(event_param, event_param_ext);
	if (ret)
		goto event_error;

	switch (event_param->instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_SYSCALL:
	{
		struct lttng_event_counter_enabler *event_enabler;

		if (strutils_is_star_glob_pattern(event_param->name)) {
			/*
			 * If the event name is a star globbing pattern,
			 * we create the special star globbing enabler.
			 */
			event_enabler = lttng_event_counter_enabler_create(LTTNG_ENABLER_FORMAT_STAR_GLOB,
				event_param, counter_key, channel);
		} else {
			event_enabler = lttng_event_counter_enabler_create(LTTNG_ENABLER_FORMAT_NAME,
				event_param, counter_key, channel);
		}
		if (event_enabler)
			lttng_event_enabler_session_add(channel->parent.session, &event_enabler->parent);
		priv = event_enabler;
		break;
	}

	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KRETPROBE:
	{
		struct lttng_event_counter_enabler *event_enabler;

		event_enabler = lttng_event_counter_enabler_create(LTTNG_ENABLER_FORMAT_NAME,
				event_param, counter_key, channel);
		if (event_enabler)
			lttng_event_enabler_session_add(channel->parent.session, &event_enabler->parent);
		priv = event_enabler;
		break;
	}

	case LTTNG_KERNEL_ABI_UPROBE:
	{
		struct lttng_kernel_event_common *event;
		struct lttng_event_counter_enabler *event_enabler;

		event_enabler = lttng_event_counter_enabler_create(LTTNG_ENABLER_FORMAT_NAME,
				event_param, counter_key, channel);
		if (!event_enabler) {
			ret = -ENOMEM;
			goto event_error;
		}
		/*
		 * We tolerate no failure path after event creation. It
		 * will stay invariant for the rest of the session.
		 */
		event = lttng_kernel_event_create(&event_enabler->parent.parent, NULL, NULL);
		lttng_event_enabler_destroy(&event_enabler->parent.parent);
		if (IS_ERR(event)) {
			ret = PTR_ERR(event);
			goto event_error;
		}
		priv = event;
		break;
	}

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_NOOP:
		lttng_fallthrough;
	default:
		ret = -EINVAL;
		goto event_error;
	}
	event_file->private_data = priv;
	fd_install(event_fd, event_file);
	return event_fd;

event_error:
	if (!lttng_file_ref_put(channel_file)) {
		/* Don't change return code */
	}
refcount_error:
	fput(event_file);
file_error:
	put_unused_fd(event_fd);
fd_error:
	return ret;
}

static
long lttng_event_notifier_event_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct lttng_kernel_event_common *event = file->private_data;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_ENABLE:
		return lttng_event_enable(event);
	case LTTNG_KERNEL_ABI_DISABLE:
		return lttng_event_disable(event);
	case LTTNG_KERNEL_ABI_FILTER:
		return -EINVAL;
	case LTTNG_KERNEL_ABI_CAPTURE:
		return -EINVAL;
	case LTTNG_KERNEL_ABI_ADD_CALLSITE:
		return lttng_event_add_callsite(event,
			(struct lttng_kernel_abi_event_callsite __user *) arg);
	default:
		return -ENOIOCTLCMD;
	}
}

static
long lttng_event_notifier_enabler_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct lttng_event_enabler_common *event = file->private_data;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_ENABLE:
		return lttng_event_enabler_enable(event);
	case LTTNG_KERNEL_ABI_DISABLE:
		return lttng_event_enabler_disable(event);
	case LTTNG_KERNEL_ABI_FILTER:
		return lttng_event_enabler_attach_filter_bytecode(event,
			(struct lttng_kernel_abi_filter_bytecode __user *) arg);
	case LTTNG_KERNEL_ABI_CAPTURE:
	{
		struct lttng_event_notifier_enabler *event_notifier_enabler =
			container_of(event, struct lttng_event_notifier_enabler, parent);
		return lttng_event_notifier_enabler_attach_capture_bytecode(
			event_notifier_enabler,
			(struct lttng_kernel_abi_capture_bytecode __user *) arg);
	}
	case LTTNG_KERNEL_ABI_ADD_CALLSITE:
		return -EINVAL;
	default:
		return -ENOIOCTLCMD;
	}
}

static
int lttng_event_notifier_event_release(struct inode *inode, struct file *file)
{
	struct lttng_kernel_event_common *event = file->private_data;

	if (event) {
		struct lttng_kernel_event_notifier *event_notifier =
			container_of(event, struct lttng_kernel_event_notifier, parent);
		fput(event_notifier->priv->group->file);
	}
	return 0;
}

static
int lttng_event_notifier_enabler_release(struct inode *inode, struct file *file)
{
	struct lttng_event_enabler_common *event_enabler = file->private_data;

	if (event_enabler) {
		struct lttng_event_notifier_enabler *event_notifier_enabler =
			container_of(event_enabler, struct lttng_event_notifier_enabler, parent);
		fput(event_notifier_enabler->group->file);
	}
	return 0;
}

static const struct file_operations lttng_event_notifier_event_fops = {
	.owner = THIS_MODULE,
	.release = lttng_event_notifier_event_release,
	.unlocked_ioctl = lttng_event_notifier_event_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_event_notifier_event_ioctl,
#endif
};

static const struct file_operations lttng_event_notifier_enabler_fops = {
	.owner = THIS_MODULE,
	.release = lttng_event_notifier_enabler_release,
	.unlocked_ioctl = lttng_event_notifier_enabler_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_event_notifier_enabler_ioctl,
#endif
};

static
int lttng_abi_create_event_notifier(struct file *event_notifier_group_file,
		struct lttng_kernel_abi_event_notifier *event_notifier_param,
		struct lttng_kernel_abi_event_ext *event_param_ext)
{
	struct lttng_event_notifier_group *event_notifier_group =
			event_notifier_group_file->private_data;
	const struct file_operations *fops;
	int event_notifier_fd, ret;
	struct file *event_notifier_file;
	void *priv;

	switch (event_notifier_param->event.instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_SYSCALL:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KRETPROBE:
		fops = &lttng_event_notifier_enabler_fops;
		break;

	case LTTNG_KERNEL_ABI_UPROBE:
		fops = &lttng_event_notifier_event_fops;
		break;

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_NOOP:
		lttng_fallthrough;
	default:
		ret = -EINVAL;
		goto inval_instr;
	}

	event_notifier_param->event.name[LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1] = '\0';
	switch (event_notifier_param->event.instrumentation) {
	case LTTNG_KERNEL_ABI_KRETPROBE:
		event_notifier_param->event.u.kretprobe.symbol_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1] = '\0';
		break;
	case LTTNG_KERNEL_ABI_KPROBE:
		event_notifier_param->event.u.kprobe.symbol_name[LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1] = '\0';
		break;
	default:
		break;
	}

	event_notifier_fd = get_unused_fd_flags(0);
	if (event_notifier_fd < 0) {
		ret = event_notifier_fd;
		goto fd_error;
	}

	event_notifier_file = anon_inode_getfile("[lttng_event_notifier]",
					fops, NULL, O_RDWR);
	if (IS_ERR(event_notifier_file)) {
		ret = PTR_ERR(event_notifier_file);
		goto file_error;
	}

	/* The event notifier holds a reference on the event notifier group. */
	if (!lttng_file_ref_get(event_notifier_group_file)) {
		ret = -EOVERFLOW;
		goto refcount_error;
	}

	ret = lttng_abi_validate_event_param(&event_notifier_param->event, event_param_ext);
	if (ret)
		goto event_notifier_error;
	ret = lttng_abi_validate_event_match(&event_notifier_param->event, event_param_ext);
	if (ret)
		goto event_notifier_error;

	switch (event_notifier_param->event.instrumentation) {
	case LTTNG_KERNEL_ABI_TRACEPOINT:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_SYSCALL:
	{
		struct lttng_event_notifier_enabler *enabler;

		if (strutils_is_star_glob_pattern(event_notifier_param->event.name)) {
			/*
			 * If the event name is a star globbing pattern,
			 * we create the special star globbing enabler.
			 */
			enabler = lttng_event_notifier_enabler_create(
					LTTNG_ENABLER_FORMAT_STAR_GLOB,
					event_notifier_param,
					event_notifier_group);
		} else {
			enabler = lttng_event_notifier_enabler_create(
					LTTNG_ENABLER_FORMAT_NAME,
					event_notifier_param,
					event_notifier_group);
		}
		if (enabler)
			lttng_event_notifier_enabler_group_add(event_notifier_group, enabler);
		priv = enabler;
		break;
	}

	case LTTNG_KERNEL_ABI_KPROBE:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_KRETPROBE:
	{
		struct lttng_event_notifier_enabler *enabler;

		enabler = lttng_event_notifier_enabler_create(LTTNG_ENABLER_FORMAT_NAME,
					event_notifier_param, event_notifier_group);
		if (enabler)
			lttng_event_notifier_enabler_group_add(event_notifier_group, enabler);
		priv = enabler;
		break;
	}

	case LTTNG_KERNEL_ABI_UPROBE:
	{
		struct lttng_kernel_event_common *event;
		struct lttng_event_notifier_enabler *event_notifier_enabler;

		event_notifier_enabler = lttng_event_notifier_enabler_create(LTTNG_ENABLER_FORMAT_NAME,
				event_notifier_param, event_notifier_group);
		if (!event_notifier_enabler) {
			ret = -ENOMEM;
			goto event_notifier_error;
		}
		event = lttng_kernel_event_create(&event_notifier_enabler->parent, NULL, NULL);
		lttng_event_enabler_destroy(&event_notifier_enabler->parent);
		if (IS_ERR(event)) {
			ret = PTR_ERR(event);
			goto event_notifier_error;
		}
		priv = event;
		break;
	}

	case LTTNG_KERNEL_ABI_FUNCTION:
		lttng_fallthrough;
	case LTTNG_KERNEL_ABI_NOOP:
		lttng_fallthrough;
	default:
		ret = -EINVAL;
		goto event_notifier_error;
	}
	event_notifier_file->private_data = priv;
	fd_install(event_notifier_fd, event_notifier_file);
	return event_notifier_fd;

event_notifier_error:
	if (!lttng_file_ref_put(event_notifier_group_file)) {
		/* Don't change return code */
	}
refcount_error:
	fput(event_notifier_file);
file_error:
	put_unused_fd(event_notifier_fd);
fd_error:
inval_instr:
	return ret;
}

static
long lttng_abi_session_create_counter(
		struct lttng_kernel_session *session,
		const struct lttng_kernel_counter_conf *counter_conf)
{
	int counter_fd, ret;
	char *counter_transport_name;
	struct lttng_kernel_channel_counter *chan_counter = NULL;
	struct file *counter_file;

	if (counter_conf->arithmetic != LTTNG_KERNEL_COUNTER_ARITHMETIC_MODULAR) {
		printk(KERN_ERR "LTTng: Maps: Counter of the wrong arithmetic type.\n");
		return -EINVAL;
	}

	switch (counter_conf->bitness) {
	case LTTNG_KERNEL_COUNTER_BITNESS_64:
		counter_transport_name = "counter-per-cpu-64-modular";
		break;
	case LTTNG_KERNEL_COUNTER_BITNESS_32:
		counter_transport_name = "counter-per-cpu-32-modular";
		break;
	default:
		return -EINVAL;
	}

	lttng_lock_sessions();

	counter_fd = get_unused_fd_flags(0);
	if (counter_fd < 0) {
		ret = counter_fd;
		goto fd_error;
	}

	counter_file = anon_inode_getfile("[lttng_counter]",
				       &lttng_counter_fops,
				       NULL, O_RDONLY);
	if (IS_ERR(counter_file)) {
		ret = PTR_ERR(counter_file);
		goto file_error;
	}

	if (!lttng_file_ref_get(session->priv->file)) {
		ret = -EOVERFLOW;
		goto refcount_error;
	}

	chan_counter = lttng_kernel_counter_create(counter_transport_name, 1, counter_conf->dimension_array, 0,
				counter_conf->flags & LTTNG_KERNEL_COUNTER_CONF_FLAG_COALESCE_HITS);
	if (!chan_counter) {
		ret = -EINVAL;
		goto create_error;
	}

	chan_counter->priv->parent.file = counter_file;
	chan_counter->priv->owner = session->priv->file;
	chan_counter->parent.session = session;
	list_add(&chan_counter->priv->parent.node, &session->priv->chan_head);
	counter_file->private_data = chan_counter;

	fd_install(counter_fd, counter_file);
	lttng_unlock_sessions();

	return counter_fd;

create_error:
	if (!lttng_file_ref_put(session->priv->file)) {
		/* Don't change return  code */
	}
refcount_error:
	fput(counter_file);
file_error:
	put_unused_fd(counter_fd);
fd_error:
	lttng_unlock_sessions();
	return ret;
}

static
long lttng_abi_event_notifier_group_create_error_counter(
		struct file *event_notifier_group_file,
		const struct lttng_kernel_counter_conf *counter_conf)
{
	int counter_fd, ret;
	char *counter_transport_name;
	struct lttng_kernel_channel_counter *chan_counter = NULL;
	struct file *counter_file;
	struct lttng_event_notifier_group *event_notifier_group =
			(struct lttng_event_notifier_group *) event_notifier_group_file->private_data;
	size_t counter_len;

	if (counter_conf->arithmetic != LTTNG_KERNEL_COUNTER_ARITHMETIC_MODULAR) {
		printk(KERN_ERR "LTTng: event_notifier: Error counter of the wrong arithmetic type.\n");
		return -EINVAL;
	}

	switch (counter_conf->bitness) {
	case LTTNG_KERNEL_COUNTER_BITNESS_64:
		counter_transport_name = "counter-per-cpu-64-modular";
		break;
	case LTTNG_KERNEL_COUNTER_BITNESS_32:
		counter_transport_name = "counter-per-cpu-32-modular";
		break;
	default:
		return -EINVAL;
	}

	/*
	 * Lock sessions to provide mutual exclusion against concurrent
	 * modification of event_notifier group, which would result in
	 * overwriting the error counter if set concurrently.
	 */
	lttng_lock_sessions();

	if (event_notifier_group->error_counter) {
		printk(KERN_ERR "Error counter already created in event_notifier group\n");
		ret = -EBUSY;
		goto fd_error;
	}

	counter_fd = get_unused_fd_flags(0);
	if (counter_fd < 0) {
		ret = counter_fd;
		goto fd_error;
	}

	counter_file = anon_inode_getfile("[lttng_counter]",
				       &lttng_counter_fops,
				       NULL, O_RDONLY);
	if (IS_ERR(counter_file)) {
		ret = PTR_ERR(counter_file);
		goto file_error;
	}

	if (!lttng_file_ref_get(event_notifier_group_file)) {
		ret = -EOVERFLOW;
		goto refcount_error;
	}

	counter_len = counter_conf->dimension_array[0].size;
	chan_counter = lttng_kernel_counter_create(counter_transport_name, 1, counter_conf->dimension_array, 0, false);
	if (!chan_counter) {
		ret = -EINVAL;
		goto create_error;
	}

	chan_counter->priv->parent.file = counter_file;
	chan_counter->priv->owner = event_notifier_group->file;
	counter_file->private_data = chan_counter;
	event_notifier_group->error_counter_len = counter_len;
	/*
	 * store-release to publish error counter matches load-acquire
	 * in record_error. Ensures the counter is created and the
	 * error_counter_len is set before they are used.
	 */
	smp_store_release(&event_notifier_group->error_counter,
				chan_counter);

	fd_install(counter_fd, counter_file);
	lttng_unlock_sessions();

	return counter_fd;

create_error:
	if (!lttng_file_ref_put(event_notifier_group_file)) {
		/* Don't change return code */
	}
refcount_error:
	fput(counter_file);
file_error:
	put_unused_fd(counter_fd);
fd_error:
	lttng_unlock_sessions();
	return ret;
}

static
long lttng_event_notifier_group_ioctl(struct file *file, unsigned int cmd,
		unsigned long arg)
{
	int ret;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_EVENT_NOTIFIER_GROUP_NOTIFICATION_FD:
	{
		return lttng_abi_open_event_notifier_group_stream(file);
	}
	case LTTNG_KERNEL_ABI_EVENT_NOTIFIER_CREATE:
	{
		struct lttng_kernel_abi_event_notifier uevent_notifier_param;
		struct lttng_kernel_abi_event_ext uevent_param_ext = {};
		int ret;

		if (copy_from_user(&uevent_notifier_param,
				(struct lttng_kernel_abi_event_notifier __user *) arg,
				sizeof(uevent_notifier_param)))
			return -EFAULT;
		ret = copy_user_event_param_ext(&uevent_param_ext, &uevent_notifier_param.event);
		if (ret)
			return ret;
		return lttng_abi_create_event_notifier(file, &uevent_notifier_param, &uevent_param_ext);
	}
	case LTTNG_KERNEL_ABI_OLD_COUNTER:
	{
		struct lttng_kernel_counter_conf counter_conf;

		ret = lttng_abi_copy_user_old_counter_conf(&counter_conf,
				(struct lttng_kernel_abi_old_counter_conf __user *) arg);
		if (ret)
			return ret;
		return lttng_abi_event_notifier_group_create_error_counter(file, &counter_conf);
	}
	case LTTNG_KERNEL_ABI_COUNTER:
	{
		struct lttng_kernel_counter_conf counter_conf;

		ret = lttng_abi_copy_user_counter_conf(&counter_conf,
				(struct lttng_kernel_abi_counter_conf __user *) arg);
		if (ret)
			return ret;
		return lttng_abi_event_notifier_group_create_error_counter(file, &counter_conf);
	}
	default:
		return -ENOIOCTLCMD;
	}
	return 0;
}

static
int lttng_event_notifier_group_release(struct inode *inode, struct file *file)
{
	struct lttng_event_notifier_group *event_notifier_group =
			file->private_data;

	if (event_notifier_group)
		lttng_event_notifier_group_destroy(event_notifier_group);
	return 0;
}

static const struct file_operations lttng_event_notifier_group_fops = {
	.owner = THIS_MODULE,
	.release = lttng_event_notifier_group_release,
	.unlocked_ioctl = lttng_event_notifier_group_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_event_notifier_group_ioctl,
#endif
};

/**
 *	lttng_channel_ioctl - lttng syscall through ioctl
 *
 *	@file: the file
 *	@cmd: the command
 *	@arg: command arg
 *
 *	This ioctl implements lttng commands:
 *      LTTNG_KERNEL_ABI_STREAM
 *              Returns an event stream file descriptor or failure.
 *              (typically, one event stream records events from one CPU)
 *	LTTNG_KERNEL_ABI_EVENT
 *		Returns an event file descriptor or failure.
 *	LTTNG_KERNEL_ABI_CONTEXT
 *		Prepend a context field to each event in the channel
 *	LTTNG_KERNEL_ABI_ENABLE
 *		Enable recording for events in this channel (weak enable)
 *	LTTNG_KERNEL_ABI_DISABLE
 *		Disable recording for events in this channel (strong disable)
 *
 * Channel and event file descriptors also hold a reference on the session.
 */
static
long lttng_channel_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct lttng_kernel_channel_buffer *channel = file->private_data;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_OLD_STREAM:
	case LTTNG_KERNEL_ABI_STREAM:
		return lttng_abi_open_stream(file);
	case LTTNG_KERNEL_ABI_OLD_EVENT:
	{
		struct lttng_kernel_abi_event *uevent_param;
		struct lttng_kernel_abi_old_event *old_uevent_param;
		int ret;

		uevent_param = kmalloc(sizeof(struct lttng_kernel_abi_event),
				GFP_KERNEL);
		if (!uevent_param) {
			ret = -ENOMEM;
			goto old_event_end;
		}
		old_uevent_param = kmalloc(
				sizeof(struct lttng_kernel_abi_old_event),
				GFP_KERNEL);
		if (!old_uevent_param) {
			ret = -ENOMEM;
			goto old_event_error_free_param;
		}
		if (copy_from_user(old_uevent_param,
				(struct lttng_kernel_abi_old_event __user *) arg,
				sizeof(struct lttng_kernel_abi_old_event))) {
			ret = -EFAULT;
			goto old_event_error_free_old_param;
		}

		memcpy(uevent_param->name, old_uevent_param->name,
				sizeof(uevent_param->name));
		uevent_param->instrumentation =
			old_uevent_param->instrumentation;

		switch (old_uevent_param->instrumentation) {
		case LTTNG_KERNEL_ABI_KPROBE:
			uevent_param->u.kprobe.addr =
				old_uevent_param->u.kprobe.addr;
			uevent_param->u.kprobe.offset =
				old_uevent_param->u.kprobe.offset;
			memcpy(uevent_param->u.kprobe.symbol_name,
				old_uevent_param->u.kprobe.symbol_name,
				sizeof(uevent_param->u.kprobe.symbol_name));
			break;
		case LTTNG_KERNEL_ABI_KRETPROBE:
			uevent_param->u.kretprobe.addr =
				old_uevent_param->u.kretprobe.addr;
			uevent_param->u.kretprobe.offset =
				old_uevent_param->u.kretprobe.offset;
			memcpy(uevent_param->u.kretprobe.symbol_name,
				old_uevent_param->u.kretprobe.symbol_name,
				sizeof(uevent_param->u.kretprobe.symbol_name));
			break;
		case LTTNG_KERNEL_ABI_FUNCTION:
			WARN_ON_ONCE(1);
			/* Not implemented. */
			break;
		default:
			break;
		}
		ret = lttng_abi_create_event_recorder_enabler(file, uevent_param, NULL);

old_event_error_free_old_param:
		kfree(old_uevent_param);
old_event_error_free_param:
		kfree(uevent_param);
old_event_end:
		return ret;
	}
	case LTTNG_KERNEL_ABI_EVENT:
	{
		struct lttng_kernel_abi_event uevent_param;
		struct lttng_kernel_abi_event_ext uevent_param_ext = {};
		int ret;

		if (copy_from_user(&uevent_param,
				(struct lttng_kernel_abi_event __user *) arg,
				sizeof(uevent_param)))
			return -EFAULT;
		ret = copy_user_event_param_ext(&uevent_param_ext, &uevent_param);
		if (ret)
			return ret;
		return lttng_abi_create_event_recorder_enabler(file, &uevent_param, &uevent_param_ext);
	}
	case LTTNG_KERNEL_ABI_OLD_CONTEXT:
	{
		struct lttng_kernel_abi_context *ucontext_param;
		struct lttng_kernel_abi_old_context *old_ucontext_param;
		int ret;

		ucontext_param = kmalloc(sizeof(struct lttng_kernel_abi_context),
				GFP_KERNEL);
		if (!ucontext_param) {
			ret = -ENOMEM;
			goto old_ctx_end;
		}
		old_ucontext_param = kmalloc(sizeof(struct lttng_kernel_abi_old_context),
				GFP_KERNEL);
		if (!old_ucontext_param) {
			ret = -ENOMEM;
			goto old_ctx_error_free_param;
		}

		if (copy_from_user(old_ucontext_param,
				(struct lttng_kernel_abi_old_context __user *) arg,
				sizeof(struct lttng_kernel_abi_old_context))) {
			ret = -EFAULT;
			goto old_ctx_error_free_old_param;
		}
		ucontext_param->ctx = old_ucontext_param->ctx;
		memcpy(ucontext_param->padding, old_ucontext_param->padding,
				sizeof(ucontext_param->padding));
		/* only type that uses the union */
		if (old_ucontext_param->ctx == LTTNG_KERNEL_ABI_CONTEXT_PERF_COUNTER) {
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
				&channel->priv->ctx, channel->parent.session);

old_ctx_error_free_old_param:
		kfree(old_ucontext_param);
old_ctx_error_free_param:
		kfree(ucontext_param);
old_ctx_end:
		return ret;
	}
	case LTTNG_KERNEL_ABI_CONTEXT:
	{
		struct lttng_kernel_abi_context ucontext_param;

		if (copy_from_user(&ucontext_param,
				(struct lttng_kernel_abi_context __user *) arg,
				sizeof(ucontext_param)))
			return -EFAULT;
		return lttng_abi_add_context(file,
				&ucontext_param,
				&channel->priv->ctx, channel->parent.session);
	}
	case LTTNG_KERNEL_ABI_OLD_ENABLE:
	case LTTNG_KERNEL_ABI_ENABLE:
		return lttng_channel_enable(&channel->parent);
	case LTTNG_KERNEL_ABI_OLD_DISABLE:
	case LTTNG_KERNEL_ABI_DISABLE:
		return lttng_channel_disable(&channel->parent);
	case LTTNG_KERNEL_ABI_SYSCALL_MASK:
		return lttng_syscall_table_get_active_mask(&channel->priv->parent.syscall_table,
			(struct lttng_kernel_abi_syscall_mask __user *) arg);
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
 *      LTTNG_KERNEL_ABI_STREAM
 *              Returns an event stream file descriptor or failure.
 *
 * Channel and event file descriptors also hold a reference on the session.
 */
static
long lttng_metadata_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case LTTNG_KERNEL_ABI_OLD_STREAM:
	case LTTNG_KERNEL_ABI_STREAM:
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
static
unsigned int lttng_channel_poll(struct file *file, poll_table *wait)
{
	struct lttng_kernel_channel_buffer *channel = file->private_data;
	unsigned int mask = 0;

	if (file->f_mode & FMODE_READ) {
		poll_wait_set_exclusive(wait);
		poll_wait(file, channel->ops->priv->get_hp_wait_queue(channel->priv->rb_chan),
			  wait);

		if (channel->ops->priv->is_disabled(channel->priv->rb_chan))
			return POLLERR;
		if (channel->ops->priv->is_finalized(channel->priv->rb_chan))
			return POLLHUP;
		if (channel->ops->priv->buffer_has_read_closed_stream(channel->priv->rb_chan))
			return POLLIN | POLLRDNORM;
		return 0;
	}
	return mask;

}

static
int lttng_channel_release(struct inode *inode, struct file *file)
{
	struct lttng_kernel_channel_buffer *channel = file->private_data;

	if (channel)
		fput(channel->parent.session->priv->file);
	return 0;
}

static
int lttng_metadata_channel_release(struct inode *inode, struct file *file)
{
	struct lttng_kernel_channel_buffer *channel = file->private_data;

	if (channel) {
		fput(channel->parent.session->priv->file);
		lttng_metadata_channel_buffer_destroy(channel);
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
 *	LTTNG_KERNEL_ABI_CONTEXT
 *		Prepend a context field to each record of this event
 *	LTTNG_KERNEL_ABI_ENABLE
 *		Enable recording for this event (weak enable)
 *	LTTNG_KERNEL_ABI_DISABLE
 *		Disable recording for this event (strong disable)
 */
static
long lttng_event_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct lttng_kernel_event_common *event = file->private_data;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_OLD_CONTEXT:
	{
		/* Not implemented */
		return -ENOSYS;
	}
	case LTTNG_KERNEL_ABI_CONTEXT:
	{
		/* Not implemented */
		return -ENOSYS;
	}
	case LTTNG_KERNEL_ABI_OLD_ENABLE:
	case LTTNG_KERNEL_ABI_ENABLE:
		return lttng_event_enable(event);
	case LTTNG_KERNEL_ABI_OLD_DISABLE:
	case LTTNG_KERNEL_ABI_DISABLE:
		return lttng_event_disable(event);
	case LTTNG_KERNEL_ABI_FILTER:
		return -EINVAL;
	case LTTNG_KERNEL_ABI_ADD_CALLSITE:
		return lttng_event_add_callsite(event,
			(struct lttng_kernel_abi_event_callsite __user *) arg);
	default:
		return -ENOIOCTLCMD;
	}
}

/**
 *	lttng_event_recorder_enabler_ioctl - lttng syscall through ioctl
 *
 *	@file: the file
 *	@cmd: the command
 *	@arg: command arg
 *
 *	This ioctl implements lttng commands:
 *	LTTNG_KERNEL_ABI_CONTEXT
 *		Prepend a context field to each record of this event
 *	LTTNG_KERNEL_ABI_ENABLE
 *		Enable recording for this event (weak enable)
 *	LTTNG_KERNEL_ABI_DISABLE
 *		Disable recording for this event (strong disable)
 */
static
long lttng_event_enabler_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct lttng_event_enabler_common *event_enabler = file->private_data;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_OLD_CONTEXT:
	{
		/* Not implemented */
		return -ENOSYS;
	}
	case LTTNG_KERNEL_ABI_CONTEXT:
	{
		/* Not implemented */
		return -ENOSYS;
	}
	case LTTNG_KERNEL_ABI_OLD_ENABLE:
	case LTTNG_KERNEL_ABI_ENABLE:
		return lttng_event_enabler_enable(event_enabler);
	case LTTNG_KERNEL_ABI_OLD_DISABLE:
	case LTTNG_KERNEL_ABI_DISABLE:
		return lttng_event_enabler_disable(event_enabler);
	case LTTNG_KERNEL_ABI_FILTER:
		/*
		 * Filters are only supported by tracepoint and syscall instrumentation.
		 */
		switch (event_enabler->event_param.instrumentation) {
		case LTTNG_KERNEL_ABI_TRACEPOINT:
			lttng_fallthrough;
		case LTTNG_KERNEL_ABI_SYSCALL:
			return lttng_event_enabler_attach_filter_bytecode(event_enabler,
				(struct lttng_kernel_abi_filter_bytecode __user *) arg);

		default:
			return -EINVAL;
		}
	case LTTNG_KERNEL_ABI_ADD_CALLSITE:
		return -EINVAL;
	default:
		return -ENOIOCTLCMD;
	}
}

static
int lttng_event_session_release(struct inode *inode, struct file *file)
{
	struct lttng_kernel_event_common *event = file->private_data;

	if (event) {
		struct lttng_kernel_event_session_common_private *event_session_priv =
			container_of(event->priv, struct lttng_kernel_event_session_common_private, parent);
		fput(event_session_priv->chan->priv->file);
	}
	return 0;
}

static
int lttng_event_session_enabler_release(struct inode *inode, struct file *file)
{
	struct lttng_event_enabler_common *event_enabler = file->private_data;

	if (event_enabler) {
		struct lttng_event_enabler_session_common *event_session_enabler =
			container_of(event_enabler, struct lttng_event_enabler_session_common, parent);
		fput(event_session_enabler->chan->priv->file);
	}
	return 0;
}

static const struct file_operations lttng_event_session_fops = {
	.owner = THIS_MODULE,
	.release = lttng_event_session_release,
	.unlocked_ioctl = lttng_event_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_event_ioctl,
#endif
};

static const struct file_operations lttng_event_session_enabler_fops = {
	.owner = THIS_MODULE,
	.release = lttng_event_session_enabler_release,
	.unlocked_ioctl = lttng_event_enabler_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = lttng_event_enabler_ioctl,
#endif
};

static int put_u64(uint64_t val, unsigned long arg)
{
	return put_user(val, (uint64_t __user *) arg);
}

static int put_u32(uint32_t val, unsigned long arg)
{
	return put_user(val, (uint32_t __user *) arg);
}

static long lttng_stream_ring_buffer_ioctl(struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	struct lttng_kernel_ring_buffer *buf = filp->private_data;
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	const struct lttng_kernel_channel_buffer_ops *ops = chan->backend.priv_ops;
	int ret;

	if (atomic_read(&chan->record_disabled))
		return -EIO;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_TIMESTAMP_BEGIN:
	{
		uint64_t ts;

		ret = ops->priv->timestamp_begin(config, buf, &ts);
		if (ret < 0)
			goto error;
		return put_u64(ts, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_TIMESTAMP_END:
	{
		uint64_t ts;

		ret = ops->priv->timestamp_end(config, buf, &ts);
		if (ret < 0)
			goto error;
		return put_u64(ts, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_EVENTS_DISCARDED:
	{
		uint64_t ed;

		ret = ops->priv->events_discarded(config, buf, &ed);
		if (ret < 0)
			goto error;
		return put_u64(ed, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_CONTENT_SIZE:
	{
		uint64_t cs;

		ret = ops->priv->content_size(config, buf, &cs);
		if (ret < 0)
			goto error;
		return put_u64(cs, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_PACKET_SIZE:
	{
		uint64_t ps;

		ret = ops->priv->packet_size(config, buf, &ps);
		if (ret < 0)
			goto error;
		return put_u64(ps, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_STREAM_ID:
	{
		uint64_t si;

		ret = ops->priv->stream_id(config, buf, &si);
		if (ret < 0)
			goto error;
		return put_u64(si, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_CURRENT_TIMESTAMP:
	{
		uint64_t ts;

		ret = ops->priv->current_timestamp(config, buf, &ts);
		if (ret < 0)
			goto error;
		return put_u64(ts, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_SEQ_NUM:
	{
		uint64_t seq;

		ret = ops->priv->sequence_number(config, buf, &seq);
		if (ret < 0)
			goto error;
		return put_u64(seq, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_INSTANCE_ID:
	{
		uint64_t id;

		ret = ops->priv->instance_id(config, buf, &id);
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
	struct lttng_kernel_ring_buffer *buf = filp->private_data;
	struct lttng_kernel_ring_buffer_channel *chan = buf->backend.chan;
	const struct lttng_kernel_ring_buffer_config *config = &chan->backend.config;
	const struct lttng_kernel_channel_buffer_ops *ops = chan->backend.priv_ops;
	int ret;

	if (atomic_read(&chan->record_disabled))
		return -EIO;

	switch (cmd) {
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_TIMESTAMP_BEGIN:
	{
		uint64_t ts;

		ret = ops->priv->timestamp_begin(config, buf, &ts);
		if (ret < 0)
			goto error;
		return put_u64(ts, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_TIMESTAMP_END:
	{
		uint64_t ts;

		ret = ops->priv->timestamp_end(config, buf, &ts);
		if (ret < 0)
			goto error;
		return put_u64(ts, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_EVENTS_DISCARDED:
	{
		uint64_t ed;

		ret = ops->priv->events_discarded(config, buf, &ed);
		if (ret < 0)
			goto error;
		return put_u64(ed, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_CONTENT_SIZE:
	{
		uint64_t cs;

		ret = ops->priv->content_size(config, buf, &cs);
		if (ret < 0)
			goto error;
		return put_u64(cs, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_PACKET_SIZE:
	{
		uint64_t ps;

		ret = ops->priv->packet_size(config, buf, &ps);
		if (ret < 0)
			goto error;
		return put_u64(ps, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_STREAM_ID:
	{
		uint64_t si;

		ret = ops->priv->stream_id(config, buf, &si);
		if (ret < 0)
			goto error;
		return put_u64(si, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_GET_CURRENT_TIMESTAMP:
	{
		uint64_t ts;

		ret = ops->priv->current_timestamp(config, buf, &ts);
		if (ret < 0)
			goto error;
		return put_u64(ts, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_GET_SEQ_NUM:
	{
		uint64_t seq;

		ret = ops->priv->sequence_number(config, buf, &seq);
		if (ret < 0)
			goto error;
		return put_u64(seq, arg);
	}
	case LTTNG_KERNEL_ABI_RING_BUFFER_COMPAT_INSTANCE_ID:
	{
		uint64_t id;

		ret = ops->priv->instance_id(config, buf, &id);
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

	wrapper_vmalloc_sync_mappings();
	lttng_clock_ref();

	ret = lttng_tp_mempool_init();
	if (ret) {
		goto error;
	}

	lttng_proc_dentry = proc_create_data("lttng", S_IRUSR | S_IWUSR, NULL,
					&lttng_proc_ops, NULL);

	if (!lttng_proc_dentry) {
		printk(KERN_ERR "LTTng: Error creating control file\n");
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
