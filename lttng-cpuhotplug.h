/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-cpuhotplug.h
 *
 * Copyright (C) 2016 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef LTTNG_CPUHOTPLUG_H
#define LTTNG_CPUHOTPLUG_H

struct lttng_cpuhp_node;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0))

#include <linux/cpuhotplug.h>

enum lttng_cpuhp_component {
	LTTNG_RING_BUFFER_FRONTEND,
	LTTNG_RING_BUFFER_BACKEND,
	LTTNG_RING_BUFFER_ITER,
	LTTNG_CONTEXT_PERF_COUNTERS,
};

struct lttng_cpuhp_node {
	enum lttng_cpuhp_component component;
	struct hlist_node node;
};

extern enum cpuhp_state lttng_hp_prepare;
extern enum cpuhp_state lttng_hp_online;

int lttng_cpuhp_rb_backend_prepare(unsigned int cpu,
                struct lttng_cpuhp_node *node);
int lttng_cpuhp_rb_frontend_dead(unsigned int cpu,
		struct lttng_cpuhp_node *node);
int lttng_cpuhp_rb_frontend_online(unsigned int cpu,
		struct lttng_cpuhp_node *node);
int lttng_cpuhp_rb_frontend_offline(unsigned int cpu,
		struct lttng_cpuhp_node *node);
int lttng_cpuhp_rb_iter_online(unsigned int cpu,
		struct lttng_cpuhp_node *node);

/* Ring buffer is a separate library. */
void lttng_rb_set_hp_prepare(enum cpuhp_state val);
void lttng_rb_set_hp_online(enum cpuhp_state val);

extern enum cpuhp_state lttng_rb_hp_prepare;
extern enum cpuhp_state lttng_rb_hp_online;

#endif

#endif /* LTTNG_CPUHOTPLUG_H */
