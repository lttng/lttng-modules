/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * wrapper/atomic.h
 *
 * wrapper around linux/atomic.h.
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_ATOMIC_H
#define _LTTNG_WRAPPER_ATOMIC_H

#include <lttng/kernel-version.h>
#include <linux/atomic.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,16,0))
static inline void lttng_smp_mb__before_atomic(void)
{
	smp_mb__before_atomic();
}

static inline void lttng_smp_mb__after_atomic(void)
{
	smp_mb__after_atomic();
}
#else /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,16,0)) */
static inline void lttng_smp_mb__before_atomic(void)
{
	smp_mb__before_atomic_inc();
}

static inline void lttng_smp_mb__after_atomic(void)
{
	smp_mb__after_atomic_inc();
}
#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,16,0)) */

#endif /* _LTTNG_WRAPPER_ATOMIC_H */
