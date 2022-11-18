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
#include <wrapper/rcu.h>
#include <wrapper/syscall.h>
#include <wrapper/limits.h>
#include <lttng/events.h>
#include <lttng/events-internal.h>
#include <lttng/utils.h>

#include "lttng-syscalls.h"

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

void syscall_entry_event_probe(void *__data, struct pt_regs *regs, long id);
void syscall_exit_event_probe(void *__data, struct pt_regs *regs, long ret);

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

#undef TP_PROBE_CB

extern const struct trace_syscall_table sc_table;
extern const struct trace_syscall_table compat_sc_table;

/* Event syscall exit table */
extern const struct trace_syscall_table sc_exit_table;
extern const struct trace_syscall_table compat_sc_exit_table;


#undef SC_EXIT

#undef CREATE_SYSCALL_TABLE

struct lttng_syscall_filter {
	DECLARE_BITMAP(sc_entry, NR_syscalls);
	DECLARE_BITMAP(sc_exit, NR_syscalls);
	DECLARE_BITMAP(sc_compat_entry, NR_compat_syscalls);
	DECLARE_BITMAP(sc_compat_exit, NR_compat_syscalls);

	/*
	 * Reference counters keeping track of number of events enabled
	 * for each bit.
	 */
	u32 sc_entry_refcount_map[NR_syscalls];
	u32 sc_exit_refcount_map[NR_syscalls];
	u32 sc_compat_entry_refcount_map[NR_compat_syscalls];
	u32 sc_compat_exit_refcount_map[NR_compat_syscalls];
};

static void syscall_entry_event_unknown(struct hlist_head *unknown_action_list_head,
	struct pt_regs *regs, long id)
{
	unsigned long args[LTTNG_SYSCALL_NR_ARGS];
	struct lttng_kernel_event_common_private *event_priv;

	lttng_syscall_get_arguments(current, regs, args);
	lttng_hlist_for_each_entry_rcu(event_priv, unknown_action_list_head, u.syscall.node) {
		if (unlikely(in_compat_syscall()))
			__event_probe__compat_syscall_entry_unknown(event_priv->pub, id, args);
		else
			__event_probe__syscall_entry_unknown(event_priv->pub, id, args);
	}
}

static __always_inline
void syscall_entry_event_call_func(struct hlist_head *action_list,
		void *func, unsigned int nrargs,
		struct pt_regs *regs)
{
	struct lttng_kernel_event_common_private *event_priv;

	switch (nrargs) {
	case 0:
	{
		void (*fptr)(void *__data) = func;

		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub);
		break;
	}
	case 1:
	{
		void (*fptr)(void *__data, unsigned long arg0) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, args[0]);
		break;
	}
	case 2:
	{
		void (*fptr)(void *__data,
			unsigned long arg0,
			unsigned long arg1) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, args[0], args[1]);
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
		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, args[0], args[1], args[2]);
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
		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, args[0], args[1], args[2], args[3]);
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
		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, args[0], args[1], args[2], args[3], args[4]);
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
		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, args[0], args[1], args[2],
			     args[3], args[4], args[5]);
		break;
	}
	default:
		break;
	}
}

void syscall_entry_event_probe(void *__data, struct pt_regs *regs, long id)
{
	struct lttng_kernel_syscall_table *syscall_table = __data;
	struct hlist_head *action_list, *unknown_action_list;
	const struct trace_syscall_entry *table, *entry;
	size_t table_len;

	if (unlikely(in_compat_syscall())) {
		struct lttng_syscall_filter *filter = syscall_table->sc_filter;

		if (id < 0 || id >= NR_compat_syscalls
			|| (!READ_ONCE(syscall_table->syscall_all_entry) && !test_bit(id, filter->sc_compat_entry))) {
			/* System call filtered out. */
			return;
		}
		table = compat_sc_table.table;
		table_len = compat_sc_table.len;
		unknown_action_list = &syscall_table->compat_unknown_syscall_dispatch;
	} else {
		struct lttng_syscall_filter *filter = syscall_table->sc_filter;

		if (id < 0 || id >= NR_syscalls
			|| (!READ_ONCE(syscall_table->syscall_all_entry) && !test_bit(id, filter->sc_entry))) {
			/* System call filtered out. */
			return;
		}
		table = sc_table.table;
		table_len = sc_table.len;
		unknown_action_list = &syscall_table->unknown_syscall_dispatch;
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
		action_list = &syscall_table->compat_syscall_dispatch[id];
	} else {
		action_list = &syscall_table->syscall_dispatch[id];
	}
	if (unlikely(hlist_empty(action_list)))
		return;

	syscall_entry_event_call_func(action_list, entry->event_func, entry->nrargs, regs);
}

static void syscall_exit_event_unknown(struct hlist_head *unknown_action_list_head,
	struct pt_regs *regs, long id, long ret)
{
	unsigned long args[LTTNG_SYSCALL_NR_ARGS];
	struct lttng_kernel_event_common_private *event_priv;

	lttng_syscall_get_arguments(current, regs, args);
	lttng_hlist_for_each_entry_rcu(event_priv, unknown_action_list_head, u.syscall.node) {
		if (unlikely(in_compat_syscall()))
			__event_probe__compat_syscall_exit_unknown(event_priv->pub, id, ret,
				args);
		else
			__event_probe__syscall_exit_unknown(event_priv->pub, id, ret, args);
	}
}

static __always_inline
void syscall_exit_event_call_func(struct hlist_head *action_list,
		void *func, unsigned int nrargs,
		struct pt_regs *regs, long ret)
{
	struct lttng_kernel_event_common_private *event_priv;

	switch (nrargs) {
	case 0:
	{
		void (*fptr)(void *__data, long ret) = func;

		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, ret);
		break;
	}
	case 1:
	{
		void (*fptr)(void *__data,
			long ret,
			unsigned long arg0) = func;
		unsigned long args[LTTNG_SYSCALL_NR_ARGS];

		lttng_syscall_get_arguments(current, regs, args);
		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, ret, args[0]);
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
		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, ret, args[0], args[1]);
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
		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, ret, args[0], args[1], args[2]);
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
		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, ret, args[0], args[1], args[2], args[3]);
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
		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, ret, args[0], args[1], args[2], args[3], args[4]);
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
		lttng_hlist_for_each_entry_rcu(event_priv, action_list, u.syscall.node)
			fptr(event_priv->pub, ret, args[0], args[1], args[2],
			     args[3], args[4], args[5]);
		break;
	}
	default:
		break;
	}
}

void syscall_exit_event_probe(void *__data, struct pt_regs *regs, long ret)
{
	struct lttng_kernel_syscall_table *syscall_table = __data;
	struct hlist_head *action_list, *unknown_action_list;
	const struct trace_syscall_entry *table, *entry;
	size_t table_len;
	long id;

	id = syscall_get_nr(current, regs);

	if (unlikely(in_compat_syscall())) {
		struct lttng_syscall_filter *filter = syscall_table->sc_filter;

		if (id < 0 || id >= NR_compat_syscalls
			|| (!READ_ONCE(syscall_table->syscall_all_exit) && !test_bit(id, filter->sc_compat_exit))) {
			/* System call filtered out. */
			return;
		}
		table = compat_sc_exit_table.table;
		table_len = compat_sc_exit_table.len;
		unknown_action_list = &syscall_table->compat_unknown_syscall_exit_dispatch;
	} else {
		struct lttng_syscall_filter *filter = syscall_table->sc_filter;

		if (id < 0 || id >= NR_syscalls
			|| (!READ_ONCE(syscall_table->syscall_all_exit) && !test_bit(id, filter->sc_exit))) {
			/* System call filtered out. */
			return;
		}
		table = sc_exit_table.table;
		table_len = sc_exit_table.len;
		unknown_action_list = &syscall_table->unknown_syscall_exit_dispatch;
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
		action_list = &syscall_table->compat_syscall_exit_dispatch[id];
	} else {
		action_list = &syscall_table->syscall_exit_dispatch[id];
	}
	if (unlikely(hlist_empty(action_list)))
		return;

	syscall_exit_event_call_func(action_list, entry->event_func, entry->nrargs,
			       regs, ret);
}

static
struct lttng_kernel_syscall_table *get_syscall_table_from_enabler(struct lttng_event_enabler_common *event_enabler)
{
	switch (event_enabler->enabler_type) {
	case LTTNG_EVENT_ENABLER_TYPE_RECORDER:
	{
		struct lttng_event_recorder_enabler *event_recorder_enabler =
			container_of(event_enabler, struct lttng_event_recorder_enabler, parent);
		return &event_recorder_enabler->chan->priv->parent.syscall_table;
	}
	case LTTNG_EVENT_ENABLER_TYPE_NOTIFIER:
	{
		struct lttng_event_notifier_enabler *event_notifier_enabler =
			container_of(event_enabler, struct lttng_event_notifier_enabler, parent);
		return &event_notifier_enabler->group->syscall_table;
	}
	default:
		return NULL;
	}
}

static
struct lttng_kernel_syscall_table *get_syscall_table_from_event(struct lttng_kernel_event_common *event)
{
	switch (event->type) {
	case LTTNG_KERNEL_EVENT_TYPE_RECORDER:
	{
		struct lttng_kernel_event_recorder *event_recorder =
			container_of(event, struct lttng_kernel_event_recorder, parent);
		return &event_recorder->chan->priv->parent.syscall_table;
	}
	case LTTNG_KERNEL_EVENT_TYPE_NOTIFIER:
	{
		struct lttng_kernel_event_notifier *event_notifier =
			container_of(event, struct lttng_kernel_event_notifier, parent);
		return &event_notifier->priv->group->syscall_table;
	}
	default:
		return NULL;
	}
}

static
void lttng_syscall_event_enabler_create_event(struct lttng_event_enabler_common *syscall_event_enabler,
		const struct lttng_kernel_event_desc *desc, enum sc_type type, unsigned int syscall_nr)
{
	struct lttng_kernel_event_common *event;

	switch (syscall_event_enabler->enabler_type) {
	case LTTNG_EVENT_ENABLER_TYPE_RECORDER:
	{
		struct lttng_event_recorder_enabler *syscall_event_recorder_enabler =
			container_of(syscall_event_enabler, struct lttng_event_recorder_enabler, parent);
		struct lttng_event_recorder_enabler *event_recorder_enabler;
		struct lttng_kernel_abi_event ev;

		/* We need to create an event for this syscall/enabler. */
		memset(&ev, 0, sizeof(ev));
		switch (type) {
		case SC_TYPE_ENTRY:
			ev.u.syscall.entryexit = LTTNG_KERNEL_ABI_SYSCALL_ENTRY;
			ev.u.syscall.abi = LTTNG_KERNEL_ABI_SYSCALL_ABI_NATIVE;
			break;
		case SC_TYPE_EXIT:
			ev.u.syscall.entryexit = LTTNG_KERNEL_ABI_SYSCALL_EXIT;
			ev.u.syscall.abi = LTTNG_KERNEL_ABI_SYSCALL_ABI_NATIVE;
			break;
		case SC_TYPE_COMPAT_ENTRY:
			ev.u.syscall.entryexit = LTTNG_KERNEL_ABI_SYSCALL_ENTRY;
			ev.u.syscall.abi = LTTNG_KERNEL_ABI_SYSCALL_ABI_COMPAT;
			break;
		case SC_TYPE_COMPAT_EXIT:
			ev.u.syscall.entryexit = LTTNG_KERNEL_ABI_SYSCALL_EXIT;
			ev.u.syscall.abi = LTTNG_KERNEL_ABI_SYSCALL_ABI_COMPAT;
			break;
		}
		strncpy(ev.name, desc->event_name, LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1);
		ev.name[LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1] = '\0';
		ev.instrumentation = LTTNG_KERNEL_ABI_SYSCALL;
		event_recorder_enabler = lttng_event_recorder_enabler_create(LTTNG_ENABLER_FORMAT_NAME, &ev,
				syscall_event_recorder_enabler->chan);
		WARN_ON_ONCE(!event_recorder_enabler);
		if (!event_recorder_enabler)
			return;
		event = _lttng_kernel_event_create(&event_recorder_enabler->parent, desc);
		WARN_ON_ONCE(IS_ERR(event));
		lttng_event_enabler_destroy(&event_recorder_enabler->parent);
		if (IS_ERR(event)) {
			printk(KERN_INFO "Unable to create event recorder %s\n", desc->event_name);
			return;
		}
		event->priv->u.syscall.syscall_id = syscall_nr;
		break;
	}
	case LTTNG_EVENT_ENABLER_TYPE_NOTIFIER:
	{
		struct lttng_event_notifier_enabler *syscall_event_notifier_enabler =
			container_of(syscall_event_enabler, struct lttng_event_notifier_enabler, parent);
		struct lttng_event_notifier_enabler *event_notifier_enabler;
		struct lttng_kernel_abi_event_notifier event_notifier_param;
		uint64_t user_token = syscall_event_enabler->user_token;
		uint64_t error_counter_index = syscall_event_notifier_enabler->error_counter_index;

		memset(&event_notifier_param, 0, sizeof(event_notifier_param));
		switch (type) {
		case SC_TYPE_ENTRY:
			event_notifier_param.event.u.syscall.entryexit = LTTNG_KERNEL_ABI_SYSCALL_ENTRY;
			event_notifier_param.event.u.syscall.abi = LTTNG_KERNEL_ABI_SYSCALL_ABI_NATIVE;
			break;
		case SC_TYPE_EXIT:
			event_notifier_param.event.u.syscall.entryexit = LTTNG_KERNEL_ABI_SYSCALL_EXIT;
			event_notifier_param.event.u.syscall.abi = LTTNG_KERNEL_ABI_SYSCALL_ABI_NATIVE;
			break;
		case SC_TYPE_COMPAT_ENTRY:
			event_notifier_param.event.u.syscall.entryexit = LTTNG_KERNEL_ABI_SYSCALL_ENTRY;
			event_notifier_param.event.u.syscall.abi = LTTNG_KERNEL_ABI_SYSCALL_ABI_COMPAT;
			break;
		case SC_TYPE_COMPAT_EXIT:
			event_notifier_param.event.u.syscall.entryexit = LTTNG_KERNEL_ABI_SYSCALL_EXIT;
			event_notifier_param.event.u.syscall.abi = LTTNG_KERNEL_ABI_SYSCALL_ABI_COMPAT;
			break;
		}
		strncat(event_notifier_param.event.name, desc->event_name,
			LTTNG_KERNEL_ABI_SYM_NAME_LEN - strlen(event_notifier_param.event.name) - 1);
		event_notifier_param.event.name[LTTNG_KERNEL_ABI_SYM_NAME_LEN - 1] = '\0';
		event_notifier_param.event.instrumentation = LTTNG_KERNEL_ABI_SYSCALL;
		event_notifier_param.event.token = user_token;
		event_notifier_param.error_counter_index = error_counter_index;

		event_notifier_enabler = lttng_event_notifier_enabler_create(LTTNG_ENABLER_FORMAT_NAME,
				&event_notifier_param, syscall_event_notifier_enabler->group);
		WARN_ON_ONCE(!event_notifier_enabler);
		event = _lttng_kernel_event_create(&event_notifier_enabler->parent, desc);
		WARN_ON_ONCE(IS_ERR(event));
		lttng_event_enabler_destroy(&event_notifier_enabler->parent);
		if (IS_ERR(event)) {
			printk(KERN_INFO "Unable to create event notifier %s\n", desc->event_name);
			return;
		}
		event->priv->u.syscall.syscall_id = syscall_nr;
		break;
	}
	default:
		break;
	}
}

static
void lttng_syscall_event_enabler_create_matching_syscall_table_events(struct lttng_event_enabler_common *syscall_event_enabler_common,
		const struct trace_syscall_entry *table, size_t table_len, enum sc_type type)
{
	struct lttng_event_ht *events_ht = lttng_get_event_ht_from_enabler(syscall_event_enabler_common);
	const struct lttng_kernel_event_desc *desc;
	unsigned int i;

#ifndef CONFIG_COMPAT
	if (type == SC_TYPE_COMPAT_ENTRY || type == SC_TYPE_COMPAT_EXIT)
		return;
#endif
	/* iterate over all syscall and create event that match */
	for (i = 0; i < table_len; i++) {
		struct lttng_kernel_event_common_private *event_priv;
		struct hlist_head *head;
		bool found = false;

		desc = table[i].desc;
		if (!desc) {
			/* Unknown syscall */
			continue;
		}

		if (!lttng_desc_match_enabler(desc, syscall_event_enabler_common))
			continue;

		/*
		 * Check if already created.
		 */
		head = utils_borrow_hash_table_bucket(events_ht->table, LTTNG_EVENT_HT_SIZE, desc->event_name);
		lttng_hlist_for_each_entry(event_priv, head, hlist_node) {
			if (lttng_event_enabler_desc_match_event(syscall_event_enabler_common, desc, event_priv->pub)) {
				found = true;
				break;
			}
		}
		if (found)
			continue;

		lttng_syscall_event_enabler_create_event(syscall_event_enabler_common, desc, type, i);
	}
}

static
bool lttng_syscall_event_enabler_is_wildcard_all(struct lttng_event_enabler_common *event_enabler)
{
	if (event_enabler->event_param.instrumentation != LTTNG_KERNEL_ABI_SYSCALL)
		return false;
	if (event_enabler->event_param.u.syscall.abi != LTTNG_KERNEL_ABI_SYSCALL_ABI_ALL)
		return false;
	if (event_enabler->event_param.u.syscall.match != LTTNG_KERNEL_ABI_SYSCALL_MATCH_NAME)
		return false;
	if (strcmp(event_enabler->event_param.name, "*"))
		return false;
	return true;
}

static
void create_unknown_syscall_event(struct lttng_event_enabler_common *event_enabler, enum sc_type type)
{
	struct lttng_event_ht *events_ht = lttng_get_event_ht_from_enabler(event_enabler);
	struct lttng_kernel_event_common_private *event_priv;
	const struct lttng_kernel_event_desc *desc;
	bool found = false;
	struct hlist_head *head;

#ifndef CONFIG_COMPAT
	if (type == SC_TYPE_COMPAT_ENTRY || type == SC_TYPE_COMPAT_EXIT)
		return;
#endif
	/*
	 * Considering that currently system calls can only be enabled on a per
	 * name basis (or wildcard based on a name), unknown syscall events are
	 * only used when matching *all* system calls, because this is the only
	 * case which can be associated with an unknown system call.
	 *
	 * When enabling system call on a per system call number basis will be
	 * supported, this will need to be revisited.
	 */
	if (!lttng_syscall_event_enabler_is_wildcard_all(event_enabler))
		return;

	switch (type) {
	case SC_TYPE_ENTRY:
		desc = &__event_desc___syscall_entry_unknown;
		break;
	case SC_TYPE_EXIT:
		desc = &__event_desc___syscall_exit_unknown;
		break;
	case SC_TYPE_COMPAT_ENTRY:
		desc = &__event_desc___compat_syscall_entry_unknown;
		break;
	case SC_TYPE_COMPAT_EXIT:
		desc = &__event_desc___compat_syscall_exit_unknown;
		break;
	default:
		WARN_ON_ONCE(1);
	}

	/*
	 * Check if already created.
	 */
	head = utils_borrow_hash_table_bucket(events_ht->table, LTTNG_EVENT_HT_SIZE, desc->event_name);
	lttng_hlist_for_each_entry(event_priv, head, hlist_node) {
		if (lttng_event_enabler_desc_match_event(event_enabler, desc, event_priv->pub)) {
			found = true;
			break;
		}
	}
	if (!found)
		lttng_syscall_event_enabler_create_event(event_enabler, desc, type, -1U);
}

static
void lttng_syscall_event_enabler_create_matching_events(struct lttng_event_enabler_common *event_enabler)
{
	enum lttng_kernel_abi_syscall_entryexit entryexit = event_enabler->event_param.u.syscall.entryexit;

	if (entryexit == LTTNG_KERNEL_ABI_SYSCALL_ENTRY || entryexit == LTTNG_KERNEL_ABI_SYSCALL_ENTRYEXIT) {
		lttng_syscall_event_enabler_create_matching_syscall_table_events(event_enabler,
			sc_table.table, sc_table.len, SC_TYPE_ENTRY);
		lttng_syscall_event_enabler_create_matching_syscall_table_events(event_enabler,
			compat_sc_table.table, compat_sc_table.len, SC_TYPE_COMPAT_ENTRY);
		create_unknown_syscall_event(event_enabler, SC_TYPE_ENTRY);
		create_unknown_syscall_event(event_enabler, SC_TYPE_COMPAT_ENTRY);
	}

	if (entryexit == LTTNG_KERNEL_ABI_SYSCALL_EXIT || entryexit == LTTNG_KERNEL_ABI_SYSCALL_ENTRYEXIT) {
		lttng_syscall_event_enabler_create_matching_syscall_table_events(event_enabler,
			sc_exit_table.table, sc_exit_table.len, SC_TYPE_EXIT);
		lttng_syscall_event_enabler_create_matching_syscall_table_events(event_enabler,
			compat_sc_exit_table.table, compat_sc_exit_table.len, SC_TYPE_COMPAT_EXIT);
		create_unknown_syscall_event(event_enabler, SC_TYPE_EXIT);
		create_unknown_syscall_event(event_enabler, SC_TYPE_COMPAT_EXIT);
	}
}

/*
 * Should be called with sessions lock held.
 */
int lttng_event_enabler_create_syscall_events_if_missing(struct lttng_event_enabler_common *syscall_event_enabler)
{
	struct lttng_kernel_syscall_table *syscall_table = get_syscall_table_from_enabler(syscall_event_enabler);
	int ret;

	if (!syscall_table->syscall_dispatch) {
		/* create syscall table mapping syscall to events */
		syscall_table->syscall_dispatch = kzalloc(sizeof(struct hlist_head) * sc_table.len, GFP_KERNEL);
		if (!syscall_table->syscall_dispatch)
			return -ENOMEM;
	}
	if (!syscall_table->syscall_exit_dispatch) {
		/* create syscall table mapping syscall to events */
		syscall_table->syscall_exit_dispatch = kzalloc(sizeof(struct hlist_head) * sc_exit_table.len, GFP_KERNEL);
		if (!syscall_table->syscall_exit_dispatch)
			return -ENOMEM;
	}

#ifdef CONFIG_COMPAT
	if (!syscall_table->compat_syscall_dispatch) {
		/* create syscall table mapping compat syscall to events */
		syscall_table->compat_syscall_dispatch = kzalloc(sizeof(struct hlist_head) * compat_sc_table.len, GFP_KERNEL);
		if (!syscall_table->compat_syscall_dispatch)
			return -ENOMEM;
	}

	if (!syscall_table->compat_syscall_exit_dispatch) {
		/* create syscall table mapping compat syscall to events */
		syscall_table->compat_syscall_exit_dispatch = kzalloc(sizeof(struct hlist_head) * compat_sc_exit_table.len, GFP_KERNEL);
		if (!syscall_table->compat_syscall_exit_dispatch)
			return -ENOMEM;
	}
#endif
	if (!syscall_table->sc_filter) {
		syscall_table->sc_filter = kzalloc(sizeof(struct lttng_syscall_filter),
				GFP_KERNEL);
		if (!syscall_table->sc_filter)
			return -ENOMEM;
	}

	if (!syscall_table->sys_enter_registered) {
		ret = lttng_tracepoint_probe_register("sys_enter",
				(void *) syscall_entry_event_probe, syscall_table);
		if (ret)
			return ret;
		syscall_table->sys_enter_registered = 1;
	}
	if (!syscall_table->sys_exit_registered) {
		ret = lttng_tracepoint_probe_register("sys_exit",
				(void *) syscall_exit_event_probe, syscall_table);
		if (ret) {
			WARN_ON_ONCE(lttng_tracepoint_probe_unregister("sys_enter",
				(void *) syscall_entry_event_probe, syscall_table));
			return ret;
		}
		syscall_table->sys_exit_registered = 1;
	}

	lttng_syscall_event_enabler_create_matching_events(syscall_event_enabler);

	return 0;
}

int lttng_syscalls_unregister_syscall_table(struct lttng_kernel_syscall_table *syscall_table)
{
	int ret;

	if (!syscall_table->syscall_dispatch)
		return 0;
	if (syscall_table->sys_enter_registered) {
		ret = lttng_tracepoint_probe_unregister("sys_enter",
				(void *) syscall_entry_event_probe, syscall_table);
		if (ret)
			return ret;
		syscall_table->sys_enter_registered = 0;
	}
	if (syscall_table->sys_exit_registered) {
		ret = lttng_tracepoint_probe_unregister("sys_exit",
				(void *) syscall_exit_event_probe, syscall_table);
		if (ret)
			return ret;
		syscall_table->sys_exit_registered = 0;
	}
	return 0;
}

int lttng_syscalls_destroy_syscall_table(struct lttng_kernel_syscall_table *syscall_table)
{
	kfree(syscall_table->syscall_dispatch);
	kfree(syscall_table->syscall_exit_dispatch);
#ifdef CONFIG_COMPAT
	kfree(syscall_table->compat_syscall_dispatch);
	kfree(syscall_table->compat_syscall_exit_dispatch);
#endif
	kfree(syscall_table->sc_filter);
	return 0;
}

static
uint32_t get_sc_tables_len(void)
{
	return sc_table.len + compat_sc_table.len;
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
		enum lttng_syscall_entryexit entryexit,
		unsigned int syscall_id)
{
	const char *syscall_name;
	unsigned long *bitmap;
	u32 *refcount_map;

	syscall_name = get_syscall_name(desc_name, abi, entryexit);

	switch (entryexit) {
	case LTTNG_SYSCALL_ENTRY:
		switch (abi) {
		case LTTNG_SYSCALL_ABI_NATIVE:
			bitmap = filter->sc_entry;
			refcount_map = filter->sc_entry_refcount_map;
			break;
		case LTTNG_SYSCALL_ABI_COMPAT:
			bitmap = filter->sc_compat_entry;
			refcount_map = filter->sc_compat_entry_refcount_map;
			break;
		default:
			return -EINVAL;
		}
		break;
	case LTTNG_SYSCALL_EXIT:
		switch (abi) {
		case LTTNG_SYSCALL_ABI_NATIVE:
			bitmap = filter->sc_exit;
			refcount_map = filter->sc_exit_refcount_map;
			break;
		case LTTNG_SYSCALL_ABI_COMPAT:
			bitmap = filter->sc_compat_exit;
			refcount_map = filter->sc_compat_exit_refcount_map;
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}
	if (refcount_map[syscall_id] == U32_MAX)
		return -EOVERFLOW;
	if (refcount_map[syscall_id]++ == 0)
		bitmap_set(bitmap, syscall_id, 1);
	return 0;
}

int lttng_syscall_filter_enable_event(struct lttng_kernel_event_common *event)
{
	struct lttng_kernel_syscall_table *syscall_table = get_syscall_table_from_event(event);
	unsigned int syscall_id = event->priv->u.syscall.syscall_id;
	struct hlist_head *dispatch_list;
	int ret = 0;

	WARN_ON_ONCE(event->priv->instrumentation != LTTNG_KERNEL_ABI_SYSCALL);

	/* Unknown syscall */
	if (syscall_id == -1U) {
		switch (event->priv->u.syscall.entryexit) {
		case LTTNG_SYSCALL_ENTRY:
			switch (event->priv->u.syscall.abi) {
			case LTTNG_SYSCALL_ABI_NATIVE:
				dispatch_list = &syscall_table->unknown_syscall_dispatch;
				break;
			case LTTNG_SYSCALL_ABI_COMPAT:
				dispatch_list = &syscall_table->compat_unknown_syscall_dispatch;
				break;
			default:
				ret = -EINVAL;
				goto end;
			}
			break;
		case LTTNG_SYSCALL_EXIT:
			switch (event->priv->u.syscall.abi) {
			case LTTNG_SYSCALL_ABI_NATIVE:
				dispatch_list = &syscall_table->unknown_syscall_exit_dispatch;
				break;
			case LTTNG_SYSCALL_ABI_COMPAT:
				dispatch_list = &syscall_table->compat_unknown_syscall_exit_dispatch;
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
	} else {
		ret = lttng_syscall_filter_enable(syscall_table->sc_filter,
			event->priv->desc->event_name, event->priv->u.syscall.abi,
			event->priv->u.syscall.entryexit, syscall_id);
		if (ret)
			return ret;

		switch (event->priv->u.syscall.entryexit) {
		case LTTNG_SYSCALL_ENTRY:
			switch (event->priv->u.syscall.abi) {
			case LTTNG_SYSCALL_ABI_NATIVE:
				dispatch_list = &syscall_table->syscall_dispatch[syscall_id];
				break;
			case LTTNG_SYSCALL_ABI_COMPAT:
				dispatch_list = &syscall_table->compat_syscall_dispatch[syscall_id];
				break;
			default:
				ret = -EINVAL;
				goto end;
			}
			break;
		case LTTNG_SYSCALL_EXIT:
			switch (event->priv->u.syscall.abi) {
			case LTTNG_SYSCALL_ABI_NATIVE:
				dispatch_list = &syscall_table->syscall_exit_dispatch[syscall_id];
				break;
			case LTTNG_SYSCALL_ABI_COMPAT:
				dispatch_list = &syscall_table->compat_syscall_exit_dispatch[syscall_id];
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
	}

	hlist_add_head_rcu(&event->priv->u.syscall.node, dispatch_list);
end:
	return ret;
}

static
int lttng_syscall_filter_disable(struct lttng_syscall_filter *filter,
		const char *desc_name, enum lttng_syscall_abi abi,
		enum lttng_syscall_entryexit entryexit,
		unsigned int syscall_id)
{
	const char *syscall_name;
	unsigned long *bitmap;
	u32 *refcount_map;

	syscall_name = get_syscall_name(desc_name, abi, entryexit);

	switch (entryexit) {
	case LTTNG_SYSCALL_ENTRY:
		switch (abi) {
		case LTTNG_SYSCALL_ABI_NATIVE:
			bitmap = filter->sc_entry;
			refcount_map = filter->sc_entry_refcount_map;
			break;
		case LTTNG_SYSCALL_ABI_COMPAT:
			bitmap = filter->sc_compat_entry;
			refcount_map = filter->sc_compat_entry_refcount_map;
			break;
		default:
			return -EINVAL;
		}
		break;
	case LTTNG_SYSCALL_EXIT:
		switch (abi) {
		case LTTNG_SYSCALL_ABI_NATIVE:
			bitmap = filter->sc_exit;
			refcount_map = filter->sc_exit_refcount_map;
			break;
		case LTTNG_SYSCALL_ABI_COMPAT:
			bitmap = filter->sc_compat_exit;
			refcount_map = filter->sc_compat_exit_refcount_map;
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}
	if (refcount_map[syscall_id] == 0)
		return -ENOENT;
	if (--refcount_map[syscall_id] == 0)
		bitmap_clear(bitmap, syscall_id, 1);
	return 0;
}

int lttng_syscall_filter_disable_event(struct lttng_kernel_event_common *event)
{
	struct lttng_kernel_syscall_table *syscall_table = get_syscall_table_from_event(event);
	unsigned int syscall_id = event->priv->u.syscall.syscall_id;
	int ret;

	/* Except for unknown syscall */
	if (syscall_id != -1U) {
		ret = lttng_syscall_filter_disable(syscall_table->sc_filter,
			event->priv->desc->event_name, event->priv->u.syscall.abi,
			event->priv->u.syscall.entryexit, syscall_id);
		if (ret)
			return ret;
	}
	hlist_del_rcu(&event->priv->u.syscall.node);
	return 0;
}

void lttng_syscall_table_set_wildcard_all(struct lttng_event_enabler_common *event_enabler)
{
	struct lttng_kernel_syscall_table *syscall_table = get_syscall_table_from_enabler(event_enabler);
	enum lttng_kernel_abi_syscall_entryexit entryexit;
	int enabled = event_enabler->enabled;

	if (!lttng_syscall_event_enabler_is_wildcard_all(event_enabler))
		return;
	entryexit = event_enabler->event_param.u.syscall.entryexit;
	if (entryexit == LTTNG_KERNEL_ABI_SYSCALL_ENTRY || entryexit == LTTNG_KERNEL_ABI_SYSCALL_ENTRYEXIT)
		WRITE_ONCE(syscall_table->syscall_all_entry, enabled);

	if (entryexit == LTTNG_KERNEL_ABI_SYSCALL_EXIT || entryexit == LTTNG_KERNEL_ABI_SYSCALL_ENTRYEXIT)
		WRITE_ONCE(syscall_table->syscall_all_exit, enabled);
}

static
const struct trace_syscall_entry *syscall_list_get_entry(loff_t *pos)
{
	const struct trace_syscall_entry *entry;
	int iter = 0;

	for (entry = sc_table.table;
			entry < sc_table.table + sc_table.len;
			 entry++) {
		if (iter++ >= *pos)
			return entry;
	}
	for (entry = compat_sc_table.table;
			entry < compat_sc_table.table + compat_sc_table.len;
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
	if (entry >= sc_table.table && entry < sc_table.table + sc_table.len) {
		if (bitness)
			*bitness = BITS_PER_LONG;
		if (table)
			*table = sc_table.table;
		return 0;
	}
	if (!(entry >= compat_sc_table.table
			&& entry < compat_sc_table.table + compat_sc_table.len)) {
		return -EINVAL;
	}
	if (bitness)
		*bitness = 32;
	if (table)
		*table = compat_sc_table.table;
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
	if (table == sc_table.table) {
		index = entry - table;
		name = &entry->desc->event_name[strlen(SYSCALL_ENTRY_STR)];
	} else {
		index = (entry - table) + sc_table.len;
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
long lttng_syscall_table_get_active_mask(struct lttng_kernel_syscall_table *syscall_table,
		struct lttng_kernel_abi_syscall_mask __user *usyscall_mask)
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
	filter = syscall_table->sc_filter;

	for (bit = 0; bit < sc_table.len; bit++) {
		char state;

		if (syscall_table->syscall_dispatch) {
			if (!(READ_ONCE(syscall_table->syscall_all_entry)
					|| READ_ONCE(syscall_table->syscall_all_exit)) && filter)
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

		if (syscall_table->compat_syscall_dispatch) {
			if (!(READ_ONCE(syscall_table->syscall_all_entry)
					|| READ_ONCE(syscall_table->syscall_all_exit)) && filter)
				state = test_bit(bit - sc_table.len,
						filter->sc_compat_entry)
					|| test_bit(bit - sc_table.len,
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

	file_fd = get_unused_fd_flags(0);
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
