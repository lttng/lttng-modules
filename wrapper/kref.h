/* SPDX-License-Identifier: GPL-2.0
 *
 * wrapper/kref.h
 *
 * wrapper around linux/kref.h.
 *
 * Copyright (C) 2016 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This wrapper code is derived from Linux 3.19.2 include/linux/list.h
 * and include/linux/rculist.h, hence the GPLv2 license applied to this
 * file.
 */

#ifndef _LTTNG_WRAPPER_KREF_H
#define _LTTNG_WRAPPER_KREF_H

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
