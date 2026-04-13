// SPDX-FileCopyrightText: 2026 EfficiOS Inc.
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

/* THIS FILE IS AUTO-GENERATED. DO NOT EDIT */

/* Generated from riscv-32-syscalls 7.2 */

#ifndef CREATE_SYSCALL_TABLE

#if !defined(_TRACE_SYSCALLS_INTEGERS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SYSCALLS_INTEGERS_H

#include <lttng/tracepoint-event.h>
#include <linux/syscalls.h>
#include "riscv-32-syscalls_integers_override.h"
#include "generic-32-syscalls_integers_override.h"
#include "generic-syscalls_integers_override.h"

#ifdef SC_ENTER
SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(syscalls_noargs,
	TP_FIELDS()
)
#ifndef OVERRIDE_32_vhangup
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, vhangup)
#endif
#ifndef OVERRIDE_32_sync
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, sync)
#endif
#ifndef OVERRIDE_32_sched_yield
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, sched_yield)
#endif
#ifndef OVERRIDE_32_restart_syscall
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, restart_syscall)
#endif
#ifndef OVERRIDE_32_rt_sigreturn
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, rt_sigreturn)
#endif
#ifndef OVERRIDE_32_setsid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, setsid)
#endif
#ifndef OVERRIDE_32_getpid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getpid)
#endif
#ifndef OVERRIDE_32_getppid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getppid)
#endif
#ifndef OVERRIDE_32_getuid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getuid)
#endif
#ifndef OVERRIDE_32_geteuid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, geteuid)
#endif
#ifndef OVERRIDE_32_getgid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getgid)
#endif
#ifndef OVERRIDE_32_getegid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getegid)
#endif
#ifndef OVERRIDE_32_gettid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, gettid)
#endif
#ifndef OVERRIDE_32_munlockall
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, munlockall)
#endif
#else /* #ifdef SC_ENTER */
#ifndef OVERRIDE_32_vhangup
SC_LTTNG_TRACEPOINT_EVENT(vhangup,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_sync
SC_LTTNG_TRACEPOINT_EVENT(sync,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_sched_yield
SC_LTTNG_TRACEPOINT_EVENT(sched_yield,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_restart_syscall
SC_LTTNG_TRACEPOINT_EVENT(restart_syscall,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_rt_sigreturn
SC_LTTNG_TRACEPOINT_EVENT(rt_sigreturn,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_setsid
SC_LTTNG_TRACEPOINT_EVENT(setsid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_getpid
SC_LTTNG_TRACEPOINT_EVENT(getpid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_getppid
SC_LTTNG_TRACEPOINT_EVENT(getppid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_getuid
SC_LTTNG_TRACEPOINT_EVENT(getuid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_geteuid
SC_LTTNG_TRACEPOINT_EVENT(geteuid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_getgid
SC_LTTNG_TRACEPOINT_EVENT(getgid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_getegid
SC_LTTNG_TRACEPOINT_EVENT(getegid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_gettid
SC_LTTNG_TRACEPOINT_EVENT(gettid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_munlockall
SC_LTTNG_TRACEPOINT_EVENT(munlockall,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#endif /* else #ifdef SC_ENTER */
#ifndef OVERRIDE_32_io_destroy
SC_LTTNG_TRACEPOINT_EVENT(io_destroy,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx),
	TP_ARGS(sc_exit(ret,) ctx),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(aio_context_t, ctx, ctx)))
)
#endif
#ifndef OVERRIDE_32_epoll_create1
SC_LTTNG_TRACEPOINT_EVENT(epoll_create1,
	TP_PROTO(sc_exit(long ret,) int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_dup
SC_LTTNG_TRACEPOINT_EVENT(dup,
	TP_PROTO(sc_exit(long ret,) unsigned int fildes),
	TP_ARGS(sc_exit(ret,) fildes),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fildes, fildes)))
)
#endif
#ifndef OVERRIDE_32_inotify_init1
SC_LTTNG_TRACEPOINT_EVENT(inotify_init1,
	TP_PROTO(sc_exit(long ret,) int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_fchdir
SC_LTTNG_TRACEPOINT_EVENT(fchdir,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)))
)
#endif
#ifndef OVERRIDE_32_close
SC_LTTNG_TRACEPOINT_EVENT(close,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)))
)
#endif
#ifndef OVERRIDE_32_fsync
SC_LTTNG_TRACEPOINT_EVENT(fsync,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)))
)
#endif
#ifndef OVERRIDE_32_fdatasync
SC_LTTNG_TRACEPOINT_EVENT(fdatasync,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)))
)
#endif
#ifndef OVERRIDE_32_personality
SC_LTTNG_TRACEPOINT_EVENT(personality,
	TP_PROTO(sc_exit(long ret,) unsigned int personality),
	TP_ARGS(sc_exit(ret,) personality),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, personality, personality)))
)
#endif
#ifndef OVERRIDE_32_exit
SC_LTTNG_TRACEPOINT_EVENT(exit,
	TP_PROTO(sc_exit(long ret,) int error_code),
	TP_ARGS(sc_exit(ret,) error_code),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, error_code, error_code)))
)
#endif
#ifndef OVERRIDE_32_exit_group
SC_LTTNG_TRACEPOINT_EVENT(exit_group,
	TP_PROTO(sc_exit(long ret,) int error_code),
	TP_ARGS(sc_exit(ret,) error_code),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, error_code, error_code)))
)
#endif
#ifndef OVERRIDE_32_unshare
SC_LTTNG_TRACEPOINT_EVENT(unshare,
	TP_PROTO(sc_exit(long ret,) unsigned long unshare_flags),
	TP_ARGS(sc_exit(ret,) unshare_flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, unshare_flags, unshare_flags)))
)
#endif
#ifndef OVERRIDE_32_timer_getoverrun
SC_LTTNG_TRACEPOINT_EVENT(timer_getoverrun,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id),
	TP_ARGS(sc_exit(ret,) timer_id),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(timer_t, timer_id, timer_id)))
)
#endif
#ifndef OVERRIDE_32_timer_delete
SC_LTTNG_TRACEPOINT_EVENT(timer_delete,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id),
	TP_ARGS(sc_exit(ret,) timer_id),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(timer_t, timer_id, timer_id)))
)
#endif
#ifndef OVERRIDE_32_sched_getscheduler
SC_LTTNG_TRACEPOINT_EVENT(sched_getscheduler,
	TP_PROTO(sc_exit(long ret,) pid_t pid),
	TP_ARGS(sc_exit(ret,) pid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)))
)
#endif
#ifndef OVERRIDE_32_sched_get_priority_max
SC_LTTNG_TRACEPOINT_EVENT(sched_get_priority_max,
	TP_PROTO(sc_exit(long ret,) int policy),
	TP_ARGS(sc_exit(ret,) policy),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, policy, policy)))
)
#endif
#ifndef OVERRIDE_32_sched_get_priority_min
SC_LTTNG_TRACEPOINT_EVENT(sched_get_priority_min,
	TP_PROTO(sc_exit(long ret,) int policy),
	TP_ARGS(sc_exit(ret,) policy),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, policy, policy)))
)
#endif
#ifndef OVERRIDE_32_setgid
SC_LTTNG_TRACEPOINT_EVENT(setgid,
	TP_PROTO(sc_exit(long ret,) gid_t gid),
	TP_ARGS(sc_exit(ret,) gid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(gid_t, gid, gid)))
)
#endif
#ifndef OVERRIDE_32_setuid
SC_LTTNG_TRACEPOINT_EVENT(setuid,
	TP_PROTO(sc_exit(long ret,) uid_t uid),
	TP_ARGS(sc_exit(ret,) uid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(uid_t, uid, uid)))
)
#endif
#ifndef OVERRIDE_32_setfsuid
SC_LTTNG_TRACEPOINT_EVENT(setfsuid,
	TP_PROTO(sc_exit(long ret,) uid_t uid),
	TP_ARGS(sc_exit(ret,) uid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(uid_t, uid, uid)))
)
#endif
#ifndef OVERRIDE_32_setfsgid
SC_LTTNG_TRACEPOINT_EVENT(setfsgid,
	TP_PROTO(sc_exit(long ret,) gid_t gid),
	TP_ARGS(sc_exit(ret,) gid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(gid_t, gid, gid)))
)
#endif
#ifndef OVERRIDE_32_getpgid
SC_LTTNG_TRACEPOINT_EVENT(getpgid,
	TP_PROTO(sc_exit(long ret,) pid_t pid),
	TP_ARGS(sc_exit(ret,) pid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)))
)
#endif
#ifndef OVERRIDE_32_getsid
SC_LTTNG_TRACEPOINT_EVENT(getsid,
	TP_PROTO(sc_exit(long ret,) pid_t pid),
	TP_ARGS(sc_exit(ret,) pid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)))
)
#endif
#ifndef OVERRIDE_32_umask
SC_LTTNG_TRACEPOINT_EVENT(umask,
	TP_PROTO(sc_exit(long ret,) int mask),
	TP_ARGS(sc_exit(ret,) mask),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, mask, mask)))
)
#endif
#ifndef OVERRIDE_32_brk
SC_LTTNG_TRACEPOINT_EVENT(brk,
	TP_PROTO(sc_exit(long ret,) unsigned long brk),
	TP_ARGS(sc_exit(ret,) brk),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, brk, brk)))
)
#endif
#ifndef OVERRIDE_32_mlockall
SC_LTTNG_TRACEPOINT_EVENT(mlockall,
	TP_PROTO(sc_exit(long ret,) int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_syncfs
SC_LTTNG_TRACEPOINT_EVENT(syncfs,
	TP_PROTO(sc_exit(long ret,) int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)))
)
#endif
#ifndef OVERRIDE_32_userfaultfd
SC_LTTNG_TRACEPOINT_EVENT(userfaultfd,
	TP_PROTO(sc_exit(long ret,) int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_memfd_secret
SC_LTTNG_TRACEPOINT_EVENT(memfd_secret,
	TP_PROTO(sc_exit(long ret,) unsigned int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_eventfd2
SC_LTTNG_TRACEPOINT_EVENT(eventfd2,
	TP_PROTO(sc_exit(long ret,) unsigned int count, int flags),
	TP_ARGS(sc_exit(ret,) count, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, count, count)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_inotify_rm_watch
SC_LTTNG_TRACEPOINT_EVENT(inotify_rm_watch,
	TP_PROTO(sc_exit(long ret,) int fd, __s32 wd),
	TP_ARGS(sc_exit(ret,) fd, wd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(__s32, wd, wd)))
)
#endif
#ifndef OVERRIDE_32_ioprio_get
SC_LTTNG_TRACEPOINT_EVENT(ioprio_get,
	TP_PROTO(sc_exit(long ret,) int which, int who),
	TP_ARGS(sc_exit(ret,) which, who),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(int, who, who)))
)
#endif
#ifndef OVERRIDE_32_flock
SC_LTTNG_TRACEPOINT_EVENT(flock,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd),
	TP_ARGS(sc_exit(ret,) fd, cmd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(unsigned int, cmd, cmd)))
)
#endif
#ifndef OVERRIDE_32_ftruncate64
SC_LTTNG_TRACEPOINT_EVENT(ftruncate64,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, loff_t length),
	TP_ARGS(sc_exit(ret,) fd, length),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(loff_t, length, length)))
)
#endif
#ifndef OVERRIDE_32_fchmod
SC_LTTNG_TRACEPOINT_EVENT(fchmod,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, umode_t mode),
	TP_ARGS(sc_exit(ret,) fd, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_timerfd_create
SC_LTTNG_TRACEPOINT_EVENT(timerfd_create,
	TP_PROTO(sc_exit(long ret,) int clockid, int flags),
	TP_ARGS(sc_exit(ret,) clockid, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, clockid, clockid)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_kill
SC_LTTNG_TRACEPOINT_EVENT(kill,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int sig),
	TP_ARGS(sc_exit(ret,) pid, sig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, sig, sig)))
)
#endif
#ifndef OVERRIDE_32_tkill
SC_LTTNG_TRACEPOINT_EVENT(tkill,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int sig),
	TP_ARGS(sc_exit(ret,) pid, sig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, sig, sig)))
)
#endif
#ifndef OVERRIDE_32_getpriority
SC_LTTNG_TRACEPOINT_EVENT(getpriority,
	TP_PROTO(sc_exit(long ret,) int which, int who),
	TP_ARGS(sc_exit(ret,) which, who),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(int, who, who)))
)
#endif
#ifndef OVERRIDE_32_setregid
SC_LTTNG_TRACEPOINT_EVENT(setregid,
	TP_PROTO(sc_exit(long ret,) gid_t rgid, gid_t egid),
	TP_ARGS(sc_exit(ret,) rgid, egid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(gid_t, rgid, rgid)) sc_in(ctf_integer(gid_t, egid, egid)))
)
#endif
#ifndef OVERRIDE_32_setreuid
SC_LTTNG_TRACEPOINT_EVENT(setreuid,
	TP_PROTO(sc_exit(long ret,) uid_t ruid, uid_t euid),
	TP_ARGS(sc_exit(ret,) ruid, euid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(uid_t, ruid, ruid)) sc_in(ctf_integer(uid_t, euid, euid)))
)
#endif
#ifndef OVERRIDE_32_setpgid
SC_LTTNG_TRACEPOINT_EVENT(setpgid,
	TP_PROTO(sc_exit(long ret,) pid_t pid, pid_t pgid),
	TP_ARGS(sc_exit(ret,) pid, pgid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(pid_t, pgid, pgid)))
)
#endif
#ifndef OVERRIDE_32_msgget
SC_LTTNG_TRACEPOINT_EVENT(msgget,
	TP_PROTO(sc_exit(long ret,) key_t key, int msgflg),
	TP_ARGS(sc_exit(ret,) key, msgflg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(key_t, key, key)) sc_in(ctf_integer(int, msgflg, msgflg)))
)
#endif
#ifndef OVERRIDE_32_listen
SC_LTTNG_TRACEPOINT_EVENT(listen,
	TP_PROTO(sc_exit(long ret,) int fd, int backlog),
	TP_ARGS(sc_exit(ret,) fd, backlog),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(int, backlog, backlog)))
)
#endif
#ifndef OVERRIDE_32_shutdown
SC_LTTNG_TRACEPOINT_EVENT(shutdown,
	TP_PROTO(sc_exit(long ret,) int fd, int how),
	TP_ARGS(sc_exit(ret,) fd, how),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(int, how, how)))
)
#endif
#ifndef OVERRIDE_32_munmap
SC_LTTNG_TRACEPOINT_EVENT(munmap,
	TP_PROTO(sc_exit(long ret,) unsigned long addr, size_t len),
	TP_ARGS(sc_exit(ret,) addr, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, addr, addr)) sc_in(ctf_integer(size_t, len, len)))
)
#endif
#ifndef OVERRIDE_32_mlock
SC_LTTNG_TRACEPOINT_EVENT(mlock,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len),
	TP_ARGS(sc_exit(ret,) start, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(size_t, len, len)))
)
#endif
#ifndef OVERRIDE_32_munlock
SC_LTTNG_TRACEPOINT_EVENT(munlock,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len),
	TP_ARGS(sc_exit(ret,) start, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(size_t, len, len)))
)
#endif
#ifndef OVERRIDE_32_fanotify_init
SC_LTTNG_TRACEPOINT_EVENT(fanotify_init,
	TP_PROTO(sc_exit(long ret,) unsigned int flags, unsigned int event_f_flags),
	TP_ARGS(sc_exit(ret,) flags, event_f_flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_in(ctf_integer(unsigned int, event_f_flags, event_f_flags)))
)
#endif
#ifndef OVERRIDE_32_setns
SC_LTTNG_TRACEPOINT_EVENT(setns,
	TP_PROTO(sc_exit(long ret,) int fd, int flags),
	TP_ARGS(sc_exit(ret,) fd, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_pidfd_open
SC_LTTNG_TRACEPOINT_EVENT(pidfd_open,
	TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned int flags),
	TP_ARGS(sc_exit(ret,) pid, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_landlock_restrict_self
SC_LTTNG_TRACEPOINT_EVENT(landlock_restrict_self,
	TP_PROTO(sc_exit(long ret,) const int ruleset_fd, const __u32 flags),
	TP_ARGS(sc_exit(ret,) ruleset_fd, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const int, ruleset_fd, ruleset_fd)) sc_in(ctf_integer(const __u32, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_process_mrelease
SC_LTTNG_TRACEPOINT_EVENT(process_mrelease,
	TP_PROTO(sc_exit(long ret,) int pidfd, unsigned int flags),
	TP_ARGS(sc_exit(ret,) pidfd, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, pidfd, pidfd)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_dup3
SC_LTTNG_TRACEPOINT_EVENT(dup3,
	TP_PROTO(sc_exit(long ret,) unsigned int oldfd, unsigned int newfd, int flags),
	TP_ARGS(sc_exit(ret,) oldfd, newfd, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, oldfd, oldfd)) sc_in(ctf_integer(unsigned int, newfd, newfd)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_fcntl64
SC_LTTNG_TRACEPOINT_EVENT(fcntl64,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) fd, cmd, arg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(unsigned int, cmd, cmd)) sc_inout(ctf_integer(unsigned long, arg, arg)))
)
#endif
#ifndef OVERRIDE_32_ioctl
SC_LTTNG_TRACEPOINT_EVENT(ioctl,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) fd, cmd, arg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(unsigned int, cmd, cmd)) sc_inout(ctf_integer(unsigned long, arg, arg)))
)
#endif
#ifndef OVERRIDE_32_ioprio_set
SC_LTTNG_TRACEPOINT_EVENT(ioprio_set,
	TP_PROTO(sc_exit(long ret,) int which, int who, int ioprio),
	TP_ARGS(sc_exit(ret,) which, who, ioprio),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(int, who, who)) sc_in(ctf_integer(int, ioprio, ioprio)))
)
#endif
#ifndef OVERRIDE_32_fchown
SC_LTTNG_TRACEPOINT_EVENT(fchown,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, uid_t user, gid_t group),
	TP_ARGS(sc_exit(ret,) fd, user, group),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(uid_t, user, user)) sc_in(ctf_integer(gid_t, group, group)))
)
#endif
#ifndef OVERRIDE_32_tgkill
SC_LTTNG_TRACEPOINT_EVENT(tgkill,
	TP_PROTO(sc_exit(long ret,) pid_t tgid, pid_t pid, int sig),
	TP_ARGS(sc_exit(ret,) tgid, pid, sig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, tgid, tgid)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, sig, sig)))
)
#endif
#ifndef OVERRIDE_32_setpriority
SC_LTTNG_TRACEPOINT_EVENT(setpriority,
	TP_PROTO(sc_exit(long ret,) int which, int who, int niceval),
	TP_ARGS(sc_exit(ret,) which, who, niceval),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(int, who, who)) sc_in(ctf_integer(int, niceval, niceval)))
)
#endif
#ifndef OVERRIDE_32_setresuid
SC_LTTNG_TRACEPOINT_EVENT(setresuid,
	TP_PROTO(sc_exit(long ret,) uid_t ruid, uid_t euid, uid_t suid),
	TP_ARGS(sc_exit(ret,) ruid, euid, suid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(uid_t, ruid, ruid)) sc_in(ctf_integer(uid_t, euid, euid)) sc_in(ctf_integer(uid_t, suid, suid)))
)
#endif
#ifndef OVERRIDE_32_setresgid
SC_LTTNG_TRACEPOINT_EVENT(setresgid,
	TP_PROTO(sc_exit(long ret,) gid_t rgid, gid_t egid, gid_t sgid),
	TP_ARGS(sc_exit(ret,) rgid, egid, sgid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(gid_t, rgid, rgid)) sc_in(ctf_integer(gid_t, egid, egid)) sc_in(ctf_integer(gid_t, sgid, sgid)))
)
#endif
#ifndef OVERRIDE_32_semget
SC_LTTNG_TRACEPOINT_EVENT(semget,
	TP_PROTO(sc_exit(long ret,) key_t key, int nsems, int semflg),
	TP_ARGS(sc_exit(ret,) key, nsems, semflg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(key_t, key, key)) sc_in(ctf_integer(int, nsems, nsems)) sc_in(ctf_integer(int, semflg, semflg)))
)
#endif
#ifndef OVERRIDE_32_shmget
SC_LTTNG_TRACEPOINT_EVENT(shmget,
	TP_PROTO(sc_exit(long ret,) key_t key, size_t size, int shmflg),
	TP_ARGS(sc_exit(ret,) key, size, shmflg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(key_t, key, key)) sc_in(ctf_integer(size_t, size, size)) sc_in(ctf_integer(int, shmflg, shmflg)))
)
#endif
#ifndef OVERRIDE_32_socket
SC_LTTNG_TRACEPOINT_EVENT(socket,
	TP_PROTO(sc_exit(long ret,) int family, int type, int protocol),
	TP_ARGS(sc_exit(ret,) family, type, protocol),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, family, family)) sc_in(ctf_integer(int, type, type)) sc_in(ctf_integer(int, protocol, protocol)))
)
#endif
#ifndef OVERRIDE_32_readahead
SC_LTTNG_TRACEPOINT_EVENT(readahead,
	TP_PROTO(sc_exit(long ret,) int fd, loff_t offset, size_t count),
	TP_ARGS(sc_exit(ret,) fd, offset, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(loff_t, offset, offset)) sc_in(ctf_integer(size_t, count, count)))
)
#endif
#ifndef OVERRIDE_32_mprotect
SC_LTTNG_TRACEPOINT_EVENT(mprotect,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, unsigned long prot),
	TP_ARGS(sc_exit(ret,) start, len, prot),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(size_t, len, len)) sc_in(ctf_integer(unsigned long, prot, prot)))
)
#endif
#ifndef OVERRIDE_32_msync
SC_LTTNG_TRACEPOINT_EVENT(msync,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, int flags),
	TP_ARGS(sc_exit(ret,) start, len, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(size_t, len, len)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_madvise
SC_LTTNG_TRACEPOINT_EVENT(madvise,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len_in, int behavior),
	TP_ARGS(sc_exit(ret,) start, len_in, behavior),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(size_t, len_in, len_in)) sc_in(ctf_integer(int, behavior, behavior)))
)
#endif
#ifndef OVERRIDE_32_riscv_flush_icache
SC_LTTNG_TRACEPOINT_EVENT(riscv_flush_icache,
	TP_PROTO(sc_exit(long ret,) uintptr_t start, uintptr_t end, uintptr_t flags),
	TP_ARGS(sc_exit(ret,) start, end, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(uintptr_t, start, start)) sc_in(ctf_integer(uintptr_t, end, end)) sc_in(ctf_integer(uintptr_t, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_membarrier
SC_LTTNG_TRACEPOINT_EVENT(membarrier,
	TP_PROTO(sc_exit(long ret,) int cmd, unsigned int flags, int cpu_id),
	TP_ARGS(sc_exit(ret,) cmd, flags, cpu_id),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, cmd, cmd)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_in(ctf_integer(int, cpu_id, cpu_id)))
)
#endif
#ifndef OVERRIDE_32_mlock2
SC_LTTNG_TRACEPOINT_EVENT(mlock2,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, int flags),
	TP_ARGS(sc_exit(ret,) start, len, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(size_t, len, len)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_fsmount
SC_LTTNG_TRACEPOINT_EVENT(fsmount,
	TP_PROTO(sc_exit(long ret,) int fs_fd, unsigned int flags, unsigned int attr_flags),
	TP_ARGS(sc_exit(ret,) fs_fd, flags, attr_flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fs_fd, fs_fd)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_in(ctf_integer(unsigned int, attr_flags, attr_flags)))
)
#endif
#ifndef OVERRIDE_32_close_range
SC_LTTNG_TRACEPOINT_EVENT(close_range,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int max_fd, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, max_fd, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(unsigned int, max_fd, max_fd)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_pidfd_getfd
SC_LTTNG_TRACEPOINT_EVENT(pidfd_getfd,
	TP_PROTO(sc_exit(long ret,) int pidfd, int fd, unsigned int flags),
	TP_ARGS(sc_exit(ret,) pidfd, fd, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, pidfd, pidfd)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_fallocate
SC_LTTNG_TRACEPOINT_EVENT(fallocate,
	TP_PROTO(sc_exit(long ret,) int fd, int mode, loff_t offset, loff_t len),
	TP_ARGS(sc_exit(ret,) fd, mode, offset, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(int, mode, mode)) sc_in(ctf_integer(loff_t, offset, offset)) sc_in(ctf_integer(loff_t, len, len)))
)
#endif
#ifndef OVERRIDE_32_tee
SC_LTTNG_TRACEPOINT_EVENT(tee,
	TP_PROTO(sc_exit(long ret,) int fdin, int fdout, size_t len, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fdin, fdout, len, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fdin, fdin)) sc_in(ctf_integer(int, fdout, fdout)) sc_in(ctf_integer(size_t, len, len)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_sync_file_range
SC_LTTNG_TRACEPOINT_EVENT(sync_file_range,
	TP_PROTO(sc_exit(long ret,) int fd, loff_t offset, loff_t nbytes, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, offset, nbytes, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(loff_t, offset, offset)) sc_in(ctf_integer(loff_t, nbytes, nbytes)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_ptrace
SC_LTTNG_TRACEPOINT_EVENT(ptrace,
	TP_PROTO(sc_exit(long ret,) long request, long pid, unsigned long addr, unsigned long data),
	TP_ARGS(sc_exit(ret,) request, pid, addr, data),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(long, request, request)) sc_in(ctf_integer(long, pid, pid)) sc_inout(ctf_integer(unsigned long, addr, addr)) sc_inout(ctf_integer(unsigned long, data, data)))
)
#endif
#ifndef OVERRIDE_32_semctl
SC_LTTNG_TRACEPOINT_EVENT(semctl,
	TP_PROTO(sc_exit(long ret,) int semid, int semnum, int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) semid, semnum, cmd, arg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, semid, semid)) sc_in(ctf_integer(int, semnum, semnum)) sc_in(ctf_integer(int, cmd, cmd)) sc_inout(ctf_integer(unsigned long, arg, arg)))
)
#endif
#ifndef OVERRIDE_32_fadvise64_64
SC_LTTNG_TRACEPOINT_EVENT(fadvise64_64,
	TP_PROTO(sc_exit(long ret,) int fd, loff_t offset, loff_t len, int advice),
	TP_ARGS(sc_exit(ret,) fd, offset, len, advice),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(loff_t, offset, offset)) sc_in(ctf_integer(loff_t, len, len)) sc_in(ctf_integer(int, advice, advice)))
)
#endif
#ifndef OVERRIDE_32_set_mempolicy_home_node
SC_LTTNG_TRACEPOINT_EVENT(set_mempolicy_home_node,
	TP_PROTO(sc_exit(long ret,) unsigned long start, unsigned long len, unsigned long home_node, unsigned long flags),
	TP_ARGS(sc_exit(ret,) start, len, home_node, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(unsigned long, len, len)) sc_in(ctf_integer(unsigned long, home_node, home_node)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_prctl
SC_LTTNG_TRACEPOINT_EVENT(prctl,
	TP_PROTO(sc_exit(long ret,) int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5),
	TP_ARGS(sc_exit(ret,) option, arg2, arg3, arg4, arg5),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, option, option)) sc_inout(ctf_integer(unsigned long, arg2, arg2)) sc_in(ctf_integer(unsigned long, arg3, arg3)) sc_in(ctf_integer(unsigned long, arg4, arg4)) sc_in(ctf_integer(unsigned long, arg5, arg5)))
)
#endif
#ifndef OVERRIDE_32_mremap
SC_LTTNG_TRACEPOINT_EVENT(mremap,
	TP_PROTO(sc_exit(long ret,) unsigned long addr, unsigned long old_len, unsigned long new_len, unsigned long flags, unsigned long new_addr),
	TP_ARGS(sc_exit(ret,) addr, old_len, new_len, flags, new_addr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, addr, addr)) sc_in(ctf_integer(unsigned long, old_len, old_len)) sc_in(ctf_integer(unsigned long, new_len, new_len)) sc_in(ctf_integer(unsigned long, flags, flags)) sc_in(ctf_integer(unsigned long, new_addr, new_addr)))
)
#endif
#ifndef OVERRIDE_32_keyctl
SC_LTTNG_TRACEPOINT_EVENT(keyctl,
	TP_PROTO(sc_exit(long ret,) int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5),
	TP_ARGS(sc_exit(ret,) option, arg2, arg3, arg4, arg5),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, option, option)) sc_inout(ctf_integer(unsigned long, arg2, arg2)) sc_inout(ctf_integer(unsigned long, arg3, arg3)) sc_inout(ctf_integer(unsigned long, arg4, arg4)) sc_inout(ctf_integer(unsigned long, arg5, arg5)))
)
#endif
#ifndef OVERRIDE_32_remap_file_pages
SC_LTTNG_TRACEPOINT_EVENT(remap_file_pages,
	TP_PROTO(sc_exit(long ret,) unsigned long start, unsigned long size, unsigned long prot, unsigned long pgoff, unsigned long flags),
	TP_ARGS(sc_exit(ret,) start, size, prot, pgoff, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(unsigned long, size, size)) sc_in(ctf_integer(unsigned long, prot, prot)) sc_in(ctf_integer(unsigned long, pgoff, pgoff)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_kcmp
SC_LTTNG_TRACEPOINT_EVENT(kcmp,
	TP_PROTO(sc_exit(long ret,) pid_t pid1, pid_t pid2, int type, unsigned long idx1, unsigned long idx2),
	TP_ARGS(sc_exit(ret,) pid1, pid2, type, idx1, idx2),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid1, pid1)) sc_in(ctf_integer(pid_t, pid2, pid2)) sc_in(ctf_integer(int, type, type)) sc_in(ctf_integer(unsigned long, idx1, idx1)) sc_in(ctf_integer(unsigned long, idx2, idx2)))
)
#endif
#ifndef OVERRIDE_32_mmap2
SC_LTTNG_TRACEPOINT_EVENT(mmap2,
	TP_PROTO(sc_exit(long ret,) unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, unsigned long offset),
	TP_ARGS(sc_exit(ret,) addr, len, prot, flags, fd, offset),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, addr, addr)) sc_in(ctf_integer(unsigned long, len, len)) sc_in(ctf_integer(unsigned long, prot, prot)) sc_in(ctf_integer(unsigned long, flags, flags)) sc_in(ctf_integer(unsigned long, fd, fd)) sc_in(ctf_integer(unsigned long, offset, offset)))
)
#endif

#endif /*  _TRACE_SYSCALLS_INTEGERS_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>

#else /* CREATE_SYSCALL_TABLE */

#include "riscv-32-syscalls_integers_override.h"
#include "generic-32-syscalls_integers_override.h"
#include "generic-syscalls_integers_override.h"

#ifdef SC_ENTER
#ifndef OVERRIDE_TABLE_32_vhangup
TRACE_SYSCALL_TABLE(syscalls_noargs, vhangup, 58, 0)
#endif
#ifndef OVERRIDE_TABLE_32_sync
TRACE_SYSCALL_TABLE(syscalls_noargs, sync, 81, 0)
#endif
#ifndef OVERRIDE_TABLE_32_sched_yield
TRACE_SYSCALL_TABLE(syscalls_noargs, sched_yield, 124, 0)
#endif
#ifndef OVERRIDE_TABLE_32_restart_syscall
TRACE_SYSCALL_TABLE(syscalls_noargs, restart_syscall, 128, 0)
#endif
#ifndef OVERRIDE_TABLE_32_rt_sigreturn
TRACE_SYSCALL_TABLE(syscalls_noargs, rt_sigreturn, 139, 0)
#endif
#ifndef OVERRIDE_TABLE_32_setsid
TRACE_SYSCALL_TABLE(syscalls_noargs, setsid, 157, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getpid
TRACE_SYSCALL_TABLE(syscalls_noargs, getpid, 172, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getppid
TRACE_SYSCALL_TABLE(syscalls_noargs, getppid, 173, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getuid
TRACE_SYSCALL_TABLE(syscalls_noargs, getuid, 174, 0)
#endif
#ifndef OVERRIDE_TABLE_32_geteuid
TRACE_SYSCALL_TABLE(syscalls_noargs, geteuid, 175, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getgid
TRACE_SYSCALL_TABLE(syscalls_noargs, getgid, 176, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getegid
TRACE_SYSCALL_TABLE(syscalls_noargs, getegid, 177, 0)
#endif
#ifndef OVERRIDE_TABLE_32_gettid
TRACE_SYSCALL_TABLE(syscalls_noargs, gettid, 178, 0)
#endif
#ifndef OVERRIDE_TABLE_32_munlockall
TRACE_SYSCALL_TABLE(syscalls_noargs, munlockall, 231, 0)
#endif
#else /* #ifdef SC_ENTER */
#ifndef OVERRIDE_TABLE_32_vhangup
TRACE_SYSCALL_TABLE(vhangup, vhangup, 58, 0)
#endif
#ifndef OVERRIDE_TABLE_32_sync
TRACE_SYSCALL_TABLE(sync, sync, 81, 0)
#endif
#ifndef OVERRIDE_TABLE_32_sched_yield
TRACE_SYSCALL_TABLE(sched_yield, sched_yield, 124, 0)
#endif
#ifndef OVERRIDE_TABLE_32_restart_syscall
TRACE_SYSCALL_TABLE(restart_syscall, restart_syscall, 128, 0)
#endif
#ifndef OVERRIDE_TABLE_32_rt_sigreturn
TRACE_SYSCALL_TABLE(rt_sigreturn, rt_sigreturn, 139, 0)
#endif
#ifndef OVERRIDE_TABLE_32_setsid
TRACE_SYSCALL_TABLE(setsid, setsid, 157, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getpid
TRACE_SYSCALL_TABLE(getpid, getpid, 172, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getppid
TRACE_SYSCALL_TABLE(getppid, getppid, 173, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getuid
TRACE_SYSCALL_TABLE(getuid, getuid, 174, 0)
#endif
#ifndef OVERRIDE_TABLE_32_geteuid
TRACE_SYSCALL_TABLE(geteuid, geteuid, 175, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getgid
TRACE_SYSCALL_TABLE(getgid, getgid, 176, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getegid
TRACE_SYSCALL_TABLE(getegid, getegid, 177, 0)
#endif
#ifndef OVERRIDE_TABLE_32_gettid
TRACE_SYSCALL_TABLE(gettid, gettid, 178, 0)
#endif
#ifndef OVERRIDE_TABLE_32_munlockall
TRACE_SYSCALL_TABLE(munlockall, munlockall, 231, 0)
#endif
#endif /* else #ifdef SC_ENTER */

#ifndef OVERRIDE_TABLE_32_io_destroy
TRACE_SYSCALL_TABLE(io_destroy, io_destroy, 1, 1)
#endif
#ifndef OVERRIDE_TABLE_32_eventfd2
TRACE_SYSCALL_TABLE(eventfd2, eventfd2, 19, 2)
#endif
#ifndef OVERRIDE_TABLE_32_epoll_create1
TRACE_SYSCALL_TABLE(epoll_create1, epoll_create1, 20, 1)
#endif
#ifndef OVERRIDE_TABLE_32_dup
TRACE_SYSCALL_TABLE(dup, dup, 23, 1)
#endif
#ifndef OVERRIDE_TABLE_32_dup3
TRACE_SYSCALL_TABLE(dup3, dup3, 24, 3)
#endif
#ifndef OVERRIDE_TABLE_32_fcntl64
TRACE_SYSCALL_TABLE(fcntl64, fcntl64, 25, 3)
#endif
#ifndef OVERRIDE_TABLE_32_inotify_init1
TRACE_SYSCALL_TABLE(inotify_init1, inotify_init1, 26, 1)
#endif
#ifndef OVERRIDE_TABLE_32_inotify_rm_watch
TRACE_SYSCALL_TABLE(inotify_rm_watch, inotify_rm_watch, 28, 2)
#endif
#ifndef OVERRIDE_TABLE_32_ioctl
TRACE_SYSCALL_TABLE(ioctl, ioctl, 29, 3)
#endif
#ifndef OVERRIDE_TABLE_32_ioprio_set
TRACE_SYSCALL_TABLE(ioprio_set, ioprio_set, 30, 3)
#endif
#ifndef OVERRIDE_TABLE_32_ioprio_get
TRACE_SYSCALL_TABLE(ioprio_get, ioprio_get, 31, 2)
#endif
#ifndef OVERRIDE_TABLE_32_flock
TRACE_SYSCALL_TABLE(flock, flock, 32, 2)
#endif
#ifndef OVERRIDE_TABLE_32_ftruncate64
TRACE_SYSCALL_TABLE(ftruncate64, ftruncate64, 46, 2)
#endif
#ifndef OVERRIDE_TABLE_32_fallocate
TRACE_SYSCALL_TABLE(fallocate, fallocate, 47, 4)
#endif
#ifndef OVERRIDE_TABLE_32_fchdir
TRACE_SYSCALL_TABLE(fchdir, fchdir, 50, 1)
#endif
#ifndef OVERRIDE_TABLE_32_fchmod
TRACE_SYSCALL_TABLE(fchmod, fchmod, 52, 2)
#endif
#ifndef OVERRIDE_TABLE_32_fchown
TRACE_SYSCALL_TABLE(fchown, fchown, 55, 3)
#endif
#ifndef OVERRIDE_TABLE_32_close
TRACE_SYSCALL_TABLE(close, close, 57, 1)
#endif
#ifndef OVERRIDE_TABLE_32_tee
TRACE_SYSCALL_TABLE(tee, tee, 77, 4)
#endif
#ifndef OVERRIDE_TABLE_32_fsync
TRACE_SYSCALL_TABLE(fsync, fsync, 82, 1)
#endif
#ifndef OVERRIDE_TABLE_32_fdatasync
TRACE_SYSCALL_TABLE(fdatasync, fdatasync, 83, 1)
#endif
#ifndef OVERRIDE_TABLE_32_sync_file_range
TRACE_SYSCALL_TABLE(sync_file_range, sync_file_range, 84, 4)
#endif
#ifndef OVERRIDE_TABLE_32_timerfd_create
TRACE_SYSCALL_TABLE(timerfd_create, timerfd_create, 85, 2)
#endif
#ifndef OVERRIDE_TABLE_32_personality
TRACE_SYSCALL_TABLE(personality, personality, 92, 1)
#endif
#ifndef OVERRIDE_TABLE_32_exit
TRACE_SYSCALL_TABLE(exit, exit, 93, 1)
#endif
#ifndef OVERRIDE_TABLE_32_exit_group
TRACE_SYSCALL_TABLE(exit_group, exit_group, 94, 1)
#endif
#ifndef OVERRIDE_TABLE_32_unshare
TRACE_SYSCALL_TABLE(unshare, unshare, 97, 1)
#endif
#ifndef OVERRIDE_TABLE_32_timer_getoverrun
TRACE_SYSCALL_TABLE(timer_getoverrun, timer_getoverrun, 109, 1)
#endif
#ifndef OVERRIDE_TABLE_32_timer_delete
TRACE_SYSCALL_TABLE(timer_delete, timer_delete, 111, 1)
#endif
#ifndef OVERRIDE_TABLE_32_ptrace
TRACE_SYSCALL_TABLE(ptrace, ptrace, 117, 4)
#endif
#ifndef OVERRIDE_TABLE_32_sched_getscheduler
TRACE_SYSCALL_TABLE(sched_getscheduler, sched_getscheduler, 120, 1)
#endif
#ifndef OVERRIDE_TABLE_32_sched_get_priority_max
TRACE_SYSCALL_TABLE(sched_get_priority_max, sched_get_priority_max, 125, 1)
#endif
#ifndef OVERRIDE_TABLE_32_sched_get_priority_min
TRACE_SYSCALL_TABLE(sched_get_priority_min, sched_get_priority_min, 126, 1)
#endif
#ifndef OVERRIDE_TABLE_32_kill
TRACE_SYSCALL_TABLE(kill, kill, 129, 2)
#endif
#ifndef OVERRIDE_TABLE_32_tkill
TRACE_SYSCALL_TABLE(tkill, tkill, 130, 2)
#endif
#ifndef OVERRIDE_TABLE_32_tgkill
TRACE_SYSCALL_TABLE(tgkill, tgkill, 131, 3)
#endif
#ifndef OVERRIDE_TABLE_32_setpriority
TRACE_SYSCALL_TABLE(setpriority, setpriority, 140, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getpriority
TRACE_SYSCALL_TABLE(getpriority, getpriority, 141, 2)
#endif
#ifndef OVERRIDE_TABLE_32_setregid
TRACE_SYSCALL_TABLE(setregid, setregid, 143, 2)
#endif
#ifndef OVERRIDE_TABLE_32_setgid
TRACE_SYSCALL_TABLE(setgid, setgid, 144, 1)
#endif
#ifndef OVERRIDE_TABLE_32_setreuid
TRACE_SYSCALL_TABLE(setreuid, setreuid, 145, 2)
#endif
#ifndef OVERRIDE_TABLE_32_setuid
TRACE_SYSCALL_TABLE(setuid, setuid, 146, 1)
#endif
#ifndef OVERRIDE_TABLE_32_setresuid
TRACE_SYSCALL_TABLE(setresuid, setresuid, 147, 3)
#endif
#ifndef OVERRIDE_TABLE_32_setresgid
TRACE_SYSCALL_TABLE(setresgid, setresgid, 149, 3)
#endif
#ifndef OVERRIDE_TABLE_32_setfsuid
TRACE_SYSCALL_TABLE(setfsuid, setfsuid, 151, 1)
#endif
#ifndef OVERRIDE_TABLE_32_setfsgid
TRACE_SYSCALL_TABLE(setfsgid, setfsgid, 152, 1)
#endif
#ifndef OVERRIDE_TABLE_32_setpgid
TRACE_SYSCALL_TABLE(setpgid, setpgid, 154, 2)
#endif
#ifndef OVERRIDE_TABLE_32_getpgid
TRACE_SYSCALL_TABLE(getpgid, getpgid, 155, 1)
#endif
#ifndef OVERRIDE_TABLE_32_getsid
TRACE_SYSCALL_TABLE(getsid, getsid, 156, 1)
#endif
#ifndef OVERRIDE_TABLE_32_umask
TRACE_SYSCALL_TABLE(umask, umask, 166, 1)
#endif
#ifndef OVERRIDE_TABLE_32_prctl
TRACE_SYSCALL_TABLE(prctl, prctl, 167, 5)
#endif
#ifndef OVERRIDE_TABLE_32_msgget
TRACE_SYSCALL_TABLE(msgget, msgget, 186, 2)
#endif
#ifndef OVERRIDE_TABLE_32_semget
TRACE_SYSCALL_TABLE(semget, semget, 190, 3)
#endif
#ifndef OVERRIDE_TABLE_32_semctl
TRACE_SYSCALL_TABLE(semctl, semctl, 191, 4)
#endif
#ifndef OVERRIDE_TABLE_32_shmget
TRACE_SYSCALL_TABLE(shmget, shmget, 194, 3)
#endif
#ifndef OVERRIDE_TABLE_32_socket
TRACE_SYSCALL_TABLE(socket, socket, 198, 3)
#endif
#ifndef OVERRIDE_TABLE_32_listen
TRACE_SYSCALL_TABLE(listen, listen, 201, 2)
#endif
#ifndef OVERRIDE_TABLE_32_shutdown
TRACE_SYSCALL_TABLE(shutdown, shutdown, 210, 2)
#endif
#ifndef OVERRIDE_TABLE_32_readahead
TRACE_SYSCALL_TABLE(readahead, readahead, 213, 3)
#endif
#ifndef OVERRIDE_TABLE_32_brk
TRACE_SYSCALL_TABLE(brk, brk, 214, 1)
#endif
#ifndef OVERRIDE_TABLE_32_munmap
TRACE_SYSCALL_TABLE(munmap, munmap, 215, 2)
#endif
#ifndef OVERRIDE_TABLE_32_mremap
TRACE_SYSCALL_TABLE(mremap, mremap, 216, 5)
#endif
#ifndef OVERRIDE_TABLE_32_keyctl
TRACE_SYSCALL_TABLE(keyctl, keyctl, 219, 5)
#endif
#ifndef OVERRIDE_TABLE_32_mmap2
TRACE_SYSCALL_TABLE(mmap2, mmap2, 222, 6)
#endif
#ifndef OVERRIDE_TABLE_32_fadvise64_64
TRACE_SYSCALL_TABLE(fadvise64_64, fadvise64_64, 223, 4)
#endif
#ifndef OVERRIDE_TABLE_32_mprotect
TRACE_SYSCALL_TABLE(mprotect, mprotect, 226, 3)
#endif
#ifndef OVERRIDE_TABLE_32_msync
TRACE_SYSCALL_TABLE(msync, msync, 227, 3)
#endif
#ifndef OVERRIDE_TABLE_32_mlock
TRACE_SYSCALL_TABLE(mlock, mlock, 228, 2)
#endif
#ifndef OVERRIDE_TABLE_32_munlock
TRACE_SYSCALL_TABLE(munlock, munlock, 229, 2)
#endif
#ifndef OVERRIDE_TABLE_32_mlockall
TRACE_SYSCALL_TABLE(mlockall, mlockall, 230, 1)
#endif
#ifndef OVERRIDE_TABLE_32_madvise
TRACE_SYSCALL_TABLE(madvise, madvise, 233, 3)
#endif
#ifndef OVERRIDE_TABLE_32_remap_file_pages
TRACE_SYSCALL_TABLE(remap_file_pages, remap_file_pages, 234, 5)
#endif
#ifndef OVERRIDE_TABLE_32_riscv_flush_icache
TRACE_SYSCALL_TABLE(riscv_flush_icache, riscv_flush_icache, 259, 3)
#endif
#ifndef OVERRIDE_TABLE_32_fanotify_init
TRACE_SYSCALL_TABLE(fanotify_init, fanotify_init, 262, 2)
#endif
#ifndef OVERRIDE_TABLE_32_syncfs
TRACE_SYSCALL_TABLE(syncfs, syncfs, 267, 1)
#endif
#ifndef OVERRIDE_TABLE_32_setns
TRACE_SYSCALL_TABLE(setns, setns, 268, 2)
#endif
#ifndef OVERRIDE_TABLE_32_kcmp
TRACE_SYSCALL_TABLE(kcmp, kcmp, 272, 5)
#endif
#ifndef OVERRIDE_TABLE_32_userfaultfd
TRACE_SYSCALL_TABLE(userfaultfd, userfaultfd, 282, 1)
#endif
#ifndef OVERRIDE_TABLE_32_membarrier
TRACE_SYSCALL_TABLE(membarrier, membarrier, 283, 3)
#endif
#ifndef OVERRIDE_TABLE_32_mlock2
TRACE_SYSCALL_TABLE(mlock2, mlock2, 284, 3)
#endif
#ifndef OVERRIDE_TABLE_32_fsmount
TRACE_SYSCALL_TABLE(fsmount, fsmount, 432, 3)
#endif
#ifndef OVERRIDE_TABLE_32_pidfd_open
TRACE_SYSCALL_TABLE(pidfd_open, pidfd_open, 434, 2)
#endif
#ifndef OVERRIDE_TABLE_32_close_range
TRACE_SYSCALL_TABLE(close_range, close_range, 436, 3)
#endif
#ifndef OVERRIDE_TABLE_32_pidfd_getfd
TRACE_SYSCALL_TABLE(pidfd_getfd, pidfd_getfd, 438, 3)
#endif
#ifndef OVERRIDE_TABLE_32_landlock_restrict_self
TRACE_SYSCALL_TABLE(landlock_restrict_self, landlock_restrict_self, 446, 2)
#endif
#ifndef OVERRIDE_TABLE_32_memfd_secret
TRACE_SYSCALL_TABLE(memfd_secret, memfd_secret, 447, 1)
#endif
#ifndef OVERRIDE_TABLE_32_process_mrelease
TRACE_SYSCALL_TABLE(process_mrelease, process_mrelease, 448, 2)
#endif
#ifndef OVERRIDE_TABLE_32_set_mempolicy_home_node
TRACE_SYSCALL_TABLE(set_mempolicy_home_node, set_mempolicy_home_node, 450, 4)
#endif

#endif /* CREATE_SYSCALL_TABLE */
