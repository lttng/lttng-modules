/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/bug.h
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_BUG_H
#define _LTTNG_BUG_H

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
			BUILD_BUG_ON(condition);		\
		else						\
			BUG_ON(condition);			\
	} while (0)

#endif
