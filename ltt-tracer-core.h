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

struct ltt_session;
struct ltt_channel;
struct ltt_event;

typedef int (*ltt_run_filter_functor)(struct ltt_session *session,
				      struct ltt_channel *chan,
				      struct ltt_event *event);

extern ltt_run_filter_functor ltt_run_filter;

extern void ltt_filter_register(ltt_run_filter_functor func);
extern void ltt_filter_unregister(void);

#endif /* LTT_TRACER_CORE_H */
