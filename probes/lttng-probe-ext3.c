/*
 * probes/lttng-probe-ext3.c
 *
 * LTTng ext3 probes.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright (C) 2012 Mentor Graphics Corp.
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

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/dcache.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
/*
 * Since 3.4 the is no linux/ext3_fs_i.h anymore. Instead we have to use
 * ext3.h from fs/ext3/ext3.h (which also includes trace/events/ext3.h)
 */
#include "../instrumentation/events/mainline/fs_ext3.h"
#else
#include <linux/ext3_fs_i.h>

/*
 * Create the tracepoint static inlines from the kernel to validate that our
 * trace event macros match the kernel we run on.
 */
#include <trace/events/ext3.h>
#endif

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH ../instrumentation/events/lttng-module

#include "../instrumentation/events/lttng-module/ext3.h"

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Wade Farnsworth <wade_farnsworth@mentor.com> and Paul Woegerer <paul_woegerer@mentor.com>");
MODULE_DESCRIPTION("LTTng ext3 probes");
