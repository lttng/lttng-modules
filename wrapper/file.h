/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/file.h
 *
 * wrapper around linux/file.h.
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_FILE_H
#define _LTTNG_WRAPPER_FILE_H

#include <linux/version.h>
#include <linux/file.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)

static
inline int lttng_get_unused_fd(void)
{
	return get_unused_fd_flags(0);
}

#define lttng_f_dentry	f_path.dentry

#else /* #if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0) */

static
inline int lttng_get_unused_fd(void)
{
	return get_unused_fd();
}

#define lttng_f_dentry	f_dentry

#endif /* #else #if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0) */

#endif /* _LTTNG_WRAPPER_FILE_H */
