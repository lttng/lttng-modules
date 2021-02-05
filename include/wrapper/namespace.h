/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/namespace.h
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *               2019 Michael Jeanson <mjeanson@efficios.com>
 *
 */

#ifndef _LTTNG_WRAPPER_NAMESPACE_H
#define _LTTNG_WRAPPER_NAMESPACE_H

#include <lttng/kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,19,0))
#define lttng_ns_inum ns.inum
#else
#define lttng_ns_inum proc_inum
#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,5,0))
#define lttng_user_ns_parent parent
#else
#define lttng_user_ns_parent creator->user_ns
#endif

#endif /* _LTTNG_WRAPPER_NAMESPACE_H */
