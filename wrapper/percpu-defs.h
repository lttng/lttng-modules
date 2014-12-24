#ifndef _LTTNG_WRAPPER_PERCPU_DEFS_H
#define _LTTNG_WRAPPER_PERCPU_DEFS_H

/*
 * wrapper/percpu-defs.h
 *
 * wrapper around linux/percpu-defs.h.
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)

#include <linux/percpu-defs.h>

#define lttng_this_cpu_ptr(ptr)		this_cpu_ptr(ptr)

#else /* #if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0) */

#include <linux/percpu.h>

#define lttng_this_cpu_ptr(ptr)		(&__get_cpu_var(*(ptr)))

#endif /* #else #if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0) */

#endif /* _LTTNG_WRAPPER_PERCPU_DEFS_H */
