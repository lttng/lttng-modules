/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/compiler.h
 *
 * Copyright (C) 2013 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_COMPILER_H
#define _LTTNG_WRAPPER_COMPILER_H

#include <linux/compiler.h>

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

#endif /* _LTTNG_WRAPPER_COMPILER_H */
