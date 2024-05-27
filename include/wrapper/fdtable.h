/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/fdtable.h
 *
 * Copyright (C) 2013 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_FDTABLE_H
#define _LTTNG_WRAPPER_FDTABLE_H

#include <lttng/kernel-version.h>
#include <linux/fdtable.h>
#include <linux/sched.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,7,0))
static inline
struct file *lttng_lookup_fdget_rcu(unsigned int fd)
{
	return lookup_fdget_rcu(fd);
}

#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,11,0))
static inline
struct file *lttng_lookup_fdget_rcu(unsigned int fd)
{
	struct file* file = lookup_fd_rcu(fd);

	if (unlikely(!file || !get_file_rcu(file)))
		return NULL;
	return file;
}
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,1,0))
static inline
struct file *lttng_lookup_fdget_rcu(unsigned int fd)
{
	struct file* file = fcheck(fd);

	if (unlikely(!file || !get_file_rcu(file)))
		return NULL;
	return file;
}
#else
static inline
struct file *lttng_lookup_fdget_rcu(unsigned int fd)
{
	struct file* file = fcheck(fd);

	if (unlikely(!file || !atomic_long_inc_not_zero(&file->f_count)))
		return NULL;
	return file;
}
#endif

#if (LTTNG_LINUX_VERSION_CODE < LTTNG_KERNEL_VERSION(3,7,0))

int lttng_iterate_fd(struct files_struct *files,
		unsigned int first,
		int (*cb)(const void *, struct file *, unsigned int),
		const void *ctx);

#else

/*
 * iterate_fd() appeared at commit
 * c3c073f808b22dfae15ef8412b6f7b998644139a in the Linux kernel (first
 * released kernel: v3.7).
 */
#define lttng_iterate_fd	iterate_fd

#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,10,0))
static inline
bool lttng_close_on_exec(unsigned int fd, const struct files_struct *files)
{
	return close_on_exec(fd, files);
}

#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,4,0))

static inline
bool lttng_close_on_exec(unsigned int fd, const struct files_struct *files)
{
	return close_on_exec(fd, files_fdtable(files));
}

#else

static inline
bool lttng_close_on_exec(unsigned int fd, const struct files_struct *files)
{
	return FD_ISSET(fd, files_fdtable(files)->close_on_exec);
}

#endif

#endif /* _LTTNG_WRAPPER_FDTABLE_H */
