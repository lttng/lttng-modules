/*
 * ltt/probes/ipc-trace.c
 *
 * IPC tracepoint probes.
 *
 * (C) Copyright 2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <trace/ipc.h>

void probe_ipc_msg_create(void *data, long id, int flags)
{
	trace_mark_tp(ipc, msg_create, ipc_msg_create, probe_ipc_msg_create,
		"id %ld flags %d", id, flags);
}

void probe_ipc_sem_create(void *data, long id, int flags)
{
	trace_mark_tp(ipc, sem_create, ipc_sem_create, probe_ipc_sem_create,
		"id %ld flags %d", id, flags);
}

void probe_ipc_shm_create(void *data, long id, int flags)
{
	trace_mark_tp(ipc, shm_create, ipc_shm_create, probe_ipc_shm_create,
		"id %ld flags %d", id, flags);
}

void probe_ipc_call(void *data, unsigned int call, unsigned int first)
{
	trace_mark_tp(ipc, call, ipc_call, probe_ipc_call,
		"call %u first %d", call, first);
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("IPC Tracepoint Probes");
