/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/time.h
 *
 * Copyright (C) 2020 Michael Jeanson <mjeanson@efficios.com>
 * Copyright (C) 2020 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_TIME_H
#define _LTTNG_WRAPPER_TIME_H

#include <linux/version.h>

/*
 * Use 64bit timespec on kernels that have it, this makes 32bit arch
 * y2038 compliant.
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0))
# define LTTNG_KERNEL_HAS_TIMESPEC64
#endif

#endif /* _LTTNG_WRAPPER_TIME_H */
