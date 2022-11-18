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

/*
 * We need to redefine get_pfnblock_flags_mask to our wrapper, because
 * the get_pageblock_migratetype() macro uses it.
 */
#ifdef CONFIG_KALLSYMS

#define get_pfnblock_flags_mask		wrapper_get_pfnblock_flags_mask

int wrapper_get_pfnblock_flags_mask_init(void);

#else

static inline
int wrapper_get_pfnblock_flags_mask_init(void)
{
	return 0;
}

#endif

#endif /* _LTTNG_WRAPPER_PAGE_ALLOC_H */
