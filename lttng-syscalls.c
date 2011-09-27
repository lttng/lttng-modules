/*
 * lttng-syscalls.c
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng sched probes.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <asm/ptrace.h>
#include <asm/syscall.h>

#include "ltt-events.h"

#ifndef CONFIG_COMPAT
static inline int is_compat_task(void)
{
	return 0;
}
#endif

static void syscall_entry_probe(void *__data, struct pt_regs *regs, long id);

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
#include "instrumentation/syscalls/headers/syscalls_integers.h"
#include "instrumentation/syscalls/headers/syscalls_pointers.h"
#undef TP_PROBE_CB

#include "instrumentation/syscalls/headers/syscalls_unknown.h"

#undef TP_MODULE_OVERRIDE
#undef LTTNG_PACKAGE_BUILD
#undef CREATE_TRACE_POINTS

struct trace_syscall_entry {
	void *func;
	const struct lttng_event_desc *desc;
	const struct lttng_event_field *fields;
	unsigned int nrargs;
};

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_template, _name, _nr, _nrargs)	\
	[ _nr ] = {						\
		.func = __event_probe__##_template,		\
		.nrargs = (_nrargs),				\
		.fields = __event_fields___##_template,		\
		.desc = &__event_desc___##_name,		\
	},

#define CREATE_SYSCALL_TABLE

static const struct trace_syscall_entry sc_table[] = {
#include "instrumentation/syscalls/headers/syscalls_integers.h"
#include "instrumentation/syscalls/headers/syscalls_pointers.h"
};

/* Create compatibility syscall table */
static const struct trace_syscall_entry compat_sc_table[] = {
#include "instrumentation/syscalls/headers/compat_syscalls_integers.h"
#include "instrumentation/syscalls/headers/compat_syscalls_pointers.h"
};

#undef CREATE_SYSCALL_TABLE

static void syscall_entry_unknown(struct ltt_event *event,
	struct pt_regs *regs, unsigned int id)
{
	unsigned long args[UNKNOWN_SYSCALL_NRARGS];

	syscall_get_arguments(current, regs, 0, UNKNOWN_SYSCALL_NRARGS, args);
	__event_probe__sys_unknown(event, id, args);
}

/*
 * Currently, given that the kernel syscall metadata extraction only
 * considers native system calls (not 32-bit compability ones), we
 * fall-back on the "unknown" system call tracing for 32-bit compat.
 */
static void syscall_entry_probe(void *__data, struct pt_regs *regs, long id)
{
	struct ltt_channel *chan = __data;
	struct ltt_event *event, *unknown_event;
	const struct trace_syscall_entry *table, *entry;
	size_t table_len;

	if (unlikely(is_compat_task())) {
		table = compat_sc_table;
		table_len = ARRAY_SIZE(compat_sc_table);
		unknown_event = chan->sc_compat_unknown;
	} else {
		table = sc_table;
		table_len = ARRAY_SIZE(sc_table);
		unknown_event = chan->sc_unknown;
	}
	if (unlikely(id >= table_len)) {
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

static
int fill_table(const struct trace_syscall_entry *table, size_t table_len,
	struct ltt_event **chan_table, struct ltt_channel *chan, void *filter)
{
	unsigned int i;

	/* Allocate events for each syscall, insert into table */
	for (i = 0; i < table_len; i++) {
		struct lttng_kernel_event ev;
		const struct lttng_event_desc *desc = table[i].desc;

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
		strncpy(ev.name, desc->name, LTTNG_SYM_NAME_LEN);
		ev.name[LTTNG_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_NOOP;
		chan_table[i] = ltt_event_create(chan, &ev, filter,
						desc);
		if (!chan_table[i]) {
			/*
			 * If something goes wrong in event registration
			 * after the first one, we have no choice but to
			 * leave the previous events in there, until
			 * deleted by session teardown.
			 */
			return -EINVAL;
		}
	}
	return 0;
}

int lttng_syscalls_register(struct ltt_channel *chan, void *filter)
{
	int ret;

	wrapper_vmalloc_sync_all();

	if (!chan->sc_table) {
		/* create syscall table mapping syscall to events */
		chan->sc_table = kzalloc(sizeof(struct ltt_event *)
					* ARRAY_SIZE(sc_table), GFP_KERNEL);
		if (!chan->sc_table)
			return -ENOMEM;
	}

#ifdef CONFIG_COMPAT
	if (!chan->compat_sc_table) {
		/* create syscall table mapping compat syscall to events */
		chan->compat_sc_table = kzalloc(sizeof(struct ltt_event *)
					* ARRAY_SIZE(compat_sc_table), GFP_KERNEL);
		if (!chan->compat_sc_table)
			return -ENOMEM;
	}
#endif
	if (!chan->sc_unknown) {
		struct lttng_kernel_event ev;
		const struct lttng_event_desc *desc =
			&__event_desc___sys_unknown;

		memset(&ev, 0, sizeof(ev));
		strncpy(ev.name, desc->name, LTTNG_SYM_NAME_LEN);
		ev.name[LTTNG_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_NOOP;
		chan->sc_unknown = ltt_event_create(chan, &ev, filter,
						    desc);
		if (!chan->sc_unknown) {
			return -EINVAL;
		}
	}

	if (!chan->sc_compat_unknown) {
		struct lttng_kernel_event ev;
		const struct lttng_event_desc *desc =
			&__event_desc___compat_sys_unknown;

		memset(&ev, 0, sizeof(ev));
		strncpy(ev.name, desc->name, LTTNG_SYM_NAME_LEN);
		ev.name[LTTNG_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_NOOP;
		chan->sc_compat_unknown = ltt_event_create(chan, &ev, filter,
							   desc);
		if (!chan->sc_compat_unknown) {
			return -EINVAL;
		}
	}

	if (!chan->sc_exit) {
		struct lttng_kernel_event ev;
		const struct lttng_event_desc *desc =
			&__event_desc___exit_syscall;

		memset(&ev, 0, sizeof(ev));
		strncpy(ev.name, desc->name, LTTNG_SYM_NAME_LEN);
		ev.name[LTTNG_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_NOOP;
		chan->sc_exit = ltt_event_create(chan, &ev, filter,
						 desc);
		if (!chan->sc_exit) {
			return -EINVAL;
		}
	}

	ret = fill_table(sc_table, ARRAY_SIZE(sc_table),
			chan->sc_table, chan, filter);
	if (ret)
		return ret;
#ifdef CONFIG_COMPAT
	ret = fill_table(compat_sc_table, ARRAY_SIZE(compat_sc_table),
			chan->compat_sc_table, chan, filter);
	if (ret)
		return ret;
#endif
	ret = tracepoint_probe_register("sys_enter",
			(void *) syscall_entry_probe, chan);
	if (ret)
		return ret;
	/*
	 * We change the name of sys_exit tracepoint due to namespace
	 * conflict with sys_exit syscall entry.
	 */
	ret = tracepoint_probe_register("sys_exit",
			(void *) __event_probe__exit_syscall,
			chan->sc_exit);
	if (ret) {
		WARN_ON_ONCE(tracepoint_probe_unregister("sys_enter",
			(void *) syscall_entry_probe, chan));
	}
	return ret;
}

/*
 * Only called at session destruction.
 */
int lttng_syscalls_unregister(struct ltt_channel *chan)
{
	int ret;

	if (!chan->sc_table)
		return 0;
	ret = tracepoint_probe_unregister("sys_exit",
			(void *) __event_probe__exit_syscall,
			chan->sc_exit);
	if (ret)
		return ret;
	ret = tracepoint_probe_unregister("sys_enter",
			(void *) syscall_entry_probe, chan);
	if (ret)
		return ret;
	/* ltt_event destroy will be performed by ltt_session_destroy() */
	kfree(chan->sc_table);
#ifdef CONFIG_COMPAT
	kfree(chan->compat_sc_table);
#endif
	return 0;
}
