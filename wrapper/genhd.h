/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/genhd.h
 *
 * wrapper around block layer functions and data structures. Using
 * KALLSYMS to get its address when available, else we need to have a
 * kernel that exports this function to GPL modules.
 *
 * Copyright (C) 2011-2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_GENHD_H
#define _LTTNG_WRAPPER_GENHD_H

#include <linux/genhd.h>

#ifdef CONFIG_KALLSYMS

#include <linux/kallsyms.h>
#include <wrapper/kallsyms.h>

static inline
char *wrapper_disk_name(struct gendisk *hd, int partno, char *buf)
{
	char *(*disk_name_sym)(struct gendisk *hd, int partno, char *buf);

	disk_name_sym = (void *) kallsyms_lookup_funcptr("disk_name");
	if (disk_name_sym) {
		return disk_name_sym(hd, partno, buf);
	} else {
		printk_once(KERN_WARNING "LTTng: disk_name symbol lookup failed.\n");
		return NULL;
	}
}

#else

static inline
char *wrapper_disk_name(struct gendisk *hd, int partno, char *buf)
{
	return disk_name(hd, partno, buf);
}

#endif

#ifdef CONFIG_KALLSYMS_ALL

static inline
struct class *wrapper_get_block_class(void)
{
	struct class *ptr_block_class;

	ptr_block_class = (struct class *) kallsyms_lookup_dataptr("block_class");
	if (!ptr_block_class) {
		printk_once(KERN_WARNING "LTTng: block_class symbol lookup failed.\n");
		return NULL;
	}
	return ptr_block_class;
}

static inline
struct device_type *wrapper_get_disk_type(void)
{
	struct device_type *ptr_disk_type;

	ptr_disk_type = (struct device_type *) kallsyms_lookup_dataptr("disk_type");
	if (!ptr_disk_type) {
		printk_once(KERN_WARNING "LTTng: disk_type symbol lookup failed.\n");
		return NULL;
	}
	return ptr_disk_type;
}

#else

static inline
struct class *wrapper_get_block_class(void)
{
	/*
	 * Symbol block_class is not exported.
	 * TODO: return &block_class;
	 */
	/* Feature currently unavailable without KALLSYMS_ALL */
	return NULL;
}

static inline
struct device_type *wrapper_get_disk_type(void)
{
	/*
	 * Symbol disk_type is not exported.
	 * TODO: return &disk_type;
	 */
	/* Feature currently unavailable without KALLSYMS_ALL */
	return NULL;
}

#endif

#endif /* _LTTNG_WRAPPER_GENHD_H */
