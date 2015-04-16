#ifndef _LTTNG_WRAPPER_PAGE_ALLOC_H
#define _LTTNG_WRAPPER_PAGE_ALLOC_H

/*
 * wrapper/page_alloc.h
 *
 * wrapper around get_pfnblock_flags_mask. Using KALLSYMS to get its address
 * when available, else we need to have a kernel that exports this function to
 * GPL modules.
 *
 * Copyright (C) 2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

#include "../lttng-kernel-version.h"

/*
 * We need to redefine get_pfnblock_flags_mask to our wrapper, because
 * the get_pageblock_migratetype() macro uses it.
 */
#if defined(CONFIG_KALLSYMS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,2))

#define get_pfnblock_flags_mask		wrapper_get_pfnblock_flags_mask

#include <linux/mm_types.h>

int wrapper_get_pfnblock_flags_mask_init(void);

#else /* #if defined(CONFIG_KALLSYMS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,2)) */

#include <linux/mm_types.h>

static inline
int wrapper_get_pfnblock_flags_mask_init(void)
{
	return 0;
}

#endif /* else #if defined(CONFIG_KALLSYMS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,2)) */

#endif /* _LTTNG_WRAPPER_PAGE_ALLOC_H */
