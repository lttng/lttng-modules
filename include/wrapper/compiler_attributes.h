/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/compiler_attributes.h
 *
 * Copyright (C) 2021 Michael Jeanson <mjeanson@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_COMPILER_ATTRIBUTES_H
#define _LTTNG_WRAPPER_COMPILER_ATTRIBUTES_H

#include <lttng/kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,20,0))
#include <linux/compiler_attributes.h>
#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,4,0))

/*
 * Use the kernel provided fallthrough attribute macro.
 */
#define lttng_fallthrough fallthrough

#else /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,4,0) */

/*
 * Fallback to the comment for kernels pre 5.15 that don't build with
 * '-Wimplicit-fallthrough=5'.
 */
#define lttng_fallthrough do {} while (0)  /* fallthrough */

#endif /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,4,0) */

#endif /* _LTTNG_WRAPPER_COMPILER_ATTRIBUTES_H */
