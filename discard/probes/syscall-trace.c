/*
 * ltt/probes/syscall-trace.c
 *
 * System call tracepoint probes.
 *
 * (C) Copyright 2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <trace/syscall.h>

#include "../ltt-type-serializer.h"


/* kernel_syscall_entry specialized tracepoint probe */

void probe_syscall_entry(void *_data, struct pt_regs *regs, long id);

DEFINE_MARKER_TP(kernel, syscall_entry, syscall_entry,
	probe_syscall_entry, "ip #p%ld syscall_id #2u%u");

notrace void probe_syscall_entry(void *_data, struct pt_regs *regs, long id)
{
	struct marker *marker;
	struct serialize_long_short data;

	data.f1 = instruction_pointer(regs);
	data.f2 = (unsigned short)id;

	marker = &GET_MARKER(kernel, syscall_entry);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(long));
}

/* kernel_syscall_exit specialized tracepoint probe */

void probe_syscall_exit(void *_data, long ret);

DEFINE_MARKER_TP(kernel, syscall_exit, syscall_exit,
	probe_syscall_exit, "ret %ld");

notrace void probe_syscall_exit(void *_data, long ret)
{
	struct marker *marker;

	marker = &GET_MARKER(kernel, syscall_exit);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&ret, sizeof(ret), sizeof(ret));
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("syscall Tracepoint Probes");
