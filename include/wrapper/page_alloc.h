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
#include <wrapper/kallsyms.h>


/* get_pfnblock_flags_mask() was removed in 6.17 */
#if defined(LTTNG_HAS_KALLSYMS) && \
	(LTTNG_LINUX_VERSION_CODE < LTTNG_KERNEL_VERSION(6,17,0))

/*
 * We need to redefine get_pfnblock_flags_mask to our wrapper, because
 * the get_pageblock_migratetype() macro uses it.
 */
#define get_pfnblock_flags_mask		wrapper_get_pfnblock_flags_mask

# if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,14,0))
unsigned long wrapper_get_pfnblock_flags_mask(const struct page *page,
		unsigned long pfn,
		unsigned long mask);

/*
 * Canary function to check for 'get_pfnblock_flags_mask' at compile time.
 *
 * From 'include/linux/pageblock-flags.h':
 *
 *   enum migratetype get_pfnblock_flags_mask(const struct page *page,
 *		unsigned long pfn,
 *		unsigned long mask);
 */
static inline
enum migratetype __canary__get_pfnblock_flags_mask(const struct page *page,
		unsigned long pfn,
		unsigned long mask)
{
	return get_pfnblock_flags_mask(page, pfn, mask);
}

# elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,9,0))
unsigned long wrapper_get_pfnblock_flags_mask(struct page *page,
		unsigned long pfn,
		unsigned long mask);

/*
 * Canary function to check for 'get_pfnblock_flags_mask' at compile time.
 *
 * From 'include/linux/pageblock-flags.h':
 *
 *   enum migratetype get_pfnblock_flags_mask(struct page *page,
 *		unsigned long pfn,
 *		unsigned long mask);
 */
static inline
enum migratetype __canary__get_pfnblock_flags_mask(struct page *page,
		unsigned long pfn,
		unsigned long mask)
{
	return get_pfnblock_flags_mask(page, pfn, mask);
}

# else
unsigned long wrapper_get_pfnblock_flags_mask(struct page *page,
		unsigned long pfn,
		unsigned long end_bitidx,
		unsigned long mask);

/*
 * Canary function to check for 'get_pfnblock_flags_mask' at compile time.
 *
 * From 'include/linux/pageblock-flags.h':
 *
 *   unsigned long get_pfnblock_flags_mask(struct page *page,
 *		unsigned long pfn,
 *		unsigned long end_bitidx,
 *		unsigned long mask);
 */
static inline
unsigned long __canary__get_pfnblock_flags_mask(struct page *page,
		unsigned long pfn,
		unsigned long end_bitidx,
		unsigned long mask)
{
	return get_pfnblock_flags_mask(page, pfn, end_bitidx, mask);
}

# endif

int wrapper_get_pfnblock_flags_mask_init(void);

#else

static inline
int wrapper_get_pfnblock_flags_mask_init(void)
{
	return 0;
}

#endif


/* get_pfnblock_migratetype() was introduced in 6.17 */
#if defined(LTTNG_HAS_KALLSYMS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,17,0))

/*
 * We need to redefine get_pfnblock_migratetype to our wrapper because
 * the get_pageblock_migratetype() macro uses it.
 */
#define get_pfnblock_migratetype	wrapper_get_pfnblock_migratetype

enum migratetype wrapper_get_pfnblock_migratetype(const struct page *page,
		unsigned long pfn);
int wrapper_get_pfnblock_migratetype_init(void);

/*
 * Canary function to check for 'get_pfnblock_migratetype' at compile time.
 *
 * From 'include/linux/pageblock-flags.h':
 *
 *   enum migratetype get_pfnblock_migratetype(const struct page *page,
 *		unsigned long pfn);
 */
static inline
enum migratetype __canary__get_pfnblock_migratetype(const struct page *page,
		unsigned long pfn)
{
	return get_pfnblock_migratetype(page, pfn);
}

#else

static inline
int wrapper_get_pfnblock_migratetype_init(void)
{
	return 0;
}

#endif /* else CONFIG_KALLSYMS */

#endif /* _LTTNG_WRAPPER_PAGE_ALLOC_H */
