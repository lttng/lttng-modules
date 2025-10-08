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

#include <linux/mm_types.h>
#include <lttng/kernel-version.h>

#if defined(CONFIG_KALLSYMS) && \
	(LTTNG_LINUX_VERSION_CODE < LTTNG_KERNEL_VERSION(6,17,0))

/*
 * We need to redefine get_pfnblock_flags_mask to our wrapper, because
 * the get_pageblock_migratetype() macro uses it.
 */
#define get_pfnblock_flags_mask		wrapper_get_pfnblock_flags_mask

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

static inline
int wrapper_get_pfnblock_flags_mask_init(void)
{
	return 0;
}

#endif


#if defined(CONFIG_KALLSYMS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,17,0))

/*
 * We need to redefine get_pfnblock_migratetype to our wrapper because
 * the get_pageblock_migratetype() macro uses it.
 */
#define get_pfnblock_migratetype	wrapper_get_pfnblock_migratetype

enum migratetype wrapper_get_pfnblock_migratetype(const struct page *page,
		unsigned long pfn);
int wrapper_get_pfnblock_migratetype_init(void);

#else

static inline
int wrapper_get_pfnblock_migratetype_init(void)
{
	return 0;
}

#endif /* else CONFIG_KALLSYMS */

#endif /* _LTTNG_WRAPPER_PAGE_ALLOC_H */
