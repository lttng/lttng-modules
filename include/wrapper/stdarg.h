// SPDX-FileCopyrightText: 2025 Philippe Proulx <pproulx@efficios.com>
// SPDX-License-Identifier: GPL-2.0-only

#ifndef _LTTNG_WRAPPER_STDARG_H
#define _LTTNG_WRAPPER_STDARG_H

#include <lttng/kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,16,0) \
	|| LTTNG_RHEL_KERNEL_RANGE(5,14,0,162,0,0, 5,15,0,0,0,0))
#include <linux/stdarg.h>
#else
#include <stdarg.h>
#endif

#endif /* _LTTNG_WRAPPER_STDARG_H */
