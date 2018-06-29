/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/vzalloc.h
 *
 * Copyright (C) 2015 Michael Jeanson <mjeanson@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_VZALLOC_H
#define _LTTNG_WRAPPER_VZALLOC_H

#include <linux/version.h>
#include <linux/vmalloc.h>

#if LTTNG_KERNEL_RANGE(2,6,36, 2,6,37)
static inline
void *lttng_vzalloc(unsigned long size)
{
	void *ret;
	ret = vmalloc(size);
	if (!ret)
		return NULL;
	memset(ret, 0, size);
	return ret;
}
#else
static inline
void *lttng_vzalloc(unsigned long size)
{
	return vzalloc(size);
}
#endif


#endif /* _LTTNG_WRAPPER_VZALLOC_H */
