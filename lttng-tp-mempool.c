/*
 * lttng-tp-mempool.c
 *
 * Copyright (C) 2018 Julien Desfossez <jdesfossez@efficios.com>
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

#include <linux/slab.h>
#include <linux/percpu.h>

#include <lttng-tp-mempool.h>

struct lttng_tp_buf_entry {
	int cpu; /* To make sure we return the entry to the right pool. */
	char buf[LTTNG_TP_MEMPOOL_BUF_SIZE];
	struct list_head list;
};

/*
 * No exclusive access strategy for now, this memory pool is currently only
 * used from a non-preemptible context, and the interrupt tracepoint probes do
 * not use this facility.
 */
struct per_cpu_buf {
	struct list_head free_list; /* Free struct lttng_tp_buf_entry. */
};

static struct per_cpu_buf __percpu *pool; /* Per-cpu buffer. */

int lttng_tp_mempool_init(void)
{
	int ret, cpu;

	/* The pool is only supposed to be allocated once. */
	if (pool) {
		WARN_ON_ONCE(1);
		ret = -1;
		goto end;
	}

	pool = alloc_percpu(struct per_cpu_buf);
	if (!pool) {
		ret = -ENOMEM;
		goto end;
	}

	for_each_possible_cpu(cpu) {
		struct per_cpu_buf *cpu_buf = per_cpu_ptr(pool, cpu);

		INIT_LIST_HEAD(&cpu_buf->free_list);
	}

	for_each_possible_cpu(cpu) {
		int i;
		struct per_cpu_buf *cpu_buf = per_cpu_ptr(pool, cpu);

		for (i = 0; i < LTTNG_TP_MEMPOOL_NR_BUF_PER_CPU; i++) {
			struct lttng_tp_buf_entry *entry;

			entry = kzalloc(sizeof(struct lttng_tp_buf_entry),
					GFP_KERNEL);
			if (!entry) {
				ret = -ENOMEM;
				goto error_free_pool;
			}
			entry->cpu = cpu;
			list_add_tail(&entry->list, &cpu_buf->free_list);
		}
	}

	ret = 0;
	goto end;

error_free_pool:
	lttng_tp_mempool_destroy();
end:
	return ret;
}

void lttng_tp_mempool_destroy(void)
{
	int cpu;

	if (!pool) {
		return;
	}

	for_each_possible_cpu(cpu) {
		struct per_cpu_buf *cpu_buf = per_cpu_ptr(pool, cpu);
		struct lttng_tp_buf_entry *entry, *tmp;
		int i = 0;

		list_for_each_entry_safe(entry, tmp, &cpu_buf->free_list, list) {
			list_del(&entry->list);
			kfree(entry);
			i++;
		}
		if (i < LTTNG_TP_MEMPOOL_NR_BUF_PER_CPU) {
			printk(KERN_WARNING "Leak detected in tp-mempool\n");
		}
	}
	free_percpu(pool);
	pool = NULL;
}

void *lttng_tp_mempool_alloc(size_t size)
{
	void *ret;
	struct lttng_tp_buf_entry *entry;
	struct per_cpu_buf *cpu_buf;
	int cpu = smp_processor_id();

	if (size > LTTNG_TP_MEMPOOL_BUF_SIZE) {
		ret = NULL;
		goto end;
	}

	cpu_buf = per_cpu_ptr(pool, cpu);
	if (list_empty(&cpu_buf->free_list)) {
		ret = NULL;
		goto end;
	}

	entry = list_first_entry(&cpu_buf->free_list, struct lttng_tp_buf_entry, list);
	/* Remove the entry from the free list. */
	list_del(&entry->list);

	memset(entry->buf, 0, LTTNG_TP_MEMPOOL_BUF_SIZE);

	ret = (void *) entry->buf;

end:
	return ret;
}

void lttng_tp_mempool_free(void *ptr)
{
	struct lttng_tp_buf_entry *entry;
	struct per_cpu_buf *cpu_buf;

	if (!ptr) {
		goto end;
	}

	entry = container_of(ptr, struct lttng_tp_buf_entry, buf);
	if (!entry) {
		goto end;
	}

	cpu_buf = per_cpu_ptr(pool, entry->cpu);
	if (!cpu_buf) {
		goto end;
	}
	/* Add it to the free list. */
	list_add_tail(&entry->list, &cpu_buf->free_list);

end:
	return;
}
