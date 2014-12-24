#ifndef _LTTNG_TRACE_CLOCK_H
#define _LTTNG_TRACE_CLOCK_H

/*
 * wrapper/trace-clock.h
 *
 * Contains LTTng trace clock mapping to LTTng trace clock or mainline monotonic
 * clock. This wrapper depends on CONFIG_HIGH_RES_TIMERS=y.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifdef CONFIG_HAVE_TRACE_CLOCK
#include <linux/trace-clock.h>
#else /* CONFIG_HAVE_TRACE_CLOCK */

#include <linux/hardirq.h>
#include <linux/ktime.h>
#include <linux/time.h>
#include <linux/hrtimer.h>
#include <linux/percpu.h>
#include <linux/version.h>
#include <asm/local.h>
#include "../lttng-kernel-version.h"
#include "percpu-defs.h"
#include "random.h"

#if LTTNG_KERNEL_RANGE(3,10,0, 3,10,14) || LTTNG_KERNEL_RANGE(3,11,0, 3,11,3)
#error "Linux kernels 3.10 and 3.11 introduce a deadlock in the timekeeping subsystem. Fixed by commit 7bd36014460f793c19e7d6c94dab67b0afcfcb7f \"timekeeping: Fix HRTICK related deadlock from ntp lock changes\" in Linux."
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0))

DECLARE_PER_CPU(local_t, lttng_last_tsc);

#if (BITS_PER_LONG == 32)
/*
 * Fixup "src_now" using the 32 LSB from "last". We need to handle overflow and
 * underflow of the 32nd bit. "last" can be above, below or equal to the 32 LSB
 * of "src_now".
 */
static inline u64 trace_clock_fixup(u64 src_now, u32 last)
{
	u64 now;

	now = src_now & 0xFFFFFFFF00000000ULL;
	now |= (u64) last;
	/* Detect overflow or underflow between now and last. */
	if ((src_now & 0x80000000U) && !(last & 0x80000000U)) {
		/*
		 * If 32nd bit transitions from 1 to 0, and we move forward in
		 * time from "now" to "last", then we have an overflow.
		 */
		if (((s32) now - (s32) last) < 0)
			now += 0x0000000100000000ULL;
	} else if (!(src_now & 0x80000000U) && (last & 0x80000000U)) {
		/*
		 * If 32nd bit transitions from 0 to 1, and we move backward in
		 * time from "now" to "last", then we have an underflow.
		 */
		if (((s32) now - (s32) last) > 0)
			now -= 0x0000000100000000ULL;
	}
	return now;
}
#else /* #if (BITS_PER_LONG == 32) */
/*
 * The fixup is pretty easy on 64-bit architectures: "last" is a 64-bit
 * value, so we can use last directly as current time.
 */
static inline u64 trace_clock_fixup(u64 src_now, u64 last)
{
	return last;
}
#endif /* #else #if (BITS_PER_LONG == 32) */

/*
 * Always called with preemption disabled. Can be interrupted.
 */
static inline u64 trace_clock_monotonic_wrapper(void)
{
	u64 now;
	unsigned long last, result;
	local_t *last_tsc;

	/* Use fast nmi-safe monotonic clock provided by the Linux kernel. */
	last_tsc = lttng_this_cpu_ptr(&lttng_last_tsc);
	last = local_read(last_tsc);
	/*
	 * Read "last" before "now". It is not strictly required, but it ensures
	 * that an interrupt coming in won't artificially trigger a case where
	 * "now" < "last". This kind of situation should only happen if the
	 * mono_fast time source goes slightly backwards.
	 */
	barrier();
	now = ktime_get_mono_fast_ns();
	if (((long) now - (long) last) < 0)
		now = trace_clock_fixup(now, last);
	result = local_cmpxchg(last_tsc, last, (unsigned long) now);
	if (result == last) {
		/* Update done. */
		return now;
	} else {
		/*
		 * Update not done, due to concurrent update. We can use
		 * "result", since it has been sampled concurrently with our
		 * time read, so it should not be far from "now".
		 */
		return trace_clock_fixup(now, result);
	}
}

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0)) */
static inline u64 trace_clock_monotonic_wrapper(void)
{
	ktime_t ktime;

	/*
	 * Refuse to trace from NMIs with this wrapper, because an NMI could
	 * nest over the xtime write seqlock and deadlock.
	 */
	if (in_nmi())
		return (u64) -EIO;

	ktime = ktime_get();
	return ktime_to_ns(ktime);
}
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0)) */

static inline u64 trace_clock_read64(void)
{
	return (u64) trace_clock_monotonic_wrapper();
}

static inline u64 trace_clock_freq(void)
{
	return (u64) NSEC_PER_SEC;
}

static inline int trace_clock_uuid(char *uuid)
{
	return wrapper_get_bootid(uuid);
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0))
static inline int get_trace_clock(void)
{
	printk(KERN_WARNING "LTTng: Using mainline kernel monotonic fast clock, which is NMI-safe.\n");
	return 0;
}
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0)) */
static inline int get_trace_clock(void)
{
	printk(KERN_WARNING "LTTng: Using mainline kernel monotonic clock. NMIs will not be traced.\n");
	return 0;
}
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0)) */

static inline void put_trace_clock(void)
{
}

#endif /* CONFIG_HAVE_TRACE_CLOCK */

#endif /* _LTTNG_TRACE_CLOCK_H */
