/*
 * ltt/probes/rcu-trace.c
 *
 * RCU tracepoint probes.
 *
 * (C) Copyright 2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <trace/rcu.h>

#ifdef CONFIG_TREE_RCU
void probe_rcu_tree_callback(void *data, struct rcu_head *head)
{
	trace_mark_tp(rcu, tree_callback, rcu_tree_callback,
		probe_rcu_tree_callback, "func %p", head->func);
}

void probe_rcu_tree_call_rcu(void *data, struct rcu_head *head, unsigned long ip)
{
	trace_mark_tp(rcu, tree_call_rcu, rcu_tree_call_rcu,
		probe_rcu_tree_call_rcu, "func %p ip 0x%lX", head->func, ip);
}

void probe_rcu_tree_call_rcu_bh(void *data, struct rcu_head *head, unsigned long ip)
{
	trace_mark_tp(rcu, tree_call_rcu_bh, rcu_tree_call_rcu_bh,
		probe_rcu_tree_call_rcu_bh, "func %p ip 0x%lX",
		head->func, ip);
}
#endif

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("RCU Tracepoint Probes");
