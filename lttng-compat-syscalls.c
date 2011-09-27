/*
 * lttng-compat-syscalls.c
 *
 * Copyright 2010-2011 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng compat syscall probes.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <asm/ptrace.h>
#include <asm/syscall.h>

#include "ltt-events.h"

extern
void syscall_entry_probe(void *__data, struct pt_regs *regs, long id);

/*
 * Take care of NOARGS not supported by mainline.
 */
#define DECLARE_EVENT_CLASS_NOARGS(name, tstruct, assign, print)
#define DEFINE_EVENT_NOARGS(template, name)
#define TRACE_EVENT_NOARGS(name, struct, assign, print)

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TP_MODULE_OVERRIDE
#define TRACE_INCLUDE_PATH ../instrumentation/syscalls/headers

/* Hijack probe callback for system calls */
#define TP_PROBE_CB(_template)		&syscall_entry_probe
#include "instrumentation/syscalls/headers/compat_syscalls_integers.h"
#include "instrumentation/syscalls/headers/compat_syscalls_pointers.h"
#undef TP_PROBE_CB

#undef TP_MODULE_OVERRIDE
#undef LTTNG_PACKAGE_BUILD
#undef CREATE_TRACE_POINTS

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_template, _name, _nr, _nrargs)	\
	[ _nr ] = {						\
		.func = __event_probe__##_template,		\
		.nrargs = (_nrargs),				\
		.fields = __event_fields___##_template,		\
		.desc = &__event_desc___##_name,		\
	},

#define CREATE_SYSCALL_TABLE

/* Create compatibility syscall table */
const struct trace_syscall_entry compat_sc_table[] = {
#include "instrumentation/syscalls/headers/compat_syscalls_integers.h"
#include "instrumentation/syscalls/headers/compat_syscalls_pointers.h"
};
const size_t compat_sc_table_len = ARRAY_SIZE(compat_sc_table);

#undef CREATE_SYSCALL_TABLE
