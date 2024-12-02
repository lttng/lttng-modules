/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/fdtable.h
 *
 * Copyright (C) 2013 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_FDTABLE_H
#define _LTTNG_WRAPPER_FDTABLE_H

#include <lttng/kernel-version.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/sched.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,13,0))
static inline
struct file *lttng_lookup_fdget_rcu(unsigned int fd)
{
	return fget_raw(fd);
}
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,7,0))
static inline
struct file *lttng_lookup_fdget_rcu(unsigned int fd)
{
	return lookup_fdget_rcu(fd);
}

#elif ((LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,11,0)) || \
	LTTNG_KERNEL_RANGE(5,10,220, 5,11,0))
static inline
struct file *lttng_lookup_fdget_rcu(unsigned int fd)
{
	struct file* file = lookup_fd_rcu(fd);

	if (unlikely(!file || !get_file_rcu(file)))
		return NULL;
	return file;
}
#else
static inline
struct file *lttng_lookup_fdget_rcu(unsigned int fd)
{
	struct file* file = fcheck(fd);

	if (unlikely(!file || !get_file_rcu(file)))
		return NULL;
	return file;
}
#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,10,0))
static inline
bool lttng_close_on_exec(unsigned int fd, const struct files_struct *files)
{
	return close_on_exec(fd, files);
}
#else
static inline
bool lttng_close_on_exec(unsigned int fd, const struct files_struct *files)
{
	return close_on_exec(fd, files_fdtable(files));
}
#endif

#endif /* _LTTNG_WRAPPER_FDTABLE_H */
