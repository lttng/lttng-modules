/*
 * ltt/probes/lockdep-trace.c
 *
 * lockdep tracepoint probes.
 *
 * (C) Copyright 2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/lockdep.h>
#include <trace/lockdep.h>

void probe_lockdep_hardirqs_on(void *data, unsigned long retaddr)
{
	trace_mark_tp(lockdep, hardirqs_on, lockdep_hardirqs_on,
		probe_lockdep_hardirqs_on, "retaddr 0x%lX", retaddr);
}

void probe_lockdep_hardirqs_off(void *data, unsigned long retaddr)
{
	trace_mark_tp(lockdep, hardirqs_off, lockdep_hardirqs_off,
		probe_lockdep_hardirqs_off, "retaddr 0x%lX", retaddr);
}

void probe_lockdep_softirqs_on(void *data, unsigned long retaddr)
{
	trace_mark_tp(lockdep, softirqs_on, lockdep_softirqs_on,
		probe_lockdep_softirqs_on, "retaddr 0x%lX", retaddr);
}

void probe_lockdep_softirqs_off(void *data, unsigned long retaddr)
{
	trace_mark_tp(lockdep, softirqs_off, lockdep_softirqs_off,
		probe_lockdep_softirqs_off, "retaddr 0x%lX", retaddr);
}

void probe_lockdep_lock_acquire(void *data, unsigned long retaddr,
		unsigned int subclass, struct lockdep_map *lock, int trylock,
		int read, int hardirqs_off)
{
	trace_mark_tp(lockdep, lock_acquire, lockdep_lock_acquire,
		probe_lockdep_lock_acquire,
		"retaddr 0x%lX subclass %u lock %p trylock %d read %d "
		"hardirqs_off %d",
		retaddr, subclass, lock, trylock, read, hardirqs_off);
}

void probe_lockdep_lock_release(void *data, unsigned long retaddr,
		struct lockdep_map *lock, int nested)
{
	trace_mark_tp(lockdep, lock_release, lockdep_lock_release,
		probe_lockdep_lock_release,
		"retaddr 0x%lX lock %p nested %d",
		retaddr, lock, nested);
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("lockdep Tracepoint Probes");
