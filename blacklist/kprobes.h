/* SPDX-License-Identifier: (GPL-2.0 OR LGPL-2.1)
 *
 * blacklist/kprobes.h
 *
 * Blacklist of kernel for buggy kprobes implementation.
 *
 * Copyright (C) 2019 Jonathan Rajotte-Julien <jonathan.rajotte-julien@efficios.com>
 */

#ifndef _LTTNG_BLACKLIST_KPROBES_H
#define _LTTNG_BLACKLIST_KPROBES_H

#include <lttng-kernel-version.h>

#if LTTNG_KERNEL_RANGE(4,20,0, 4,20,13) \
	|| LTTNG_KERNEL_RANGE(4,19,9, 4,19,26) \
	|| LTTNG_KERNEL_RANGE(4,14,87, 4,14,104)
# if defined(CONFIG_ARM) && defined(CONFIG_OPTPROBES)
#  error "Your kernel is known to have buggy optimized kprobes implementation. Fixed by commit 0ac569bf6a7983c0c5747d6df8db9dc05bc92b6c \"ARM: 8834/1: Fix: kprobes: optimized kprobes illegal instruction\" in Linux. Disable CONFIG_OPTPROBES or upgrade your kernel."
# endif /* #if defined(CONFIG_ARM) && defined(CONFIG_OPTPROBES) */
#endif

#endif /* _LTTNG_BLACKLIST_KPROBES_H */
