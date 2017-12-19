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
#include <lttng-kernel-version.h>
#include <lttng-clock.h>
#include <wrapper/compiler.h>
#include <wrapper/percpu-defs.h>
#include <wrapper/random.h>

#if ((LTTNG_KERNEL_RANGE(3,10,0, 3,10,14) && !LTTNG_RHEL_KERNEL_RANGE(3,10,0,123,0,0, 3,10,14,0,0,0)) \
	|| LTTNG_KERNEL_RANGE(3,11,0, 3,11,3))
#error "Linux kernels 3.10 and 3.11 introduce a deadlock in the timekeeping subsystem. Fixed by commit 7bd36014460f793c19e7d6c94dab67b0afcfcb7f \"timekeeping: Fix HRTICK related deadlock from ntp lock changes\" in Linux."
#endif

extern struct lttng_trace_clock *lttng_trace_clock;

/*
 * Upstream Linux commit 27727df240c7 ("Avoid taking lock in NMI path with
 * CONFIG_DEBUG_TIMEKEEPING") introduces a buggy ktime_get_mono_fast_ns().
 * This is fixed by patch "timekeeping: Fix __ktime_get_fast_ns() regression".
 */
#if (LTTNG_KERNEL_RANGE(4,8,0, 4,8,2) \
	|| LTTNG_KERNEL_RANGE(4,7,4, 4,7,8) \
	|| LTTNG_KERNEL_RANGE(4,4,20, 4,4,25) \
	|| LTTNG_KERNEL_RANGE(4,1,32, 4,1,35))
#define LTTNG_CLOCK_NMI_SAFE_BROKEN
#endif

/*
 * We need clock values to be monotonically increasing per-cpu, which is
 * not strictly guaranteed by ktime_get_mono_fast_ns(). It is
 * straightforward to do on architectures with a 64-bit cmpxchg(), but
 * not so on architectures without 64-bit cmpxchg. For now, only enable
 * this feature on 64-bit architectures.
 */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0) \
	&& BITS_PER_LONG == 64 \
	&& !defined(LTTNG_CLOCK_NMI_SAFE_BROKEN))
#define LTTNG_USE_NMI_SAFE_CLOCK
#endif

#ifdef LTTNG_USE_NMI_SAFE_CLOCK

DECLARE_PER_CPU(u64, lttng_last_tsc);

/*
 * Sometimes called with preemption enabled. Can be interrupted.
 */
static inline u64 trace_clock_monotonic_wrapper(void)
{
	u64 now, last, result;
	u64 *last_tsc_ptr;

	/* Use fast nmi-safe monotonic clock provided by the Linux kernel. */
	preempt_disable();
	last_tsc_ptr = lttng_this_cpu_ptr(&lttng_last_tsc);
	last = *last_tsc_ptr;
	/*
	 * Read "last" before "now". It is not strictly required, but it ensures
	 * that an interrupt coming in won't artificially trigger a case where
	 * "now" < "last". This kind of situation should only happen if the
	 * mono_fast time source goes slightly backwards.
	 */
	barrier();
	now = ktime_get_mono_fast_ns();
	if (U64_MAX / 2 < now - last)
		now = last;
	result = cmpxchg64_local(last_tsc_ptr, last, now);
	preempt_enable();
	if (result == last) {
		/* Update done. */
		return now;
	} else {
		/*
		 * Update not done, due to concurrent update. We can use
		 * "result", since it has been sampled concurrently with our
		 * time read, so it should not be far from "now".
		 */
		return result;
	}
}

#else /* #ifdef LTTNG_USE_NMI_SAFE_CLOCK */
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
#endif /* #else #ifdef LTTNG_USE_NMI_SAFE_CLOCK */

static inline u64 trace_clock_read64_monotonic(void)
{
	return (u64) trace_clock_monotonic_wrapper();
}

static inline u64 trace_clock_freq_monotonic(void)
{
	return (u64) NSEC_PER_SEC;
}

static inline int trace_clock_uuid_monotonic(char *uuid)
{
	return wrapper_get_bootid(uuid);
}

static inline const char *trace_clock_name_monotonic(void)
{
	return "monotonic";
}

static inline const char *trace_clock_description_monotonic(void)
{
	return "Monotonic Clock";
}

#ifdef LTTNG_USE_NMI_SAFE_CLOCK
static inline int get_trace_clock(void)
{
	printk_once(KERN_WARNING "LTTng: Using mainline kernel monotonic fast clock, which is NMI-safe.\n");
	return 0;
}
#else /* #ifdef LTTNG_USE_NMI_SAFE_CLOCK */
static inline int get_trace_clock(void)
{
	printk_once(KERN_WARNING "LTTng: Using mainline kernel monotonic clock. NMIs will not be traced.\n");
	return 0;
}
#endif /* #else #ifdef LTTNG_USE_NMI_SAFE_CLOCK */

static inline void put_trace_clock(void)
{
}

static inline u64 trace_clock_read64(void)
{
	struct lttng_trace_clock *ltc = READ_ONCE(lttng_trace_clock);

	if (likely(!ltc)) {
		return trace_clock_read64_monotonic();
	} else {
		read_barrier_depends();	/* load ltc before content */
		return ltc->read64();
	}
}

static inline u64 trace_clock_freq(void)
{
	struct lttng_trace_clock *ltc = READ_ONCE(lttng_trace_clock);

	if (!ltc) {
		return trace_clock_freq_monotonic();
	} else {
		read_barrier_depends();	/* load ltc before content */
		return ltc->freq();
	}
}

static inline int trace_clock_uuid(char *uuid)
{
	struct lttng_trace_clock *ltc = READ_ONCE(lttng_trace_clock);

	read_barrier_depends();	/* load ltc before content */
	/* Use default UUID cb when NULL */
	if (!ltc || !ltc->uuid) {
		return trace_clock_uuid_monotonic(uuid);
	} else {
		return ltc->uuid(uuid);
	}
}

static inline const char *trace_clock_name(void)
{
	struct lttng_trace_clock *ltc = READ_ONCE(lttng_trace_clock);

	if (!ltc) {
		return trace_clock_name_monotonic();
	} else {
		read_barrier_depends();	/* load ltc before content */
		return ltc->name();
	}
}

static inline const char *trace_clock_description(void)
{
	struct lttng_trace_clock *ltc = READ_ONCE(lttng_trace_clock);

	if (!ltc) {
		return trace_clock_description_monotonic();
	} else {
		read_barrier_depends();	/* load ltc before content */
		return ltc->description();
	}
}

#endif /* CONFIG_HAVE_TRACE_CLOCK */

#endif /* _LTTNG_TRACE_CLOCK_H */
