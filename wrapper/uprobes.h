/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/uprobes.h
 *
 * wrapper around uprobes. Using KALLSYMS to get its address when
 * available, else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * Copyright (C) 2013 Yannick Brosseau <yannick.brosseau@gmail.com>
 * Copyright (C) 2017 Francis Deslauriers <francis.deslauriers@efficios.com>
 *
 */

#ifndef _LTTNG_WRAPPER_UPROBES_H
#define _LTTNG_WRAPPER_UPROBES_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,5,0))
#include <linux/uprobes.h>

/* Use kallsym lookup for version before 3.9. */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))

static inline
int wrapper_uprobe_register(struct inode *inode, loff_t offset, struct uprobe_consumer *uc)
{
	return uprobe_register(inode, offset, uc);
}

static inline
void wrapper_uprobe_unregister(struct inode *inode, loff_t offset, struct uprobe_consumer *uc)
{
	uprobe_unregister(inode, offset, uc);
}

#else /* Version < 3.9, use kallsym lookup. */
#include "kallsyms.h"

static inline
int wrapper_uprobe_register(struct inode *inode, loff_t offset, struct uprobe_consumer *uc)
{
	int (*uprobe_register_sym)(struct inode *inode, loff_t offset, struct uprobe_consumer *uc);

	uprobe_register_sym = (void *) kallsyms_lookup_funcptr("uprobe_register");

	if (uprobe_register_sym) {
		return uprobe_register_sym(inode, offset, uc);
	} else {
                printk(KERN_WARNING "LTTng: uprobe_register symbol lookup failed.\n");
                return -EINVAL;
	}
}

/*
 * Canary function to check for 'uprobe_register()' at compile time.
 *
 * From 'include/linux/uprobes.h':
 *
 *   extern int uprobe_register(struct inode *inode, loff_t offset, struct uprobe_consumer *uc);
 */
static inline
int __canary__uprobe_register(struct inode *inode, loff_t offset, struct uprobe_consumer *uc)
{
	return uprobe_register(inode, offset, uc);
}

static inline
void wrapper_uprobe_unregister(struct inode *inode, loff_t offset, struct uprobe_consumer *uc)
{
        int (*uprobe_unregister_sym)(struct inode *inode, loff_t offset, struct uprobe_consumer *uc);

        uprobe_unregister_sym = (void *) kallsyms_lookup_funcptr("uprobe_unregister");

        if (uprobe_unregister_sym) {
                uprobe_unregister_sym(inode, offset, uc);
        } else {
                printk(KERN_WARNING "LTTng: uprobe_unregister symbol lookup failed.\n");
                WARN_ON(1);
        }
}

/*
 * Canary function to check for 'uprobe_unregister()' at compile time.
 *
 * From 'include/linux/uprobes.h':
 *
 *   extern void uprobe_unregister(struct inode *inode, loff_t offset, struct uprobe_consumer *uc);
 */
static inline
void __canary__uprobe_unregister(struct inode *inode, loff_t offset, struct uprobe_consumer *uc)
{
	uprobe_unregister(inode, offset, uc);
}

#endif
#else
/* Version <  3.5, before uprobe was added. */
struct uprobe_consumer {};

#endif
#endif
