/* SPDX-License-Identifier: (GPL-2.0 OR LGPL-2.1)
 *
 * wrapper/atomic.h
 *
 * wrapper around linux/atomic.h.
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_ATOMIC_H
#define _LTTNG_WRAPPER_ATOMIC_H

#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
#include <linux/atomic.h>
#else
#include <asm/atomic.h>
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0))
static inline void lttng_smp_mb__before_atomic(void)
{
	smp_mb__before_atomic();
}

static inline void lttng_smp_mb__after_atomic(void)
{
	smp_mb__after_atomic();
}
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)) */
static inline void lttng_smp_mb__before_atomic(void)
{
	smp_mb__before_atomic_inc();
}

static inline void lttng_smp_mb__after_atomic(void)
{
	smp_mb__after_atomic_inc();
}
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)) */

#endif /* _LTTNG_WRAPPER_ATOMIC_H */
