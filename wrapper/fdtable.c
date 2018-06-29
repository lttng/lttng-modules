/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 * wrapper/fdtable.c
 *
 * Copyright (C) 2013 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/version.h>
#include <linux/spinlock.h>
#include <wrapper/fdtable.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0))

/*
 * Reimplementation of iterate_fd() for kernels between 2.6.32 and 3.6
 * (inclusive).
 */
int lttng_iterate_fd(struct files_struct *files,
		unsigned int first,
		int (*cb)(const void *, struct file *, unsigned int),
		const void *ctx)
{
	struct fdtable *fdt;
	struct file *filp;
	unsigned int i;
	int res = 0;

	if (!files)
		return 0;
	spin_lock(&files->file_lock);
	fdt = files_fdtable(files);
	for (i = 0; i < fdt->max_fds; i++) {
		filp = fcheck_files(files, i);
		if (!filp)
			continue;
		res = cb(ctx, filp, i);
		if (res)
			break;
	}
	spin_unlock(&files->file_lock);
	return res;
}

#endif
