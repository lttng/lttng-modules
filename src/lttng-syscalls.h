/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-syscalls.h
 *
 * LTTng syscall header.
 *
 * Copyright (C) 2021 Francis Deslauriers <francis.deslauriers@efficios.com>
 */

#ifndef LTTNG_SYSCALLS_H
#define LTTNG_SYSCALLS_H

#include <stddef.h>

#include <asm/ptrace.h>

#include <lttng/events.h>
#include <lttng/kernel-version.h>


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

struct trace_syscall_entry {
	void *event_func;
	const struct lttng_kernel_event_desc *desc;
	const struct lttng_kernel_event_field * const *fields;
	unsigned int nrargs;
};

struct trace_syscall_table {
	const struct trace_syscall_entry *table;
	const size_t len;
};

extern
void syscall_entry_event_probe(void *__data, struct pt_regs *regs, long id);

extern
void syscall_exit_event_probe(void *__data, struct pt_regs *regs, long ret);

extern
void syscall_entry_event_notifier_probe(void *__data, struct pt_regs *regs,
		long id);
extern
void syscall_exit_event_notifier_probe(void *__data, struct pt_regs *regs,
		long ret);

#endif /* LTTNG_SYSCALLS_H */
