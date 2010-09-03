/*
 * Copyright (C) 2005,2006 Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * This contains the core definitions for the Linux Trace Toolkit.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#ifndef LTT_TRACER_CORE_H
#define LTT_TRACER_CORE_H

#include <linux/list.h>
#include <linux/percpu.h>
#include <linux/ltt-core.h>

/* ltt's root dir in debugfs */
#define LTT_ROOT        "ltt"

/*
 * All modifications of ltt_traces must be done by ltt-tracer.c, while holding
 * the semaphore. Only reading of this information can be done elsewhere, with
 * the RCU mechanism : the preemption must be disabled while reading the
 * list.
 */
struct ltt_traces {
	struct list_head setup_head;	/* Pre-allocated traces list */
	struct list_head head;		/* Allocated Traces list */
	unsigned int num_active_traces;	/* Number of active traces */
} ____cacheline_aligned;

extern struct ltt_traces ltt_traces;

/*
 * get dentry of ltt's root dir
 */
struct dentry *get_ltt_root(void);

void put_ltt_root(void);

/* Keep track of trap nesting inside LTT */
DECLARE_PER_CPU(unsigned int, ltt_nesting);

typedef int (*ltt_run_filter_functor)(void *trace, uint16_t eID);

extern ltt_run_filter_functor ltt_run_filter;

extern void ltt_filter_register(ltt_run_filter_functor func);
extern void ltt_filter_unregister(void);

#endif /* LTT_TRACER_CORE_H */
