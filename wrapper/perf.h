/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/perf.h
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_PERF_H
#define _LTTNG_WRAPPER_PERF_H

#include <linux/perf_event.h>

#ifdef CONFIG_PERF_EVENTS

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,1,0))
static inline struct perf_event *
wrapper_perf_event_create_kernel_counter(struct perf_event_attr *attr,
				int cpu,
				struct task_struct *task,
				perf_overflow_handler_t callback)
{
	return perf_event_create_kernel_counter(attr, cpu, task, callback, NULL);
}
#else /* (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,1,0)) */

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(2,6,37))
static inline struct perf_event *
wrapper_perf_event_create_kernel_counter(struct perf_event_attr *attr,
				int cpu,
				struct task_struct *task,
				perf_overflow_handler_t callback)
{
	return perf_event_create_kernel_counter(attr, cpu, task, callback);
}
#else /* (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(2,6,37)) */
static inline struct perf_event *
wrapper_perf_event_create_kernel_counter(struct perf_event_attr *attr,
				int cpu,
				struct task_struct *task,
				perf_overflow_handler_t callback)
{
	pid_t pid;

	if (!task)
		pid = -1;
	else
		pid = task->pid;

	return perf_event_create_kernel_counter(attr, cpu, pid, callback);
}

#endif /* (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(2,6,37)) */

#if (LTTNG_LINUX_VERSION_CODE < LTTNG_KERNEL_VERSION(2,6,36))
#define local64_read(l)		atomic64_read(l)
#endif

#endif /* (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,1,0)) */

#endif /* CONFIG_PERF_EVENTS */

#endif /* _LTTNG_WRAPPER_PERF_H */
