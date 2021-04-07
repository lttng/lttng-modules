/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-syscalls.c
 *
 * LTTng syscall probes.
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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
#include <linux/fcntl.h>
#include <linux/mman.h>
#include <asm/ptrace.h>
#include <asm/syscall.h>

#include <lttng/bitfield.h>
#include <wrapper/tracepoint.h>
#include <wrapper/file.h>
#include <wrapper/rcu.h>
#include <wrapper/syscall.h>
#include <lttng/events.h>
#include <lttng/utils.h>

#ifndef CONFIG_COMPAT
# ifndef is_compat_task
#  define is_compat_task()	(0)
# endif
#endif

/* in_compat_syscall appears in kernel 4.6. */
#ifndef in_compat_syscall
 #define in_compat_syscall()	is_compat_task()
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
void syscall_entry_event_probe(void *__data, struct pt_regs *regs, long id);
static
void syscall_exit_event_probe(void *__data, struct pt_regs *regs, long ret);

static
void syscall_entry_event_notifier_probe(void *__data, struct pt_regs *regs,
		long id);
static
void syscall_exit_event_notifier_probe(void *__data, struct pt_regs *regs,
		long ret);

/*
 * Forward declarations for old kernels.
 */
struct mmsghdr;
struct rlimit64;
struct oldold_utsname;
struct old_utsname;
struct sel_arg_struct;
struct mmap_arg_struct;
struct file_handle;
struct user_msghdr;

/*
 * Forward declaration for kernels >= 5.6
 */
struct timex;
struct timeval;
struct itimerval;
struct itimerspec;

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,6,0))
typedef __kernel_old_time_t time_t;
#endif

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
#define TRACE_INCLUDE_PATH instrumentation/syscalls/headers

#define PARAMS(args...)	args

/* Handle unknown syscalls */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM syscalls_unknown
#include <instrumentation/syscalls/headers/syscalls_unknown.h>
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
#define TP_PROBE_CB(_template)		&syscall_entry_event_probe
#define SC_LTTNG_TRACEPOINT_EVENT(_name, _proto, _args, _fields) \
	LTTNG_TRACEPOINT_EVENT(syscall_entry_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post) \
	LTTNG_TRACEPOINT_EVENT_CODE(syscall_entry_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_locvar), PARAMS(_code_pre),				\
		PARAMS(_fields), PARAMS(_code_post))
#define SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(_name, _fields) \
	LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(syscall_entry_##_name, PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(_template, _name)		\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscall_entry_##_template, syscall_entry_##_name)
/* Enumerations only defined at first inclusion. */
#define SC_LTTNG_TRACEPOINT_ENUM(_name, _values) \
	LTTNG_TRACEPOINT_ENUM(_name, PARAMS(_values))
#undef TRACE_SYSTEM
#define TRACE_SYSTEM syscall_entry_integers
#define TRACE_INCLUDE_FILE syscalls_integers
#include <instrumentation/syscalls/headers/syscalls_integers.h>
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#define TRACE_SYSTEM syscall_entry_pointers
#define TRACE_INCLUDE_FILE syscalls_pointers
#include <instrumentation/syscalls/headers/syscalls_pointers.h>
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#undef SC_LTTNG_TRACEPOINT_ENUM
#undef SC_LTTNG_TRACEPOINT_EVENT_CODE
#undef SC_LTTNG_TRACEPOINT_EVENT
#undef SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS
#undef SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#undef TP_PROBE_CB
#undef _TRACE_SYSCALLS_INTEGERS_H
#undef _TRACE_SYSCALLS_POINTERS_H

/* Hijack probe callback for compat system call enter */
#define TP_PROBE_CB(_template)		&syscall_entry_event_probe
#define LTTNG_SC_COMPAT
#define SC_LTTNG_TRACEPOINT_EVENT(_name, _proto, _args, _fields) \
	LTTNG_TRACEPOINT_EVENT(compat_syscall_entry_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post) \
	LTTNG_TRACEPOINT_EVENT_CODE(compat_syscall_entry_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_locvar), PARAMS(_code_pre), PARAMS(_fields), PARAMS(_code_post))
#define SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(_name, _fields) \
	LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(compat_syscall_entry_##_name, PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(_template, _name)		\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(compat_syscall_entry_##_template, \
		compat_syscall_entry_##_name)
/* Enumerations only defined at inital inclusion (not here). */
#define SC_LTTNG_TRACEPOINT_ENUM(_name, _values)
#define TRACE_SYSTEM compat_syscall_entry_integers
#define TRACE_INCLUDE_FILE compat_syscalls_integers
#include <instrumentation/syscalls/headers/compat_syscalls_integers.h>
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#define TRACE_SYSTEM compat_syscall_entry_pointers
#define TRACE_INCLUDE_FILE compat_syscalls_pointers
#include <instrumentation/syscalls/headers/compat_syscalls_pointers.h>
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#undef SC_LTTNG_TRACEPOINT_ENUM
#undef SC_LTTNG_TRACEPOINT_EVENT_CODE
#undef SC_LTTNG_TRACEPOINT_EVENT
#undef SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS
#undef SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#undef TP_PROBE_CB
#undef _TRACE_SYSCALLS_INTEGERS_H
#undef _TRACE_SYSCALLS_POINTERS_H
#undef LTTNG_SC_COMPAT

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
#define TP_PROBE_CB(_template)		&syscall_exit_event_probe
#define SC_LTTNG_TRACEPOINT_EVENT(_name, _proto, _args, _fields) \
	LTTNG_TRACEPOINT_EVENT(syscall_exit_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post) \
	LTTNG_TRACEPOINT_EVENT_CODE(syscall_exit_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_locvar), PARAMS(_code_pre), PARAMS(_fields), PARAMS(_code_post))
#define SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(_name, _fields) \
	LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(syscall_exit_##_name, PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(_template, _name) 		\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscall_exit_##_template,	\
		syscall_exit_##_name)
/* Enumerations only defined at inital inclusion (not here). */
#define SC_LTTNG_TRACEPOINT_ENUM(_name, _values)
#define TRACE_SYSTEM syscall_exit_integers
#define TRACE_INCLUDE_FILE syscalls_integers
#include <instrumentation/syscalls/headers/syscalls_integers.h>
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#define TRACE_SYSTEM syscall_exit_pointers
#define TRACE_INCLUDE_FILE syscalls_pointers
#include <instrumentation/syscalls/headers/syscalls_pointers.h>
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#undef SC_LTTNG_TRACEPOINT_ENUM
#undef SC_LTTNG_TRACEPOINT_EVENT_CODE
#undef SC_LTTNG_TRACEPOINT_EVENT
#undef SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS
#undef SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#undef TP_PROBE_CB
#undef _TRACE_SYSCALLS_INTEGERS_H
#undef _TRACE_SYSCALLS_POINTERS_H


/* Hijack probe callback for compat system call exit */
#define TP_PROBE_CB(_template)		&syscall_exit_event_probe
#define LTTNG_SC_COMPAT
#define SC_LTTNG_TRACEPOINT_EVENT(_name, _proto, _args, _fields) \
	LTTNG_TRACEPOINT_EVENT(compat_syscall_exit_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_CODE(_name, _proto, _args, _locvar, _code_pre, _fields, _code_post) \
	LTTNG_TRACEPOINT_EVENT_CODE(compat_syscall_exit_##_name, PARAMS(_proto), PARAMS(_args), \
		PARAMS(_locvar), PARAMS(_code_pre), PARAMS(_fields), PARAMS(_code_post))
#define SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(_name, _fields) \
	LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(compat_syscall_exit_##_name, PARAMS(_fields))
#define SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(_template, _name)		\
	LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(compat_syscall_exit_##_template, \
		compat_syscall_exit_##_name)
/* Enumerations only defined at inital inclusion (not here). */
#define SC_LTTNG_TRACEPOINT_ENUM(_name, _values)
#define TRACE_SYSTEM compat_syscall_exit_integers
#define TRACE_INCLUDE_FILE compat_syscalls_integers
#include <instrumentation/syscalls/headers/compat_syscalls_integers.h>
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#define TRACE_SYSTEM compat_syscall_exit_pointers
#define TRACE_INCLUDE_FILE compat_syscalls_pointers
#include <instrumentation/syscalls/headers/compat_syscalls_pointers.h>
#undef TRACE_INCLUDE_FILE
#undef TRACE_SYSTEM
#undef SC_LTTNG_TRACEPOINT_ENUM
#undef SC_LTTNG_TRACEPOINT_EVENT_CODE
#undef SC_LTTNG_TRACEPOINT_EVENT
#undef SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS
#undef SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS
#undef TP_PROBE_CB
#undef _TRACE_SYSCALLS_INTEGERS_H
#undef _TRACE_SYSCALLS_POINTERS_H
#undef LTTNG_SC_COMPAT

#undef SC_EXIT

#undef TP_MODULE_NOINIT
#undef LTTNG_PACKAGE_BUILD
#undef CREATE_TRACE_POINTS

struct trace_syscall_entry {
	void *event_func;
	void *event_notifier_func;
	const struct lttng_kernel_event_desc *desc;
	const struct lttng_kernel_event_field **fields;
	unsigned int nrargs;
};

#define CREATE_SYSCALL_TABLE

#define SC_ENTER

#undef sc_exit
#define sc_exit(...)

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_template, _name, _nr, _nrargs)	\
	[ _nr ] = {						\
		.event_func = __event_probe__syscall_entry_##_template, \
		.event_notifier_func = __event_notifier_probe__syscall_entry_##_template, \
		.nrargs = (_nrargs),				\
		.fields = __event_fields___syscall_entry_##_template, \
		.desc = &__event_desc___syscall_entry_##_name,	\
	},

/* Event syscall enter tracing table */
static const struct trace_syscall_entry sc_table[] = {
#include <instrumentation/syscalls/headers/syscalls_integers.h>
#include <instrumentation/syscalls/headers/syscalls_pointers.h>
};

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_template, _name, _nr, _nrargs)	\
	[ _nr ] = {						\
		.event_func = __event_probe__compat_syscall_entry_##_template, \
		.event_notifier_func = __event_notifier_probe__compat_syscall_entry_##_template, \
		.nrargs = (_nrargs),				\
		.fields = __event_fields___compat_syscall_entry_##_template, \
		.desc = &__event_desc___compat_syscall_entry_##_name, \
	},

/* Event compat syscall enter table */
const struct trace_syscall_entry compat_sc_table[] = {
#include <instrumentation/syscalls/headers/compat_syscalls_integers.h>
#include <instrumentation/syscalls/headers/compat_syscalls_pointers.h>
};

#undef SC_ENTER

#define SC_EXIT

#undef sc_exit
#define sc_exit(...)		__VA_ARGS__

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_template, _name, _nr, _nrargs)	\
	[ _nr ] = {						\
		.event_func = __event_probe__syscall_exit_##_template, \
		.event_notifier_func = __event_notifier_probe__syscall_exit_##_template, \
		.nrargs = (_nrargs),				\
		.fields = __event_fields___syscall_exit_##_template, \
		.desc = &__event_desc___syscall_exit_##_name, \
	},

/* Event syscall exit table */
static const struct trace_syscall_entry sc_exit_table[] = {
#include <instrumentation/syscalls/headers/syscalls_integers.h>
#include <instrumentation/syscalls/headers/syscalls_pointers.h>
};

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_template, _name, _nr, _nrargs)	\
	[ _nr ] = {						\
		.event_func = __event_probe__compat_syscall_exit_##_template, \
		.event_notifier_func = __event_notifier_probe__compat_syscall_exit_##_template, \
		.nrargs = (_nrargs),				\
		.fields = __event_fields___compat_syscall_exit_##_template, \
		.desc = &__event_desc___compat_syscall_exit_##_name, \
	},

/* Event compat syscall exit table */
const struct trace_syscall_entry compat_sc_exit_table[] = {
#include <instrumentation/syscalls/headers/compat_syscalls_integers.h>
#include <instrumentation/syscalls/headers/compat_syscalls_pointers.h>
};

#undef SC_EXIT

#undef CREATE_SYSCALL_TABLE

struct lttng_syscall_filter {
	DECLARE_BITMAP(sc_entry, NR_syscalls);
	DECLARE_BITMAP(sc_exit, NR_syscalls);
	DECLARE_BITMAP(sc_compat_entry, NR_compat_syscalls);
	DECLARE_BITMAP(sc_compat_exit, NR_compat_syscalls);
};

static void syscall_entry_event_unknown(struct hlist_head *unknown_action_list_head,
	struct pt_regs *regs, long id)
{
	unsigned long args[LTTNG_SYSCALL_NR_ARGS];
	struct lttng_event *event;

	lttng_syscall_get_arguments(current, regs, args);
	lttng_hlist_for_each_entry_rcu(event, unknown_action_list_head, u.syscall.node) {
		if (unlikely(in_compat_syscall()))
			__event_probe__compat_syscall_entry_unknown(event, id, args);
		else
			__event_probe__syscall_entry_unknown(event, id, args);
	}
}

static void syscall_entry_event_notifier_unknown(
		struct hlist_head *unknown_dispatch_list_head,
		struct pt_regs *regs, long id)
{
	unsigned long args[LTTNG_SYSCALL_NR_ARGS];
	struct lttng_event_notifier *notifier;

	lttng_syscall_get_arguments(current, regs, args);
	lttng_hlist_for_each_entry_rcu(notifier, unknown_dispatch_list_head, u.syscall.node) {
		if (unlikely(in_compat_syscall()))
			__event_notifier_probe__compat_syscall_entry_unknown(notifier, id, args);
		else
			__event_notifier_probe__syscall_entry_unknown(notifier, id, args);
	}
}

static void syscall_exit_event_notifier_unknown(
		struct hlist_head *unknown_dispatch_list_head,
		struct pt_regs *regs, long id, long ret)
{
	unsigned long args[LTTNG_SYSCALL_NR_ARGS];
	struct lttng_event_notifier *notifier;

	lttng_syscall_get_arguments(current, regs, args);
	lttng_hlist_for_each_entry_rcu(notifier, unknown_dispatch_list_head, u.syscall.node) {
		if (unlikely(in_compat_syscall()))
			__event_notifier_probe__compat_syscall_exit_unknown(notifier, id, ret, args);
		else
			__event_notifier_probe__syscall_exit_unknown(notifier, id, ret, args);
	}
}

static __always_inline
void syscall_entry_call_func(struct hlist_head *action_list,
		void *func, unsigned int nrargs,
		struct pt_regs *regs)
{
	struct lttng_event *event;

	switch (nrargs) {
	case 0:
	{
		void (*fptr)(void *__data) = func;

		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
			fptr(event);
		break;
	}
	case 1:
	{
		void (*fptr)(void *__data, unsigned long arg0) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
			fptr(event, args[0]);
		break;
	}
	case 2:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
			fptr(event, args[0], args[1]);
		break;
	}
	case 3:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
			fptr(event, args[0], args[1], args[2]);
		break;
	}
	case 4:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2,
			unsigned long arg3) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
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
			unsigned long arg4) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
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
			unsigned long arg5) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
			fptr(event, args[0], args[1], args[2],
			     args[3], args[4], args[5]);
		break;
	}
	default:
		break;
	}
}

static __always_inline
void syscall_entry_event_notifier_call_func(struct hlist_head *dispatch_list,
		void *func, unsigned int nrargs, struct pt_regs *regs)
{
	struct lttng_event_notifier *notifier;

	switch (nrargs) {
	case 0:
	{
		void (*fptr)(void *__data) = func;

		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier);
		break;
	}
	case 1:
	{
		void (*fptr)(void *__data, unsigned long arg0) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, args[0]);
		break;
	}
	case 2:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, args[0], args[1]);
		break;
	}
	case 3:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, args[0], args[1], args[2]);
		break;
	}
	case 4:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2,
			unsigned long arg3) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, args[0], args[1], args[2], args[3]);
		break;
	}
	case 5:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2,
			unsigned long arg3,
			unsigned long arg4) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, args[0], args[1], args[2], args[3], args[4]);
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
			unsigned long arg5) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, args[0], args[1], args[2], args[3], args[4], args[5]);
		break;
	}
	default:
		break;
	}
}

void syscall_entry_event_probe(void *__data, struct pt_regs *regs, long id)
{
	struct lttng_channel *chan = __data;
	struct hlist_head *action_list, *unknown_action_list;
	const struct trace_syscall_entry *table, *entry;
	size_t table_len;

	if (unlikely(in_compat_syscall())) {
		struct lttng_syscall_filter *filter = chan->sc_filter;

		if (id < 0 || id >= NR_compat_syscalls
			|| (!READ_ONCE(chan->syscall_all_entry) && !test_bit(id, filter->sc_compat_entry))) {
			/* System call filtered out. */
			return;
		}
		table = compat_sc_table;
		table_len = ARRAY_SIZE(compat_sc_table);
		unknown_action_list = &chan->sc_compat_unknown;
	} else {
		struct lttng_syscall_filter *filter = chan->sc_filter;

		if (id < 0 || id >= NR_syscalls
			|| (!READ_ONCE(chan->syscall_all_entry) && !test_bit(id, filter->sc_entry))) {
			/* System call filtered out. */
			return;
		}
		table = sc_table;
		table_len = ARRAY_SIZE(sc_table);
		unknown_action_list = &chan->sc_unknown;
	}
	if (unlikely(id < 0 || id >= table_len)) {
		syscall_entry_event_unknown(unknown_action_list, regs, id);
		return;
	}

	entry = &table[id];
	if (!entry->event_func) {
		syscall_entry_event_unknown(unknown_action_list, regs, id);
		return;
	}

	if (unlikely(in_compat_syscall())) {
		action_list = &chan->compat_sc_table[id];
	} else {
		action_list = &chan->sc_table[id];
	}
	if (unlikely(hlist_empty(action_list)))
		return;

	syscall_entry_call_func(action_list, entry->event_func, entry->nrargs, regs);
}

void syscall_entry_event_notifier_probe(void *__data, struct pt_regs *regs,
		long id)
{
	struct lttng_event_notifier_group *group = __data;
	const struct trace_syscall_entry *table, *entry;
	struct hlist_head *dispatch_list, *unknown_dispatch_list;
	size_t table_len;

	if (unlikely(in_compat_syscall())) {
		struct lttng_syscall_filter *filter = group->sc_filter;

		if (id < 0 || id >= NR_compat_syscalls
			|| (!READ_ONCE(group->syscall_all_entry) &&
				!test_bit(id, filter->sc_compat_entry))) {
			/* System call filtered out. */
			return;
		}
		table = compat_sc_table;
		table_len = ARRAY_SIZE(compat_sc_table);
		unknown_dispatch_list = &group->event_notifier_compat_unknown_syscall_dispatch;
	} else {
		struct lttng_syscall_filter *filter = group->sc_filter;

		if (id < 0 || id >= NR_syscalls
			|| (!READ_ONCE(group->syscall_all_entry) &&
				!test_bit(id, filter->sc_entry))) {
			/* System call filtered out. */
			return;
		}
		table = sc_table;
		table_len = ARRAY_SIZE(sc_table);
		unknown_dispatch_list = &group->event_notifier_unknown_syscall_dispatch;
	}
	/* Check if the syscall id is out of bound. */
	if (unlikely(id < 0 || id >= table_len)) {
		syscall_entry_event_notifier_unknown(unknown_dispatch_list,
				regs, id);
		return;
	}

	entry = &table[id];
	if (!entry->event_notifier_func) {
		syscall_entry_event_notifier_unknown(unknown_dispatch_list,
				regs, id);
		return;
	}

	if (unlikely(in_compat_syscall())) {
		dispatch_list = &group->event_notifier_compat_syscall_dispatch[id];
	} else {
		dispatch_list = &group->event_notifier_syscall_dispatch[id];
	}
	if (unlikely(hlist_empty(dispatch_list)))
		return;

	syscall_entry_event_notifier_call_func(dispatch_list,
			entry->event_notifier_func, entry->nrargs, regs);
}

static void syscall_exit_event_unknown(struct hlist_head *unknown_action_list_head,
	struct pt_regs *regs, long id, long ret)
{
	unsigned long args[LTTNG_SYSCALL_NR_ARGS];
	struct lttng_event *event;

	lttng_syscall_get_arguments(current, regs, args);
	lttng_hlist_for_each_entry_rcu(event, unknown_action_list_head, u.syscall.node) {
		if (unlikely(in_compat_syscall()))
			__event_probe__compat_syscall_exit_unknown(event, id, ret,
				args);
		else
			__event_probe__syscall_exit_unknown(event, id, ret, args);
	}
}

static __always_inline
void syscall_exit_call_func(struct hlist_head *action_list,
		void *func, unsigned int nrargs,
		struct pt_regs *regs, long ret)
{
	struct lttng_event *event;

	switch (nrargs) {
	case 0:
	{
		void (*fptr)(void *__data, long ret) = func;

		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
			fptr(event, ret);
		break;
	}
	case 1:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
			fptr(event, ret, args[0]);
		break;
	}
	case 2:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0,
			unsigned long arg1) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
			fptr(event, ret, args[0], args[1]);
		break;
	}
	case 3:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
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
			unsigned long arg3) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
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
			unsigned long arg4) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
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
			unsigned long arg5) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event, action_list, u.syscall.node)
			fptr(event, ret, args[0], args[1], args[2],
			     args[3], args[4], args[5]);
		break;
	}
	default:
		break;
	}
}

void syscall_exit_event_probe(void *__data, struct pt_regs *regs, long ret)
{
	struct lttng_channel *chan = __data;
	struct hlist_head *action_list, *unknown_action_list;
	const struct trace_syscall_entry *table, *entry;
	size_t table_len;
	long id;

	id = syscall_get_nr(current, regs);

	if (unlikely(in_compat_syscall())) {
		struct lttng_syscall_filter *filter = chan->sc_filter;

		if (id < 0 || id >= NR_compat_syscalls
			|| (!READ_ONCE(chan->syscall_all_exit) && !test_bit(id, filter->sc_compat_exit))) {
			/* System call filtered out. */
			return;
		}
		table = compat_sc_exit_table;
		table_len = ARRAY_SIZE(compat_sc_exit_table);
		unknown_action_list = &chan->compat_sc_exit_unknown;
	} else {
		struct lttng_syscall_filter *filter = chan->sc_filter;

		if (id < 0 || id >= NR_syscalls
			|| (!READ_ONCE(chan->syscall_all_exit) && !test_bit(id, filter->sc_exit))) {
			/* System call filtered out. */
			return;
		}
		table = sc_exit_table;
		table_len = ARRAY_SIZE(sc_exit_table);
		unknown_action_list = &chan->sc_exit_unknown;
	}
	if (unlikely(id < 0 || id >= table_len)) {
		syscall_exit_event_unknown(unknown_action_list, regs, id, ret);
		return;
	}

	entry = &table[id];
	if (!entry->event_func) {
		syscall_exit_event_unknown(unknown_action_list, regs, id, ret);
		return;
	}

	if (unlikely(in_compat_syscall())) {
		action_list = &chan->compat_sc_exit_table[id];
	} else {
		action_list = &chan->sc_exit_table[id];
	}
	if (unlikely(hlist_empty(action_list)))
		return;

	syscall_exit_call_func(action_list, entry->event_func, entry->nrargs,
			       regs, ret);
}

static __always_inline
void syscall_exit_event_notifier_call_func(struct hlist_head *dispatch_list,
		void *func, unsigned int nrargs, struct pt_regs *regs, long ret)
{
	struct lttng_event_notifier *notifier;

	switch (nrargs) {
	case 0:
	{
		void (*fptr)(void *__data, long ret) = func;

		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, ret);
		break;
	}
	case 1:
	{
		void (*fptr)(void *__data, long ret, unsigned long arg0) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, ret, args[0]);
		break;
	}
	case 2:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0,
			unsigned long arg1) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, ret, args[0], args[1]);
		break;
	}
	case 3:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, ret, args[0], args[1], args[2]);
		break;
	}
	case 4:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0,
			unsigned long arg1,
			unsigned long arg2,
			unsigned long arg3) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, ret, args[0], args[1], args[2], args[3]);
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
			unsigned long arg4) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, ret, args[0], args[1], args[2], args[3], args[4]);
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
			unsigned long arg5) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(notifier, dispatch_list, u.syscall.node)
			fptr(notifier, ret, args[0], args[1], args[2], args[3], args[4], args[5]);
		break;
	}
	default:
		break;
	}
}

static
void syscall_exit_event_notifier_probe(void *__data, struct pt_regs *regs,
		long ret)
{
	struct lttng_event_notifier_group *group = __data;
	const struct trace_syscall_entry *table, *entry;
	struct hlist_head *dispatch_list, *unknown_dispatch_list;
	size_t table_len;
	long id;

	id = syscall_get_nr(current, regs);

	if (unlikely(in_compat_syscall())) {
		struct lttng_syscall_filter *filter = group->sc_filter;

		if (id < 0 || id >= NR_compat_syscalls
			|| (!READ_ONCE(group->syscall_all_exit) &&
				!test_bit(id, filter->sc_compat_exit))) {
			/* System call filtered out. */
			return;
		}
		table = compat_sc_exit_table;
		table_len = ARRAY_SIZE(compat_sc_exit_table);
		unknown_dispatch_list = &group->event_notifier_exit_compat_unknown_syscall_dispatch;
	} else {
		struct lttng_syscall_filter *filter = group->sc_filter;

		if (id < 0 || id >= NR_syscalls
			|| (!READ_ONCE(group->syscall_all_exit) &&
					!test_bit(id, filter->sc_exit))) {
			/* System call filtered out. */
			return;
		}
		table = sc_exit_table;
		table_len = ARRAY_SIZE(sc_exit_table);
		unknown_dispatch_list = &group->event_notifier_exit_unknown_syscall_dispatch;
	}
	/* Check if the syscall id is out of bound. */
	if (unlikely(id < 0 || id >= table_len)) {
		syscall_exit_event_notifier_unknown(unknown_dispatch_list,
				regs, id, ret);
		return;
	}

	entry = &table[id];
	if (!entry->event_notifier_func) {
		syscall_entry_event_notifier_unknown(unknown_dispatch_list,
				regs, id);
		return;
	}

	if (unlikely(in_compat_syscall())) {
		dispatch_list = &group->event_notifier_exit_compat_syscall_dispatch[id];
	} else {
		dispatch_list = &group->event_notifier_exit_syscall_dispatch[id];
	}
	if (unlikely(hlist_empty(dispatch_list)))
		return;

	syscall_exit_event_notifier_call_func(dispatch_list,
			entry->event_notifier_func, entry->nrargs, regs, ret);
}
/*
 * noinline to diminish caller stack size.
 * Should be called with sessions lock held.
 */
static
int lttng_create_syscall_event_if_missing(const struct trace_syscall_entry *table, size_t table_len,
	struct hlist_head *chan_table, struct lttng_event_enabler *event_enabler,
	void *filter, enum sc_type type)
{
	struct lttng_channel *chan = event_enabler->chan;
	struct lttng_session *session = chan->session;
	unsigned int i;

	/* Allocate events for each syscall matching enabler, insert into table */
	for (i = 0; i < table_len; i++) {
		const struct lttng_kernel_event_desc *desc = table[i].desc;
		struct lttng_kernel_event ev;
		struct lttng_event *event;
		struct hlist_head *head;
		bool found = false;

		if (!desc) {
			/* Unknown syscall */
			continue;
		}
		if (lttng_desc_match_enabler(desc,
				lttng_event_enabler_as_enabler(event_enabler)) <= 0)
			continue;
		/*
		 * Check if already created.
		 */
		head = utils_borrow_hash_table_bucket(
			session->events_ht.table, LTTNG_EVENT_HT_SIZE,
			desc->event_name);
		lttng_hlist_for_each_entry(event, head, hlist) {
			if (event->desc == desc
				&& event->chan == event_enabler->chan)
				found = true;
		}
		if (found)
			continue;

		/* We need to create an event for this syscall/enabler. */
		memset(&ev, 0, sizeof(ev));
		switch (type) {
		case SC_TYPE_ENTRY:
			ev.u.syscall.entryexit = LTTNG_KERNEL_SYSCALL_ENTRY;
			ev.u.syscall.abi = LTTNG_KERNEL_SYSCALL_ABI_NATIVE;
			break;
		case SC_TYPE_EXIT:
			ev.u.syscall.entryexit = LTTNG_KERNEL_SYSCALL_EXIT;
			ev.u.syscall.abi = LTTNG_KERNEL_SYSCALL_ABI_NATIVE;
			break;
		case SC_TYPE_COMPAT_ENTRY:
			ev.u.syscall.entryexit = LTTNG_KERNEL_SYSCALL_ENTRY;
			ev.u.syscall.abi = LTTNG_KERNEL_SYSCALL_ABI_COMPAT;
			break;
		case SC_TYPE_COMPAT_EXIT:
			ev.u.syscall.entryexit = LTTNG_KERNEL_SYSCALL_EXIT;
			ev.u.syscall.abi = LTTNG_KERNEL_SYSCALL_ABI_COMPAT;
			break;
		}
		strncpy(ev.name, desc->event_name, LTTNG_KERNEL_SYM_NAME_LEN - 1);
		ev.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_SYSCALL;
		event = _lttng_event_create(chan, &ev, filter,
					    desc, ev.instrumentation);
		WARN_ON_ONCE(!event);
		if (IS_ERR(event)) {
			/*
			 * If something goes wrong in event registration
			 * after the first one, we have no choice but to
			 * leave the previous events in there, until
			 * deleted by session teardown.
			 */
			return PTR_ERR(event);
		}
		hlist_add_head(&event->u.syscall.node, &chan_table[i]);
	}
	return 0;
}

/*
 * Should be called with sessions lock held.
 */
int lttng_syscalls_register_event(struct lttng_event_enabler *event_enabler, void *filter)
{
	struct lttng_channel *chan = event_enabler->chan;
	struct lttng_kernel_event ev;
	int ret;

	wrapper_vmalloc_sync_mappings();

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
	if (hlist_empty(&chan->sc_unknown)) {
		const struct lttng_kernel_event_desc *desc =
			&__event_desc___syscall_entry_unknown;
		struct lttng_event *event;

		memset(&ev, 0, sizeof(ev));
		strncpy(ev.name, desc->event_name, LTTNG_KERNEL_SYM_NAME_LEN);
		ev.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_SYSCALL;
		ev.u.syscall.entryexit = LTTNG_KERNEL_SYSCALL_ENTRY;
		ev.u.syscall.abi = LTTNG_KERNEL_SYSCALL_ABI_NATIVE;
		event = _lttng_event_create(chan, &ev, filter, desc,
					    ev.instrumentation);
		WARN_ON_ONCE(!event);
		if (IS_ERR(event)) {
			return PTR_ERR(event);
		}
		hlist_add_head(&event->u.syscall.node, &chan->sc_unknown);
	}

	if (hlist_empty(&chan->sc_compat_unknown)) {
		const struct lttng_kernel_event_desc *desc =
			&__event_desc___compat_syscall_entry_unknown;
		struct lttng_event *event;

		memset(&ev, 0, sizeof(ev));
		strncpy(ev.name, desc->event_name, LTTNG_KERNEL_SYM_NAME_LEN);
		ev.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_SYSCALL;
		ev.u.syscall.entryexit = LTTNG_KERNEL_SYSCALL_ENTRY;
		ev.u.syscall.abi = LTTNG_KERNEL_SYSCALL_ABI_COMPAT;
		event = _lttng_event_create(chan, &ev, filter, desc,
					    ev.instrumentation);
		WARN_ON_ONCE(!event);
		if (IS_ERR(event)) {
			return PTR_ERR(event);
		}
		hlist_add_head(&event->u.syscall.node, &chan->sc_compat_unknown);
	}

	if (hlist_empty(&chan->compat_sc_exit_unknown)) {
		const struct lttng_kernel_event_desc *desc =
			&__event_desc___compat_syscall_exit_unknown;
		struct lttng_event *event;

		memset(&ev, 0, sizeof(ev));
		strncpy(ev.name, desc->event_name, LTTNG_KERNEL_SYM_NAME_LEN);
		ev.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_SYSCALL;
		ev.u.syscall.entryexit = LTTNG_KERNEL_SYSCALL_EXIT;
		ev.u.syscall.abi = LTTNG_KERNEL_SYSCALL_ABI_COMPAT;
		event = _lttng_event_create(chan, &ev, filter, desc,
					    ev.instrumentation);
		WARN_ON_ONCE(!event);
		if (IS_ERR(event)) {
			return PTR_ERR(event);
		}
		hlist_add_head(&event->u.syscall.node, &chan->compat_sc_exit_unknown);
	}

	if (hlist_empty(&chan->sc_exit_unknown)) {
		const struct lttng_kernel_event_desc *desc =
			&__event_desc___syscall_exit_unknown;
		struct lttng_event *event;

		memset(&ev, 0, sizeof(ev));
		strncpy(ev.name, desc->event_name, LTTNG_KERNEL_SYM_NAME_LEN);
		ev.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_SYSCALL;
		ev.u.syscall.entryexit = LTTNG_KERNEL_SYSCALL_EXIT;
		ev.u.syscall.abi = LTTNG_KERNEL_SYSCALL_ABI_NATIVE;
		event = _lttng_event_create(chan, &ev, filter, desc,
					    ev.instrumentation);
		WARN_ON_ONCE(!event);
		if (IS_ERR(event)) {
			return PTR_ERR(event);
		}
		hlist_add_head(&event->u.syscall.node, &chan->sc_exit_unknown);
	}

	ret = lttng_create_syscall_event_if_missing(sc_table, ARRAY_SIZE(sc_table),
			chan->sc_table, event_enabler, filter, SC_TYPE_ENTRY);
	if (ret)
		return ret;
	ret = lttng_create_syscall_event_if_missing(sc_exit_table, ARRAY_SIZE(sc_exit_table),
			chan->sc_exit_table, event_enabler, filter, SC_TYPE_EXIT);
	if (ret)
		return ret;

#ifdef CONFIG_COMPAT
	ret = lttng_create_syscall_event_if_missing(compat_sc_table, ARRAY_SIZE(compat_sc_table),
			chan->compat_sc_table, event_enabler, filter,
			SC_TYPE_COMPAT_ENTRY);
	if (ret)
		return ret;
	ret = lttng_create_syscall_event_if_missing(compat_sc_exit_table, ARRAY_SIZE(compat_sc_exit_table),
			chan->compat_sc_exit_table, event_enabler, filter,
			SC_TYPE_COMPAT_EXIT);
	if (ret)
		return ret;
#endif

	if (!chan->sc_filter) {
		chan->sc_filter = kzalloc(sizeof(struct lttng_syscall_filter),
				GFP_KERNEL);
		if (!chan->sc_filter)
			return -ENOMEM;
	}

	if (!chan->sys_enter_registered) {
		ret = lttng_wrapper_tracepoint_probe_register("sys_enter",
				(void *) syscall_entry_event_probe, chan);
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
				(void *) syscall_exit_event_probe, chan);
		if (ret) {
			WARN_ON_ONCE(lttng_wrapper_tracepoint_probe_unregister("sys_enter",
				(void *) syscall_entry_event_probe, chan));
			return ret;
		}
		chan->sys_exit_registered = 1;
	}
	return ret;
}

/*
 * Should be called with sessions lock held.
 */
int lttng_syscalls_register_event_notifier(
		struct lttng_event_notifier_enabler *event_notifier_enabler,
		void *filter)
{
	struct lttng_event_notifier_group *group = event_notifier_enabler->group;
	unsigned int i;
	int ret = 0;

	wrapper_vmalloc_sync_mappings();

	if (!group->event_notifier_syscall_dispatch) {
		group->event_notifier_syscall_dispatch =
				kzalloc(sizeof(struct hlist_head) * ARRAY_SIZE(sc_table),
					GFP_KERNEL);
		if (!group->event_notifier_syscall_dispatch)
			return -ENOMEM;

		/* Initialize all list_head */
		for (i = 0; i < ARRAY_SIZE(sc_table); i++)
			INIT_HLIST_HEAD(&group->event_notifier_syscall_dispatch[i]);

		/* Init the unknown syscall notifier list. */
		INIT_HLIST_HEAD(&group->event_notifier_unknown_syscall_dispatch);
	}

	if (!group->event_notifier_exit_syscall_dispatch) {
		group->event_notifier_exit_syscall_dispatch =
				kzalloc(sizeof(struct hlist_head) * ARRAY_SIZE(sc_table),
					GFP_KERNEL);
		if (!group->event_notifier_exit_syscall_dispatch)
			return -ENOMEM;

		/* Initialize all list_head */
		for (i = 0; i < ARRAY_SIZE(sc_table); i++)
			INIT_HLIST_HEAD(&group->event_notifier_exit_syscall_dispatch[i]);

		/* Init the unknown exit syscall notifier list. */
		INIT_HLIST_HEAD(&group->event_notifier_exit_unknown_syscall_dispatch);
	}

#ifdef CONFIG_COMPAT
	if (!group->event_notifier_compat_syscall_dispatch) {
		group->event_notifier_compat_syscall_dispatch =
				kzalloc(sizeof(struct hlist_head) * ARRAY_SIZE(compat_sc_table),
					GFP_KERNEL);
		if (!group->event_notifier_syscall_dispatch)
			return -ENOMEM;

		/* Initialize all list_head */
		for (i = 0; i < ARRAY_SIZE(compat_sc_table); i++)
			INIT_HLIST_HEAD(&group->event_notifier_compat_syscall_dispatch[i]);

		/* Init the unknown syscall notifier list. */
		INIT_HLIST_HEAD(&group->event_notifier_compat_unknown_syscall_dispatch);
	}

	if (!group->event_notifier_exit_compat_syscall_dispatch) {
		group->event_notifier_exit_compat_syscall_dispatch =
				kzalloc(sizeof(struct hlist_head) * ARRAY_SIZE(compat_sc_exit_table),
					GFP_KERNEL);
		if (!group->event_notifier_exit_syscall_dispatch)
			return -ENOMEM;

		/* Initialize all list_head */
		for (i = 0; i < ARRAY_SIZE(compat_sc_exit_table); i++)
			INIT_HLIST_HEAD(&group->event_notifier_exit_compat_syscall_dispatch[i]);

		/* Init the unknown exit syscall notifier list. */
		INIT_HLIST_HEAD(&group->event_notifier_exit_compat_unknown_syscall_dispatch);
	}
#endif

	if (!group->sc_filter) {
		group->sc_filter = kzalloc(sizeof(struct lttng_syscall_filter),
				GFP_KERNEL);
		if (!group->sc_filter)
			return -ENOMEM;
	}

	if (!group->sys_enter_registered) {
		ret = lttng_wrapper_tracepoint_probe_register("sys_enter",
				(void *) syscall_entry_event_notifier_probe, group);
		if (ret)
			return ret;
		group->sys_enter_registered = 1;
	}

	if (!group->sys_exit_registered) {
		ret = lttng_wrapper_tracepoint_probe_register("sys_exit",
				(void *) syscall_exit_event_notifier_probe, group);
		if (ret) {
			WARN_ON_ONCE(lttng_wrapper_tracepoint_probe_unregister("sys_enter",
				(void *) syscall_entry_event_notifier_probe, group));
			return ret;
		}
		group->sys_exit_registered = 1;
	}

	return ret;
}

static
int create_unknown_event_notifier(
		struct lttng_event_notifier_enabler *event_notifier_enabler,
		enum sc_type type)
{
	struct lttng_event_notifier *notifier;
	const struct lttng_kernel_event_desc *desc;
	struct lttng_event_notifier_group *group = event_notifier_enabler->group;
	struct lttng_kernel_event_notifier event_notifier_param;
	uint64_t user_token = event_notifier_enabler->base.user_token;
	uint64_t error_counter_index = event_notifier_enabler->error_counter_index;
	struct lttng_enabler *base_enabler = lttng_event_notifier_enabler_as_enabler(
		event_notifier_enabler);
	struct hlist_head *unknown_dispatch_list;
	int ret = 0;
	bool found = false;
	enum lttng_kernel_syscall_abi abi;
	enum lttng_kernel_syscall_entryexit entryexit;
	struct hlist_head *head;

	switch (type) {
	case SC_TYPE_ENTRY:
		desc = &__event_desc___syscall_entry_unknown;
		unknown_dispatch_list = &group->event_notifier_unknown_syscall_dispatch;
		entryexit = LTTNG_KERNEL_SYSCALL_ENTRY;
		abi = LTTNG_KERNEL_SYSCALL_ABI_NATIVE;
		break;
	case SC_TYPE_EXIT:
		desc = &__event_desc___syscall_exit_unknown;
		unknown_dispatch_list = &group->event_notifier_exit_unknown_syscall_dispatch;
		entryexit = LTTNG_KERNEL_SYSCALL_EXIT;
		abi = LTTNG_KERNEL_SYSCALL_ABI_NATIVE;
		break;
	case SC_TYPE_COMPAT_ENTRY:
		desc = &__event_desc___compat_syscall_entry_unknown;
		unknown_dispatch_list = &group->event_notifier_compat_unknown_syscall_dispatch;
		entryexit = LTTNG_KERNEL_SYSCALL_ENTRY;
		abi = LTTNG_KERNEL_SYSCALL_ABI_COMPAT;
		break;
	case SC_TYPE_COMPAT_EXIT:
		desc = &__event_desc___compat_syscall_exit_unknown;
		unknown_dispatch_list = &group->event_notifier_exit_compat_unknown_syscall_dispatch;
		entryexit = LTTNG_KERNEL_SYSCALL_EXIT;
		abi = LTTNG_KERNEL_SYSCALL_ABI_COMPAT;
		break;
	default:
		BUG_ON(1);
	}

	/*
	 * Check if already created.
	 */
	head = utils_borrow_hash_table_bucket(group->event_notifiers_ht.table,
		LTTNG_EVENT_NOTIFIER_HT_SIZE, desc->event_name);
	lttng_hlist_for_each_entry(notifier, head, hlist) {
		if (notifier->desc == desc &&
				notifier->user_token == base_enabler->user_token)
			found = true;
	}
	if (found)
		goto end;

	memset(&event_notifier_param, 0, sizeof(event_notifier_param));
	strncat(event_notifier_param.event.name, desc->event_name,
		LTTNG_KERNEL_SYM_NAME_LEN - strlen(event_notifier_param.event.name) - 1);

	event_notifier_param.event.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';

	event_notifier_param.event.instrumentation = LTTNG_KERNEL_SYSCALL;
	event_notifier_param.event.u.syscall.abi = abi;
	event_notifier_param.event.u.syscall.entryexit = entryexit;

	notifier = _lttng_event_notifier_create(desc, user_token,
		error_counter_index, group, &event_notifier_param, NULL,
		event_notifier_param.event.instrumentation);
	if (IS_ERR(notifier)) {
		printk(KERN_INFO "Unable to create unknown notifier %s\n",
			desc->event_name);
		ret = -ENOMEM;
		goto end;
	}

	hlist_add_head_rcu(&notifier->u.syscall.node, unknown_dispatch_list);

end:
	return ret;
}

static int create_matching_event_notifiers(
		struct lttng_event_notifier_enabler *event_notifier_enabler,
		void *filter, const struct trace_syscall_entry *table,
		size_t table_len, enum sc_type type)
{
	struct lttng_event_notifier_group *group = event_notifier_enabler->group;
	const struct lttng_kernel_event_desc *desc;
	uint64_t user_token = event_notifier_enabler->base.user_token;
	uint64_t error_counter_index = event_notifier_enabler->error_counter_index;
	unsigned int i;
	int ret = 0;

	/* iterate over all syscall and create event_notifier that match */
	for (i = 0; i < table_len; i++) {
		struct lttng_event_notifier *event_notifier;
		struct lttng_kernel_event_notifier event_notifier_param;
		struct hlist_head *head;
		int found = 0;

		desc = table[i].desc;
		if (!desc) {
			/* Unknown syscall */
			continue;
		}

		if (!lttng_desc_match_enabler(desc,
				lttng_event_notifier_enabler_as_enabler(event_notifier_enabler)))
			continue;

		/*
		 * Check if already created.
		 */
		head = utils_borrow_hash_table_bucket(group->event_notifiers_ht.table,
			LTTNG_EVENT_NOTIFIER_HT_SIZE, desc->event_name);
		lttng_hlist_for_each_entry(event_notifier, head, hlist) {
			if (event_notifier->desc == desc
				&& event_notifier->user_token == event_notifier_enabler->base.user_token)
				found = 1;
		}
		if (found)
			continue;

		memset(&event_notifier_param, 0, sizeof(event_notifier_param));
		switch (type) {
		case SC_TYPE_ENTRY:
			event_notifier_param.event.u.syscall.entryexit = LTTNG_KERNEL_SYSCALL_ENTRY;
			event_notifier_param.event.u.syscall.abi = LTTNG_KERNEL_SYSCALL_ABI_NATIVE;
			break;
		case SC_TYPE_EXIT:
			event_notifier_param.event.u.syscall.entryexit = LTTNG_KERNEL_SYSCALL_EXIT;
			event_notifier_param.event.u.syscall.abi = LTTNG_KERNEL_SYSCALL_ABI_NATIVE;
			break;
		case SC_TYPE_COMPAT_ENTRY:
			event_notifier_param.event.u.syscall.entryexit = LTTNG_KERNEL_SYSCALL_ENTRY;
			event_notifier_param.event.u.syscall.abi = LTTNG_KERNEL_SYSCALL_ABI_COMPAT;
			break;
		case SC_TYPE_COMPAT_EXIT:
			event_notifier_param.event.u.syscall.entryexit = LTTNG_KERNEL_SYSCALL_EXIT;
			event_notifier_param.event.u.syscall.abi = LTTNG_KERNEL_SYSCALL_ABI_COMPAT;
			break;
		}
		strncat(event_notifier_param.event.name, desc->event_name,
			LTTNG_KERNEL_SYM_NAME_LEN - strlen(event_notifier_param.event.name) - 1);
		event_notifier_param.event.name[LTTNG_KERNEL_SYM_NAME_LEN - 1] = '\0';
		event_notifier_param.event.instrumentation = LTTNG_KERNEL_SYSCALL;

		event_notifier = _lttng_event_notifier_create(desc, user_token,
			error_counter_index, group, &event_notifier_param,
			filter, event_notifier_param.event.instrumentation);
		if (IS_ERR(event_notifier)) {
			printk(KERN_INFO "Unable to create event_notifier %s\n",
				desc->event_name);
			ret = -ENOMEM;
			goto end;
		}

		event_notifier->u.syscall.syscall_id = i;
	}

end:
	return ret;

}

int lttng_syscals_create_matching_event_notifiers(
		struct lttng_event_notifier_enabler *event_notifier_enabler,
		void *filter)
{
	int ret;
	struct lttng_enabler *base_enabler =
			lttng_event_notifier_enabler_as_enabler(event_notifier_enabler);
	enum lttng_kernel_syscall_entryexit entryexit =
			base_enabler->event_param.u.syscall.entryexit;

	if (entryexit == LTTNG_KERNEL_SYSCALL_ENTRY || entryexit == LTTNG_KERNEL_SYSCALL_ENTRYEXIT) {
		ret = create_matching_event_notifiers(event_notifier_enabler,
			filter, sc_table, ARRAY_SIZE(sc_table), SC_TYPE_ENTRY);
		if (ret)
			goto end;

		ret = create_matching_event_notifiers(event_notifier_enabler,
			filter, compat_sc_table, ARRAY_SIZE(compat_sc_table),
			SC_TYPE_COMPAT_ENTRY);
		if (ret)
			goto end;

		ret = create_unknown_event_notifier(event_notifier_enabler,
				SC_TYPE_ENTRY);
		if (ret)
			goto end;

		ret = create_unknown_event_notifier(event_notifier_enabler,
				SC_TYPE_COMPAT_ENTRY);
		if (ret)
			goto end;
	}

	if (entryexit == LTTNG_KERNEL_SYSCALL_EXIT || entryexit == LTTNG_KERNEL_SYSCALL_ENTRYEXIT) {
		ret = create_matching_event_notifiers(event_notifier_enabler,
			filter, sc_exit_table, ARRAY_SIZE(sc_exit_table),
			SC_TYPE_EXIT);
		if (ret)
			goto end;

		ret = create_unknown_event_notifier(event_notifier_enabler,
				SC_TYPE_EXIT);
		if (ret)
			goto end;

		ret = create_matching_event_notifiers(event_notifier_enabler,
			filter, compat_sc_exit_table, ARRAY_SIZE(compat_sc_exit_table),
			SC_TYPE_COMPAT_EXIT);
		if (ret)
			goto end;

		ret = create_unknown_event_notifier(event_notifier_enabler,
				SC_TYPE_COMPAT_EXIT);
		if (ret)
			goto end;
	}

end:
	return ret;
}

/*
 * Unregister the syscall event_notifier probes from the callsites.
 */
int lttng_syscalls_unregister_event_notifier_group(
		struct lttng_event_notifier_group *event_notifier_group)
{
	int ret;

	/*
	 * Only register the event_notifier probe on the `sys_enter` callsite for now.
	 * At the moment, we don't think it's desirable to have one fired
	 * event_notifier for the entry and one for the exit of a syscall.
	 */
	if (event_notifier_group->sys_enter_registered) {
		ret = lttng_wrapper_tracepoint_probe_unregister("sys_enter",
				(void *) syscall_entry_event_notifier_probe, event_notifier_group);
		if (ret)
			return ret;
		event_notifier_group->sys_enter_registered = 0;
	}
	if (event_notifier_group->sys_exit_registered) {
		ret = lttng_wrapper_tracepoint_probe_unregister("sys_exit",
				(void *) syscall_exit_event_notifier_probe, event_notifier_group);
		if (ret)
			return ret;
		event_notifier_group->sys_enter_registered = 0;
	}

	kfree(event_notifier_group->event_notifier_syscall_dispatch);
	kfree(event_notifier_group->event_notifier_exit_syscall_dispatch);
#ifdef CONFIG_COMPAT
	kfree(event_notifier_group->event_notifier_compat_syscall_dispatch);
	kfree(event_notifier_group->event_notifier_exit_compat_syscall_dispatch);
#endif
	return 0;
}

int lttng_syscalls_unregister_channel(struct lttng_channel *chan)
{
	int ret;

	if (!chan->sc_table)
		return 0;
	if (chan->sys_enter_registered) {
		ret = lttng_wrapper_tracepoint_probe_unregister("sys_enter",
				(void *) syscall_entry_event_probe, chan);
		if (ret)
			return ret;
		chan->sys_enter_registered = 0;
	}
	if (chan->sys_exit_registered) {
		ret = lttng_wrapper_tracepoint_probe_unregister("sys_exit",
				(void *) syscall_exit_event_probe, chan);
		if (ret)
			return ret;
		chan->sys_exit_registered = 0;
	}
	return 0;
}

int lttng_syscalls_destroy_event(struct lttng_channel *chan)
{
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
		it_name = entry->desc->event_name;
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
		it_name = entry->desc->event_name;
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

static
const char *get_syscall_name(const char *desc_name,
		enum lttng_syscall_abi abi,
		enum lttng_syscall_entryexit entryexit)
{
	size_t prefix_len = 0;


	switch (entryexit) {
	case LTTNG_SYSCALL_ENTRY:
		switch (abi) {
		case LTTNG_SYSCALL_ABI_NATIVE:
			prefix_len = strlen(SYSCALL_ENTRY_STR);
			break;
		case LTTNG_SYSCALL_ABI_COMPAT:
			prefix_len = strlen(COMPAT_SYSCALL_ENTRY_STR);
			break;
		}
		break;
	case LTTNG_SYSCALL_EXIT:
		switch (abi) {
		case LTTNG_SYSCALL_ABI_NATIVE:
			prefix_len = strlen(SYSCALL_EXIT_STR);
			break;
		case LTTNG_SYSCALL_ABI_COMPAT:
			prefix_len = strlen(COMPAT_SYSCALL_EXIT_STR);
			break;
		}
		break;
	}
	WARN_ON_ONCE(prefix_len == 0);
	return desc_name + prefix_len;
}

static
int lttng_syscall_filter_enable(
		struct lttng_syscall_filter *filter,
		const char *desc_name, enum lttng_syscall_abi abi,
		enum lttng_syscall_entryexit entryexit)
{
	const char *syscall_name;
	unsigned long *bitmap;
	int syscall_nr;

	syscall_name = get_syscall_name(desc_name, abi, entryexit);

	switch (abi) {
	case LTTNG_SYSCALL_ABI_NATIVE:
		syscall_nr = get_syscall_nr(syscall_name);
		break;
	case LTTNG_SYSCALL_ABI_COMPAT:
		syscall_nr = get_compat_syscall_nr(syscall_name);
		break;
	default:
		return -EINVAL;
	}
	if (syscall_nr < 0)
		return -ENOENT;

	switch (entryexit) {
	case LTTNG_SYSCALL_ENTRY:
		switch (abi) {
		case LTTNG_SYSCALL_ABI_NATIVE:
			bitmap = filter->sc_entry;
			break;
		case LTTNG_SYSCALL_ABI_COMPAT:
			bitmap = filter->sc_compat_entry;
			break;
		default:
			return -EINVAL;
		}
		break;
	case LTTNG_SYSCALL_EXIT:
		switch (abi) {
		case LTTNG_SYSCALL_ABI_NATIVE:
			bitmap = filter->sc_exit;
			break;
		case LTTNG_SYSCALL_ABI_COMPAT:
			bitmap = filter->sc_compat_exit;
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}
	if (test_bit(syscall_nr, bitmap))
		return -EEXIST;
	bitmap_set(bitmap, syscall_nr, 1);
	return 0;
}

int lttng_syscall_filter_enable_event_notifier(
		struct lttng_event_notifier *notifier)
{
	struct lttng_event_notifier_group *group = notifier->group;
	unsigned int syscall_id = notifier->u.syscall.syscall_id;
	struct hlist_head *dispatch_list;
	int ret = 0;

	WARN_ON_ONCE(notifier->instrumentation != LTTNG_KERNEL_SYSCALL);

	ret = lttng_syscall_filter_enable(group->sc_filter,
		notifier->desc->event_name, notifier->u.syscall.abi,
		notifier->u.syscall.entryexit);
	if (ret) {
		goto end;
	}

	switch (notifier->u.syscall.entryexit) {
	case LTTNG_SYSCALL_ENTRY:
		switch (notifier->u.syscall.abi) {
		case LTTNG_SYSCALL_ABI_NATIVE:
			dispatch_list = &group->event_notifier_syscall_dispatch[syscall_id];
			break;
		case LTTNG_SYSCALL_ABI_COMPAT:
			dispatch_list = &group->event_notifier_compat_syscall_dispatch[syscall_id];
			break;
		default:
			ret = -EINVAL;
			goto end;
		}
		break;
	case LTTNG_SYSCALL_EXIT:
		switch (notifier->u.syscall.abi) {
		case LTTNG_SYSCALL_ABI_NATIVE:
			dispatch_list = &group->event_notifier_exit_syscall_dispatch[syscall_id];
			break;
		case LTTNG_SYSCALL_ABI_COMPAT:
			dispatch_list = &group->event_notifier_exit_compat_syscall_dispatch[syscall_id];
			break;
		default:
			ret = -EINVAL;
			goto end;
		}
		break;
	default:
		ret = -EINVAL;
		goto end;
	}

	hlist_add_head_rcu(&notifier->u.syscall.node, dispatch_list);

end:
	return ret ;
}

int lttng_syscall_filter_enable_event(
		struct lttng_channel *channel,
		struct lttng_event *event)
{
	WARN_ON_ONCE(event->instrumentation != LTTNG_KERNEL_SYSCALL);

	return lttng_syscall_filter_enable(channel->sc_filter,
		event->desc->event_name, event->u.syscall.abi,
		event->u.syscall.entryexit);
}

static
int lttng_syscall_filter_disable(
		struct lttng_syscall_filter *filter,
		const char *desc_name, enum lttng_syscall_abi abi,
		enum lttng_syscall_entryexit entryexit)
{
	const char *syscall_name;
	unsigned long *bitmap;
	int syscall_nr;

	syscall_name = get_syscall_name(desc_name, abi, entryexit);

	switch (abi) {
	case LTTNG_SYSCALL_ABI_NATIVE:
		syscall_nr = get_syscall_nr(syscall_name);
		break;
	case LTTNG_SYSCALL_ABI_COMPAT:
		syscall_nr = get_compat_syscall_nr(syscall_name);
		break;
	default:
		return -EINVAL;
	}
	if (syscall_nr < 0)
		return -ENOENT;

	switch (entryexit) {
	case LTTNG_SYSCALL_ENTRY:
		switch (abi) {
		case LTTNG_SYSCALL_ABI_NATIVE:
			bitmap = filter->sc_entry;
			break;
		case LTTNG_SYSCALL_ABI_COMPAT:
			bitmap = filter->sc_compat_entry;
			break;
		default:
			return -EINVAL;
		}
		break;
	case LTTNG_SYSCALL_EXIT:
		switch (abi) {
		case LTTNG_SYSCALL_ABI_NATIVE:
			bitmap = filter->sc_exit;
			break;
		case LTTNG_SYSCALL_ABI_COMPAT:
			bitmap = filter->sc_compat_exit;
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}
	if (!test_bit(syscall_nr, bitmap))
		return -EEXIST;
	bitmap_clear(bitmap, syscall_nr, 1);

	return 0;
}

int lttng_syscall_filter_disable_event_notifier(
		struct lttng_event_notifier *notifier)
{
	struct lttng_event_notifier_group *group = notifier->group;
	int ret;

	WARN_ON_ONCE(notifier->instrumentation != LTTNG_KERNEL_SYSCALL);

	ret = lttng_syscall_filter_disable(group->sc_filter,
		notifier->desc->event_name, notifier->u.syscall.abi,
		notifier->u.syscall.entryexit);
	WARN_ON_ONCE(ret != 0);

	hlist_del_rcu(&notifier->u.syscall.node);
	return 0;
}

int lttng_syscall_filter_disable_event(
		struct lttng_channel *channel,
		struct lttng_event *event)
{
	return lttng_syscall_filter_disable(channel->sc_filter,
		event->desc->event_name, event->u.syscall.abi,
		event->u.syscall.entryexit);
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
		name = &entry->desc->event_name[strlen(SYSCALL_ENTRY_STR)];
	} else {
		index = (entry - table) + ARRAY_SIZE(sc_table);
		name = &entry->desc->event_name[strlen(COMPAT_SYSCALL_ENTRY_STR)];
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

/*
 * A syscall is enabled if it is traced for either entry or exit.
 */
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
		char state;

		if (channel->sc_table) {
			if (!(READ_ONCE(channel->syscall_all_entry)
					|| READ_ONCE(channel->syscall_all_exit)) && filter)
				state = test_bit(bit, filter->sc_entry)
					|| test_bit(bit, filter->sc_exit);
			else
				state = 1;
		} else {
			state = 0;
		}
		bt_bitfield_write_be(tmp_mask, char, bit, 1, state);
	}
	for (; bit < sc_tables_len; bit++) {
		char state;

		if (channel->compat_sc_table) {
			if (!(READ_ONCE(channel->syscall_all_entry)
					|| READ_ONCE(channel->syscall_all_exit)) && filter)
				state = test_bit(bit - ARRAY_SIZE(sc_table),
						filter->sc_compat_entry)
					|| test_bit(bit - ARRAY_SIZE(sc_table),
						filter->sc_compat_exit);
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
	return file_fd;

open_error:
	fput(syscall_list_file);
file_error:
	put_unused_fd(file_fd);
fd_error:
	return ret;
}
