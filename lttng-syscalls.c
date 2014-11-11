/*
 * lttng-syscalls.c
 *
 * LTTng syscall probes.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/err.h>
#include <linux/bitmap.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/seq_file.h>
#include <linux/stringify.h>
#include <linux/file.h>
#include <linux/anon_inodes.h>
#include <asm/ptrace.h>
#include <asm/syscall.h>

#include "lib/bitfield.h"
#include "wrapper/tracepoint.h"
#include "wrapper/file.h"
#include "lttng-events.h"

#ifndef CONFIG_COMPAT
# ifndef is_compat_task
#  define is_compat_task()	(0)
# endif
#endif

enum sc_type {
	SC_TYPE_ENTRY,
	SC_TYPE_EXIT,
	SC_TYPE_COMPAT_ENTRY,
	SC_TYPE_COMPAT_EXIT,
};

#define SYSCALL_ENTRY_TOK		syscall_entry_
#define COMPAT_SYSCALL_ENTRY_TOK	compat_syscall_entry_
#define SYSCALL_EXIT_TOK		syscall_exit_
#define COMPAT_SYSCALL_EXIT_TOK		compat_syscall_exit_

#define SYSCALL_ENTRY_STR		__stringify(SYSCALL_ENTRY_TOK)
#define COMPAT_SYSCALL_ENTRY_STR	__stringify(COMPAT_SYSCALL_ENTRY_TOK)
#define SYSCALL_EXIT_STR		__stringify(SYSCALL_EXIT_TOK)
#define COMPAT_SYSCALL_EXIT_STR		__stringify(COMPAT_SYSCALL_EXIT_TOK)

static
void syscall_entry_probe(void *__data, struct pt_regs *regs, long id);
static
void syscall_exit_probe(void *__data, struct pt_regs *regs, long ret);

/*
 * Forward declarations for old kernels.
 */
struct mmsghdr;
struct rlimit64;
struct oldold_utsname;
struct old_utsname;
struct sel_arg_struct;
struct mmap_arg_struct;

#ifdef IA32_NR_syscalls
#define NR_compat_syscalls IA32_NR_syscalls
#else
#define NR_compat_syscalls NR_syscalls
#endif

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TP_MODULE_NOINIT
#define TRACE_INCLUDE_PATH ../instrumentation/syscalls/headers

#define PARAMS(args...)	args

/* Handle unknown syscalls */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM syscalls_unknown
#include "instrumentation/syscalls/headers/syscalls_unknown.h"
#undef TRACE_SYSTEM

#define SC_ENTER

#undef sc_exit
#define sc_exit(...)
#undef sc_in
#define sc_in(...)	__VA_ARGS__
#undef sc_out
#define sc_out(...)
#undef sc_inout
#define sc_inout(...)	__VA_ARGS__

/* Hijack probe callback for system call enter */
#undef TP_PROBE_CB
#define TP_PROBE_CB(_template)		&syscall_entry_probe
#define SC_LTTNG_TRACEPOINT_EVENT(_name, _proto, _args, _fields) \
	LTTNG_TRACEPOINT_EVENT(syscall_entry_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CODE(_name, _proto, _args, _locvar, _code, _fields) \
	LTTNG_TRACEPOINT_EVENT_CODE(syscall_entry_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_locvar), PARAMS(_code),					\
		PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(_name, _fields) \
	LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(syscall_entry_##_name, PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(_template, _name)		\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscall_entry_##_template, syscall_entry_##_name)
#undef TRACE_SYSTEM
#define TRACE_SYSTEM syscall_entry_integers
#define TRACE_INCLUDE_FILE syscalls_integers
#include "instrumentation/syscalls/headers/syscalls_integers.h"
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#define TRACE_SYSTEM syscall_entry_pointers
#define TRACE_INCLUDE_FILE syscalls_pointers
#include "instrumentation/syscalls/headers/syscalls_pointers.h"
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#undef SC_LTTNG_TRACEPOINT_EVENT_CODE
#undef SC_LTTNG_TRACEPOINT_EVENT
#undef SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS
#undef SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#undef TP_PROBE_CB
#undef _TRACE_SYSCALLS_INTEGERS_H
#undef _TRACE_SYSCALLS_POINTERS_H

/* Hijack probe callback for compat system call enter */
#define TP_PROBE_CB(_template)		&syscall_entry_probe
#define SC_LTTNG_TRACEPOINT_EVENT(_name, _proto, _args, _fields) \
	LTTNG_TRACEPOINT_EVENT(compat_syscall_entry_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CODE(_name, _proto, _args, _locvar, _code, _fields) \
	LTTNG_TRACEPOINT_EVENT_CODE(compat_syscall_entry_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_locvar), PARAMS(_code), PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(_name, _fields) \
	LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(compat_syscall_entry_##_name, PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(_template, _name)		\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(compat_syscall_entry_##_template, \
		compat_syscall_entry_##_name)
#define TRACE_SYSTEM compat_syscall_entry_integers
#define TRACE_INCLUDE_FILE compat_syscalls_integers
#include "instrumentation/syscalls/headers/compat_syscalls_integers.h"
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#define TRACE_SYSTEM compat_syscall_entry_pointers
#define TRACE_INCLUDE_FILE compat_syscalls_pointers
#include "instrumentation/syscalls/headers/compat_syscalls_pointers.h"
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#undef SC_LTTNG_TRACEPOINT_EVENT_CODE
#undef SC_LTTNG_TRACEPOINT_EVENT
#undef SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS
#undef SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#undef TP_PROBE_CB
#undef _TRACE_SYSCALLS_INTEGERS_H
#undef _TRACE_SYSCALLS_POINTERS_H

#undef SC_ENTER

#define SC_EXIT

#undef sc_exit
#define sc_exit(...)		__VA_ARGS__
#undef sc_in
#define sc_in(...)
#undef sc_out
#define sc_out(...)		__VA_ARGS__
#undef sc_inout
#define sc_inout(...)		__VA_ARGS__

/* Hijack probe callback for system call exit */
#define TP_PROBE_CB(_template)		&syscall_exit_probe
#define SC_LTTNG_TRACEPOINT_EVENT(_name, _proto, _args, _fields) \
	LTTNG_TRACEPOINT_EVENT(syscall_exit_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CODE(_name, _proto, _args, _locvar, _code, _fields) \
	LTTNG_TRACEPOINT_EVENT_CODE(syscall_exit_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_locvar), PARAMS(_code), PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(_name, _fields) \
	LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(syscall_exit_##_name, PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(_template, _name) 		\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscall_exit_##_template,	\
		syscall_exit_##_name)
#define TRACE_SYSTEM syscall_exit_integers
#define TRACE_INCLUDE_FILE syscalls_integers
#include "instrumentation/syscalls/headers/syscalls_integers.h"
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#define TRACE_SYSTEM syscall_exit_pointers
#define TRACE_INCLUDE_FILE syscalls_pointers
#include "instrumentation/syscalls/headers/syscalls_pointers.h"
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#undef SC_LTTNG_TRACEPOINT_EVENT_CODE
#undef SC_LTTNG_TRACEPOINT_EVENT
#undef SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS
#undef SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#undef TP_PROBE_CB
#undef _TRACE_SYSCALLS_INTEGERS_H
#undef _TRACE_SYSCALLS_POINTERS_H


/* Hijack probe callback for compat system call exit */
#define TP_PROBE_CB(_template)		&syscall_exit_probe
#define SC_LTTNG_TRACEPOINT_EVENT(_name, _proto, _args, _fields) \
	LTTNG_TRACEPOINT_EVENT(compat_syscall_exit_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CODE(_name, _proto, _args, _locvar, _code, _fields) \
	LTTNG_TRACEPOINT_EVENT_CODE(compat_syscall_exit_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_locvar), PARAMS(_code), PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(_name, _fields) \
	LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(compat_syscall_exit_##_name, PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(_template, _name)		\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(compat_syscall_exit_##_template, \
		compat_syscall_exit_##_name)
#define TRACE_SYSTEM compat_syscall_exit_integers
#define TRACE_INCLUDE_FILE compat_syscalls_integers
#include "instrumentation/syscalls/headers/compat_syscalls_integers.h"
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#define TRACE_SYSTEM compat_syscall_exit_pointers
#define TRACE_INCLUDE_FILE compat_syscalls_pointers
#include "instrumentation/syscalls/headers/compat_syscalls_pointers.h"
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#undef SC_LTTNG_TRACEPOINT_EVENT_CODE
#undef SC_LTTNG_TRACEPOINT_EVENT
#undef SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS
#undef SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#undef TP_PROBE_CB
#undef _TRACE_SYSCALLS_INTEGERS_H
#undef _TRACE_SYSCALLS_POINTERS_H

#undef SC_EXIT

#undef TP_MODULE_NOINIT
#undef LTTNG_PACKAGE_BUILD
#undef CREATE_TRACE_POINTS

struct trace_syscall_entry {
	void *func;
	const struct lttng_event_desc *desc;
	const struct lttng_event_field *fields;
	unsigned int nrargs;
};

#define CREATE_SYSCALL_TABLE

#define SC_ENTER

#undef sc_exit
#define sc_exit(...)

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_template, _name, _nr, _nrargs)	\
	[ _nr ] = {						\
		.func = __event_probe__syscall_entry_##_template, \
		.nrargs = (_nrargs),				\
		.fields = __event_fields___syscall_entry_##_template, \
		.desc = &__event_desc___syscall_entry_##_name,	\
	},

/* Syscall enter tracing table */
static const struct trace_syscall_entry sc_table[] = {
#include "instrumentation/syscalls/headers/syscalls_integers.h"
#include "instrumentation/syscalls/headers/syscalls_pointers.h"
};

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_template, _name, _nr, _nrargs)	\
	[ _nr ] = {						\
		.func = __event_probe__compat_syscall_entry_##_template, \
		.nrargs = (_nrargs),				\
		.fields = __event_fields___compat_syscall_entry_##_template, \
		.desc = &__event_desc___compat_syscall_entry_##_name, \
	},

/* Compat syscall enter table */
const struct trace_syscall_entry compat_sc_table[] = {
#include "instrumentation/syscalls/headers/compat_syscalls_integers.h"
#include "instrumentation/syscalls/headers/compat_syscalls_pointers.h"
};

#undef SC_ENTER

#define SC_EXIT

#undef sc_exit
#define sc_exit(...)		__VA_ARGS__

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_template, _name, _nr, _nrargs)	\
	[ _nr ] = {						\
		.func = __event_probe__syscall_exit_##_template, \
		.nrargs = (_nrargs),				\
		.fields = __event_fields___syscall_exit_##_template, \
		.desc = &__event_desc___syscall_exit_##_name, \
	},

/* Syscall exit table */
static const struct trace_syscall_entry sc_exit_table[] = {
#include "instrumentation/syscalls/headers/syscalls_integers.h"
#include "instrumentation/syscalls/headers/syscalls_pointers.h"
};

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_template, _name, _nr, _nrargs)	\
	[ _nr ] = {						\
		.func = __event_probe__compat_syscall_exit_##_template, \
		.nrargs = (_nrargs),				\
		.fields = __event_fields___compat_syscall_exit_##_template, \
		.desc = &__event_desc___compat_syscall_exit_##_name, \
	},

/* Compat syscall exit table */
const struct trace_syscall_entry compat_sc_exit_table[] = {
#include "instrumentation/syscalls/headers/compat_syscalls_integers.h"
#include "instrumentation/syscalls/headers/compat_syscalls_pointers.h"
};

#undef SC_EXIT

#undef CREATE_SYSCALL_TABLE

struct lttng_syscall_filter {
	DECLARE_BITMAP(sc, NR_syscalls);
	DECLARE_BITMAP(sc_compat, NR_compat_syscalls);
};

static void syscall_entry_unknown(struct lttng_event *event,
	struct pt_regs *regs, unsigned int id)
{
	unsigned long args[UNKNOWN_SYSCALL_NRARGS];

	syscall_get_arguments(current, regs, 0, UNKNOWN_SYSCALL_NRARGS, args);
	if (unlikely(is_compat_task()))
		__event_probe__compat_syscall_entry_unknown(event, id, args);
	else
		__event_probe__syscall_entry_unknown(event, id, args);
}

void syscall_entry_probe(void *__data, struct pt_regs *regs, long id)
{
	struct lttng_channel *chan = __data;
	struct lttng_event *event, *unknown_event;
	const struct trace_syscall_entry *table, *entry;
	size_t table_len;

	if (unlikely(is_compat_task())) {
		struct lttng_syscall_filter *filter;

		filter = rcu_dereference(chan->sc_filter);
		if (filter) {
			if (id < 0 || id >= NR_compat_syscalls
				|| !test_bit(id, filter->sc_compat)) {
				/* System call filtered out. */
				return;
			}
		}
		table = compat_sc_table;
		table_len = ARRAY_SIZE(compat_sc_table);
		unknown_event = chan->sc_compat_unknown;
	} else {
		struct lttng_syscall_filter *filter;

		filter = rcu_dereference(chan->sc_filter);
		if (filter) {
			if (id < 0 || id >= NR_syscalls
				|| !test_bit(id, filter->sc)) {
				/* System call filtered out. */
				return;
			}
		}
		table = sc_table;
		table_len = ARRAY_SIZE(sc_table);
		unknown_event = chan->sc_unknown;
	}
	if (unlikely(id < 0 || id >= table_len)) {
		syscall_entry_unknown(unknown_event, regs, id);
		return;
	}
	if (unlikely(is_compat_task()))
		event = chan->compat_sc_table[id];
	else
		event = chan->sc_table[id];
	if (unlikely(!event)) {
		syscall_entry_unknown(unknown_event, regs, id);
		return;
	}
	entry = &table[id];
	WARN_ON_ONCE(!entry);

	switch (entry->nrargs) {
	case 0:
	{
		void (*fptr)(void *__data) = entry->func;

		fptr(event);
		break;
	}
	case 1:
	{
		void (*fptr)(void *__data, unsigned long arg0) = entry->func;
		unsigned long args[1];

		syscall_get_arguments(current, regs, 0, entry->nrargs, args);
		fptr(event, args[0]);
		break;
	}
	case 2:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1) = entry->func;
		unsigned long args[2];

		syscall_get_arguments(current, regs, 0, entry->nrargs, args);
		fptr(event, args[0], args[1]);
		break;
	}
	case 3:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2) = entry->func;
		unsigned long args[3];

		syscall_get_arguments(current, regs, 0, entry->nrargs, args);
		fptr(event, args[0], args[1], args[2]);
		break;
	}
	case 4:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2,
			unsigned long arg3) = entry->func;
		unsigned long args[4];

		syscall_get_arguments(current, regs, 0, entry->nrargs, args);
		fptr(event, args[0], args[1], args[2], args[3]);
		break;
	}
	case 5:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2,
			unsigned long arg3,
			unsigned long arg4) = entry->func;
		unsigned long args[5];

		syscall_get_arguments(current, regs, 0, entry->nrargs, args);
		fptr(event, args[0], args[1], args[2], args[3], args[4]);
		break;
	}
	case 6:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2,
			unsigned long arg3,
			unsigned long arg4,
			unsigned long arg5) = entry->func;
		unsigned long args[6];

		syscall_get_arguments(current, regs, 0, entry->nrargs, args);
		fptr(event, args[0], args[1], args[2],
			args[3], args[4], args[5]);
		break;
	}
	default:
		break;
	}
}

static void syscall_exit_unknown(struct lttng_event *event,
	struct pt_regs *regs, int id, long ret)
{
	unsigned long args[UNKNOWN_SYSCALL_NRARGS];

	syscall_get_arguments(current, regs, 0, UNKNOWN_SYSCALL_NRARGS, args);
	if (unlikely(is_compat_task()))
		__event_probe__compat_syscall_exit_unknown(event, id, ret,
			args);
	else
		__event_probe__syscall_exit_unknown(event, id, ret, args);
}

void syscall_exit_probe(void *__data, struct pt_regs *regs, long ret)
{
	struct lttng_channel *chan = __data;
	struct lttng_event *event, *unknown_event;
	const struct trace_syscall_entry *table, *entry;
	size_t table_len;
	long id;

	id = syscall_get_nr(current, regs);
	if (unlikely(is_compat_task())) {
		struct lttng_syscall_filter *filter;

		filter = rcu_dereference(chan->sc_filter);
		if (filter) {
			if (id < 0 || id >= NR_compat_syscalls
				|| !test_bit(id, filter->sc_compat)) {
				/* System call filtered out. */
				return;
			}
		}
		table = compat_sc_exit_table;
		table_len = ARRAY_SIZE(compat_sc_exit_table);
		unknown_event = chan->compat_sc_exit_unknown;
	} else {
		struct lttng_syscall_filter *filter;

		filter = rcu_dereference(chan->sc_filter);
		if (filter) {
			if (id < 0 || id >= NR_syscalls
				|| !test_bit(id, filter->sc)) {
				/* System call filtered out. */
				return;
			}
		}
		table = sc_exit_table;
		table_len = ARRAY_SIZE(sc_exit_table);
		unknown_event = chan->sc_exit_unknown;
	}
	if (unlikely(id < 0 || id >= table_len)) {
		syscall_exit_unknown(unknown_event, regs, id, ret);
		return;
	}
	if (unlikely(is_compat_task()))
		event = chan->compat_sc_exit_table[id];
	else
		event = chan->sc_exit_table[id];
	if (unlikely(!event)) {
		syscall_exit_unknown(unknown_event, regs, id, ret);
		return;
	}
	entry = &table[id];
	WARN_ON_ONCE(!entry);

	switch (entry->nrargs) {
	case 0:
	{
		void (*fptr)(void *__data, long ret) = entry->func;

		fptr(event, ret);
		break;
	}
	case 1:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0) = entry->func;
		unsigned long args[1];

		syscall_get_arguments(current, regs, 0, entry->nrargs, args);
		fptr(event, ret, args[0]);
		break;
	}
	case 2:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0,
			unsigned long arg1) = entry->func;
		unsigned long args[2];

		syscall_get_arguments(current, regs, 0, entry->nrargs, args);
		fptr(event, ret, args[0], args[1]);
		break;
	}
	case 3:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2) = entry->func;
		unsigned long args[3];

		syscall_get_arguments(current, regs, 0, entry->nrargs, args);
		fptr(event, ret, args[0], args[1], args[2]);
		break;
	}
	case 4:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2,
			unsigned long arg3) = entry->func;
		unsigned long args[4];

		syscall_get_arguments(current, regs, 0, entry->nrargs, args);
		fptr(event, ret, args[0], args[1], args[2], args[3]);
		break;
	}
	case 5:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2,
			unsigned long arg3,
			unsigned long arg4) = entry->func;
		unsigned long args[5];

		syscall_get_arguments(current, regs, 0, entry->nrargs, args);
		fptr(event, ret, args[0], args[1], args[2], args[3], args[4]);
		break;
	}
	case 6:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2,
			unsigned long arg3,
			unsigned long arg4,
			unsigned long arg5) = entry->func;
		unsigned long args[6];

		syscall_get_arguments(current, regs, 0, entry->nrargs, args);
		fptr(event, ret, args[0], args[1], args[2],
			args[3], args[4], args[5]);
		break;
	}
	default:
		break;
	}
}

/*
 * noinline to diminish caller stack size.
 * Should be called with sessions lock held.
 */
static
int fill_table(const struct trace_syscall_entry *table, size_t table_len,
	struct lttng_event **chan_table, struct lttng_channel *chan,
	void *filter, enum sc_type type)
{
	const struct lttng_event_desc *desc;
	unsigned int i;

	/* Allocate events for each syscall, insert into table */
	for (i = 0; i < table_len; i++) {
		struct lttng_kernel_event ev;
		desc = table[i].desc;

		if (!desc) {
			/* Unknown syscall */
			continue;
		}
		/*
		 * Skip those already populated by previous failed
		 * register for this channel.
		 */
		if (chan_table[i])
			continue;
		memset(&ev, 0, sizeof(ev));
		switch (type) {
		case SC_TYPE_ENTRY:
			strncpy(ev.name, SYSCALL_ENTRY_STR,
				LTTNG_KERNEL_SYM_NAME_LEN);
			break;
		case SC_TYPE_EXIT:
			strncpy(ev.name, SYSCALL_EXIT_STR,
				LTTNG_KERNEL_SYM_NAME_LEN);
			break;
		case SC_TYPE_COMPAT_ENTRY:
			strncpy(ev.name, COMPAT_SYSCALL_ENTRY_STR,
				LTTNG_KERNEL_SYM_NAME_LEN);
			break;
		case SC_TYPE_COMPAT_EXIT:
			strncpy(ev.name, COMPAT_SYSCALL_EXIT_STR,
				LTTNG_KERNEL_SYM_NAME_LEN);
			break;
		default:
			BUG_ON(1);
			break;
		}
		strncat(ev.name, desc->name,
			LTTNG_KERNEL_SYM_NAME_LEN - strlen(ev.name) - 1);
		ev.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_SYSCALL;
		chan_table[i] = _lttng_event_create(chan, &ev, filter,
						desc, ev.instrumentation);
		WARN_ON_ONCE(!chan_table[i]);
		if (IS_ERR(chan_table[i])) {
			/*
			 * If something goes wrong in event registration
			 * after the first one, we have no choice but to
			 * leave the previous events in there, until
			 * deleted by session teardown.
			 */
			return PTR_ERR(chan_table[i]);
		}
	}
	return 0;
}

/*
 * Should be called with sessions lock held.
 */
int lttng_syscalls_register(struct lttng_channel *chan, void *filter)
{
	struct lttng_kernel_event ev;
	int ret;

	wrapper_vmalloc_sync_all();

	if (!chan->sc_table) {
		/* create syscall table mapping syscall to events */
		chan->sc_table = kzalloc(sizeof(struct lttng_event *)
					* ARRAY_SIZE(sc_table), GFP_KERNEL);
		if (!chan->sc_table)
			return -ENOMEM;
	}
	if (!chan->sc_exit_table) {
		/* create syscall table mapping syscall to events */
		chan->sc_exit_table = kzalloc(sizeof(struct lttng_event *)
					* ARRAY_SIZE(sc_exit_table), GFP_KERNEL);
		if (!chan->sc_exit_table)
			return -ENOMEM;
	}


#ifdef CONFIG_COMPAT
	if (!chan->compat_sc_table) {
		/* create syscall table mapping compat syscall to events */
		chan->compat_sc_table = kzalloc(sizeof(struct lttng_event *)
					* ARRAY_SIZE(compat_sc_table), GFP_KERNEL);
		if (!chan->compat_sc_table)
			return -ENOMEM;
	}

	if (!chan->compat_sc_exit_table) {
		/* create syscall table mapping compat syscall to events */
		chan->compat_sc_exit_table = kzalloc(sizeof(struct lttng_event *)
					* ARRAY_SIZE(compat_sc_exit_table), GFP_KERNEL);
		if (!chan->compat_sc_exit_table)
			return -ENOMEM;
	}
#endif
	if (!chan->sc_unknown) {
		const struct lttng_event_desc *desc =
			&__event_desc___syscall_entry_unknown;

		memset(&ev, 0, sizeof(ev));
		strncpy(ev.name, desc->name, LTTNG_KERNEL_SYM_NAME_LEN);
		ev.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_SYSCALL;
		chan->sc_unknown = _lttng_event_create(chan, &ev, filter,
						desc,
						ev.instrumentation);
		WARN_ON_ONCE(!chan->sc_unknown);
		if (IS_ERR(chan->sc_unknown)) {
			return PTR_ERR(chan->sc_unknown);
		}
	}

	if (!chan->sc_compat_unknown) {
		const struct lttng_event_desc *desc =
			&__event_desc___compat_syscall_entry_unknown;

		memset(&ev, 0, sizeof(ev));
		strncpy(ev.name, desc->name, LTTNG_KERNEL_SYM_NAME_LEN);
		ev.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_SYSCALL;
		chan->sc_compat_unknown = _lttng_event_create(chan, &ev, filter,
						desc,
						ev.instrumentation);
		WARN_ON_ONCE(!chan->sc_unknown);
		if (IS_ERR(chan->sc_compat_unknown)) {
			return PTR_ERR(chan->sc_compat_unknown);
		}
	}

	if (!chan->compat_sc_exit_unknown) {
		const struct lttng_event_desc *desc =
			&__event_desc___compat_syscall_exit_unknown;

		memset(&ev, 0, sizeof(ev));
		strncpy(ev.name, desc->name, LTTNG_KERNEL_SYM_NAME_LEN);
		ev.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_SYSCALL;
		chan->compat_sc_exit_unknown = _lttng_event_create(chan, &ev,
						filter, desc,
						ev.instrumentation);
		WARN_ON_ONCE(!chan->compat_sc_exit_unknown);
		if (IS_ERR(chan->compat_sc_exit_unknown)) {
			return PTR_ERR(chan->compat_sc_exit_unknown);
		}
	}

	if (!chan->sc_exit_unknown) {
		const struct lttng_event_desc *desc =
			&__event_desc___syscall_exit_unknown;

		memset(&ev, 0, sizeof(ev));
		strncpy(ev.name, desc->name, LTTNG_KERNEL_SYM_NAME_LEN);
		ev.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_SYSCALL;
		chan->sc_exit_unknown = _lttng_event_create(chan, &ev, filter,
						desc, ev.instrumentation);
		WARN_ON_ONCE(!chan->sc_exit_unknown);
		if (IS_ERR(chan->sc_exit_unknown)) {
			return PTR_ERR(chan->sc_exit_unknown);
		}
	}

	ret = fill_table(sc_table, ARRAY_SIZE(sc_table),
			chan->sc_table, chan, filter, SC_TYPE_ENTRY);
	if (ret)
		return ret;
	ret = fill_table(sc_exit_table, ARRAY_SIZE(sc_exit_table),
			chan->sc_exit_table, chan, filter, SC_TYPE_EXIT);
	if (ret)
		return ret;

#ifdef CONFIG_COMPAT
	ret = fill_table(compat_sc_table, ARRAY_SIZE(compat_sc_table),
			chan->compat_sc_table, chan, filter,
			SC_TYPE_COMPAT_ENTRY);
	if (ret)
		return ret;
	ret = fill_table(compat_sc_exit_table, ARRAY_SIZE(compat_sc_exit_table),
			chan->compat_sc_exit_table, chan, filter,
			SC_TYPE_COMPAT_EXIT);
	if (ret)
		return ret;
#endif
	if (!chan->sys_enter_registered) {
		ret = lttng_wrapper_tracepoint_probe_register("sys_enter",
				(void *) syscall_entry_probe, chan);
		if (ret)
			return ret;
		chan->sys_enter_registered = 1;
	}
	/*
	 * We change the name of sys_exit tracepoint due to namespace
	 * conflict with sys_exit syscall entry.
	 */
	if (!chan->sys_exit_registered) {
		ret = lttng_wrapper_tracepoint_probe_register("sys_exit",
				(void *) syscall_exit_probe, chan);
		if (ret) {
			WARN_ON_ONCE(lttng_wrapper_tracepoint_probe_unregister("sys_enter",
				(void *) syscall_entry_probe, chan));
			return ret;
		}
		chan->sys_exit_registered = 1;
	}
	return ret;
}

/*
 * Only called at session destruction.
 */
int lttng_syscalls_unregister(struct lttng_channel *chan)
{
	int ret;

	if (!chan->sc_table)
		return 0;
	if (chan->sys_enter_registered) {
		ret = lttng_wrapper_tracepoint_probe_unregister("sys_exit",
				(void *) syscall_exit_probe, chan);
		if (ret)
			return ret;
		chan->sys_enter_registered = 0;
	}
	if (chan->sys_exit_registered) {
		ret = lttng_wrapper_tracepoint_probe_unregister("sys_enter",
				(void *) syscall_entry_probe, chan);
		if (ret)
			return ret;
		chan->sys_exit_registered = 0;
	}
	/* lttng_event destroy will be performed by lttng_session_destroy() */
	kfree(chan->sc_table);
	kfree(chan->sc_exit_table);
#ifdef CONFIG_COMPAT
	kfree(chan->compat_sc_table);
	kfree(chan->compat_sc_exit_table);
#endif
	kfree(chan->sc_filter);
	return 0;
}

static
int get_syscall_nr(const char *syscall_name)
{
	int syscall_nr = -1;
	int i;

	for (i = 0; i < ARRAY_SIZE(sc_table); i++) {
		const struct trace_syscall_entry *entry;
		const char *it_name;

		entry = &sc_table[i];
		if (!entry->desc)
			continue;
		it_name = entry->desc->name;
		it_name += strlen(SYSCALL_ENTRY_STR);
		if (!strcmp(syscall_name, it_name)) {
			syscall_nr = i;
			break;
		}
	}
	return syscall_nr;
}

static
int get_compat_syscall_nr(const char *syscall_name)
{
	int syscall_nr = -1;
	int i;

	for (i = 0; i < ARRAY_SIZE(compat_sc_table); i++) {
		const struct trace_syscall_entry *entry;
		const char *it_name;

		entry = &compat_sc_table[i];
		if (!entry->desc)
			continue;
		it_name = entry->desc->name;
		it_name += strlen(COMPAT_SYSCALL_ENTRY_STR);
		if (!strcmp(syscall_name, it_name)) {
			syscall_nr = i;
			break;
		}
	}
	return syscall_nr;
}

static
uint32_t get_sc_tables_len(void)
{
	return ARRAY_SIZE(sc_table) + ARRAY_SIZE(compat_sc_table);
}

int lttng_syscall_filter_enable(struct lttng_channel *chan,
		const char *name)
{
	int syscall_nr, compat_syscall_nr, ret;
	struct lttng_syscall_filter *filter;

	WARN_ON_ONCE(!chan->sc_table);

	if (!name) {
		/* Enable all system calls by removing filter */
		if (chan->sc_filter) {
			filter = chan->sc_filter;
			rcu_assign_pointer(chan->sc_filter, NULL);
			synchronize_trace();
			kfree(filter);
		}
		chan->syscall_all = 1;
		return 0;
	}

	if (!chan->sc_filter) {
		if (chan->syscall_all) {
			/*
			 * All syscalls are already enabled.
			 */
			return -EEXIST;
		}
		filter = kzalloc(sizeof(struct lttng_syscall_filter),
				GFP_KERNEL);
		if (!filter)
			return -ENOMEM;
	} else {
		filter = chan->sc_filter;
	}
	syscall_nr = get_syscall_nr(name);
	compat_syscall_nr = get_compat_syscall_nr(name);
	if (syscall_nr < 0 && compat_syscall_nr < 0) {
		ret = -ENOENT;
		goto error;
	}
	if (syscall_nr >= 0) {
		if (test_bit(syscall_nr, filter->sc)) {
			ret = -EEXIST;
			goto error;
		}
		bitmap_set(filter->sc, syscall_nr, 1);
	}
	if (compat_syscall_nr >= 0) {
		if (test_bit(compat_syscall_nr, filter->sc_compat)) {
			ret = -EEXIST;
			goto error;
		}
		bitmap_set(filter->sc_compat, compat_syscall_nr, 1);
	}
	if (!chan->sc_filter)
		rcu_assign_pointer(chan->sc_filter, filter);
	return 0;

error:
	if (!chan->sc_filter)
		kfree(filter);
	return ret;
}

int lttng_syscall_filter_disable(struct lttng_channel *chan,
		const char *name)
{
	int syscall_nr, compat_syscall_nr, ret;
	struct lttng_syscall_filter *filter;

	WARN_ON_ONCE(!chan->sc_table);

	if (!chan->sc_filter) {
		if (!chan->syscall_all)
			return -EEXIST;
		filter = kzalloc(sizeof(struct lttng_syscall_filter),
				GFP_KERNEL);
		if (!filter)
			return -ENOMEM;
		/* Trace all system calls, then apply disable. */
		bitmap_set(filter->sc, 0, NR_syscalls);
		bitmap_set(filter->sc_compat, 0, NR_compat_syscalls);
	} else {
		filter = chan->sc_filter;
	}

	if (!name) {
		/* Fail if all syscalls are already disabled. */
		if (bitmap_empty(filter->sc, NR_syscalls)
			&& bitmap_empty(filter->sc_compat,
				NR_compat_syscalls)) {
			ret = -EEXIST;
			goto error;
		}

		/* Disable all system calls */
		bitmap_clear(filter->sc, 0, NR_syscalls);
		bitmap_clear(filter->sc_compat, 0, NR_compat_syscalls);
		goto apply_filter;
	}
	syscall_nr = get_syscall_nr(name);
	compat_syscall_nr = get_compat_syscall_nr(name);
	if (syscall_nr < 0 && compat_syscall_nr < 0) {
		ret = -ENOENT;
		goto error;
	}
	if (syscall_nr >= 0) {
		if (!test_bit(syscall_nr, filter->sc)) {
			ret = -EEXIST;
			goto error;
		}
		bitmap_clear(filter->sc, syscall_nr, 1);
	}
	if (compat_syscall_nr >= 0) {
		if (!test_bit(compat_syscall_nr, filter->sc_compat)) {
			ret = -EEXIST;
			goto error;
		}
		bitmap_clear(filter->sc_compat, compat_syscall_nr, 1);
	}
apply_filter:
	if (!chan->sc_filter)
		rcu_assign_pointer(chan->sc_filter, filter);
	chan->syscall_all = 0;
	return 0;

error:
	if (!chan->sc_filter)
		kfree(filter);
	return ret;
}

static
const struct trace_syscall_entry *syscall_list_get_entry(loff_t *pos)
{
	const struct trace_syscall_entry *entry;
	int iter = 0;

	for (entry = sc_table;
			entry < sc_table + ARRAY_SIZE(sc_table);
			 entry++) {
		if (iter++ >= *pos)
			return entry;
	}
	for (entry = compat_sc_table;
			entry < compat_sc_table + ARRAY_SIZE(compat_sc_table);
			 entry++) {
		if (iter++ >= *pos)
			return entry;
	}
	/* End of list */
	return NULL;
}

static
void *syscall_list_start(struct seq_file *m, loff_t *pos)
{
	return (void *) syscall_list_get_entry(pos);
}

static
void *syscall_list_next(struct seq_file *m, void *p, loff_t *ppos)
{
	(*ppos)++;
	return (void *) syscall_list_get_entry(ppos);
}

static
void syscall_list_stop(struct seq_file *m, void *p)
{
}

static
int get_sc_table(const struct trace_syscall_entry *entry,
		const struct trace_syscall_entry **table,
		unsigned int *bitness)
{
	if (entry >= sc_table && entry < sc_table + ARRAY_SIZE(sc_table)) {
		if (bitness)
			*bitness = BITS_PER_LONG;
		if (table)
			*table = sc_table;
		return 0;
	}
	if (!(entry >= compat_sc_table
			&& entry < compat_sc_table + ARRAY_SIZE(compat_sc_table))) {
		return -EINVAL;
	}
	if (bitness)
		*bitness = 32;
	if (table)
		*table = compat_sc_table;
	return 0;
}

static
int syscall_list_show(struct seq_file *m, void *p)
{
	const struct trace_syscall_entry *table, *entry = p;
	unsigned int bitness;
	unsigned long index;
	int ret;
	const char *name;

	ret = get_sc_table(entry, &table, &bitness);
	if (ret)
		return ret;
	if (!entry->desc)
		return 0;
	if (table == sc_table) {
		index = entry - table;
		name = &entry->desc->name[strlen(SYSCALL_ENTRY_STR)];
	} else {
		index = (entry - table) + ARRAY_SIZE(sc_table);
		name = &entry->desc->name[strlen(COMPAT_SYSCALL_ENTRY_STR)];
	}
	seq_printf(m,	"syscall { index = %lu; name = %s; bitness = %u; };\n",
		index, name, bitness);
	return 0;
}

static
const struct seq_operations lttng_syscall_list_seq_ops = {
	.start = syscall_list_start,
	.next = syscall_list_next,
	.stop = syscall_list_stop,
	.show = syscall_list_show,
};

static
int lttng_syscall_list_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &lttng_syscall_list_seq_ops);
}

const struct file_operations lttng_syscall_list_fops = {
	.owner = THIS_MODULE,
	.open = lttng_syscall_list_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

long lttng_channel_syscall_mask(struct lttng_channel *channel,
		struct lttng_kernel_syscall_mask __user *usyscall_mask)
{
	uint32_t len, sc_tables_len, bitmask_len;
	int ret = 0, bit;
	char *tmp_mask;
	struct lttng_syscall_filter *filter;

	ret = get_user(len, &usyscall_mask->len);
	if (ret)
		return ret;
	sc_tables_len = get_sc_tables_len();
	bitmask_len = ALIGN(sc_tables_len, 8) >> 3;
	if (len < sc_tables_len) {
		return put_user(sc_tables_len, &usyscall_mask->len);
	}
	/* Array is large enough, we can copy array to user-space. */
	tmp_mask = kzalloc(bitmask_len, GFP_KERNEL);
	if (!tmp_mask)
		return -ENOMEM;
	filter = channel->sc_filter;

	for (bit = 0; bit < ARRAY_SIZE(sc_table); bit++) {
		bool state;

		if (channel->sc_table) {
			if (filter)
				state = test_bit(bit, filter->sc);
			else
				state = 1;
		} else {
			state = 0;
		}
		bt_bitfield_write_be(tmp_mask, char, bit, 1, state);
	}
	for (; bit < sc_tables_len; bit++) {
		bool state;

		if (channel->compat_sc_table) {
			if (filter)
				state = test_bit(bit - ARRAY_SIZE(sc_table),
						filter->sc_compat);
			else
				state = 1;
		} else {
			state = 0;
		}
		bt_bitfield_write_be(tmp_mask, char, bit, 1, state);
	}
	if (copy_to_user(usyscall_mask->mask, tmp_mask, bitmask_len))
		ret = -EFAULT;
	kfree(tmp_mask);
	return ret;
}

int lttng_abi_syscall_list(void)
{
	struct file *syscall_list_file;
	int file_fd, ret;

	file_fd = lttng_get_unused_fd();
	if (file_fd < 0) {
		ret = file_fd;
		goto fd_error;
	}

	syscall_list_file = anon_inode_getfile("[lttng_syscall_list]",
					  &lttng_syscall_list_fops,
					  NULL, O_RDWR);
	if (IS_ERR(syscall_list_file)) {
		ret = PTR_ERR(syscall_list_file);
		goto file_error;
	}
	ret = lttng_syscall_list_fops.open(NULL, syscall_list_file);
	if (ret < 0)
		goto open_error;
	fd_install(file_fd, syscall_list_file);
	if (file_fd < 0) {
		ret = file_fd;
		goto fd_error;
	}
	return file_fd;

open_error:
	fput(syscall_list_file);
file_error:
	put_unused_fd(file_fd);
fd_error:
	return ret;
}
