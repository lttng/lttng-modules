/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-counter-client-percpu-32-modular.c
 *
 * LTTng lib counter client. Per-cpu 32-bit counters in overflow
 * arithmetic.
 *
 * Copyright (C) 2020 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <lttng/tracer.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <counter/counter.h>
#include <counter/counter-api.h>

static const struct lib_counter_config client_config = {
	.alloc = COUNTER_ALLOC_PER_CPU,
	.sync = COUNTER_SYNC_PER_CPU,
	.arithmetic = COUNTER_ARITHMETIC_MODULAR,
	.counter_size = COUNTER_SIZE_32_BIT,
};

static struct lttng_kernel_channel_counter *counter_create(size_t nr_dimensions,
					  const struct lttng_counter_dimension *dimensions,
					  int64_t global_sum_step)
{
	size_t max_nr_elem[LTTNG_COUNTER_DIMENSION_MAX], i;
	struct lttng_kernel_channel_counter *lttng_chan_counter;
	struct lib_counter *counter;

	if (nr_dimensions > LTTNG_COUNTER_DIMENSION_MAX)
		return NULL;
	for (i = 0; i < nr_dimensions; i++) {
		if (dimensions[i].has_underflow || dimensions[i].has_overflow)
			return NULL;
		max_nr_elem[i] = dimensions[i].size;
	}
	lttng_chan_counter = lttng_kernel_alloc_channel_counter();
	if (!lttng_chan_counter)
		return NULL;
	counter = lttng_counter_create(&client_config, nr_dimensions, max_nr_elem,
				    global_sum_step);
	if (!counter)
		goto error;
	lttng_chan_counter->priv->counter = counter;
	return lttng_chan_counter;

error:
	lttng_kernel_free_channel_common(&lttng_chan_counter->parent);
	return NULL;
}

static void counter_destroy(struct lttng_kernel_channel_counter *counter)
{
	lttng_counter_destroy(counter->priv->counter);
	lttng_kernel_free_channel_common(&counter->parent);
}

static int counter_add(struct lttng_kernel_channel_counter *counter,
		       const size_t *dimension_indexes, int64_t v)
{
	return lttng_counter_add(&client_config, counter->priv->counter, dimension_indexes, v);
}

static int event_counter_add(struct lttng_kernel_event_counter *event_counter, int64_t v)
{
	struct lttng_kernel_channel_counter *counter = event_counter->chan;
	size_t index = event_counter->priv->parent.id;

	return counter_add(counter, &index, v);
}

static int counter_read(struct lttng_kernel_channel_counter *counter, const size_t *dimension_indexes, int cpu,
			int64_t *value, bool *overflow, bool *underflow)
{
	return lttng_counter_read(&client_config, counter->priv->counter, dimension_indexes, cpu, value,
				  overflow, underflow);
}

static int counter_aggregate(struct lttng_kernel_channel_counter *counter, const size_t *dimension_indexes,
			     int64_t *value, bool *overflow, bool *underflow)
{
	return lttng_counter_aggregate(&client_config, counter->priv->counter, dimension_indexes, value,
				       overflow, underflow);
}

static int counter_clear(struct lttng_kernel_channel_counter *counter, const size_t *dimension_indexes)
{
	return lttng_counter_clear(&client_config, counter->priv->counter, dimension_indexes);
}

static int counter_get_nr_dimensions(struct lttng_kernel_channel_counter *counter, size_t *nr_dimensions)
{
	return lttng_counter_get_nr_dimensions(&client_config, counter->priv->counter,  nr_dimensions);
}

static int counter_get_max_nr_elem(struct lttng_kernel_channel_counter *counter, size_t *max_nr_elem)
{
	return lttng_counter_get_max_nr_elem(&client_config, counter->priv->counter, max_nr_elem);
}

static struct lttng_counter_transport lttng_counter_transport = {
	.name = "counter-per-cpu-32-modular",
	.owner = THIS_MODULE,
	.ops = {
		.priv = __LTTNG_COMPOUND_LITERAL(struct lttng_kernel_channel_counter_ops_private, {
			.pub = &lttng_counter_transport.ops,
			.counter_create = counter_create,
			.counter_destroy = counter_destroy,
			.counter_add = counter_add,
			.counter_read = counter_read,
			.counter_aggregate = counter_aggregate,
			.counter_clear = counter_clear,
			.counter_get_nr_dimensions = counter_get_nr_dimensions,
			.counter_get_max_nr_elem = counter_get_max_nr_elem,
		}),
		.event_counter_add = event_counter_add,
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
