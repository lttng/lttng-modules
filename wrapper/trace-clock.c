/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/trace-clock.c
 *
 * Contains LTTng trace clock mapping to LTTng trace clock or mainline monotonic
 * clock. This wrapper depends on CONFIG_HIGH_RES_TIMERS=y.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <wrapper/trace-clock.h>

#ifdef LTTNG_USE_NMI_SAFE_CLOCK
DEFINE_PER_CPU(u64, lttng_last_tsc);
EXPORT_PER_CPU_SYMBOL(lttng_last_tsc);
#endif /* #ifdef LTTNG_USE_NMI_SAFE_CLOCK */

#ifdef LTTNG_CLOCK_NMI_SAFE_BROKEN
#warning "Your kernel implements a bogus nmi-safe clock source. Falling back to the non-nmi-safe clock source, which discards events traced from NMI context. Upgrade your kernel to resolve this situation."
#endif
