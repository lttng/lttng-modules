/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * counter/counter.h
 *
 * LTTng Counters API
 *
 * Copyright (C) 2020 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_COUNTER_H
#define _LTTNG_COUNTER_H

#include <linux/types.h>
#include <linux/percpu.h>
#include <counter/config.h>
#include <counter/counter-types.h>

/* max_nr_elem is for each dimension. */
struct lib_counter *lttng_counter_create(const struct lib_counter_config *config,
					 size_t nr_dimensions,
					 const size_t *max_nr_elem,
					 int64_t global_sum_step);
void lttng_counter_destroy(struct lib_counter *counter);

int lttng_counter_get_nr_dimensions(const struct lib_counter_config *config,
				    struct lib_counter *counter,
				    size_t *nr_dimensions);
int lttng_counter_get_max_nr_elem(const struct lib_counter_config *config,
				  struct lib_counter *counter,
				  size_t *max_nr_elem); /* array of size nr_dimensions */

int lttng_counter_read(const struct lib_counter_config *config,
		      struct lib_counter *counter,
		      const size_t *dimension_indexes,
		      int cpu, int64_t *value,
		      bool *overflow, bool *underflow);
int lttng_counter_aggregate(const struct lib_counter_config *config,
			    struct lib_counter *counter,
			    const size_t *dimension_indexes,
			    int64_t *value,
			    bool *overflow, bool *underflow);
int lttng_counter_clear(const struct lib_counter_config *config,
			struct lib_counter *counter,
			const size_t *dimension_indexes);

#endif /* _LTTNG_COUNTER_H */
