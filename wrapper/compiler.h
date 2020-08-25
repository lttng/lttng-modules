/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/compiler.h
 *
 * Copyright (C) 2013 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_COMPILER_H
#define _LTTNG_WRAPPER_COMPILER_H

#include <linux/compiler.h>
#include <linux/version.h>

/*
 * Don't allow compiling with buggy compiler.
 */

#ifdef GCC_VERSION

/*
 * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=58854
 */
# ifdef __ARMEL__
#  if GCC_VERSION >= 40800 && GCC_VERSION <= 40802
#   error Your gcc version produces clobbered frame accesses
#  endif
# endif
#endif

/*
 * READ/WRITE_ONCE were introduced in kernel 3.19 and ACCESS_ONCE
 * was removed in 4.15. Prefer READ/WRITE but fallback to ACCESS
 * when they are not available.
 */
#ifndef READ_ONCE
# define READ_ONCE(x)		ACCESS_ONCE(x)
#endif

#ifndef WRITE_ONCE
# define WRITE_ONCE(x, val)	({ ACCESS_ONCE(x) = val; })
#endif

/*
 * In v4.15 a smp read barrier was added to READ_ONCE to replace
 * lockless_dereference(), replicate this behavior on prior kernels
 * and remove calls to smp_read_barrier_depends which was dropped
 * in v5.9.
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0))
#define LTTNG_READ_ONCE(x)	READ_ONCE(x)
#else
#define LTTNG_READ_ONCE(x)			\
({						\
	typeof(x) __val = READ_ONCE(x);		\
	smp_read_barrier_depends();		\
	__val;					\
})
#endif

#endif /* _LTTNG_WRAPPER_COMPILER_H */
