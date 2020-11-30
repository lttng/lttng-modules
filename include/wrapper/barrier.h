/* SPDX-License-Identifier: GPL-2.0-only
 *
 * wrapper/barrier.h
 *
 * wrapper around asm/barrier.h.
 *
 * Copyright (C) 2020 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_BARRIER_H
#define _LTTNG_WRAPPER_BARRIER_H

#include <linux/version.h>
#include <asm/barrier.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)

#define lttng_smp_store_release(x, v) smp_store_release(x, v)
#define lttng_smp_load_acquire(x) smp_load_acquire(x)

#else

/*
 * Acquire-release semantics act as a one-way permeable barrier when
 * pairing a store with a release. Use a full memory barrier to emulate
 * the acquire-release semantic with a stronger barrier on older
 * kernels.
 */

#define lttng_smp_store_release(x, v)		\
	do {					\
		smp_mb();			\
		ACCESS_ONCE(*(x)) = (v);	\
	} while (0)

#define lttng_smp_load_acquire(x)		\
	({					\
		__typeof__(*(x)) ___ret;	\
						\
		___ret = ACCESS_ONCE(*(x));	\
		smp_mb();			\
		___ret;				\
	})

#endif

#endif /* _LTTNG_WRAPPER_BARRIER_H */
