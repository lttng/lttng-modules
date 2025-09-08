/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/page_alloc.h
 *
 * wrapper around get_pfnblock_flags_mask. Using KALLSYMS to get its address
 * when available, else we need to have a kernel that exports this function to
 * GPL modules.
 *
 * Copyright (C) 2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_PAGE_ALLOC_H
#define _LTTNG_WRAPPER_PAGE_ALLOC_H

#include <lttng/kernel-version.h>

/*
 * We need to redefine get_pfnblock_flags_mask to our wrapper, because
 * the get_pageblock_migratetype() macro uses it.
 */
#if (defined(CONFIG_KALLSYMS) \
	&& (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,19,2)	\
		|| LTTNG_KERNEL_RANGE(3,14,36, 3,15,0)		\
		|| LTTNG_KERNEL_RANGE(3,18,10, 3,19,0)		\
		|| LTTNG_DEBIAN_KERNEL_RANGE(3,16,7,9,0,0, 3,17,0,0,0,0) \
		|| LTTNG_UBUNTU_KERNEL_RANGE(3,16,7,34, 3,17,0,0)))

#define get_pfnblock_flags_mask		wrapper_get_pfnblock_flags_mask

#include <linux/mm_types.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,14,0))
unsigned long wrapper_get_pfnblock_flags_mask(const struct page *page,
		unsigned long pfn,
		unsigned long mask);
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,9,0))
unsigned long wrapper_get_pfnblock_flags_mask(struct page *page,
		unsigned long pfn,
		unsigned long mask);
#else
unsigned long wrapper_get_pfnblock_flags_mask(struct page *page,
		unsigned long pfn,
		unsigned long end_bitidx,
		unsigned long mask);
#endif

int wrapper_get_pfnblock_flags_mask_init(void);

#else

#include <linux/mm_types.h>

static inline
int wrapper_get_pfnblock_flags_mask_init(void)
{
	return 0;
}

#endif

/*
 * For a specific range of Ubuntu 3.13 kernels, we need to redefine
 * get_pageblock_flags_mask to our wrapper, because the
 * get_pageblock_migratetype() macro uses it. This function has been
 * introduced into mainline within commit
 * e58469bafd0524e848c3733bc3918d854595e20f, but never actually showed
 * up in a stable kernel version, since it has been changed by commit
 * dc4b0caff24d9b2918e9f27bc65499ee63187eba. Since Ubuntu chose to only
 * backport the former commit but not the latter, we need to do a
 * special case to cover this.
 */
#if (defined(CONFIG_KALLSYMS) \
	&& LTTNG_UBUNTU_KERNEL_RANGE(3,13,11,50, 3,14,0,0))

#define get_pageblock_flags_mask	wrapper_get_pageblock_flags_mask

#include <linux/mm_types.h>

int wrapper_get_pageblock_flags_mask_init(void);

#else

#include <linux/mm_types.h>

static inline
int wrapper_get_pageblock_flags_mask_init(void)
{
	return 0;
}

#endif

#endif /* _LTTNG_WRAPPER_PAGE_ALLOC_H */
