/* THIS FILE IS AUTO-GENERATED. DO NOT EDIT */
#ifndef CREATE_SYSCALL_TABLE

#if !defined(_TRACE_SYSCALLS_INTEGERS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SYSCALLS_INTEGERS_H

#include <linux/tracepoint.h>
#include <linux/syscalls.h>
#include "x86-64-syscalls-3.10.0-rc7_integers_override.h"
#include "syscalls_integers_override.h"

#ifdef SC_ENTER
SC_DECLARE_EVENT_CLASS_NOARGS(syscalls_noargs,
	TP_STRUCT__entry(),
	TP_fast_assign(),
	TP_printk()
)
#ifndef OVERRIDE_64_sys_sched_yield
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_sched_yield)
#endif
#ifndef OVERRIDE_64_sys_pause
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_pause)
#endif
#ifndef OVERRIDE_64_sys_getpid
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_getpid)
#endif
#ifndef OVERRIDE_64_sys_getuid
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_getuid)
#endif
#ifndef OVERRIDE_64_sys_getgid
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_getgid)
#endif
#ifndef OVERRIDE_64_sys_geteuid
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_geteuid)
#endif
#ifndef OVERRIDE_64_sys_getegid
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_getegid)
#endif
#ifndef OVERRIDE_64_sys_getppid
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_getppid)
#endif
#ifndef OVERRIDE_64_sys_getpgrp
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_getpgrp)
#endif
#ifndef OVERRIDE_64_sys_setsid
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_setsid)
#endif
#ifndef OVERRIDE_64_sys_munlockall
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_munlockall)
#endif
#ifndef OVERRIDE_64_sys_vhangup
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_vhangup)
#endif
#ifndef OVERRIDE_64_sys_sync
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_sync)
#endif
#ifndef OVERRIDE_64_sys_gettid
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_gettid)
#endif
#ifndef OVERRIDE_64_sys_restart_syscall
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_restart_syscall)
#endif
#ifndef OVERRIDE_64_sys_inotify_init
SC_DEFINE_EVENT_NOARGS(syscalls_noargs, sys_inotify_init)
#endif
#else /* #ifdef SC_ENTER */
#ifndef OVERRIDE_64_sys_sched_yield
SC_TRACE_EVENT(sys_sched_yield,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_pause
SC_TRACE_EVENT(sys_pause,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_getpid
SC_TRACE_EVENT(sys_getpid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_getuid
SC_TRACE_EVENT(sys_getuid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_getgid
SC_TRACE_EVENT(sys_getgid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_geteuid
SC_TRACE_EVENT(sys_geteuid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_getegid
SC_TRACE_EVENT(sys_getegid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_getppid
SC_TRACE_EVENT(sys_getppid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_getpgrp
SC_TRACE_EVENT(sys_getpgrp,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_setsid
SC_TRACE_EVENT(sys_setsid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_munlockall
SC_TRACE_EVENT(sys_munlockall,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_vhangup
SC_TRACE_EVENT(sys_vhangup,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_sync
SC_TRACE_EVENT(sys_sync,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_gettid
SC_TRACE_EVENT(sys_gettid,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_restart_syscall
SC_TRACE_EVENT(sys_restart_syscall,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_inotify_init
SC_TRACE_EVENT(sys_inotify_init,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_STRUCT__entry(sc_exit(__field(long, ret))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))),
	TP_printk()
)
#endif
#endif /* else #ifdef SC_ENTER */
#ifndef OVERRIDE_64_sys_close
SC_TRACE_EVENT(sys_close,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_brk
SC_TRACE_EVENT(sys_brk,
	TP_PROTO(sc_exit(long ret,) unsigned long brk),
	TP_ARGS(sc_exit(ret,) brk),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, brk))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(brk, brk))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_dup
SC_TRACE_EVENT(sys_dup,
	TP_PROTO(sc_exit(long ret,) unsigned int fildes),
	TP_ARGS(sc_exit(ret,) fildes),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fildes))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fildes, fildes))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_alarm
SC_TRACE_EVENT(sys_alarm,
	TP_PROTO(sc_exit(long ret,) unsigned int seconds),
	TP_ARGS(sc_exit(ret,) seconds),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, seconds))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(seconds, seconds))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_exit
SC_TRACE_EVENT(sys_exit,
	TP_PROTO(sc_exit(long ret,) int error_code),
	TP_ARGS(sc_exit(ret,) error_code),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, error_code))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(error_code, error_code))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_fsync
SC_TRACE_EVENT(sys_fsync,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_fdatasync
SC_TRACE_EVENT(sys_fdatasync,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_fchdir
SC_TRACE_EVENT(sys_fchdir,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_umask
SC_TRACE_EVENT(sys_umask,
	TP_PROTO(sc_exit(long ret,) int mask),
	TP_ARGS(sc_exit(ret,) mask),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, mask))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(mask, mask))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_setuid
SC_TRACE_EVENT(sys_setuid,
	TP_PROTO(sc_exit(long ret,) uid_t uid),
	TP_ARGS(sc_exit(ret,) uid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(uid_t, uid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(uid, uid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_setgid
SC_TRACE_EVENT(sys_setgid,
	TP_PROTO(sc_exit(long ret,) gid_t gid),
	TP_ARGS(sc_exit(ret,) gid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(gid_t, gid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(gid, gid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_getpgid
SC_TRACE_EVENT(sys_getpgid,
	TP_PROTO(sc_exit(long ret,) pid_t pid),
	TP_ARGS(sc_exit(ret,) pid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_setfsuid
SC_TRACE_EVENT(sys_setfsuid,
	TP_PROTO(sc_exit(long ret,) uid_t uid),
	TP_ARGS(sc_exit(ret,) uid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(uid_t, uid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(uid, uid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_setfsgid
SC_TRACE_EVENT(sys_setfsgid,
	TP_PROTO(sc_exit(long ret,) gid_t gid),
	TP_ARGS(sc_exit(ret,) gid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(gid_t, gid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(gid, gid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_getsid
SC_TRACE_EVENT(sys_getsid,
	TP_PROTO(sc_exit(long ret,) pid_t pid),
	TP_ARGS(sc_exit(ret,) pid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_personality
SC_TRACE_EVENT(sys_personality,
	TP_PROTO(sc_exit(long ret,) unsigned int personality),
	TP_ARGS(sc_exit(ret,) personality),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, personality))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(personality, personality))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_sched_getscheduler
SC_TRACE_EVENT(sys_sched_getscheduler,
	TP_PROTO(sc_exit(long ret,) pid_t pid),
	TP_ARGS(sc_exit(ret,) pid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_sched_get_priority_max
SC_TRACE_EVENT(sys_sched_get_priority_max,
	TP_PROTO(sc_exit(long ret,) int policy),
	TP_ARGS(sc_exit(ret,) policy),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, policy))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(policy, policy))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_sched_get_priority_min
SC_TRACE_EVENT(sys_sched_get_priority_min,
	TP_PROTO(sc_exit(long ret,) int policy),
	TP_ARGS(sc_exit(ret,) policy),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, policy))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(policy, policy))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_mlockall
SC_TRACE_EVENT(sys_mlockall,
	TP_PROTO(sc_exit(long ret,) int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_io_destroy
SC_TRACE_EVENT(sys_io_destroy,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx),
	TP_ARGS(sc_exit(ret,) ctx),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(aio_context_t, ctx))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ctx, ctx))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_epoll_create
SC_TRACE_EVENT(sys_epoll_create,
	TP_PROTO(sc_exit(long ret,) int size),
	TP_ARGS(sc_exit(ret,) size),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, size))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(size, size))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_timer_getoverrun
SC_TRACE_EVENT(sys_timer_getoverrun,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id),
	TP_ARGS(sc_exit(ret,) timer_id),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(timer_t, timer_id))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(timer_id, timer_id))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_timer_delete
SC_TRACE_EVENT(sys_timer_delete,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id),
	TP_ARGS(sc_exit(ret,) timer_id),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(timer_t, timer_id))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(timer_id, timer_id))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_exit_group
SC_TRACE_EVENT(sys_exit_group,
	TP_PROTO(sc_exit(long ret,) int error_code),
	TP_ARGS(sc_exit(ret,) error_code),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, error_code))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(error_code, error_code))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_unshare
SC_TRACE_EVENT(sys_unshare,
	TP_PROTO(sc_exit(long ret,) unsigned long unshare_flags),
	TP_ARGS(sc_exit(ret,) unshare_flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, unshare_flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(unshare_flags, unshare_flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_eventfd
SC_TRACE_EVENT(sys_eventfd,
	TP_PROTO(sc_exit(long ret,) unsigned int count),
	TP_ARGS(sc_exit(ret,) count),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, count))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(count, count))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_epoll_create1
SC_TRACE_EVENT(sys_epoll_create1,
	TP_PROTO(sc_exit(long ret,) int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_inotify_init1
SC_TRACE_EVENT(sys_inotify_init1,
	TP_PROTO(sc_exit(long ret,) int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_syncfs
SC_TRACE_EVENT(sys_syncfs,
	TP_PROTO(sc_exit(long ret,) int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_munmap
SC_TRACE_EVENT(sys_munmap,
	TP_PROTO(sc_exit(long ret,) unsigned long addr, size_t len),
	TP_ARGS(sc_exit(ret,) addr, len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field_hex(unsigned long, addr)) sc_in(__field(size_t, len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(addr, addr)) sc_in(tp_assign(len, len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_dup2
SC_TRACE_EVENT(sys_dup2,
	TP_PROTO(sc_exit(long ret,) unsigned int oldfd, unsigned int newfd),
	TP_ARGS(sc_exit(ret,) oldfd, newfd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, oldfd)) sc_in(__field(unsigned int, newfd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(oldfd, oldfd)) sc_in(tp_assign(newfd, newfd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_shutdown
SC_TRACE_EVENT(sys_shutdown,
	TP_PROTO(sc_exit(long ret,) int fd, int how),
	TP_ARGS(sc_exit(ret,) fd, how),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(int, how))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(how, how))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_listen
SC_TRACE_EVENT(sys_listen,
	TP_PROTO(sc_exit(long ret,) int fd, int backlog),
	TP_ARGS(sc_exit(ret,) fd, backlog),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(int, backlog))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(backlog, backlog))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_kill
SC_TRACE_EVENT(sys_kill,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int sig),
	TP_ARGS(sc_exit(ret,) pid, sig),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field(int, sig))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(sig, sig))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_msgget
SC_TRACE_EVENT(sys_msgget,
	TP_PROTO(sc_exit(long ret,) key_t key, int msgflg),
	TP_ARGS(sc_exit(ret,) key, msgflg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(key_t, key)) sc_in(__field(int, msgflg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(key, key)) sc_in(tp_assign(msgflg, msgflg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_flock
SC_TRACE_EVENT(sys_flock,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd),
	TP_ARGS(sc_exit(ret,) fd, cmd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(unsigned int, cmd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(cmd, cmd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_ftruncate
SC_TRACE_EVENT(sys_ftruncate,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned long length),
	TP_ARGS(sc_exit(ret,) fd, length),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(unsigned long, length))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(length, length))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_fchmod
SC_TRACE_EVENT(sys_fchmod,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, umode_t mode),
	TP_ARGS(sc_exit(ret,) fd, mode),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(umode_t, mode))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(mode, mode))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_setpgid
SC_TRACE_EVENT(sys_setpgid,
	TP_PROTO(sc_exit(long ret,) pid_t pid, pid_t pgid),
	TP_ARGS(sc_exit(ret,) pid, pgid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field(pid_t, pgid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(pgid, pgid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_setreuid
SC_TRACE_EVENT(sys_setreuid,
	TP_PROTO(sc_exit(long ret,) uid_t ruid, uid_t euid),
	TP_ARGS(sc_exit(ret,) ruid, euid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(uid_t, ruid)) sc_in(__field(uid_t, euid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ruid, ruid)) sc_in(tp_assign(euid, euid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_setregid
SC_TRACE_EVENT(sys_setregid,
	TP_PROTO(sc_exit(long ret,) gid_t rgid, gid_t egid),
	TP_ARGS(sc_exit(ret,) rgid, egid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(gid_t, rgid)) sc_in(__field(gid_t, egid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(rgid, rgid)) sc_in(tp_assign(egid, egid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_getpriority
SC_TRACE_EVENT(sys_getpriority,
	TP_PROTO(sc_exit(long ret,) int which, int who),
	TP_ARGS(sc_exit(ret,) which, who),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, which)) sc_in(__field(int, who))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which, which)) sc_in(tp_assign(who, who))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_mlock
SC_TRACE_EVENT(sys_mlock,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len),
	TP_ARGS(sc_exit(ret,) start, len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(size_t, len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(len, len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_munlock
SC_TRACE_EVENT(sys_munlock,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len),
	TP_ARGS(sc_exit(ret,) start, len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(size_t, len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(len, len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_tkill
SC_TRACE_EVENT(sys_tkill,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int sig),
	TP_ARGS(sc_exit(ret,) pid, sig),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field(int, sig))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(sig, sig))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_ioprio_get
SC_TRACE_EVENT(sys_ioprio_get,
	TP_PROTO(sc_exit(long ret,) int which, int who),
	TP_ARGS(sc_exit(ret,) which, who),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, which)) sc_in(__field(int, who))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which, which)) sc_in(tp_assign(who, who))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_inotify_rm_watch
SC_TRACE_EVENT(sys_inotify_rm_watch,
	TP_PROTO(sc_exit(long ret,) int fd, __s32 wd),
	TP_ARGS(sc_exit(ret,) fd, wd),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(__s32, wd))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(wd, wd))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_timerfd_create
SC_TRACE_EVENT(sys_timerfd_create,
	TP_PROTO(sc_exit(long ret,) int clockid, int flags),
	TP_ARGS(sc_exit(ret,) clockid, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, clockid)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(clockid, clockid)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_eventfd2
SC_TRACE_EVENT(sys_eventfd2,
	TP_PROTO(sc_exit(long ret,) unsigned int count, int flags),
	TP_ARGS(sc_exit(ret,) count, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, count)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(count, count)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_fanotify_init
SC_TRACE_EVENT(sys_fanotify_init,
	TP_PROTO(sc_exit(long ret,) unsigned int flags, unsigned int event_f_flags),
	TP_ARGS(sc_exit(ret,) flags, event_f_flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, flags)) sc_in(__field(unsigned int, event_f_flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(event_f_flags, event_f_flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_setns
SC_TRACE_EVENT(sys_setns,
	TP_PROTO(sc_exit(long ret,) int fd, int nstype),
	TP_ARGS(sc_exit(ret,) fd, nstype),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(int, nstype))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(nstype, nstype))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_lseek
SC_TRACE_EVENT(sys_lseek,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, off_t offset, unsigned int whence),
	TP_ARGS(sc_exit(ret,) fd, offset, whence),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(off_t, offset)) sc_in(__field(unsigned int, whence))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(offset, offset)) sc_in(tp_assign(whence, whence))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_mprotect
SC_TRACE_EVENT(sys_mprotect,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, unsigned long prot),
	TP_ARGS(sc_exit(ret,) start, len, prot),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(size_t, len)) sc_in(__field(unsigned long, prot))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(len, len)) sc_in(tp_assign(prot, prot))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_ioctl
SC_TRACE_EVENT(sys_ioctl,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) fd, cmd, arg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(unsigned int, cmd)) sc_inout(__field(unsigned long, arg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(cmd, cmd)) sc_inout(tp_assign(arg, arg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_msync
SC_TRACE_EVENT(sys_msync,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, int flags),
	TP_ARGS(sc_exit(ret,) start, len, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(size_t, len)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(len, len)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_madvise
SC_TRACE_EVENT(sys_madvise,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len_in, int behavior),
	TP_ARGS(sc_exit(ret,) start, len_in, behavior),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(size_t, len_in)) sc_in(__field(int, behavior))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(len_in, len_in)) sc_in(tp_assign(behavior, behavior))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_shmget
SC_TRACE_EVENT(sys_shmget,
	TP_PROTO(sc_exit(long ret,) key_t key, size_t size, int shmflg),
	TP_ARGS(sc_exit(ret,) key, size, shmflg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(key_t, key)) sc_in(__field(size_t, size)) sc_in(__field(int, shmflg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(key, key)) sc_in(tp_assign(size, size)) sc_in(tp_assign(shmflg, shmflg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_socket
SC_TRACE_EVENT(sys_socket,
	TP_PROTO(sc_exit(long ret,) int family, int type, int protocol),
	TP_ARGS(sc_exit(ret,) family, type, protocol),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, family)) sc_in(__field(int, type)) sc_in(__field(int, protocol))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(family, family)) sc_in(tp_assign(type, type)) sc_in(tp_assign(protocol, protocol))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_semget
SC_TRACE_EVENT(sys_semget,
	TP_PROTO(sc_exit(long ret,) key_t key, int nsems, int semflg),
	TP_ARGS(sc_exit(ret,) key, nsems, semflg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(key_t, key)) sc_in(__field(int, nsems)) sc_in(__field(int, semflg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(key, key)) sc_in(tp_assign(nsems, nsems)) sc_in(tp_assign(semflg, semflg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_fcntl
SC_TRACE_EVENT(sys_fcntl,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) fd, cmd, arg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(unsigned int, cmd)) sc_inout(__field(unsigned long, arg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(cmd, cmd)) sc_inout(tp_assign(arg, arg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_fchown
SC_TRACE_EVENT(sys_fchown,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, uid_t user, gid_t group),
	TP_ARGS(sc_exit(ret,) fd, user, group),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field(uid_t, user)) sc_in(__field(gid_t, group))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(user, user)) sc_in(tp_assign(group, group))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_setresuid
SC_TRACE_EVENT(sys_setresuid,
	TP_PROTO(sc_exit(long ret,) uid_t ruid, uid_t euid, uid_t suid),
	TP_ARGS(sc_exit(ret,) ruid, euid, suid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(uid_t, ruid)) sc_in(__field(uid_t, euid)) sc_in(__field(uid_t, suid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ruid, ruid)) sc_in(tp_assign(euid, euid)) sc_in(tp_assign(suid, suid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_setresgid
SC_TRACE_EVENT(sys_setresgid,
	TP_PROTO(sc_exit(long ret,) gid_t rgid, gid_t egid, gid_t sgid),
	TP_ARGS(sc_exit(ret,) rgid, egid, sgid),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(gid_t, rgid)) sc_in(__field(gid_t, egid)) sc_in(__field(gid_t, sgid))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(rgid, rgid)) sc_in(tp_assign(egid, egid)) sc_in(tp_assign(sgid, sgid))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_sysfs
SC_TRACE_EVENT(sys_sysfs,
	TP_PROTO(sc_exit(long ret,) int option, unsigned long arg1, unsigned long arg2),
	TP_ARGS(sc_exit(ret,) option, arg1, arg2),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, option)) sc_in(__field(unsigned long, arg1)) sc_in(__field(unsigned long, arg2))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(option, option)) sc_in(tp_assign(arg1, arg1)) sc_in(tp_assign(arg2, arg2))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_setpriority
SC_TRACE_EVENT(sys_setpriority,
	TP_PROTO(sc_exit(long ret,) int which, int who, int niceval),
	TP_ARGS(sc_exit(ret,) which, who, niceval),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, which)) sc_in(__field(int, who)) sc_in(__field(int, niceval))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which, which)) sc_in(tp_assign(who, who)) sc_in(tp_assign(niceval, niceval))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_readahead
SC_TRACE_EVENT(sys_readahead,
	TP_PROTO(sc_exit(long ret,) int fd, loff_t offset, size_t count),
	TP_ARGS(sc_exit(ret,) fd, offset, count),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(loff_t, offset)) sc_in(__field(size_t, count))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(offset, offset)) sc_in(tp_assign(count, count))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_tgkill
SC_TRACE_EVENT(sys_tgkill,
	TP_PROTO(sc_exit(long ret,) pid_t tgid, pid_t pid, int sig),
	TP_ARGS(sc_exit(ret,) tgid, pid, sig),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, tgid)) sc_in(__field(pid_t, pid)) sc_in(__field(int, sig))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(tgid, tgid)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(sig, sig))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_ioprio_set
SC_TRACE_EVENT(sys_ioprio_set,
	TP_PROTO(sc_exit(long ret,) int which, int who, int ioprio),
	TP_ARGS(sc_exit(ret,) which, who, ioprio),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, which)) sc_in(__field(int, who)) sc_in(__field(int, ioprio))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which, which)) sc_in(tp_assign(who, who)) sc_in(tp_assign(ioprio, ioprio))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_dup3
SC_TRACE_EVENT(sys_dup3,
	TP_PROTO(sc_exit(long ret,) unsigned int oldfd, unsigned int newfd, int flags),
	TP_ARGS(sc_exit(ret,) oldfd, newfd, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, oldfd)) sc_in(__field(unsigned int, newfd)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(oldfd, oldfd)) sc_in(tp_assign(newfd, newfd)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_semctl
SC_TRACE_EVENT(sys_semctl,
	TP_PROTO(sc_exit(long ret,) int semid, int semnum, int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) semid, semnum, cmd, arg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, semid)) sc_in(__field(int, semnum)) sc_in(__field(int, cmd)) sc_inout(__field(unsigned long, arg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(semid, semid)) sc_in(tp_assign(semnum, semnum)) sc_in(tp_assign(cmd, cmd)) sc_inout(tp_assign(arg, arg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_ptrace
SC_TRACE_EVENT(sys_ptrace,
	TP_PROTO(sc_exit(long ret,) long request, long pid, unsigned long addr, unsigned long data),
	TP_ARGS(sc_exit(ret,) request, pid, addr, data),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(long, request)) sc_in(__field(long, pid)) sc_inout(__field_hex(unsigned long, addr)) sc_inout(__field(unsigned long, data))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(request, request)) sc_in(tp_assign(pid, pid)) sc_inout(tp_assign(addr, addr)) sc_inout(tp_assign(data, data))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_fadvise64
SC_TRACE_EVENT(sys_fadvise64,
	TP_PROTO(sc_exit(long ret,) int fd, loff_t offset, size_t len, int advice),
	TP_ARGS(sc_exit(ret,) fd, offset, len, advice),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(loff_t, offset)) sc_in(__field(size_t, len)) sc_in(__field(int, advice))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(offset, offset)) sc_in(tp_assign(len, len)) sc_in(tp_assign(advice, advice))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_tee
SC_TRACE_EVENT(sys_tee,
	TP_PROTO(sc_exit(long ret,) int fdin, int fdout, size_t len, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fdin, fdout, len, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fdin)) sc_in(__field(int, fdout)) sc_in(__field(size_t, len)) sc_in(__field(unsigned int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fdin, fdin)) sc_in(tp_assign(fdout, fdout)) sc_in(tp_assign(len, len)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_sync_file_range
SC_TRACE_EVENT(sys_sync_file_range,
	TP_PROTO(sc_exit(long ret,) int fd, loff_t offset, loff_t nbytes, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, offset, nbytes, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(loff_t, offset)) sc_in(__field(loff_t, nbytes)) sc_in(__field(unsigned int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(offset, offset)) sc_in(tp_assign(nbytes, nbytes)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_fallocate
SC_TRACE_EVENT(sys_fallocate,
	TP_PROTO(sc_exit(long ret,) int fd, int mode, loff_t offset, loff_t len),
	TP_ARGS(sc_exit(ret,) fd, mode, offset, len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(int, mode)) sc_in(__field(loff_t, offset)) sc_in(__field(loff_t, len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(mode, mode)) sc_in(tp_assign(offset, offset)) sc_in(tp_assign(len, len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_mremap
SC_TRACE_EVENT(sys_mremap,
	TP_PROTO(sc_exit(long ret,) unsigned long addr, unsigned long old_len, unsigned long new_len, unsigned long flags, unsigned long new_addr),
	TP_ARGS(sc_exit(ret,) addr, old_len, new_len, flags, new_addr),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field_hex(unsigned long, addr)) sc_in(__field(unsigned long, old_len)) sc_in(__field(unsigned long, new_len)) sc_in(__field(unsigned long, flags)) sc_in(__field_hex(unsigned long, new_addr))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(addr, addr)) sc_in(tp_assign(old_len, old_len)) sc_in(tp_assign(new_len, new_len)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(new_addr, new_addr))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_prctl
SC_TRACE_EVENT(sys_prctl,
	TP_PROTO(sc_exit(long ret,) int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5),
	TP_ARGS(sc_exit(ret,) option, arg2, arg3, arg4, arg5),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, option)) sc_inout(__field(unsigned long, arg2)) sc_in(__field(unsigned long, arg3)) sc_in(__field(unsigned long, arg4)) sc_in(__field(unsigned long, arg5))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(option, option)) sc_inout(tp_assign(arg2, arg2)) sc_in(tp_assign(arg3, arg3)) sc_in(tp_assign(arg4, arg4)) sc_in(tp_assign(arg5, arg5))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_remap_file_pages
SC_TRACE_EVENT(sys_remap_file_pages,
	TP_PROTO(sc_exit(long ret,) unsigned long start, unsigned long size, unsigned long prot, unsigned long pgoff, unsigned long flags),
	TP_ARGS(sc_exit(ret,) start, size, prot, pgoff, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(unsigned long, size)) sc_in(__field(unsigned long, prot)) sc_in(__field(unsigned long, pgoff)) sc_in(__field(unsigned long, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(size, size)) sc_in(tp_assign(prot, prot)) sc_in(tp_assign(pgoff, pgoff)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_keyctl
SC_TRACE_EVENT(sys_keyctl,
	TP_PROTO(sc_exit(long ret,) int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5),
	TP_ARGS(sc_exit(ret,) option, arg2, arg3, arg4, arg5),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, option)) sc_inout(__field(unsigned long, arg2)) sc_inout(__field(unsigned long, arg3)) sc_inout(__field(unsigned long, arg4)) sc_inout(__field(unsigned long, arg5))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(option, option)) sc_inout(tp_assign(arg2, arg2)) sc_inout(tp_assign(arg3, arg3)) sc_inout(tp_assign(arg4, arg4)) sc_inout(tp_assign(arg5, arg5))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sys_mmap
SC_TRACE_EVENT(sys_mmap,
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
#ifndef OVERRIDE_TABLE_64_sys_sched_yield
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_sched_yield, 24, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_pause
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_pause, 34, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getpid
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_getpid, 39, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getuid
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_getuid, 102, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getgid
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_getgid, 104, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_geteuid
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_geteuid, 107, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getegid
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_getegid, 108, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getppid
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_getppid, 110, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getpgrp
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_getpgrp, 111, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setsid
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_setsid, 112, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_munlockall
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_munlockall, 152, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_vhangup
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_vhangup, 153, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_sync
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_sync, 162, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_gettid
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_gettid, 186, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_restart_syscall
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_restart_syscall, 219, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_inotify_init
TRACE_SYSCALL_TABLE(syscalls_noargs, sys_inotify_init, 253, 0)
#endif
#else /* #ifdef SC_ENTER */
#ifndef OVERRIDE_TABLE_64_sys_sched_yield
TRACE_SYSCALL_TABLE(sys_sched_yield, sys_sched_yield, 24, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_pause
TRACE_SYSCALL_TABLE(sys_pause, sys_pause, 34, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getpid
TRACE_SYSCALL_TABLE(sys_getpid, sys_getpid, 39, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getuid
TRACE_SYSCALL_TABLE(sys_getuid, sys_getuid, 102, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getgid
TRACE_SYSCALL_TABLE(sys_getgid, sys_getgid, 104, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_geteuid
TRACE_SYSCALL_TABLE(sys_geteuid, sys_geteuid, 107, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getegid
TRACE_SYSCALL_TABLE(sys_getegid, sys_getegid, 108, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getppid
TRACE_SYSCALL_TABLE(sys_getppid, sys_getppid, 110, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getpgrp
TRACE_SYSCALL_TABLE(sys_getpgrp, sys_getpgrp, 111, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setsid
TRACE_SYSCALL_TABLE(sys_setsid, sys_setsid, 112, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_munlockall
TRACE_SYSCALL_TABLE(sys_munlockall, sys_munlockall, 152, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_vhangup
TRACE_SYSCALL_TABLE(sys_vhangup, sys_vhangup, 153, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_sync
TRACE_SYSCALL_TABLE(sys_sync, sys_sync, 162, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_gettid
TRACE_SYSCALL_TABLE(sys_gettid, sys_gettid, 186, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_restart_syscall
TRACE_SYSCALL_TABLE(sys_restart_syscall, sys_restart_syscall, 219, 0)
#endif
#ifndef OVERRIDE_TABLE_64_sys_inotify_init
TRACE_SYSCALL_TABLE(sys_inotify_init, sys_inotify_init, 253, 0)
#endif
#endif /* else #ifdef SC_ENTER */
#ifndef OVERRIDE_TABLE_64_sys_close
TRACE_SYSCALL_TABLE(sys_close, sys_close, 3, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_lseek
TRACE_SYSCALL_TABLE(sys_lseek, sys_lseek, 8, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_mmap
TRACE_SYSCALL_TABLE(sys_mmap, sys_mmap, 9, 6)
#endif
#ifndef OVERRIDE_TABLE_64_sys_mprotect
TRACE_SYSCALL_TABLE(sys_mprotect, sys_mprotect, 10, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_munmap
TRACE_SYSCALL_TABLE(sys_munmap, sys_munmap, 11, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_brk
TRACE_SYSCALL_TABLE(sys_brk, sys_brk, 12, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_ioctl
TRACE_SYSCALL_TABLE(sys_ioctl, sys_ioctl, 16, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_mremap
TRACE_SYSCALL_TABLE(sys_mremap, sys_mremap, 25, 5)
#endif
#ifndef OVERRIDE_TABLE_64_sys_msync
TRACE_SYSCALL_TABLE(sys_msync, sys_msync, 26, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_madvise
TRACE_SYSCALL_TABLE(sys_madvise, sys_madvise, 28, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_shmget
TRACE_SYSCALL_TABLE(sys_shmget, sys_shmget, 29, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_dup
TRACE_SYSCALL_TABLE(sys_dup, sys_dup, 32, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_dup2
TRACE_SYSCALL_TABLE(sys_dup2, sys_dup2, 33, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_alarm
TRACE_SYSCALL_TABLE(sys_alarm, sys_alarm, 37, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_socket
TRACE_SYSCALL_TABLE(sys_socket, sys_socket, 41, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_shutdown
TRACE_SYSCALL_TABLE(sys_shutdown, sys_shutdown, 48, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_listen
TRACE_SYSCALL_TABLE(sys_listen, sys_listen, 50, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_exit
TRACE_SYSCALL_TABLE(sys_exit, sys_exit, 60, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_kill
TRACE_SYSCALL_TABLE(sys_kill, sys_kill, 62, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_semget
TRACE_SYSCALL_TABLE(sys_semget, sys_semget, 64, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_semctl
TRACE_SYSCALL_TABLE(sys_semctl, sys_semctl, 66, 4)
#endif
#ifndef OVERRIDE_TABLE_64_sys_msgget
TRACE_SYSCALL_TABLE(sys_msgget, sys_msgget, 68, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_fcntl
TRACE_SYSCALL_TABLE(sys_fcntl, sys_fcntl, 72, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_flock
TRACE_SYSCALL_TABLE(sys_flock, sys_flock, 73, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_fsync
TRACE_SYSCALL_TABLE(sys_fsync, sys_fsync, 74, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_fdatasync
TRACE_SYSCALL_TABLE(sys_fdatasync, sys_fdatasync, 75, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_ftruncate
TRACE_SYSCALL_TABLE(sys_ftruncate, sys_ftruncate, 77, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_fchdir
TRACE_SYSCALL_TABLE(sys_fchdir, sys_fchdir, 81, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_fchmod
TRACE_SYSCALL_TABLE(sys_fchmod, sys_fchmod, 91, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_fchown
TRACE_SYSCALL_TABLE(sys_fchown, sys_fchown, 93, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_umask
TRACE_SYSCALL_TABLE(sys_umask, sys_umask, 95, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_ptrace
TRACE_SYSCALL_TABLE(sys_ptrace, sys_ptrace, 101, 4)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setuid
TRACE_SYSCALL_TABLE(sys_setuid, sys_setuid, 105, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setgid
TRACE_SYSCALL_TABLE(sys_setgid, sys_setgid, 106, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setpgid
TRACE_SYSCALL_TABLE(sys_setpgid, sys_setpgid, 109, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setreuid
TRACE_SYSCALL_TABLE(sys_setreuid, sys_setreuid, 113, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setregid
TRACE_SYSCALL_TABLE(sys_setregid, sys_setregid, 114, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setresuid
TRACE_SYSCALL_TABLE(sys_setresuid, sys_setresuid, 117, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setresgid
TRACE_SYSCALL_TABLE(sys_setresgid, sys_setresgid, 119, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getpgid
TRACE_SYSCALL_TABLE(sys_getpgid, sys_getpgid, 121, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setfsuid
TRACE_SYSCALL_TABLE(sys_setfsuid, sys_setfsuid, 122, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setfsgid
TRACE_SYSCALL_TABLE(sys_setfsgid, sys_setfsgid, 123, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getsid
TRACE_SYSCALL_TABLE(sys_getsid, sys_getsid, 124, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_personality
TRACE_SYSCALL_TABLE(sys_personality, sys_personality, 135, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_sysfs
TRACE_SYSCALL_TABLE(sys_sysfs, sys_sysfs, 139, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_getpriority
TRACE_SYSCALL_TABLE(sys_getpriority, sys_getpriority, 140, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setpriority
TRACE_SYSCALL_TABLE(sys_setpriority, sys_setpriority, 141, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_sched_getscheduler
TRACE_SYSCALL_TABLE(sys_sched_getscheduler, sys_sched_getscheduler, 145, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_sched_get_priority_max
TRACE_SYSCALL_TABLE(sys_sched_get_priority_max, sys_sched_get_priority_max, 146, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_sched_get_priority_min
TRACE_SYSCALL_TABLE(sys_sched_get_priority_min, sys_sched_get_priority_min, 147, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_mlock
TRACE_SYSCALL_TABLE(sys_mlock, sys_mlock, 149, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_munlock
TRACE_SYSCALL_TABLE(sys_munlock, sys_munlock, 150, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_mlockall
TRACE_SYSCALL_TABLE(sys_mlockall, sys_mlockall, 151, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_prctl
TRACE_SYSCALL_TABLE(sys_prctl, sys_prctl, 157, 5)
#endif
#ifndef OVERRIDE_TABLE_64_sys_readahead
TRACE_SYSCALL_TABLE(sys_readahead, sys_readahead, 187, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_tkill
TRACE_SYSCALL_TABLE(sys_tkill, sys_tkill, 200, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_io_destroy
TRACE_SYSCALL_TABLE(sys_io_destroy, sys_io_destroy, 207, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_epoll_create
TRACE_SYSCALL_TABLE(sys_epoll_create, sys_epoll_create, 213, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_remap_file_pages
TRACE_SYSCALL_TABLE(sys_remap_file_pages, sys_remap_file_pages, 216, 5)
#endif
#ifndef OVERRIDE_TABLE_64_sys_fadvise64
TRACE_SYSCALL_TABLE(sys_fadvise64, sys_fadvise64, 221, 4)
#endif
#ifndef OVERRIDE_TABLE_64_sys_timer_getoverrun
TRACE_SYSCALL_TABLE(sys_timer_getoverrun, sys_timer_getoverrun, 225, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_timer_delete
TRACE_SYSCALL_TABLE(sys_timer_delete, sys_timer_delete, 226, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_exit_group
TRACE_SYSCALL_TABLE(sys_exit_group, sys_exit_group, 231, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_tgkill
TRACE_SYSCALL_TABLE(sys_tgkill, sys_tgkill, 234, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_keyctl
TRACE_SYSCALL_TABLE(sys_keyctl, sys_keyctl, 250, 5)
#endif
#ifndef OVERRIDE_TABLE_64_sys_ioprio_set
TRACE_SYSCALL_TABLE(sys_ioprio_set, sys_ioprio_set, 251, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_ioprio_get
TRACE_SYSCALL_TABLE(sys_ioprio_get, sys_ioprio_get, 252, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_inotify_rm_watch
TRACE_SYSCALL_TABLE(sys_inotify_rm_watch, sys_inotify_rm_watch, 255, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_unshare
TRACE_SYSCALL_TABLE(sys_unshare, sys_unshare, 272, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_tee
TRACE_SYSCALL_TABLE(sys_tee, sys_tee, 276, 4)
#endif
#ifndef OVERRIDE_TABLE_64_sys_sync_file_range
TRACE_SYSCALL_TABLE(sys_sync_file_range, sys_sync_file_range, 277, 4)
#endif
#ifndef OVERRIDE_TABLE_64_sys_timerfd_create
TRACE_SYSCALL_TABLE(sys_timerfd_create, sys_timerfd_create, 283, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_eventfd
TRACE_SYSCALL_TABLE(sys_eventfd, sys_eventfd, 284, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_fallocate
TRACE_SYSCALL_TABLE(sys_fallocate, sys_fallocate, 285, 4)
#endif
#ifndef OVERRIDE_TABLE_64_sys_eventfd2
TRACE_SYSCALL_TABLE(sys_eventfd2, sys_eventfd2, 290, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_epoll_create1
TRACE_SYSCALL_TABLE(sys_epoll_create1, sys_epoll_create1, 291, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_dup3
TRACE_SYSCALL_TABLE(sys_dup3, sys_dup3, 292, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sys_inotify_init1
TRACE_SYSCALL_TABLE(sys_inotify_init1, sys_inotify_init1, 294, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_fanotify_init
TRACE_SYSCALL_TABLE(sys_fanotify_init, sys_fanotify_init, 300, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sys_syncfs
TRACE_SYSCALL_TABLE(sys_syncfs, sys_syncfs, 306, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sys_setns
TRACE_SYSCALL_TABLE(sys_setns, sys_setns, 308, 2)
#endif

#endif /* CREATE_SYSCALL_TABLE */
