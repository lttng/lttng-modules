// SPDX-FileCopyrightText: 2024 Michael Jeanson <mjeanson@efficios.com>
// SPDX-License-Identifier: GPL-2.0-only

#ifndef _LTTNG_WRAPPER_BITOPS_H
#define _LTTNG_WRAPPER_BITOPS_H

#include <linux/bitops.h>
#include <lttng/kernel-version.h>

#if LTTNG_LINUX_VERSION_CODE < LTTNG_KERNEL_VERSION(5,4,0)

#include <asm/byteorder.h>

/* Set bits in the first 'n' bytes when loaded from memory */
# ifdef __LITTLE_ENDIAN
#  define lttng_aligned_byte_mask(n) ((1UL << 8*(n))-1)
# else
#  define lttng_aligned_byte_mask(n) (~0xffUL << (BITS_PER_LONG - 8 - 8*(n)))
# endif

#endif

#endif /* _LTTNG_WRAPPER_BITOPS_H */
