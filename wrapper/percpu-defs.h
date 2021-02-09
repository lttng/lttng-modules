/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/percpu-defs.h
 *
 * wrapper around linux/percpu-defs.h.
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_PERCPU_DEFS_H
#define _LTTNG_WRAPPER_PERCPU_DEFS_H

#include <lttng-kernel-version.h>

#if LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,19,0)

#include <linux/percpu-defs.h>

#define lttng_this_cpu_ptr(ptr)		this_cpu_ptr(ptr)

#else /* #if LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,19,0) */

#include <linux/percpu.h>

#define lttng_this_cpu_ptr(ptr)		(&__get_cpu_var(*(ptr)))

#endif /* #else #if LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,19,0) */

#endif /* _LTTNG_WRAPPER_PERCPU_DEFS_H */
