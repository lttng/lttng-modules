#ifndef _LTTNG_WRAPPER_KREF_H
#define _LTTNG_WRAPPER_KREF_H

/*
 * wrapper/kref.h
 *
 * wrapper around linux/kref.h.
 *
 * Copyright (C) 2016 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * This wrapper code is derived from Linux 3.19.2 include/linux/list.h
 * and include/linux/rculist.h, hence the GPLv2 license applied to this
 * file.
 */

#include <linux/kref.h>
#include <linux/rculist.h>
#include <linux/version.h>

/*
 * lttng_kref_get: get reference count, checking for overflow.
 *
 * Return 1 if reference is taken, 0 otherwise (overflow).
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0))
static inline int lttng_kref_get(struct kref *kref)
{
	kref_get(kref);
	return 1;
}
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)) */
static inline int lttng_kref_get(struct kref *kref)
{
	return atomic_add_unless(&kref->refcount, 1, INT_MAX);
}
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)) */

#endif /* _LTTNG_WRAPPER_KREF_H */
