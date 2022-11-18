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

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,11,0))
static inline
struct file *lttng_lookup_fd_rcu(unsigned int fd)
{
	return lookup_fd_rcu(fd);
}
#else
static inline
struct file *lttng_lookup_fd_rcu(unsigned int fd)
{
	return fcheck(fd);
}
#endif

#endif /* _LTTNG_WRAPPER_FDTABLE_H */
