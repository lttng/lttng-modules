/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * src/wrapper/kprobes.h
 *
 * Copyright (C) 2021 Michael Jeanson <mjeanson@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_SCHED_H
#define _LTTNG_WRAPPER_SCHED_H

#include <linux/sched.h>
#include <lttng-kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,14,0) || \
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,372,0,0, 4,19,0,0,0,0))

#define lttng_get_task_state(task)	READ_ONCE((task)->__state)
#define lttng_task_is_running(task)	task_is_running(task)

#else /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,14,0) */

#define lttng_get_task_state(task)	((task)->state)
#define lttng_task_is_running(task)	(lttng_get_task_state(task) == TASK_RUNNING)

#endif /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,14,0) */

#endif /* _LTTNG_WRAPPER_SCHED_H */
