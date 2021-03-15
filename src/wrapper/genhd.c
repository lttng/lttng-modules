/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * wrapper/genhd.c
 *
 * Wrapper around disk_part_iter_(init|next|exit). Using KALLSYMS to get the
 * addresses when available, else we need to have a kernel that exports this
 * function to GPL modules. This export was removed in 5.12.
 *
 * Copyright (C) 2021 Michael Jeanson <mjeanson@efficios.com>
 */

#include <lttng/kernel-version.h>
#include <linux/module.h>
#include <wrapper/genhd.h>

#if (defined(CONFIG_KALLSYMS) && \
	(LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,12,0)))

#include <wrapper/kallsyms.h>

static
void (*disk_part_iter_init_sym)(struct disk_part_iter *piter, struct gendisk *disk,
			unsigned int flags);

static
LTTNG_DISK_PART_TYPE *(*disk_part_iter_next_sym)(struct disk_part_iter *piter);

static
void (*disk_part_iter_exit_sym)(struct disk_part_iter *piter);

/*
 * This wrapper has an 'int' return type instead of the original 'void', to be
 * able to report the symbol lookup failure to the caller.
 *
 * Return 0 on success, -1 on error.
 */
int wrapper_disk_part_iter_init(struct disk_part_iter *piter, struct gendisk *disk,
                          unsigned int flags)
{
	if (!disk_part_iter_init_sym)
		disk_part_iter_init_sym = (void *) kallsyms_lookup_funcptr("disk_part_iter_init");

	if (disk_part_iter_init_sym) {
		disk_part_iter_init_sym(piter, disk, flags);
	} else {
		printk_once(KERN_WARNING "LTTng: disk_part_iter_init symbol lookup failed.\n");
		return -1;
	}
	return 0;
}
EXPORT_SYMBOL_GPL(wrapper_disk_part_iter_init);

LTTNG_DISK_PART_TYPE *wrapper_disk_part_iter_next(struct disk_part_iter *piter)
{
	if (!disk_part_iter_next_sym)
		disk_part_iter_next_sym = (void *) kallsyms_lookup_funcptr("disk_part_iter_next");

	if (disk_part_iter_next_sym) {
		return disk_part_iter_next_sym(piter);
	} else {
		printk_once(KERN_WARNING "LTTng: disk_part_iter_next symbol lookup failed.\n");
		return NULL;
	}
}
EXPORT_SYMBOL_GPL(wrapper_disk_part_iter_next);

/*
 * We don't return an error on symbol lookup failure here because there is
 * nothing the caller can do to cleanup the iterator.
 */
void wrapper_disk_part_iter_exit(struct disk_part_iter *piter)
{
	if (!disk_part_iter_exit_sym)
		disk_part_iter_exit_sym = (void *) kallsyms_lookup_funcptr("disk_part_iter_exit");

	if (disk_part_iter_exit_sym) {
		disk_part_iter_exit_sym(piter);
	} else {
		printk_once(KERN_WARNING "LTTng: disk_part_iter_exit symbol lookup failed.\n");
	}
}
EXPORT_SYMBOL_GPL(wrapper_disk_part_iter_exit);

#else

/*
 * This wrapper has an 'int' return type instead of the original 'void', so the
 * kallsyms variant can report the symbol lookup failure to the caller.
 *
 * This variant always succeeds and returns 0.
 */
int wrapper_disk_part_iter_init(struct disk_part_iter *piter, struct gendisk *disk,
                          unsigned int flags)
{
	disk_part_iter_init(piter, disk, flags);
	return 0;
}
EXPORT_SYMBOL_GPL(wrapper_disk_part_iter_init);

LTTNG_DISK_PART_TYPE *wrapper_disk_part_iter_next(struct disk_part_iter *piter)
{
	return disk_part_iter_next(piter);
}
EXPORT_SYMBOL_GPL(wrapper_disk_part_iter_next);

void wrapper_disk_part_iter_exit(struct disk_part_iter *piter)
{
	disk_part_iter_exit(piter);
}
EXPORT_SYMBOL_GPL(wrapper_disk_part_iter_exit);
#endif
