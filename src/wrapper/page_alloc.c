/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 * wrapper/page_alloc.c
 *
 * wrapper around get_pfnblock_flags_mask and Ubuntu
 * get_pageblock_flags_mask. Using KALLSYMS to get their address when
 * available, else we need to have a kernel that exports this function
 * to GPL modules.
 *
 * Copyright (C) 2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifdef CONFIG_KALLSYMS

/* Include page_alloc wrapper before pageblock-flags.h. */
#include <wrapper/page_alloc.h>

#include <linux/pageblock-flags.h>
#include <linux/kallsyms.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <wrapper/kallsyms.h>
#include <lttng/kernel-version.h>

/* get_pfnblock_flags_mask() was removed in 6.17 */
#if (LTTNG_LINUX_VERSION_CODE < LTTNG_KERNEL_VERSION(6,17,0))
# if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,14,0))
static
unsigned long (*get_pfnblock_flags_mask_sym)(const struct page *page,
		unsigned long pfn,
		unsigned long mask);

unsigned long wrapper_get_pfnblock_flags_mask(const struct page *page,
		unsigned long pfn,
		unsigned long mask)
{
	WARN_ON_ONCE(!get_pfnblock_flags_mask_sym);
	if (get_pfnblock_flags_mask_sym) {
		struct irq_ibt_state irq_ibt_state;
		unsigned long ret;

		irq_ibt_state = wrapper_irq_ibt_save();
		ret = get_pfnblock_flags_mask_sym(page, pfn, mask);
		wrapper_irq_ibt_restore(irq_ibt_state);
		return ret;
	} else {
		return -ENOSYS;
	}
}
# elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,9,0))
static
unsigned long (*get_pfnblock_flags_mask_sym)(struct page *page,
		unsigned long pfn,
		unsigned long mask);

unsigned long wrapper_get_pfnblock_flags_mask(struct page *page,
		unsigned long pfn,
		unsigned long mask)
{
	WARN_ON_ONCE(!get_pfnblock_flags_mask_sym);
	if (get_pfnblock_flags_mask_sym) {
		struct irq_ibt_state irq_ibt_state;
		unsigned long ret;

		irq_ibt_state = wrapper_irq_ibt_save();
		ret = get_pfnblock_flags_mask_sym(page, pfn, mask);
		wrapper_irq_ibt_restore(irq_ibt_state);
		return ret;
	} else {
		return -ENOSYS;
	}
}
# else	/* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,9,0)) */
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
		struct irq_ibt_state irq_ibt_state;
		unsigned long ret;

		irq_ibt_state = wrapper_irq_ibt_save();
		ret = get_pfnblock_flags_mask_sym(page, pfn, end_bitidx, mask);
		wrapper_irq_ibt_restore(irq_ibt_state);
		return ret;
	} else {
		return -ENOSYS;
	}
}
# endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,9,0)) */

EXPORT_SYMBOL_GPL(wrapper_get_pfnblock_flags_mask);

int wrapper_get_pfnblock_flags_mask_init(void)
{
	get_pfnblock_flags_mask_sym =
		(void *) kallsyms_lookup_funcptr("get_pfnblock_flags_mask");
	if (!get_pfnblock_flags_mask_sym)
		return -1;
	return 0;
}
EXPORT_SYMBOL_GPL(wrapper_get_pfnblock_flags_mask_init);
#endif


/* get_pfnblock_migratetype() was introduced in 6.17 */
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,17,0))
static
enum migratetype (*get_pfnblock_migratetype_sym)(const struct page *page,
		unsigned long pfn);

enum migratetype wrapper_get_pfnblock_migratetype(const struct page *page,
		unsigned long pfn)
{
	WARN_ON_ONCE(!get_pfnblock_migratetype_sym);
	if (get_pfnblock_migratetype_sym) {
		struct irq_ibt_state irq_ibt_state;
		enum migratetype ret;

		irq_ibt_state = wrapper_irq_ibt_save();
		ret = get_pfnblock_migratetype_sym(page, pfn);
		wrapper_irq_ibt_restore(irq_ibt_state);
		return ret;
	}
	return -ENOSYS;
}
EXPORT_SYMBOL_GPL(wrapper_get_pfnblock_migratetype);

int wrapper_get_pfnblock_migratetype_init(void)
{
	get_pfnblock_migratetype_sym =
		(void *) kallsyms_lookup_funcptr("get_pfnblock_migratetype");
	if (!get_pfnblock_migratetype_sym)
		return -1;
	return 0;
}
EXPORT_SYMBOL_GPL(wrapper_get_pfnblock_migratetype_init);
#endif /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,17,0) */

#else /* CONFIG_KALLSYMS */

#include <linux/pageblock-flags.h>

#endif
