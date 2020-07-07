/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/writeback.h
 *
 * wrapper around global_dirty_limit read. Using KALLSYMS with KALLSYMS_ALL
 * to get its address when available, else we need to have a kernel that
 * exports this variable to GPL modules.
 *
 * Copyright (C) 2013 Mentor Graphics Corp.
 */

#ifndef _LTTNG_WRAPPER_WRITEBACK_H
#define _LTTNG_WRAPPER_WRITEBACK_H

#include <lttng-kernel-version.h>

#ifdef CONFIG_KALLSYMS_ALL
#include <linux/kallsyms.h>
#include <wrapper/kallsyms.h>



#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,2,0)

static struct wb_domain *global_wb_domain_sym;

static inline
unsigned long wrapper_global_dirty_limit(void)
{
	if (!global_wb_domain_sym)
		global_wb_domain_sym =
			(void *) kallsyms_lookup_dataptr("global_wb_domain");
	if (global_wb_domain_sym) {
		return global_wb_domain_sym->dirty_limit;
	} else {
		printk_once(KERN_WARNING "LTTng: global_wb_domain symbol lookup failed.\n");
		return 0;
	}
}

/*
 * Canary function to check for 'global_wb_domain' at compile time.
 *
 * From 'include/linux/writeback.h':
 *
 *   extern struct wb_domain global_wb_domain;
 */
static inline
unsigned long __canary__global_wb_domain(void)
{
	return global_wb_domain.dirty_limit;
}

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0)

static unsigned long *global_dirty_limit_sym;

static inline
unsigned long wrapper_global_dirty_limit(void)
{
	if (!global_dirty_limit_sym)
		global_dirty_limit_sym =
			(void *) kallsyms_lookup_dataptr("global_dirty_limit");
	if (global_dirty_limit_sym) {
		return *global_dirty_limit_sym;
	} else {
		printk_once(KERN_WARNING "LTTng: global_dirty_limit symbol lookup failed.\n");
		return 0;
	}
}

/*
 * Canary function to check for 'global_dirty_limit' at compile time.
 *
 * From 'include/linux/writeback.h':
 *
 *   extern unsigned long global_dirty_limit;
 */
static inline
unsigned long __canary__global_dirty_limit(void)
{
	return global_dirty_limit;
}

#endif

#else /* CONFIG_KALLSYMS_ALL */

#include <linux/writeback.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0)
static inline
unsigned long wrapper_global_dirty_limit(void)
{
	return global_dirty_limit;
}
#endif

#endif

#endif /* _LTTNG_WRAPPER_WRITEBACK_H */
