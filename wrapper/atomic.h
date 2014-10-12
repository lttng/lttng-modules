#ifndef _LTTNG_WRAPPER_ATOMIC_H
#define _LTTNG_WRAPPER_ATOMIC_H

/*
 * wrapper/atomic.h
 *
 * wrapper around linux/atomic.h.
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <linux/version.h>
#include <linux/atomic.h>

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
