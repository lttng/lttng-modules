/* THIS FILE IS AUTO-GENERATED. DO NOT EDIT */
#ifndef CREATE_SYSCALL_TABLE

#if !defined(_TRACE_SYSCALLS_INTEGERS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SYSCALLS_INTEGERS_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/syscalls.h>
#include "x86-64-syscalls-3.10.0-rc7_integers_override.h"
#include "syscalls_integers_override.h"

#ifdef SC_ENTER
SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(syscalls_noargs,
	TP_STRUCT__entry(),
	TP_fast_assign(),
	TP_printk()
)
#ifndef OVERRIDE_64_sched_yield
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, sched_yield)
#endif
#ifndef OVERRIDE_64_pause
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, pause)
#endif
#ifndef OVERRIDE_64_getpid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getpid)
#endif
#ifndef OVERRIDE_64_getuid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getuid)
#endif
#ifndef OVERRIDE_64_getgid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getgid)
#endif
#ifndef OVERRIDE_64_geteuid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, geteuid)
#endif
#ifndef OVERRIDE_64_getegid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getegid)
#endif
#ifndef OVERRIDE_64_getppid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getppid)
#endif
#ifndef OVERRIDE_64_getpgrp
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getpgrp)
#endif
#ifndef OVERRIDE_64_setsid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, setsid)
#endif
#ifndef OVERRIDE_64_munlockall
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, munlockall)
#endif
#ifndef OVERRIDE_64_vhangup
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, vhangup)
#endif
#ifndef OVERRIDE_64_sync
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, sync)
#endif
#ifndef OVERRIDE_64_gettid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, gettid)
#endif
#ifndef OVERRIDE_64_restart_syscall
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, restart_syscall)
#endif
#ifndef OVERRIDE_64_inotify_init
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, inotify_init)
#endif
#else /* #ifdef SC_ENTER */
#ifndef OVERRIDE_64_sched_yield
SC_LTTNG_TRACEPOINT_EVENT(sched_yield,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_pause
SC_LTTNG_TRACEPOINT_EVENT(pause,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getpid
SC_LTTNG_TRACEPOINT_EVENT(getpid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getuid
SC_LTTNG_TRACEPOINT_EVENT(getuid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getgid
SC_LTTNG_TRACEPOINT_EVENT(getgid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_geteuid
SC_LTTNG_TRACEPOINT_EVENT(geteuid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getegid
SC_LTTNG_TRACEPOINT_EVENT(getegid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getppid
SC_LTTNG_TRACEPOINT_EVENT(getppid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getpgrp
SC_LTTNG_TRACEPOINT_EVENT(getpgrp,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setsid
SC_LTTNG_TRACEPOINT_EVENT(setsid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_munlockall
SC_LTTNG_TRACEPOINT_EVENT(munlockall,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_vhangup
SC_LTTNG_TRACEPOINT_EVENT(vhangup,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sync
SC_LTTNG_TRACEPOINT_EVENT(sync,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_gettid
SC_LTTNG_TRACEPOINT_EVENT(gettid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_restart_syscall
SC_LTTNG_TRACEPOINT_EVENT(restart_syscall,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_inotify_init
SC_LTTNG_TRACEPOINT_EVENT(inotify_init,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#endif /* else #ifdef SC_ENTER */
#ifndef OVERRIDE_64_close
SC_LTTNG_TRACEPOINT_EVENT(close,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_brk
SC_LTTNG_TRACEPOINT_EVENT(brk,
	TP_PROTO(sc_exit(long ret,) unsigned long brk),
	TP_ARGS(sc_exit(ret,) brk),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, brk))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(brk, brk))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_dup
SC_LTTNG_TRACEPOINT_EVENT(dup,
	TP_PROTO(sc_exit(long ret,) unsigned int fildes),
	TP_ARGS(sc_exit(ret,) fildes),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fildes))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fildes, fildes))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_alarm
SC_LTTNG_TRACEPOINT_EVENT(alarm,
	TP_PROTO(sc_exit(long ret,) unsigned int seconds),
	TP_ARGS(sc_exit(ret,) seconds),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, seconds))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(seconds, seconds))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_exit
SC_LTTNG_TRACEPOINT_EVENT(exit,
	TP_PROTO(sc_exit(long ret,) int error_code),
	TP_ARGS(sc_exit(ret,) error_code),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, error_code))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(error_code, error_code))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fsync
SC_LTTNG_TRACEPOINT_EVENT(fsync,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fdatasync
SC_LTTNG_TRACEPOINT_EVENT(fdatasync,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fchdir
SC_LTTNG_TRACEPOINT_EVENT(fchdir,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_umask
SC_LTTNG_TRACEPOINT_EVENT(umask,
	TP_PROTO(sc_exit(long ret,) int mask),
	TP_ARGS(sc_exit(ret,) mask),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, mask))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(mask, mask))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setuid
SC_LTTNG_TRACEPOINT_EVENT(setuid,
	TP_PROTO(sc_exit(long ret,) uid_t uid),
	TP_ARGS(sc_exit(ret,) uid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(uid_t, uid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(uid, uid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setgid
SC_LTTNG_TRACEPOINT_EVENT(setgid,
	TP_PROTO(sc_exit(long ret,) gid_t gid),
	TP_ARGS(sc_exit(ret,) gid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(gid_t, gid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(gid, gid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getpgid
SC_LTTNG_TRACEPOINT_EVENT(getpgid,
	TP_PROTO(sc_exit(long ret,) pid_t pid),
	TP_ARGS(sc_exit(ret,) pid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setfsuid
SC_LTTNG_TRACEPOINT_EVENT(setfsuid,
	TP_PROTO(sc_exit(long ret,) uid_t uid),
	TP_ARGS(sc_exit(ret,) uid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(uid_t, uid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(uid, uid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setfsgid
SC_LTTNG_TRACEPOINT_EVENT(setfsgid,
	TP_PROTO(sc_exit(long ret,) gid_t gid),
	TP_ARGS(sc_exit(ret,) gid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(gid_t, gid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(gid, gid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getsid
SC_LTTNG_TRACEPOINT_EVENT(getsid,
	TP_PROTO(sc_exit(long ret,) pid_t pid),
	TP_ARGS(sc_exit(ret,) pid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_personality
SC_LTTNG_TRACEPOINT_EVENT(personality,
	TP_PROTO(sc_exit(long ret,) unsigned int personality),
	TP_ARGS(sc_exit(ret,) personality),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, personality))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(personality, personality))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sched_getscheduler
SC_LTTNG_TRACEPOINT_EVENT(sched_getscheduler,
	TP_PROTO(sc_exit(long ret,) pid_t pid),
	TP_ARGS(sc_exit(ret,) pid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sched_get_priority_max
SC_LTTNG_TRACEPOINT_EVENT(sched_get_priority_max,
	TP_PROTO(sc_exit(long ret,) int policy),
	TP_ARGS(sc_exit(ret,) policy),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, policy))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(policy, policy))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sched_get_priority_min
SC_LTTNG_TRACEPOINT_EVENT(sched_get_priority_min,
	TP_PROTO(sc_exit(long ret,) int policy),
	TP_ARGS(sc_exit(ret,) policy),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, policy))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(policy, policy))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_mlockall
SC_LTTNG_TRACEPOINT_EVENT(mlockall,
	TP_PROTO(sc_exit(long ret,) int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_io_destroy
SC_LTTNG_TRACEPOINT_EVENT(io_destroy,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx),
	TP_ARGS(sc_exit(ret,) ctx),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(aio_context_t, ctx))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ctx, ctx))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_epoll_create
SC_LTTNG_TRACEPOINT_EVENT(epoll_create,
	TP_PROTO(sc_exit(long ret,) int size),
	TP_ARGS(sc_exit(ret,) size),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, size))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(size, size))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_timer_getoverrun
SC_LTTNG_TRACEPOINT_EVENT(timer_getoverrun,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id),
	TP_ARGS(sc_exit(ret,) timer_id),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(timer_t, timer_id))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(timer_id, timer_id))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_timer_delete
SC_LTTNG_TRACEPOINT_EVENT(timer_delete,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id),
	TP_ARGS(sc_exit(ret,) timer_id),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(timer_t, timer_id))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(timer_id, timer_id))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_exit_group
SC_LTTNG_TRACEPOINT_EVENT(exit_group,
	TP_PROTO(sc_exit(long ret,) int error_code),
	TP_ARGS(sc_exit(ret,) error_code),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, error_code))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(error_code, error_code))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_unshare
SC_LTTNG_TRACEPOINT_EVENT(unshare,
	TP_PROTO(sc_exit(long ret,) unsigned long unshare_flags),
	TP_ARGS(sc_exit(ret,) unshare_flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, unshare_flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(unshare_flags, unshare_flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_eventfd
SC_LTTNG_TRACEPOINT_EVENT(eventfd,
	TP_PROTO(sc_exit(long ret,) unsigned int count),
	TP_ARGS(sc_exit(ret,) count),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, count))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(count, count))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_epoll_create1
SC_LTTNG_TRACEPOINT_EVENT(epoll_create1,
	TP_PROTO(sc_exit(long ret,) int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_inotify_init1
SC_LTTNG_TRACEPOINT_EVENT(inotify_init1,
	TP_PROTO(sc_exit(long ret,) int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_syncfs
SC_LTTNG_TRACEPOINT_EVENT(syncfs,
	TP_PROTO(sc_exit(long ret,) int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_munmap
SC_LTTNG_TRACEPOINT_EVENT(munmap,
	TP_PROTO(sc_exit(long ret,) unsigned long addr, size_t len),
	TP_ARGS(sc_exit(ret,) addr, len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field_hex(unsigned long, addr)) sc_in(__field(size_t, len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(addr, addr)) sc_in(tp_assign(len, len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_dup2
SC_LTTNG_TRACEPOINT_EVENT(dup2,
	TP_PROTO(sc_exit(long ret,) unsigned int oldfd, unsigned int newfd),
	TP_ARGS(sc_exit(ret,) oldfd, newfd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, oldfd)) sc_in(__field(unsigned int, newfd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(oldfd, oldfd)) sc_in(tp_assign(newfd, newfd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_shutdown
SC_LTTNG_TRACEPOINT_EVENT(shutdown,
	TP_PROTO(sc_exit(long ret,) int fd, int how),
	TP_ARGS(sc_exit(ret,) fd, how),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(int, how))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(how, how))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_listen
SC_LTTNG_TRACEPOINT_EVENT(listen,
	TP_PROTO(sc_exit(long ret,) int fd, int backlog),
	TP_ARGS(sc_exit(ret,) fd, backlog),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(int, backlog))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(backlog, backlog))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_kill
SC_LTTNG_TRACEPOINT_EVENT(kill,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int sig),
	TP_ARGS(sc_exit(ret,) pid, sig),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field(int, sig))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(sig, sig))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_msgget
SC_LTTNG_TRACEPOINT_EVENT(msgget,
	TP_PROTO(sc_exit(long ret,) key_t key, int msgflg),
	TP_ARGS(sc_exit(ret,) key, msgflg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(key_t, key)) sc_in(__field(int, msgflg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(key, key)) sc_in(tp_assign(msgflg, msgflg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_flock
SC_LTTNG_TRACEPOINT_EVENT(flock,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd),
	TP_ARGS(sc_exit(ret,) fd, cmd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(unsigned int, cmd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(cmd, cmd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_ftruncate
SC_LTTNG_TRACEPOINT_EVENT(ftruncate,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned long length),
	TP_ARGS(sc_exit(ret,) fd, length),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(unsigned long, length))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(length, length))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fchmod
SC_LTTNG_TRACEPOINT_EVENT(fchmod,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, umode_t mode),
	TP_ARGS(sc_exit(ret,) fd, mode),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(umode_t, mode))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(mode, mode))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setpgid
SC_LTTNG_TRACEPOINT_EVENT(setpgid,
	TP_PROTO(sc_exit(long ret,) pid_t pid, pid_t pgid),
	TP_ARGS(sc_exit(ret,) pid, pgid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field(pid_t, pgid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(pgid, pgid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setreuid
SC_LTTNG_TRACEPOINT_EVENT(setreuid,
	TP_PROTO(sc_exit(long ret,) uid_t ruid, uid_t euid),
	TP_ARGS(sc_exit(ret,) ruid, euid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(uid_t, ruid)) sc_in(__field(uid_t, euid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ruid, ruid)) sc_in(tp_assign(euid, euid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setregid
SC_LTTNG_TRACEPOINT_EVENT(setregid,
	TP_PROTO(sc_exit(long ret,) gid_t rgid, gid_t egid),
	TP_ARGS(sc_exit(ret,) rgid, egid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(gid_t, rgid)) sc_in(__field(gid_t, egid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(rgid, rgid)) sc_in(tp_assign(egid, egid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getpriority
SC_LTTNG_TRACEPOINT_EVENT(getpriority,
	TP_PROTO(sc_exit(long ret,) int which, int who),
	TP_ARGS(sc_exit(ret,) which, who),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, which)) sc_in(__field(int, who))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which, which)) sc_in(tp_assign(who, who))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_mlock
SC_LTTNG_TRACEPOINT_EVENT(mlock,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len),
	TP_ARGS(sc_exit(ret,) start, len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(size_t, len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(len, len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_munlock
SC_LTTNG_TRACEPOINT_EVENT(munlock,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len),
	TP_ARGS(sc_exit(ret,) start, len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(size_t, len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(len, len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_tkill
SC_LTTNG_TRACEPOINT_EVENT(tkill,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int sig),
	TP_ARGS(sc_exit(ret,) pid, sig),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field(int, sig))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(sig, sig))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_ioprio_get
SC_LTTNG_TRACEPOINT_EVENT(ioprio_get,
	TP_PROTO(sc_exit(long ret,) int which, int who),
	TP_ARGS(sc_exit(ret,) which, who),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, which)) sc_in(__field(int, who))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which, which)) sc_in(tp_assign(who, who))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_inotify_rm_watch
SC_LTTNG_TRACEPOINT_EVENT(inotify_rm_watch,
	TP_PROTO(sc_exit(long ret,) int fd, __s32 wd),
	TP_ARGS(sc_exit(ret,) fd, wd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(__s32, wd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(wd, wd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_timerfd_create
SC_LTTNG_TRACEPOINT_EVENT(timerfd_create,
	TP_PROTO(sc_exit(long ret,) int clockid, int flags),
	TP_ARGS(sc_exit(ret,) clockid, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, clockid)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(clockid, clockid)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_eventfd2
SC_LTTNG_TRACEPOINT_EVENT(eventfd2,
	TP_PROTO(sc_exit(long ret,) unsigned int count, int flags),
	TP_ARGS(sc_exit(ret,) count, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, count)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(count, count)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fanotify_init
SC_LTTNG_TRACEPOINT_EVENT(fanotify_init,
	TP_PROTO(sc_exit(long ret,) unsigned int flags, unsigned int event_f_flags),
	TP_ARGS(sc_exit(ret,) flags, event_f_flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, flags)) sc_in(__field(unsigned int, event_f_flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(event_f_flags, event_f_flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setns
SC_LTTNG_TRACEPOINT_EVENT(setns,
	TP_PROTO(sc_exit(long ret,) int fd, int nstype),
	TP_ARGS(sc_exit(ret,) fd, nstype),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(int, nstype))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(nstype, nstype))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_lseek
SC_LTTNG_TRACEPOINT_EVENT(lseek,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, off_t offset, unsigned int whence),
	TP_ARGS(sc_exit(ret,) fd, offset, whence),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(off_t, offset)) sc_in(__field(unsigned int, whence))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(offset, offset)) sc_in(tp_assign(whence, whence))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_mprotect
SC_LTTNG_TRACEPOINT_EVENT(mprotect,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, unsigned long prot),
	TP_ARGS(sc_exit(ret,) start, len, prot),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(size_t, len)) sc_in(__field(unsigned long, prot))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(len, len)) sc_in(tp_assign(prot, prot))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_ioctl
SC_LTTNG_TRACEPOINT_EVENT(ioctl,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) fd, cmd, arg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(unsigned int, cmd)) sc_inout(__field(unsigned long, arg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(cmd, cmd)) sc_inout(tp_assign(arg, arg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_msync
SC_LTTNG_TRACEPOINT_EVENT(msync,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, int flags),
	TP_ARGS(sc_exit(ret,) start, len, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(size_t, len)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(len, len)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_madvise
SC_LTTNG_TRACEPOINT_EVENT(madvise,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len_in, int behavior),
	TP_ARGS(sc_exit(ret,) start, len_in, behavior),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(size_t, len_in)) sc_in(__field(int, behavior))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(len_in, len_in)) sc_in(tp_assign(behavior, behavior))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_shmget
SC_LTTNG_TRACEPOINT_EVENT(shmget,
	TP_PROTO(sc_exit(long ret,) key_t key, size_t size, int shmflg),
	TP_ARGS(sc_exit(ret,) key, size, shmflg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(key_t, key)) sc_in(__field(size_t, size)) sc_in(__field(int, shmflg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(key, key)) sc_in(tp_assign(size, size)) sc_in(tp_assign(shmflg, shmflg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_socket
SC_LTTNG_TRACEPOINT_EVENT(socket,
	TP_PROTO(sc_exit(long ret,) int family, int type, int protocol),
	TP_ARGS(sc_exit(ret,) family, type, protocol),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, family)) sc_in(__field(int, type)) sc_in(__field(int, protocol))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(family, family)) sc_in(tp_assign(type, type)) sc_in(tp_assign(protocol, protocol))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_semget
SC_LTTNG_TRACEPOINT_EVENT(semget,
	TP_PROTO(sc_exit(long ret,) key_t key, int nsems, int semflg),
	TP_ARGS(sc_exit(ret,) key, nsems, semflg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(key_t, key)) sc_in(__field(int, nsems)) sc_in(__field(int, semflg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(key, key)) sc_in(tp_assign(nsems, nsems)) sc_in(tp_assign(semflg, semflg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fcntl
SC_LTTNG_TRACEPOINT_EVENT(fcntl,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) fd, cmd, arg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(unsigned int, cmd)) sc_inout(__field(unsigned long, arg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(cmd, cmd)) sc_inout(tp_assign(arg, arg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fchown
SC_LTTNG_TRACEPOINT_EVENT(fchown,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, uid_t user, gid_t group),
	TP_ARGS(sc_exit(ret,) fd, user, group),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(uid_t, user)) sc_in(__field(gid_t, group))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(user, user)) sc_in(tp_assign(group, group))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setresuid
SC_LTTNG_TRACEPOINT_EVENT(setresuid,
	TP_PROTO(sc_exit(long ret,) uid_t ruid, uid_t euid, uid_t suid),
	TP_ARGS(sc_exit(ret,) ruid, euid, suid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(uid_t, ruid)) sc_in(__field(uid_t, euid)) sc_in(__field(uid_t, suid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ruid, ruid)) sc_in(tp_assign(euid, euid)) sc_in(tp_assign(suid, suid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setresgid
SC_LTTNG_TRACEPOINT_EVENT(setresgid,
	TP_PROTO(sc_exit(long ret,) gid_t rgid, gid_t egid, gid_t sgid),
	TP_ARGS(sc_exit(ret,) rgid, egid, sgid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(gid_t, rgid)) sc_in(__field(gid_t, egid)) sc_in(__field(gid_t, sgid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(rgid, rgid)) sc_in(tp_assign(egid, egid)) sc_in(tp_assign(sgid, sgid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sysfs
SC_LTTNG_TRACEPOINT_EVENT(sysfs,
	TP_PROTO(sc_exit(long ret,) int option, unsigned long arg1, unsigned long arg2),
	TP_ARGS(sc_exit(ret,) option, arg1, arg2),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, option)) sc_in(__field(unsigned long, arg1)) sc_in(__field(unsigned long, arg2))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(option, option)) sc_in(tp_assign(arg1, arg1)) sc_in(tp_assign(arg2, arg2))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setpriority
SC_LTTNG_TRACEPOINT_EVENT(setpriority,
	TP_PROTO(sc_exit(long ret,) int which, int who, int niceval),
	TP_ARGS(sc_exit(ret,) which, who, niceval),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, which)) sc_in(__field(int, who)) sc_in(__field(int, niceval))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which, which)) sc_in(tp_assign(who, who)) sc_in(tp_assign(niceval, niceval))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_readahead
SC_LTTNG_TRACEPOINT_EVENT(readahead,
	TP_PROTO(sc_exit(long ret,) int fd, loff_t offset, size_t count),
	TP_ARGS(sc_exit(ret,) fd, offset, count),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(loff_t, offset)) sc_in(__field(size_t, count))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(offset, offset)) sc_in(tp_assign(count, count))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_tgkill
SC_LTTNG_TRACEPOINT_EVENT(tgkill,
	TP_PROTO(sc_exit(long ret,) pid_t tgid, pid_t pid, int sig),
	TP_ARGS(sc_exit(ret,) tgid, pid, sig),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, tgid)) sc_in(__field(pid_t, pid)) sc_in(__field(int, sig))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(tgid, tgid)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(sig, sig))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_ioprio_set
SC_LTTNG_TRACEPOINT_EVENT(ioprio_set,
	TP_PROTO(sc_exit(long ret,) int which, int who, int ioprio),
	TP_ARGS(sc_exit(ret,) which, who, ioprio),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, which)) sc_in(__field(int, who)) sc_in(__field(int, ioprio))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which, which)) sc_in(tp_assign(who, who)) sc_in(tp_assign(ioprio, ioprio))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_dup3
SC_LTTNG_TRACEPOINT_EVENT(dup3,
	TP_PROTO(sc_exit(long ret,) unsigned int oldfd, unsigned int newfd, int flags),
	TP_ARGS(sc_exit(ret,) oldfd, newfd, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, oldfd)) sc_in(__field(unsigned int, newfd)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(oldfd, oldfd)) sc_in(tp_assign(newfd, newfd)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_semctl
SC_LTTNG_TRACEPOINT_EVENT(semctl,
	TP_PROTO(sc_exit(long ret,) int semid, int semnum, int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) semid, semnum, cmd, arg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, semid)) sc_in(__field(int, semnum)) sc_in(__field(int, cmd)) sc_inout(__field(unsigned long, arg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(semid, semid)) sc_in(tp_assign(semnum, semnum)) sc_in(tp_assign(cmd, cmd)) sc_inout(tp_assign(arg, arg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_ptrace
SC_LTTNG_TRACEPOINT_EVENT(ptrace,
	TP_PROTO(sc_exit(long ret,) long request, long pid, unsigned long addr, unsigned long data),
	TP_ARGS(sc_exit(ret,) request, pid, addr, data),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(long, request)) sc_in(__field(long, pid)) sc_inout(__field_hex(unsigned long, addr)) sc_inout(__field(unsigned long, data))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(request, request)) sc_in(tp_assign(pid, pid)) sc_inout(tp_assign(addr, addr)) sc_inout(tp_assign(data, data))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fadvise64
SC_LTTNG_TRACEPOINT_EVENT(fadvise64,
	TP_PROTO(sc_exit(long ret,) int fd, loff_t offset, size_t len, int advice),
	TP_ARGS(sc_exit(ret,) fd, offset, len, advice),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(loff_t, offset)) sc_in(__field(size_t, len)) sc_in(__field(int, advice))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(offset, offset)) sc_in(tp_assign(len, len)) sc_in(tp_assign(advice, advice))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_tee
SC_LTTNG_TRACEPOINT_EVENT(tee,
	TP_PROTO(sc_exit(long ret,) int fdin, int fdout, size_t len, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fdin, fdout, len, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fdin)) sc_in(__field(int, fdout)) sc_in(__field(size_t, len)) sc_in(__field(unsigned int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fdin, fdin)) sc_in(tp_assign(fdout, fdout)) sc_in(tp_assign(len, len)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sync_file_range
SC_LTTNG_TRACEPOINT_EVENT(sync_file_range,
	TP_PROTO(sc_exit(long ret,) int fd, loff_t offset, loff_t nbytes, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, offset, nbytes, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(loff_t, offset)) sc_in(__field(loff_t, nbytes)) sc_in(__field(unsigned int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(offset, offset)) sc_in(tp_assign(nbytes, nbytes)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fallocate
SC_LTTNG_TRACEPOINT_EVENT(fallocate,
	TP_PROTO(sc_exit(long ret,) int fd, int mode, loff_t offset, loff_t len),
	TP_ARGS(sc_exit(ret,) fd, mode, offset, len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(int, mode)) sc_in(__field(loff_t, offset)) sc_in(__field(loff_t, len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(mode, mode)) sc_in(tp_assign(offset, offset)) sc_in(tp_assign(len, len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_mremap
SC_LTTNG_TRACEPOINT_EVENT(mremap,
	TP_PROTO(sc_exit(long ret,) unsigned long addr, unsigned long old_len, unsigned long new_len, unsigned long flags, unsigned long new_addr),
	TP_ARGS(sc_exit(ret,) addr, old_len, new_len, flags, new_addr),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field_hex(unsigned long, addr)) sc_in(__field(unsigned long, old_len)) sc_in(__field(unsigned long, new_len)) sc_in(__field(unsigned long, flags)) sc_in(__field_hex(unsigned long, new_addr))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(addr, addr)) sc_in(tp_assign(old_len, old_len)) sc_in(tp_assign(new_len, new_len)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(new_addr, new_addr))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_prctl
SC_LTTNG_TRACEPOINT_EVENT(prctl,
	TP_PROTO(sc_exit(long ret,) int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5),
	TP_ARGS(sc_exit(ret,) option, arg2, arg3, arg4, arg5),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, option)) sc_inout(__field(unsigned long, arg2)) sc_in(__field(unsigned long, arg3)) sc_in(__field(unsigned long, arg4)) sc_in(__field(unsigned long, arg5))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(option, option)) sc_inout(tp_assign(arg2, arg2)) sc_in(tp_assign(arg3, arg3)) sc_in(tp_assign(arg4, arg4)) sc_in(tp_assign(arg5, arg5))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_remap_file_pages
SC_LTTNG_TRACEPOINT_EVENT(remap_file_pages,
	TP_PROTO(sc_exit(long ret,) unsigned long start, unsigned long size, unsigned long prot, unsigned long pgoff, unsigned long flags),
	TP_ARGS(sc_exit(ret,) start, size, prot, pgoff, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(unsigned long, size)) sc_in(__field(unsigned long, prot)) sc_in(__field(unsigned long, pgoff)) sc_in(__field(unsigned long, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(size, size)) sc_in(tp_assign(prot, prot)) sc_in(tp_assign(pgoff, pgoff)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_keyctl
SC_LTTNG_TRACEPOINT_EVENT(keyctl,
	TP_PROTO(sc_exit(long ret,) int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5),
	TP_ARGS(sc_exit(ret,) option, arg2, arg3, arg4, arg5),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, option)) sc_inout(__field(unsigned long, arg2)) sc_inout(__field(unsigned long, arg3)) sc_inout(__field(unsigned long, arg4)) sc_inout(__field(unsigned long, arg5))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(option, option)) sc_inout(tp_assign(arg2, arg2)) sc_inout(tp_assign(arg3, arg3)) sc_inout(tp_assign(arg4, arg4)) sc_inout(tp_assign(arg5, arg5))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_mmap
SC_LTTNG_TRACEPOINT_EVENT(mmap,
	TP_PROTO(sc_exit(long ret,) unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, unsigned long off),
	TP_ARGS(sc_exit(ret,) addr, len, prot, flags, fd, off),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field_hex(unsigned long, addr)) sc_in(__field(unsigned long, len)) sc_in(__field(unsigned long, prot)) sc_in(__field(unsigned long, flags)) sc_in(__field(unsigned long, fd)) sc_in(__field(unsigned long, off))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(addr, addr)) sc_in(tp_assign(len, len)) sc_in(tp_assign(prot, prot)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(off, off))),
	TP_printk()
)
#endif

#endif /*  _TRACE_SYSCALLS_INTEGERS_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"

#else /* CREATE_SYSCALL_TABLE */

#include "x86-64-syscalls-3.10.0-rc7_integers_override.h"
#include "syscalls_integers_override.h"

#ifdef SC_ENTER
#ifndef OVERRIDE_TABLE_64_sched_yield
TRACE_SYSCALL_TABLE(syscalls_noargs, sched_yield, 24, 0)
#endif
#ifndef OVERRIDE_TABLE_64_pause
TRACE_SYSCALL_TABLE(syscalls_noargs, pause, 34, 0)
#endif
#ifndef OVERRIDE_TABLE_64_getpid
TRACE_SYSCALL_TABLE(syscalls_noargs, getpid, 39, 0)
#endif
#ifndef OVERRIDE_TABLE_64_getuid
TRACE_SYSCALL_TABLE(syscalls_noargs, getuid, 102, 0)
#endif
#ifndef OVERRIDE_TABLE_64_getgid
TRACE_SYSCALL_TABLE(syscalls_noargs, getgid, 104, 0)
#endif
#ifndef OVERRIDE_TABLE_64_geteuid
TRACE_SYSCALL_TABLE(syscalls_noargs, geteuid, 107, 0)
#endif
#ifndef OVERRIDE_TABLE_64_getegid
TRACE_SYSCALL_TABLE(syscalls_noargs, getegid, 108, 0)
#endif
#ifndef OVERRIDE_TABLE_64_getppid
TRACE_SYSCALL_TABLE(syscalls_noargs, getppid, 110, 0)
#endif
#ifndef OVERRIDE_TABLE_64_getpgrp
TRACE_SYSCALL_TABLE(syscalls_noargs, getpgrp, 111, 0)
#endif
#ifndef OVERRIDE_TABLE_64_setsid
TRACE_SYSCALL_TABLE(syscalls_noargs, setsid, 112, 0)
#endif
#ifndef OVERRIDE_TABLE_64_munlockall
TRACE_SYSCALL_TABLE(syscalls_noargs, munlockall, 152, 0)
#endif
#ifndef OVERRIDE_TABLE_64_vhangup
TRACE_SYSCALL_TABLE(syscalls_noargs, vhangup, 153, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sync
TRACE_SYSCALL_TABLE(syscalls_noargs, sync, 162, 0)
#endif
#ifndef OVERRIDE_TABLE_64_gettid
TRACE_SYSCALL_TABLE(syscalls_noargs, gettid, 186, 0)
#endif
#ifndef OVERRIDE_TABLE_64_restart_syscall
TRACE_SYSCALL_TABLE(syscalls_noargs, restart_syscall, 219, 0)
#endif
#ifndef OVERRIDE_TABLE_64_inotify_init
TRACE_SYSCALL_TABLE(syscalls_noargs, inotify_init, 253, 0)
#endif
#else /* #ifdef SC_ENTER */
#ifndef OVERRIDE_TABLE_64_sched_yield
TRACE_SYSCALL_TABLE(sched_yield, sched_yield, 24, 0)
#endif
#ifndef OVERRIDE_TABLE_64_pause
TRACE_SYSCALL_TABLE(pause, pause, 34, 0)
#endif
#ifndef OVERRIDE_TABLE_64_getpid
TRACE_SYSCALL_TABLE(getpid, getpid, 39, 0)
#endif
#ifndef OVERRIDE_TABLE_64_getuid
TRACE_SYSCALL_TABLE(getuid, getuid, 102, 0)
#endif
#ifndef OVERRIDE_TABLE_64_getgid
TRACE_SYSCALL_TABLE(getgid, getgid, 104, 0)
#endif
#ifndef OVERRIDE_TABLE_64_geteuid
TRACE_SYSCALL_TABLE(geteuid, geteuid, 107, 0)
#endif
#ifndef OVERRIDE_TABLE_64_getegid
TRACE_SYSCALL_TABLE(getegid, getegid, 108, 0)
#endif
#ifndef OVERRIDE_TABLE_64_getppid
TRACE_SYSCALL_TABLE(getppid, getppid, 110, 0)
#endif
#ifndef OVERRIDE_TABLE_64_getpgrp
TRACE_SYSCALL_TABLE(getpgrp, getpgrp, 111, 0)
#endif
#ifndef OVERRIDE_TABLE_64_setsid
TRACE_SYSCALL_TABLE(setsid, setsid, 112, 0)
#endif
#ifndef OVERRIDE_TABLE_64_munlockall
TRACE_SYSCALL_TABLE(munlockall, munlockall, 152, 0)
#endif
#ifndef OVERRIDE_TABLE_64_vhangup
TRACE_SYSCALL_TABLE(vhangup, vhangup, 153, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sync
TRACE_SYSCALL_TABLE(sync, sync, 162, 0)
#endif
#ifndef OVERRIDE_TABLE_64_gettid
TRACE_SYSCALL_TABLE(gettid, gettid, 186, 0)
#endif
#ifndef OVERRIDE_TABLE_64_restart_syscall
TRACE_SYSCALL_TABLE(restart_syscall, restart_syscall, 219, 0)
#endif
#ifndef OVERRIDE_TABLE_64_inotify_init
TRACE_SYSCALL_TABLE(inotify_init, inotify_init, 253, 0)
#endif
#endif /* else #ifdef SC_ENTER */
#ifndef OVERRIDE_TABLE_64_close
TRACE_SYSCALL_TABLE(close, close, 3, 1)
#endif
#ifndef OVERRIDE_TABLE_64_lseek
TRACE_SYSCALL_TABLE(lseek, lseek, 8, 3)
#endif
#ifndef OVERRIDE_TABLE_64_mmap
TRACE_SYSCALL_TABLE(mmap, mmap, 9, 6)
#endif
#ifndef OVERRIDE_TABLE_64_mprotect
TRACE_SYSCALL_TABLE(mprotect, mprotect, 10, 3)
#endif
#ifndef OVERRIDE_TABLE_64_munmap
TRACE_SYSCALL_TABLE(munmap, munmap, 11, 2)
#endif
#ifndef OVERRIDE_TABLE_64_brk
TRACE_SYSCALL_TABLE(brk, brk, 12, 1)
#endif
#ifndef OVERRIDE_TABLE_64_ioctl
TRACE_SYSCALL_TABLE(ioctl, ioctl, 16, 3)
#endif
#ifndef OVERRIDE_TABLE_64_mremap
TRACE_SYSCALL_TABLE(mremap, mremap, 25, 5)
#endif
#ifndef OVERRIDE_TABLE_64_msync
TRACE_SYSCALL_TABLE(msync, msync, 26, 3)
#endif
#ifndef OVERRIDE_TABLE_64_madvise
TRACE_SYSCALL_TABLE(madvise, madvise, 28, 3)
#endif
#ifndef OVERRIDE_TABLE_64_shmget
TRACE_SYSCALL_TABLE(shmget, shmget, 29, 3)
#endif
#ifndef OVERRIDE_TABLE_64_dup
TRACE_SYSCALL_TABLE(dup, dup, 32, 1)
#endif
#ifndef OVERRIDE_TABLE_64_dup2
TRACE_SYSCALL_TABLE(dup2, dup2, 33, 2)
#endif
#ifndef OVERRIDE_TABLE_64_alarm
TRACE_SYSCALL_TABLE(alarm, alarm, 37, 1)
#endif
#ifndef OVERRIDE_TABLE_64_socket
TRACE_SYSCALL_TABLE(socket, socket, 41, 3)
#endif
#ifndef OVERRIDE_TABLE_64_shutdown
TRACE_SYSCALL_TABLE(shutdown, shutdown, 48, 2)
#endif
#ifndef OVERRIDE_TABLE_64_listen
TRACE_SYSCALL_TABLE(listen, listen, 50, 2)
#endif
#ifndef OVERRIDE_TABLE_64_exit
TRACE_SYSCALL_TABLE(exit, exit, 60, 1)
#endif
#ifndef OVERRIDE_TABLE_64_kill
TRACE_SYSCALL_TABLE(kill, kill, 62, 2)
#endif
#ifndef OVERRIDE_TABLE_64_semget
TRACE_SYSCALL_TABLE(semget, semget, 64, 3)
#endif
#ifndef OVERRIDE_TABLE_64_semctl
TRACE_SYSCALL_TABLE(semctl, semctl, 66, 4)
#endif
#ifndef OVERRIDE_TABLE_64_msgget
TRACE_SYSCALL_TABLE(msgget, msgget, 68, 2)
#endif
#ifndef OVERRIDE_TABLE_64_fcntl
TRACE_SYSCALL_TABLE(fcntl, fcntl, 72, 3)
#endif
#ifndef OVERRIDE_TABLE_64_flock
TRACE_SYSCALL_TABLE(flock, flock, 73, 2)
#endif
#ifndef OVERRIDE_TABLE_64_fsync
TRACE_SYSCALL_TABLE(fsync, fsync, 74, 1)
#endif
#ifndef OVERRIDE_TABLE_64_fdatasync
TRACE_SYSCALL_TABLE(fdatasync, fdatasync, 75, 1)
#endif
#ifndef OVERRIDE_TABLE_64_ftruncate
TRACE_SYSCALL_TABLE(ftruncate, ftruncate, 77, 2)
#endif
#ifndef OVERRIDE_TABLE_64_fchdir
TRACE_SYSCALL_TABLE(fchdir, fchdir, 81, 1)
#endif
#ifndef OVERRIDE_TABLE_64_fchmod
TRACE_SYSCALL_TABLE(fchmod, fchmod, 91, 2)
#endif
#ifndef OVERRIDE_TABLE_64_fchown
TRACE_SYSCALL_TABLE(fchown, fchown, 93, 3)
#endif
#ifndef OVERRIDE_TABLE_64_umask
TRACE_SYSCALL_TABLE(umask, umask, 95, 1)
#endif
#ifndef OVERRIDE_TABLE_64_ptrace
TRACE_SYSCALL_TABLE(ptrace, ptrace, 101, 4)
#endif
#ifndef OVERRIDE_TABLE_64_setuid
TRACE_SYSCALL_TABLE(setuid, setuid, 105, 1)
#endif
#ifndef OVERRIDE_TABLE_64_setgid
TRACE_SYSCALL_TABLE(setgid, setgid, 106, 1)
#endif
#ifndef OVERRIDE_TABLE_64_setpgid
TRACE_SYSCALL_TABLE(setpgid, setpgid, 109, 2)
#endif
#ifndef OVERRIDE_TABLE_64_setreuid
TRACE_SYSCALL_TABLE(setreuid, setreuid, 113, 2)
#endif
#ifndef OVERRIDE_TABLE_64_setregid
TRACE_SYSCALL_TABLE(setregid, setregid, 114, 2)
#endif
#ifndef OVERRIDE_TABLE_64_setresuid
TRACE_SYSCALL_TABLE(setresuid, setresuid, 117, 3)
#endif
#ifndef OVERRIDE_TABLE_64_setresgid
TRACE_SYSCALL_TABLE(setresgid, setresgid, 119, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getpgid
TRACE_SYSCALL_TABLE(getpgid, getpgid, 121, 1)
#endif
#ifndef OVERRIDE_TABLE_64_setfsuid
TRACE_SYSCALL_TABLE(setfsuid, setfsuid, 122, 1)
#endif
#ifndef OVERRIDE_TABLE_64_setfsgid
TRACE_SYSCALL_TABLE(setfsgid, setfsgid, 123, 1)
#endif
#ifndef OVERRIDE_TABLE_64_getsid
TRACE_SYSCALL_TABLE(getsid, getsid, 124, 1)
#endif
#ifndef OVERRIDE_TABLE_64_personality
TRACE_SYSCALL_TABLE(personality, personality, 135, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sysfs
TRACE_SYSCALL_TABLE(sysfs, sysfs, 139, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getpriority
TRACE_SYSCALL_TABLE(getpriority, getpriority, 140, 2)
#endif
#ifndef OVERRIDE_TABLE_64_setpriority
TRACE_SYSCALL_TABLE(setpriority, setpriority, 141, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sched_getscheduler
TRACE_SYSCALL_TABLE(sched_getscheduler, sched_getscheduler, 145, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sched_get_priority_max
TRACE_SYSCALL_TABLE(sched_get_priority_max, sched_get_priority_max, 146, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sched_get_priority_min
TRACE_SYSCALL_TABLE(sched_get_priority_min, sched_get_priority_min, 147, 1)
#endif
#ifndef OVERRIDE_TABLE_64_mlock
TRACE_SYSCALL_TABLE(mlock, mlock, 149, 2)
#endif
#ifndef OVERRIDE_TABLE_64_munlock
TRACE_SYSCALL_TABLE(munlock, munlock, 150, 2)
#endif
#ifndef OVERRIDE_TABLE_64_mlockall
TRACE_SYSCALL_TABLE(mlockall, mlockall, 151, 1)
#endif
#ifndef OVERRIDE_TABLE_64_prctl
TRACE_SYSCALL_TABLE(prctl, prctl, 157, 5)
#endif
#ifndef OVERRIDE_TABLE_64_readahead
TRACE_SYSCALL_TABLE(readahead, readahead, 187, 3)
#endif
#ifndef OVERRIDE_TABLE_64_tkill
TRACE_SYSCALL_TABLE(tkill, tkill, 200, 2)
#endif
#ifndef OVERRIDE_TABLE_64_io_destroy
TRACE_SYSCALL_TABLE(io_destroy, io_destroy, 207, 1)
#endif
#ifndef OVERRIDE_TABLE_64_epoll_create
TRACE_SYSCALL_TABLE(epoll_create, epoll_create, 213, 1)
#endif
#ifndef OVERRIDE_TABLE_64_remap_file_pages
TRACE_SYSCALL_TABLE(remap_file_pages, remap_file_pages, 216, 5)
#endif
#ifndef OVERRIDE_TABLE_64_fadvise64
TRACE_SYSCALL_TABLE(fadvise64, fadvise64, 221, 4)
#endif
#ifndef OVERRIDE_TABLE_64_timer_getoverrun
TRACE_SYSCALL_TABLE(timer_getoverrun, timer_getoverrun, 225, 1)
#endif
#ifndef OVERRIDE_TABLE_64_timer_delete
TRACE_SYSCALL_TABLE(timer_delete, timer_delete, 226, 1)
#endif
#ifndef OVERRIDE_TABLE_64_exit_group
TRACE_SYSCALL_TABLE(exit_group, exit_group, 231, 1)
#endif
#ifndef OVERRIDE_TABLE_64_tgkill
TRACE_SYSCALL_TABLE(tgkill, tgkill, 234, 3)
#endif
#ifndef OVERRIDE_TABLE_64_keyctl
TRACE_SYSCALL_TABLE(keyctl, keyctl, 250, 5)
#endif
#ifndef OVERRIDE_TABLE_64_ioprio_set
TRACE_SYSCALL_TABLE(ioprio_set, ioprio_set, 251, 3)
#endif
#ifndef OVERRIDE_TABLE_64_ioprio_get
TRACE_SYSCALL_TABLE(ioprio_get, ioprio_get, 252, 2)
#endif
#ifndef OVERRIDE_TABLE_64_inotify_rm_watch
TRACE_SYSCALL_TABLE(inotify_rm_watch, inotify_rm_watch, 255, 2)
#endif
#ifndef OVERRIDE_TABLE_64_unshare
TRACE_SYSCALL_TABLE(unshare, unshare, 272, 1)
#endif
#ifndef OVERRIDE_TABLE_64_tee
TRACE_SYSCALL_TABLE(tee, tee, 276, 4)
#endif
#ifndef OVERRIDE_TABLE_64_sync_file_range
TRACE_SYSCALL_TABLE(sync_file_range, sync_file_range, 277, 4)
#endif
#ifndef OVERRIDE_TABLE_64_timerfd_create
TRACE_SYSCALL_TABLE(timerfd_create, timerfd_create, 283, 2)
#endif
#ifndef OVERRIDE_TABLE_64_eventfd
TRACE_SYSCALL_TABLE(eventfd, eventfd, 284, 1)
#endif
#ifndef OVERRIDE_TABLE_64_fallocate
TRACE_SYSCALL_TABLE(fallocate, fallocate, 285, 4)
#endif
#ifndef OVERRIDE_TABLE_64_eventfd2
TRACE_SYSCALL_TABLE(eventfd2, eventfd2, 290, 2)
#endif
#ifndef OVERRIDE_TABLE_64_epoll_create1
TRACE_SYSCALL_TABLE(epoll_create1, epoll_create1, 291, 1)
#endif
#ifndef OVERRIDE_TABLE_64_dup3
TRACE_SYSCALL_TABLE(dup3, dup3, 292, 3)
#endif
#ifndef OVERRIDE_TABLE_64_inotify_init1
TRACE_SYSCALL_TABLE(inotify_init1, inotify_init1, 294, 1)
#endif
#ifndef OVERRIDE_TABLE_64_fanotify_init
TRACE_SYSCALL_TABLE(fanotify_init, fanotify_init, 300, 2)
#endif
#ifndef OVERRIDE_TABLE_64_syncfs
TRACE_SYSCALL_TABLE(syncfs, syncfs, 306, 1)
#endif
#ifndef OVERRIDE_TABLE_64_setns
TRACE_SYSCALL_TABLE(setns, setns, 308, 2)
#endif

#endif /* CREATE_SYSCALL_TABLE */
