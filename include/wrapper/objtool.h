/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/objtool.h
 *
 * Copyright (C) 2016 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_OBJTOOL_H
#define _LTTNG_WRAPPER_OBJTOOL_H

#include <lttng/kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,10,0))
#include <linux/objtool.h>
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,6,0))
#include <linux/frame.h>
#endif


#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,6,0))

#define LTTNG_STACK_FRAME_NON_STANDARD(func) \
	STACK_FRAME_NON_STANDARD(func)

#else

#define LTTNG_STACK_FRAME_NON_STANDARD(func)

#endif

#endif /* _LTTNG_WRAPPER_OBJTOOL_H */
