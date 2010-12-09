/*
 * ltt/probes/trap-trace.c
 *
 * Trap tracepoint probes.
 *
 * (C) Copyright 2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <trace/trap.h>

#include "../ltt-type-serializer.h"

/* kernel_trap_entry specialized tracepoint probe */

void probe_trap_entry(void *_data, struct pt_regs *regs, long id);

DEFINE_MARKER_TP(kernel, trap_entry, trap_entry,
	probe_trap_entry, "ip #p%ld trap_id #2u%u");

notrace void probe_trap_entry(void *_data, struct pt_regs *regs, long id)
{
	struct marker *marker;
	struct serialize_long_short data;

	if (likely(regs))
		data.f1 = instruction_pointer(regs);
	else
		data.f1 = 0UL;
	data.f2 = (unsigned short)id;

	marker = &GET_MARKER(kernel, trap_entry);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(long));
}

/* kernel_syscall_exit specialized tracepoint probe */

void probe_trap_exit(void *_data);

DEFINE_MARKER_TP(kernel, trap_exit, trap_exit,
	probe_trap_exit, MARK_NOARGS);

notrace void probe_trap_exit(void *_data)
{
	struct marker *marker;

	marker = &GET_MARKER(kernel, trap_exit);
	ltt_specialized_trace(marker, marker->single.probe_private,
		NULL, 0, 0);
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Trap Tracepoint Probes");
