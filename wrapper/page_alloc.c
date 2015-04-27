/*
 * wrapper/page_alloc.c
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

#if (defined(CONFIG_KALLSYMS) \
	&& (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,2) \
		|| LTTNG_DEBIAN_KERNEL_RANGE(3,16,7,9,0,0, 3,17,0,0,0,0)))

#include <linux/kallsyms.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include "kallsyms.h"
#include "page_alloc.h"

static
unsigned long (*get_pfnblock_flags_mask_sym)(struct page *page,
		unsigned long pfn,
		unsigned long end_bitidx,
		unsigned long mask);

unsigned long wrapper_get_pfnblock_flags_mask(struct page *page,
		unsigned long pfn,
		unsigned long end_bitidx,
		unsigned long mask)
{
	WARN_ON_ONCE(!get_pfnblock_flags_mask_sym);
	if (get_pfnblock_flags_mask_sym) {
		return get_pfnblock_flags_mask_sym(page, pfn, end_bitidx, mask);
	} else {
		return -ENOSYS;
	}
}
EXPORT_SYMBOL_GPL(wrapper_get_pfnblock_flags_mask);

int wrapper_get_pfnblock_flags_mask_init(void)
{
	get_pfnblock_flags_mask_sym =
		(void *) kallsyms_lookup_funcptr("get_pfnblock_flags_mask");
	if (!get_pfnblock_flags_mask_sym)
		return -1;
	return 0;
}

#else /* #if defined(CONFIG_KALLSYMS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,2)) */

#include <linux/pageblock-flags.h>

#endif /* #else #if defined(CONFIG_KALLSYMS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,2)) */
