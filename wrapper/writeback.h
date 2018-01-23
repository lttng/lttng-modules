#ifndef _LTTNG_WRAPPER_WRITEBACK_H
#define _LTTNG_WRAPPER_WRITEBACK_H

/*
 * wrapper/writeback.h
 *
 * wrapper around global_dirty_limit read. Using KALLSYMS with KALLSYMS_ALL
 * to get its address when available, else we need to have a kernel that
 * exports this variable to GPL modules.
 *
 * Copyright (C) 2013 Mentor Graphics Corp.
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
#else

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
#endif

#else /* CONFIG_KALLSYMS_ALL */

#include <linux/writeback.h>

static inline
unsigned long wrapper_global_dirty_limit(void)
{
	return global_dirty_limit;
}

#endif

#endif /* _LTTNG_WRAPPER_WRITEBACK_H */
