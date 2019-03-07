/* SPDX-License-Identifier: (GPL-2.0 OR LGPL-2.1)
 *
 * blacklist/timekeeping.h
 *
 * Blacklist of kernel for buggy timekeeping implementation.
 *
 * Copyright (C) 2019 Jonathan Rajotte-Julien <jonathan.rajotte-julien@efficios.com>
 */

#ifndef _LTTNG_BLACKLIST_TIMEKEEPING_H
#define _LTTNG_BLACKLIST_TIMEKEEPING_H

#include <lttng-kernel-version.h>

#if ((LTTNG_KERNEL_RANGE(3,10,0, 3,10,14) && !LTTNG_RHEL_KERNEL_RANGE(3,10,0,123,0,0, 3,10,14,0,0,0)) \
	|| LTTNG_KERNEL_RANGE(3,11,0, 3,11,3))
# error "Linux kernels 3.10 and 3.11 introduce a deadlock in the timekeeping subsystem. Fixed by commit 7bd36014460f793c19e7d6c94dab67b0afcfcb7f \"timekeeping: Fix HRTICK related deadlock from ntp lock changes\" in Linux."
#endif

#endif /* _LTTNG_BLACKLIST_TIMEKEEPING_H */
