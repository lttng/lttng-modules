/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/cpu.h
 *
 * Copyright (C) 2021 Michael Jeanson <mjeanson@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_CPU_H
#define _LTTNG_WRAPPER_CPU_H

#include <linux/cpu.h>
#include <lttng-kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,13,0))

static inline
void lttng_cpus_read_lock(void)
{
	cpus_read_lock();
}

static inline
void lttng_cpus_read_unlock(void)
{
	cpus_read_unlock();
}

#else /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,13,0) */

static inline
void lttng_cpus_read_lock(void)
{
	get_online_cpus();
}

static inline
void lttng_cpus_read_unlock(void)
{
	put_online_cpus();
}

#endif /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,13,0) */

#endif /* _LTTNG_WRAPPER_CPU_H */
