#ifndef _LTTNG_WRAPPER_VZALLOC_H
#define _LTTNG_WRAPPER_VZALLOC_H

/*
 * wrapper/vzalloc.h
 *
 * Copyright (C) 2015 Michael Jeanson <mjeanson@efficios.com>
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
