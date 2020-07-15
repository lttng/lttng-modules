/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-counter-client-percpu-64-modular.c
 *
 * LTTng lib counter client. Per-cpu 64-bit counters in overflow
 * arithmetic.
 *
 * Copyright (C) 2020 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <lttng/tracer.h>
#include <counter/counter.h>
#include <counter/counter-api.h>

static const struct lib_counter_config client_config = {
	.alloc = COUNTER_ALLOC_PER_CPU,
	.sync = COUNTER_SYNC_PER_CPU,
	.arithmetic = COUNTER_ARITHMETIC_MODULAR,
	.counter_size = COUNTER_SIZE_64_BIT,
};

static struct lib_counter *counter_create(size_t nr_dimensions,
					  const size_t *max_nr_elem,
					  int64_t global_sum_step)
{
	return lttng_counter_create(&client_config, nr_dimensions, max_nr_elem,
				    global_sum_step);
}

static void counter_destroy(struct lib_counter *counter)
{
	return lttng_counter_destroy(counter);
}

static int counter_add(struct lib_counter *counter, const size_t *dimension_indexes, int64_t v)
{
	return lttng_counter_add(&client_config, counter, dimension_indexes, v);
}

static int counter_read(struct lib_counter *counter, const size_t *dimension_indexes, int cpu,
			int64_t *value, bool *overflow, bool *underflow)
{
	return lttng_counter_read(&client_config, counter, dimension_indexes, cpu, value,
				  overflow, underflow);
}

static int counter_aggregate(struct lib_counter *counter, const size_t *dimension_indexes,
			     int64_t *value, bool *overflow, bool *underflow)
{
	return lttng_counter_aggregate(&client_config, counter, dimension_indexes, value,
				       overflow, underflow);
}

static int counter_clear(struct lib_counter *counter, const size_t *dimension_indexes)
{
	return lttng_counter_clear(&client_config, counter, dimension_indexes);
}

static struct lttng_counter_transport lttng_counter_transport = {
	.name = "counter-per-cpu-64-modular",
	.owner = THIS_MODULE,
	.ops = {
		.counter_create = counter_create,
		.counter_destroy = counter_destroy,
		.counter_add = counter_add,
		.counter_read = counter_read,
		.counter_aggregate = counter_aggregate,
		.counter_clear = counter_clear,
	},
};

static int __init lttng_counter_client_init(void)
{
	/*
	 * This vmalloc sync all also takes care of the lib counter
	 * vmalloc'd module pages when it is built as a module into LTTng.
	 */
	wrapper_vmalloc_sync_mappings();
	lttng_counter_transport_register(&lttng_counter_transport);
	return 0;
}

module_init(lttng_counter_client_init);

static void __exit lttng_counter_client_exit(void)
{
	lttng_counter_transport_unregister(&lttng_counter_transport);
}

module_exit(lttng_counter_client_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng counter per-cpu 32-bit overflow client");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
