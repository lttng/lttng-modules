#ifndef _LTTNG_BUG_H
#define _LTTNG_BUG_H

/*
 * lib/bug.h
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
#define LTTNG_BUILD_BUG_ON(cond) BUILD_BUG_ON(cond)
#else
#define LTTNG_BUILD_BUG_ON(cond) MAYBE_BUILD_BUG_ON(cond)
#endif

/**
 * BUILD_RUNTIME_BUG_ON - check condition at build (if constant) or runtime
 * @condition: the condition which should be false.
 *
 * If the condition is a constant and true, the compiler will generate a build
 * error. If the condition is not constant, a BUG will be triggered at runtime
 * if the condition is ever true. If the condition is constant and false, no
 * code is emitted.
 */
#define BUILD_RUNTIME_BUG_ON(condition)				\
	do {							\
		if (__builtin_constant_p(condition))		\
			LTTNG_BUILD_BUG_ON(condition);		\
		else						\
			BUG_ON(condition);			\
	} while (0)

#endif
