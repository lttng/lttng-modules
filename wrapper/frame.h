/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/frame.h
 *
 * Copyright (C) 2016 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_FRAME_H
#define _LTTNG_WRAPPER_FRAME_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,6,0))

#include <linux/frame.h>

#define LTTNG_STACK_FRAME_NON_STANDARD(func) \
	STACK_FRAME_NON_STANDARD(func)

#else

#define LTTNG_STACK_FRAME_NON_STANDARD(func)

#endif

#endif /* _LTTNG_WRAPPER_FRAME_H */
