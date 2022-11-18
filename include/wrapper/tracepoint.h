/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/tracepoint.h
 *
 * wrapper around DECLARE_EVENT_CLASS.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_TRACEPOINT_H
#define _LTTNG_WRAPPER_TRACEPOINT_H

#include <lttng/kernel-version.h>
#include <lttng/tracepoint.h>
#include <linux/tracepoint.h>
#include <linux/module.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,10,0))
#define LTTNG_DEFINE_TRACE(name, proto, args)		\
	DEFINE_TRACE(name, PARAMS(proto), PARAMS(args))
#else
#define LTTNG_DEFINE_TRACE(name, proto, args)		\
	DEFINE_TRACE(name)
#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,19,0))
static inline struct tracepoint *lttng_tracepoint_ptr_deref(tracepoint_ptr_t *p)
{
	return tracepoint_ptr_deref(p);
}
#else /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,19,0)) */
static inline struct tracepoint *lttng_tracepoint_ptr_deref(struct tracepoint * const *p)
{
	return *p;
}
#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,19,0)) */

#endif /* _LTTNG_WRAPPER_TRACEPOINT_H */
