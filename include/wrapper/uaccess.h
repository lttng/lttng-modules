/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/uaccess.h
 *
 * wrapper around linux/uaccess.h.
 *
 * Copyright (C) 2019 Michael Jeanson <mjeanson@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_UACCESS_H
#define _LTTNG_WRAPPER_UACCESS_H

#include <linux/uaccess.h>
#include <lttng/kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,0,0) || \
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,147,0,0, 4,19,0,0,0,0))

#define VERIFY_READ	0
#define VERIFY_WRITE	1
#define lttng_access_ok(type, addr, size) access_ok(addr, size)

#else /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,0,0) */

#define lttng_access_ok(type, addr, size) access_ok(type, addr, size)

#endif /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,0,0) */

#endif /* _LTTNG_WRAPPER_UACCESS_H */
