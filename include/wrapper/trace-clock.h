/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/trace-clock.h
 *
 * Contains LTTng trace clock mapping to LTTng trace clock or mainline monotonic
 * clock. This wrapper depends on CONFIG_HIGH_RES_TIMERS=y.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_TRACE_CLOCK_H
#define _LTTNG_TRACE_CLOCK_H

#ifdef CONFIG_HAVE_TRACE_CLOCK
#include <linux/trace-clock.h>
#else /* CONFIG_HAVE_TRACE_CLOCK */

#include <linux/hardirq.h>
#include <linux/ktime.h>
#include <linux/time.h>
#include <linux/hrtimer.h>
#include <linux/percpu.h>
#include <linux/version.h>
#include <linux/percpu-defs.h>
#include <asm/local.h>
#include <lttng/lttng-kernel-version.h>
#include <lttng/lttng-clock.h>
#include <linux/random.h>

extern struct lttng_trace_clock *lttng_trace_clock;

/*
 * We need clock values to be monotonically increasing per-cpu, which is
 * not strictly guaranteed by ktime_get_mono_fast_ns(). It is
 * straightforward to do on architectures with a 64-bit cmpxchg(), but
 * not so on architectures without 64-bit cmpxchg. For now, only enable
 * this feature on 64-bit architectures.
 */

#if BITS_PER_LONG == 64
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
	last_tsc_ptr = this_cpu_ptr(&lttng_last_tsc);
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
	unsigned char *boot_id;

	boot_id = get_kernel_boot_id();
	sprintf(uuid, "%pU", boot_id);
	return 0;
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
