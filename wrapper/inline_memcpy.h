/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/inline_memcpy.h
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#if !defined(__HAVE_ARCH_INLINE_MEMCPY) && !defined(inline_memcpy)
#define inline_memcpy memcpy
#endif
