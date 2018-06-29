/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-tp-mempool.h
 *
 * Copyright (C) 2018 Julien Desfossez <jdesfossez@efficios.com>
 */

#ifndef LTTNG_TP_MEMPOOL_H
#define LTTNG_TP_MEMPOOL_H

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
