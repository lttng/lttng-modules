/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/fs.h
 *
 * Copyright (C) 2020 Michael Jeanson <mjeanson@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_FS_H
#define _LTTNG_WRAPPER_FS_H

#include <linux/fs.h>
#include <lttng-kernel-version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0))

static inline
ssize_t lttng_kernel_read(struct file *file, void *buf, size_t count, loff_t *pos)
{
	return kernel_read(file, buf, count, pos);
}

#else /* LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0) */

static inline
ssize_t lttng_kernel_read(struct file *file, void *buf, size_t count, loff_t *pos)
{
	ssize_t len;

	len = kernel_read(file, *pos, buf, count);

	/*
	 * Move 'pos' forward since it's passed by value in this
	 * implementation of 'kernel_read'.
	 */
	if (len > 0)
		(*pos) += len;

	return len;
}

#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0) */

#endif /* _LTTNG_WRAPPER_FS_H */
