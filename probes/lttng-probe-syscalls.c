/*
 * probes/lttng-probe-sched.c
 *
 * Copyright 2010 (c) - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng sched probes.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include "../ltt-events.h"

#ifndef SYSCALL_DETAIL

/*
 * Create the tracepoint static inlines from the kernel to validate that our
 * trace event macros match the kernel we run on.
 */
#include <trace/events/syscalls.h>

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH ../instrumentation/events/lttng-module

#include "../instrumentation/events/lttng-module/syscalls.h"

#else	/* SYSCALL_DETAIL */

#include <linux/slab.h>
#include <asm/ptrace.h>
#include <asm/syscall.h>

static void syscall_entry_probe(void *__data, struct pt_regs *regs, long id);
static int lttng_syscalls_register_probe(struct lttng_probe_desc *desc);
static void lttng_syscalls_unregister_probe(struct lttng_probe_desc *desc);

static struct lttng_probe_desc *syscall_probe_desc;

/*
 * Create LTTng tracepoint probes.
 */
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS

/* Hijack probe callback for system calls */
#define TP_PROBE_CB(_template)		&syscall_entry_probe
#define TP_REGISTER_OVERRIDE		lttng_syscalls_register_probe
#define TP_UNREGISTER_OVERRIDE		lttng_syscalls_unregister_probe

#define TRACE_INCLUDE_PATH ../instrumentation/syscalls/headers

#include "../instrumentation/syscalls/headers/syscalls.h"

#undef TP_UNREGISTER_OVERRIDE
#undef TP_REGISTER_OVERRIDE
#undef TP_PROBE_CB
#undef LTTNG_PACKAGE_BUILD
#undef CREATE_TRACE_POINTS

struct trace_syscall_entry {
	void *func;
	const struct lttng_event_desc *desc;	/* Set dynamically */
	const struct lttng_event_field *fields;
	unsigned int nrargs;
};

static int sc_table_desc_filled;

#define CREATE_SYSCALL_TABLE

#undef TRACE_SYSCALL_TABLE
#define TRACE_SYSCALL_TABLE(_name, _nr, _nrargs)		\
	[ _nr ] = {						\
		.func = __event_probe__##_name,			\
		.nrargs = (_nrargs),				\
		.fields = __event_fields___##_name,		\
	},

static struct trace_syscall_entry sc_table[] = {
#include "../instrumentation/syscalls/headers/syscalls.h"
};

#undef CREATE_SYSCALL_TABLE

static void syscall_entry_probe(void *__data, struct pt_regs *regs, long id)
{
	struct trace_syscall_entry *entry;
	struct ltt_channel *chan = __data;
	struct ltt_event *event;

	if (unlikely(id >= ARRAY_SIZE(sc_table)))
		return;
	entry = &sc_table[id];
	if (unlikely(!entry->func))
		return;
	event = chan->sc_table[id];
	WARN_ON_ONCE(!event);

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

static const struct lttng_event_desc *find_syscall_desc(unsigned int id)
{
	unsigned int i;

	for (i = 0; i < syscall_probe_desc->nr_events; i++) {
		if (syscall_probe_desc->event_desc[i].fields
				== sc_table[id].fields)
			return &syscall_probe_desc->event_desc[i];
	}
	WARN_ON_ONCE(1);
	return NULL;
}

static void fill_sc_table_desc(void)
{
	unsigned int i;

	if (sc_table_desc_filled)
		return;
	/*
	 * This is O(n^2), but rare. Eventually get the TRACE_EVENT code
	 * to emit per-event symbols to skip this.
	 */
	for (i = 0; i < ARRAY_SIZE(sc_table); i++) {
		const struct lttng_event_desc **desc = &sc_table[i].desc;

		if (!sc_table[i].func)
			continue;
		(*desc) = find_syscall_desc(i);
	}
	sc_table_desc_filled = 1;
}


int lttng_syscalls_register(struct ltt_channel *chan, void *filter)
{
	unsigned int i;
	int ret;

	fill_sc_table_desc();

	if (!chan->sc_table) {
		/* create syscall table mapping syscall to events */
		chan->sc_table = kzalloc(sizeof(struct ltt_event *)
					* ARRAY_SIZE(sc_table), GFP_KERNEL);
		if (!chan->sc_table)
			return -ENOMEM;
	}

	/* Allocate events for each syscall, insert into table */
	for (i = 0; i < ARRAY_SIZE(sc_table); i++) {
		struct lttng_kernel_event ev;
		const struct lttng_event_desc *desc = sc_table[i].desc;

		/*
		 * Skip those already populated by previous failed
		 * register for this channel.
		 */
		if (chan->sc_table[i])
			continue;
		memset(&ev, 0, sizeof(ev));
		strncpy(ev.name, desc->name, LTTNG_SYM_NAME_LEN);
		ev.name[LTTNG_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_NOOP;
		chan->sc_table[i] = ltt_event_create(chan, &ev, filter);
		if (!chan->sc_table[i]) {
			/*
			 * If something goes wrong in event registration
			 * after the first one, we have no choice but to
			 * leave the previous events in there, until
			 * deleted by session teardown.
			 */
			return -EINVAL;
		}
	}
	ret = tracepoint_probe_register("syscall_entry",
			(void *) syscall_entry_probe, chan);
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
	ret = tracepoint_probe_unregister("syscall_entry",
			(void *) syscall_entry_probe, chan);
	if (ret)
		return ret;
	/* ltt_event destroy will be performed by ltt_session_destroy() */
	kfree(chan->sc_table);
	return 0;
}

static int lttng_syscalls_register_probe(struct lttng_probe_desc *desc)
{
	WARN_ON_ONCE(syscall_probe_desc);
	syscall_probe_desc = desc;
	return 0;
}

static void lttng_syscalls_unregister_probe(struct lttng_probe_desc *desc)
{
	WARN_ON_ONCE(!syscall_probe_desc);
	syscall_probe_desc = NULL;
}

#endif	/* SYSCALL_DETAIL */

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng sched probes");
