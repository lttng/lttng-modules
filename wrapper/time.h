/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/time.h
 *
 * Copyright (C) 2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_TIME_H
#define _LTTNG_WRAPPER_TIME_H

#include <linux/version.h>
#include <linux/time.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))

static inline bool lttng_close_on_exec(int fd, const struct fdtable *fdt)
{
	return close_on_exec(fd, fdt);
}

#else

static inline bool lttng_close_on_exec(int fd, const struct fdtable *fdt)
{
	return FD_ISSET(fd, fdt->close_on_exec);
}

#endif

#endif /* _LTTNG_WRAPPER_TIME_H */
