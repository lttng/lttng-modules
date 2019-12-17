/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/mm.h
 *
 * Copyright (C) 2018 Francis Deslauriers <francis.deslauriers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_MM_H
#define _LTTNG_WRAPPER_MM_H

#include <linux/mm.h>
#include <linux/oom.h>

#include <lttng-kernel-version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,6,0) \
		|| LTTNG_UBUNTU_KERNEL_RANGE(4,4,25,44, 4,5,0,0))

/*
 * Returns true if the current estimation of the number of page available is
 * larger than the number of pages passed as parameter.
 */
static inline
bool wrapper_check_enough_free_pages(unsigned long num_pages)
{
	return num_pages < si_mem_available();
}

#else

static inline
bool wrapper_check_enough_free_pages(unsigned long num_pages)
{
	/*
	 * The si_mem_available function is not available on this kernel. Since
	 * we can't reliably know if there is enough memory available, so we
	 * return true.
	 */
	return true;
}
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
static inline
void wrapper_set_current_oom_origin(void)
{
	return set_current_oom_origin();
}

static inline
void wrapper_clear_current_oom_origin(void)
{
	return clear_current_oom_origin();
}

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)) */

static inline
void wrapper_set_current_oom_origin(void)
{
	return;
}

static inline
void wrapper_clear_current_oom_origin(void)
{
	return;
}
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)) */
#endif /* _LTTNG_WRAPPER_MM_H */
