/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-calibrate.c
 *
 * LTTng probe calibration.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <lttng-abi.h>
#include <lttng-events.h>

noinline
void lttng_calibrate_kretprobe(void)
{
	asm volatile ("");
}

int lttng_calibrate(struct lttng_kernel_calibrate *calibrate)
{
	switch (calibrate->type) {
	case LTTNG_KERNEL_CALIBRATE_KRETPROBE:
		lttng_calibrate_kretprobe();
		break;
	default:
		return -EINVAL;
	}
	return 0;
}
