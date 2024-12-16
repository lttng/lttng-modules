/*
 * SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 * SPDX-FileCopyrightText: 2024 Kienan Stewart <kstewart@efficios.com>
 */

#ifndef LTTNG_WRAPPER_FILE_REF_H
#define LTTNG_WRAPPER_FILE_REF_H

#include <linux/file.h>
#include <linux/fs.h>
#include <lttng/kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,13,0))
#include <linux/file_ref.h>

static inline __must_check
bool lttng_file_ref_get(struct file *file)
{
	return file_ref_get(&file->f_ref);
}

static inline __must_check
bool lttng_file_ref_put(struct file *file)
{
	return file_ref_put(&file->f_ref);
}

#else
static inline __must_check
bool lttng_file_ref_get(struct file *file)
{
	atomic_long_add_unless(&file->f_count, 1, LONG_MAX);
	return true;
}

static inline __must_check
bool lttng_file_ref_put(struct file *file)
{
	atomic_long_dec(&file->f_count);
	return true;
}
#endif
#endif /* LTTNG_WRAPPER_FILE_REF_H */
