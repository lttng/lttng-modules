/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * metadata-ctf-common.h
 *
 * Common CTF metadata stream serialization.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright (C) 2025 Philippe Proulx <pproulx@efficios.com>
 */

#ifndef _METADATA_CTF_COMMON_H
#define _METADATA_CTF_COMMON_H

#include <linux/types.h>
#include <linux/time64.h>
#include <linux/time.h>

 /*
 * Approximation of NTP time of day to clock monotonic correlation,
 * taken at start of trace.
 * Yes, this is only an approximation. Yes, we can (and will) do better
 * in future versions.
 * This function may return a negative offset. It may happen if the
 * system sets the REALTIME clock to 0 after boot.
 *
 * Use 64bit timespec on kernels that have it, this makes 32bit arch
 * y2038 compliant.
 */
static inline
int64_t measure_clock_offset(void)
{
	uint64_t monotonic_avg, monotonic[2], realtime;
	uint64_t tcf = trace_clock_freq();
	int64_t offset;
	unsigned long flags;
	struct timespec64 rts = { 0, 0 };

	/* Disable interrupts to increase correlation precision. */
	local_irq_save(flags);
	monotonic[0] = trace_clock_read64();
	ktime_get_real_ts64(&rts);
	monotonic[1] = trace_clock_read64();
	local_irq_restore(flags);

	monotonic_avg = (monotonic[0] + monotonic[1]) >> 1;
	realtime = (uint64_t) rts.tv_sec * tcf;
	if (tcf == NSEC_PER_SEC) {
		realtime += rts.tv_nsec;
	} else {
		uint64_t n = rts.tv_nsec * tcf;

		do_div(n, NSEC_PER_SEC);
		realtime += n;
	}
	offset = (int64_t) realtime - monotonic_avg;
	return offset;
}

#endif /* _METADATA_CTF_COMMON_H */
