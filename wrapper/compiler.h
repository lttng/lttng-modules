/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
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

#define __LTTNG_COMPOUND_LITERAL(type, ...)	(type[]) { __VA_ARGS__ }

#endif /* _LTTNG_WRAPPER_COMPILER_H */
