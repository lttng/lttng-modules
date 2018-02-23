#ifndef LTTNG_TP_MEMPOOL_H
#define LTTNG_TP_MEMPOOL_H

/*
 * lttng-tp-mempool.h
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

#include <linux/percpu.h>

#define LTTNG_TP_MEMPOOL_NR_BUF_PER_CPU 4
#define LTTNG_TP_MEMPOOL_BUF_SIZE 4096

/*
 * Initialize the pool, only performed once. The pool is a set of
 * LTTNG_TP_MEMPOOL_NR_BUF_PER_CPU buffers of size LTTNG_TP_MEMPOOL_BUF_SIZE
 * per-cpu.
 *
 * Returns 0 on success, a negative value on error.
 */
int lttng_tp_mempool_init(void);

/*
 * Destroy the pool and free all the memory allocated.
 */
void lttng_tp_mempool_destroy(void);

/*
 * Ask for a buffer on the current cpu.
 *
 * The pool is per-cpu, but there is no exclusive access guarantee on the
 * per-cpu free-list, the caller needs to ensure it cannot get preempted or
 * interrupted while performing the allocation.
 *
 * The maximum size that can be allocated is LTTNG_TP_MEMPOOL_BUF_SIZE, and the
 * maximum number of buffers allocated simultaneously on the same CPU is
 * LTTNG_TP_MEMPOOL_NR_BUF_PER_CPU.
 *
 * Return a pointer to a buffer on success, NULL on error.
 */
void *lttng_tp_mempool_alloc(size_t size);

/*
 * Release the memory reserved. Same concurrency limitations as the allocation.
 */
void lttng_tp_mempool_free(void *ptr);

#endif /* LTTNG_TP_MEMPOOL_H */
