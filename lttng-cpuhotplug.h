#ifndef LTTNG_CPUHOTPLUG_H
#define LTTNG_CPUHOTPLUG_H

/*
 * lttng-cpuhotplug.h
 *
 * Copyright (C) 2016 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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
