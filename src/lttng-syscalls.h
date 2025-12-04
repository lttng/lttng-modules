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

#include <asm/ptrace.h>
#include <linux/compat.h>
#include <linux/fcntl.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/mman.h>
#include <linux/posix_types.h>

#include <lttng/events.h>
#include <lttng/kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,13,0))
#include <linux/landlock.h>
#else
struct landlock_ruleset_attr;

enum landlock_rule_type {
	LANDLOCK_RULE_PATH_BENEATH = 1,
};
#endif

/*
 * Forward declarations allowing LTTng to build its system call instrumentation
 * against old kernels which do not declare the more recent system call
 * argument structure types.
 */
struct clone_args;
struct file_handle;
struct futex_waitv;
struct io_uring_params;
struct mmap_arg_struct;
struct mmsghdr;
struct mount_attr;
struct oldold_utsname;
struct old_utsname;
struct old_itimerspec32;
struct old_timespec32;
struct old_timeval32;
struct old_timex32;
struct old_utimbuf32;
struct open_how;
struct rlimit64;
struct rseq;
struct sched_attr;
struct sel_arg_struct;
struct statx;
struct user_msghdr;

struct __aio_sigset;
struct __kernel_old_itimerval;
struct __kernel_timespec;
struct __kernel_timex;
struct __kernel_old_timeval;
struct __kernel_itimerspec;

union bpf_attr;

#ifndef __kernel_long_t
typedef long __kernel_long_t;
#endif

typedef __kernel_long_t __kernel_old_time_t;
typedef int __bitwise __kernel_rwf_t;
typedef __kernel_rwf_t rwf_t;
typedef s32 old_time32_t;

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,6,0))
/*
 * Forward declaration and typedef for old types expected by MIPS and POWER
 * system call instrumentation when building against kernel >= 5.6.0. To be
 * removed after those system call instrumentation headers are regenerated
 * against a recent kernel.
 */
typedef __kernel_old_time_t time_t;

struct itimerspec;
struct itimerval;
struct timeval;
struct timex;
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

void syscall_entry_event_probe(void *__data, struct pt_regs *regs, long id);

void syscall_exit_event_probe(void *__data, struct pt_regs *regs, long ret);

void syscall_entry_event_notifier_probe(void *__data, struct pt_regs *regs,
		long id);
void syscall_exit_event_notifier_probe(void *__data, struct pt_regs *regs,
		long ret);

int lttng_syscall_tables_init(void);
static inline void lttng_syscall_tables_exit(void) { }

#endif /* LTTNG_SYSCALLS_H */
