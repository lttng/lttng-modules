/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1) */
#ifndef _LTTNG_ENDIAN_H
#define _LTTNG_ENDIAN_H

/*
 * lttng-endian.h
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifdef __KERNEL__
# include <asm/byteorder.h>
# ifdef __BIG_ENDIAN
#  define __BYTE_ORDER __BIG_ENDIAN
# elif defined(__LITTLE_ENDIAN)
#  define __BYTE_ORDER __LITTLE_ENDIAN
# else
#  error "unknown endianness"
# endif
#ifndef __BIG_ENDIAN
# define __BIG_ENDIAN 4321
#endif
#ifndef __LITTLE_ENDIAN
# define __LITTLE_ENDIAN 1234
#endif
#else
# include <endian.h>
#endif

#endif /* _LTTNG_ENDIAN_H */
