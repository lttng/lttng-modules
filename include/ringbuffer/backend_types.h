/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * ringbuffer/backend_types.h
 *
 * Ring buffer backend (types).
 *
 * Copyright (C) 2008-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LIB_RING_BUFFER_BACKEND_TYPES_H
#define _LIB_RING_BUFFER_BACKEND_TYPES_H

#include <linux/cpumask.h>
#include <linux/types.h>
#include <lttng/kernel-version.h>
#include <lttng/cpuhotplug.h>

struct lttng_kernel_ring_buffer_backend_page {
	void *virt;			/* page virtual address (cached) */
	unsigned long pfn;		/* page frame number */
};

struct lttng_kernel_ring_buffer_backend_pages {
	unsigned long mmap_offset;	/* offset of the subbuffer in mmap */
	union v_atomic records_commit;	/* current records committed count */
	union v_atomic records_unread;	/* records to read */
	unsigned long data_size;	/* Amount of data to read from subbuf */
	struct lttng_kernel_ring_buffer_backend_page p[];
};

struct lttng_kernel_ring_buffer_backend_subbuffer {
	/* Identifier for subbuf backend pages. Exchanged atomically. */
	unsigned long id;		/* backend subbuffer identifier */
};

struct lttng_kernel_ring_buffer_backend_counts {
	/*
	 * Counter specific to the sub-buffer location within the ring buffer.
	 * The actual sequence number of the packet within the entire ring
	 * buffer can be derived from the formula nr_subbuffers * seq_cnt +
	 * subbuf_idx.
	 */
	uint64_t seq_cnt;		/* packet sequence number */
};

/*
 * Forward declaration of frontend-specific channel and ring_buffer.
 */
struct lttng_kernel_ring_buffer_channel;
struct lttng_kernel_ring_buffer;

struct lttng_kernel_ring_buffer_backend {
	/* Array of ring_buffer_backend_subbuffer for writer */
	struct lttng_kernel_ring_buffer_backend_subbuffer *buf_wsb;
	/* ring_buffer_backend_subbuffer for reader */
	struct lttng_kernel_ring_buffer_backend_subbuffer buf_rsb;
	/* Array of lib_ring_buffer_backend_counts for the packet counter */
	struct lttng_kernel_ring_buffer_backend_counts *buf_cnt;
	/*
	 * Pointer array of backend pages, for whole buffer.
	 * Indexed by ring_buffer_backend_subbuffer identifier (id) index.
	 */
	struct lttng_kernel_ring_buffer_backend_pages **array;
	unsigned int num_pages_per_subbuf;

	struct lttng_kernel_ring_buffer_channel *chan;		/* Associated channel */
	int cpu;			/* This buffer's cpu. -1 if global. */
	union v_atomic records_read;	/* Number of records read */
	unsigned int allocated:1;	/* is buffer allocated ? */
};

struct channel_backend {
	unsigned long buf_size;		/* Size of the buffer */
	unsigned long subbuf_size;	/* Sub-buffer size */
	unsigned int subbuf_size_order;	/* Order of sub-buffer size */
	unsigned int num_subbuf_order;	/*
					 * Order of number of sub-buffers/buffer
					 * for writer.
					 */
	unsigned int buf_size_order;	/* Order of buffer size */
	unsigned int extra_reader_sb:1;	/* has extra reader subbuffer ? */
	union {
		struct lttng_kernel_ring_buffer *global_buf;	/* Channel global buffer */
		struct lttng_kernel_ring_buffer __percpu *percpu_buf;	/* Channel per-cpu buffers */
	};

	unsigned long num_subbuf;	/* Number of sub-buffers for writer */
	u64 start_tsc;			/* Channel creation TSC value */
	void *priv;			/* Client-specific information */
	void *priv_ops;			/* Client-specific ops pointer */
	void (*release_priv_ops)(void *priv_ops);
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,10,0))
	struct lttng_cpuhp_node cpuhp_prepare;	/* CPU hotplug prepare */
#else
	struct notifier_block cpu_hp_notifier;	 /* CPU hotplug notifier */
#endif
	/*
	 * We need to copy config because the module containing the
	 * source config can vanish before the last reference to this
	 * channel's streams is released.
	 */
	struct lttng_kernel_ring_buffer_config config; /* Ring buffer configuration */
	cpumask_var_t cpumask;		/* Allocated per-cpu buffers cpumask */
	char name[NAME_MAX];		/* Channel name */
};

#endif /* _LIB_RING_BUFFER_BACKEND_TYPES_H */
