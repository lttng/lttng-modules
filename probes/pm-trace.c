/*
 * ltt/probes/pm-trace.c
 *
 * Power Management tracepoint probes.
 *
 * (C) Copyright 2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <trace/pm.h>

void probe_pm_idle_entry(void *_data)
{
	trace_mark_tp(pm, idle_entry, pm_idle_entry,
		probe_pm_idle_entry, "irqstate #1%d",
		irqs_disabled());
}

void probe_pm_idle_exit(void *_data)
{
	trace_mark_tp(pm, idle_exit, pm_idle_exit,
		probe_pm_idle_exit, "irqstate #1%d",
		irqs_disabled());
}

void probe_pm_suspend_entry(void *_data)
{
	trace_mark_tp(pm, suspend_entry, pm_suspend_entry,
		probe_pm_suspend_entry, "irqstate #1%d",
		irqs_disabled());
}

void probe_pm_suspend_exit(void *_data)
{
	trace_mark_tp(pm, suspend_exit, pm_suspend_exit,
		probe_pm_suspend_exit, "irqstate #1%d",
		irqs_disabled());
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Power Management Tracepoint Probes");
