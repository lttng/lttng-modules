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

#ifdef CONFIG_KALLSYMS_ALL

#include <linux/kallsyms.h>
#include <wrapper/kallsyms.h>

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

/*
 * Canary function to check for 'block_class' at compile time.
 *
 * From 'include/linux/genhd.h':
 *
 *   extern struct class block_class;
 */
static inline
struct class *__canary__get_block_class(void)
{
	return &block_class;
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

/*
 * No canary for 'disk_type', it's only defined in 'block/genhd.c'.
 *
 * static inline
 * struct device_type *__canary__get_disk_type(void)
 * {
 * 	return &disk_type;
 * }
 */

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
