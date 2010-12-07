/*
 * LTT core in-kernel infrastructure.
 *
 * Copyright 2006 - Mathieu Desnoyers mathieu.desnoyers@polymtl.ca
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/percpu.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/kref.h>
#include <linux/cpu.h>

#include "ltt-tracer-core.h"

int ltt_run_filter_default(struct ltt_session *session,
			   struct ltt_channel *chan,
			   struct ltt_event *event)
{
	return 1;
}

/* This function pointer is protected by a trace activation check */
ltt_run_filter_functor ltt_run_filter = ltt_run_filter_default;
EXPORT_SYMBOL_GPL(ltt_run_filter);

void ltt_filter_register(ltt_run_filter_functor func)
{
	ltt_run_filter = func;
}
EXPORT_SYMBOL_GPL(ltt_filter_register);

void ltt_filter_unregister(void)
{
	ltt_run_filter = ltt_run_filter_default;
}
EXPORT_SYMBOL_GPL(ltt_filter_unregister);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Linux Trace Toolkit Next Generation Tracer Core");
