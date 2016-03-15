/* THIS FILE IS AUTO-GENERATED. DO NOT EDIT */
#ifndef CREATE_SYSCALL_TABLE

#if !defined(_TRACE_SYSCALLS_INTEGERS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SYSCALLS_INTEGERS_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/syscalls.h>
#include "arm-32-syscalls-3.4.25_integers_override.h"
#include "syscalls_integers_override.h"

#ifdef SC_ENTER
SC_LTTNG_TRACEPOINT_EVENT_CLASS_NOARGS(syscalls_noargs,
	TP_FIELDS()
)
#ifndef OVERRIDE_32_restart_syscall
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, restart_syscall)
#endif
#ifndef OVERRIDE_32_getpid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getpid)
#endif
#ifndef OVERRIDE_32_getuid16
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getuid16)
#endif
#ifndef OVERRIDE_32_pause
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, pause)
#endif
#ifndef OVERRIDE_32_sync
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, sync)
#endif
#ifndef OVERRIDE_32_getgid16
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getgid16)
#endif
#ifndef OVERRIDE_32_geteuid16
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, geteuid16)
#endif
#ifndef OVERRIDE_32_getegid16
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getegid16)
#endif
#ifndef OVERRIDE_32_getppid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getppid)
#endif
#ifndef OVERRIDE_32_getpgrp
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getpgrp)
#endif
#ifndef OVERRIDE_32_setsid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, setsid)
#endif
#ifndef OVERRIDE_32_vhangup
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, vhangup)
#endif
#ifndef OVERRIDE_32_munlockall
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, munlockall)
#endif
#ifndef OVERRIDE_32_sched_yield
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, sched_yield)
#endif
#ifndef OVERRIDE_32_getuid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getuid)
#endif
#ifndef OVERRIDE_32_getgid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getgid)
#endif
#ifndef OVERRIDE_32_geteuid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, geteuid)
#endif
#ifndef OVERRIDE_32_getegid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, getegid)
#endif
#ifndef OVERRIDE_32_gettid
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, gettid)
#endif
#ifndef OVERRIDE_32_inotify_init
SC_LTTNG_TRACEPOINT_EVENT_INSTANCE_NOARGS(syscalls_noargs, inotify_init)
#endif
#else /* #ifdef SC_ENTER */
#ifndef OVERRIDE_32_restart_syscall
SC_LTTNG_TRACEPOINT_EVENT(restart_syscall,
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
#ifndef OVERRIDE_32_getuid16
SC_LTTNG_TRACEPOINT_EVENT(getuid16,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_pause
SC_LTTNG_TRACEPOINT_EVENT(pause,
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
#ifndef OVERRIDE_32_getgid16
SC_LTTNG_TRACEPOINT_EVENT(getgid16,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_geteuid16
SC_LTTNG_TRACEPOINT_EVENT(geteuid16,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#ifndef OVERRIDE_32_getegid16
SC_LTTNG_TRACEPOINT_EVENT(getegid16,
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
#ifndef OVERRIDE_32_getpgrp
SC_LTTNG_TRACEPOINT_EVENT(getpgrp,
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
#ifndef OVERRIDE_32_vhangup
SC_LTTNG_TRACEPOINT_EVENT(vhangup,
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
#ifndef OVERRIDE_32_sched_yield
SC_LTTNG_TRACEPOINT_EVENT(sched_yield,
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
#ifndef OVERRIDE_32_getgid
SC_LTTNG_TRACEPOINT_EVENT(getgid,
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
#ifndef OVERRIDE_32_inotify_init
SC_LTTNG_TRACEPOINT_EVENT(inotify_init,
	TP_PROTO(sc_exit(long ret)),
	TP_ARGS(sc_exit(ret)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)))
)
#endif
#endif /* else #ifdef SC_ENTER */
#ifndef OVERRIDE_32_exit
SC_LTTNG_TRACEPOINT_EVENT(exit,
	TP_PROTO(sc_exit(long ret,) int error_code),
	TP_ARGS(sc_exit(ret,) error_code),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, error_code, error_code)))
)
#endif
#ifndef OVERRIDE_32_close
SC_LTTNG_TRACEPOINT_EVENT(close,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)))
)
#endif
#ifndef OVERRIDE_32_setuid16
SC_LTTNG_TRACEPOINT_EVENT(setuid16,
	TP_PROTO(sc_exit(long ret,) old_uid_t uid),
	TP_ARGS(sc_exit(ret,) uid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_uid_t, uid, uid)))
)
#endif
#ifndef OVERRIDE_32_nice
SC_LTTNG_TRACEPOINT_EVENT(nice,
	TP_PROTO(sc_exit(long ret,) int increment),
	TP_ARGS(sc_exit(ret,) increment),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, increment, increment)))
)
#endif
#ifndef OVERRIDE_32_dup
SC_LTTNG_TRACEPOINT_EVENT(dup,
	TP_PROTO(sc_exit(long ret,) unsigned int fildes),
	TP_ARGS(sc_exit(ret,) fildes),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fildes, fildes)))
)
#endif
#ifndef OVERRIDE_32_brk
SC_LTTNG_TRACEPOINT_EVENT(brk,
	TP_PROTO(sc_exit(long ret,) unsigned long brk),
	TP_ARGS(sc_exit(ret,) brk),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, brk, brk)))
)
#endif
#ifndef OVERRIDE_32_setgid16
SC_LTTNG_TRACEPOINT_EVENT(setgid16,
	TP_PROTO(sc_exit(long ret,) old_gid_t gid),
	TP_ARGS(sc_exit(ret,) gid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_gid_t, gid, gid)))
)
#endif
#ifndef OVERRIDE_32_umask
SC_LTTNG_TRACEPOINT_EVENT(umask,
	TP_PROTO(sc_exit(long ret,) int mask),
	TP_ARGS(sc_exit(ret,) mask),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, mask, mask)))
)
#endif
#ifndef OVERRIDE_32_fsync
SC_LTTNG_TRACEPOINT_EVENT(fsync,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)))
)
#endif
#ifndef OVERRIDE_32_getpgid
SC_LTTNG_TRACEPOINT_EVENT(getpgid,
	TP_PROTO(sc_exit(long ret,) pid_t pid),
	TP_ARGS(sc_exit(ret,) pid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)))
)
#endif
#ifndef OVERRIDE_32_fchdir
SC_LTTNG_TRACEPOINT_EVENT(fchdir,
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
#ifndef OVERRIDE_32_setfsuid16
SC_LTTNG_TRACEPOINT_EVENT(setfsuid16,
	TP_PROTO(sc_exit(long ret,) old_uid_t uid),
	TP_ARGS(sc_exit(ret,) uid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_uid_t, uid, uid)))
)
#endif
#ifndef OVERRIDE_32_setfsgid16
SC_LTTNG_TRACEPOINT_EVENT(setfsgid16,
	TP_PROTO(sc_exit(long ret,) old_gid_t gid),
	TP_ARGS(sc_exit(ret,) gid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_gid_t, gid, gid)))
)
#endif
#ifndef OVERRIDE_32_getsid
SC_LTTNG_TRACEPOINT_EVENT(getsid,
	TP_PROTO(sc_exit(long ret,) pid_t pid),
	TP_ARGS(sc_exit(ret,) pid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)))
)
#endif
#ifndef OVERRIDE_32_fdatasync
SC_LTTNG_TRACEPOINT_EVENT(fdatasync,
	TP_PROTO(sc_exit(long ret,) unsigned int fd),
	TP_ARGS(sc_exit(ret,) fd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)))
)
#endif
#ifndef OVERRIDE_32_mlockall
SC_LTTNG_TRACEPOINT_EVENT(mlockall,
	TP_PROTO(sc_exit(long ret,) int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, flags, flags)))
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
#ifndef OVERRIDE_32_setuid
SC_LTTNG_TRACEPOINT_EVENT(setuid,
	TP_PROTO(sc_exit(long ret,) uid_t uid),
	TP_ARGS(sc_exit(ret,) uid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(uid_t, uid, uid)))
)
#endif
#ifndef OVERRIDE_32_setgid
SC_LTTNG_TRACEPOINT_EVENT(setgid,
	TP_PROTO(sc_exit(long ret,) gid_t gid),
	TP_ARGS(sc_exit(ret,) gid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(gid_t, gid, gid)))
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
#ifndef OVERRIDE_32_io_destroy
SC_LTTNG_TRACEPOINT_EVENT(io_destroy,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx),
	TP_ARGS(sc_exit(ret,) ctx),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(aio_context_t, ctx, ctx)))
)
#endif
#ifndef OVERRIDE_32_exit_group
SC_LTTNG_TRACEPOINT_EVENT(exit_group,
	TP_PROTO(sc_exit(long ret,) int error_code),
	TP_ARGS(sc_exit(ret,) error_code),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, error_code, error_code)))
)
#endif
#ifndef OVERRIDE_32_epoll_create
SC_LTTNG_TRACEPOINT_EVENT(epoll_create,
	TP_PROTO(sc_exit(long ret,) int size),
	TP_ARGS(sc_exit(ret,) size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, size, size)))
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
#ifndef OVERRIDE_32_unshare
SC_LTTNG_TRACEPOINT_EVENT(unshare,
	TP_PROTO(sc_exit(long ret,) unsigned long unshare_flags),
	TP_ARGS(sc_exit(ret,) unshare_flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, unshare_flags, unshare_flags)))
)
#endif
#ifndef OVERRIDE_32_eventfd
SC_LTTNG_TRACEPOINT_EVENT(eventfd,
	TP_PROTO(sc_exit(long ret,) unsigned int count),
	TP_ARGS(sc_exit(ret,) count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, count, count)))
)
#endif
#ifndef OVERRIDE_32_epoll_create1
SC_LTTNG_TRACEPOINT_EVENT(epoll_create1,
	TP_PROTO(sc_exit(long ret,) int flags),
	TP_ARGS(sc_exit(ret,) flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_inotify_init1
SC_LTTNG_TRACEPOINT_EVENT(inotify_init1,
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
#ifndef OVERRIDE_32_kill
SC_LTTNG_TRACEPOINT_EVENT(kill,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int sig),
	TP_ARGS(sc_exit(ret,) pid, sig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, sig, sig)))
)
#endif
#ifndef OVERRIDE_32_setpgid
SC_LTTNG_TRACEPOINT_EVENT(setpgid,
	TP_PROTO(sc_exit(long ret,) pid_t pid, pid_t pgid),
	TP_ARGS(sc_exit(ret,) pid, pgid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(pid_t, pgid, pgid)))
)
#endif
#ifndef OVERRIDE_32_dup2
SC_LTTNG_TRACEPOINT_EVENT(dup2,
	TP_PROTO(sc_exit(long ret,) unsigned int oldfd, unsigned int newfd),
	TP_ARGS(sc_exit(ret,) oldfd, newfd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, oldfd, oldfd)) sc_in(ctf_integer(unsigned int, newfd, newfd)))
)
#endif
#ifndef OVERRIDE_32_setreuid16
SC_LTTNG_TRACEPOINT_EVENT(setreuid16,
	TP_PROTO(sc_exit(long ret,) old_uid_t ruid, old_uid_t euid),
	TP_ARGS(sc_exit(ret,) ruid, euid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_uid_t, ruid, ruid)) sc_inout(ctf_integer(old_uid_t, euid, euid)))
)
#endif
#ifndef OVERRIDE_32_setregid16
SC_LTTNG_TRACEPOINT_EVENT(setregid16,
	TP_PROTO(sc_exit(long ret,) old_gid_t rgid, old_gid_t egid),
	TP_ARGS(sc_exit(ret,) rgid, egid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_gid_t, rgid, rgid)) sc_inout(ctf_integer(old_gid_t, egid, egid)))
)
#endif
#ifndef OVERRIDE_32_munmap
SC_LTTNG_TRACEPOINT_EVENT(munmap,
	TP_PROTO(sc_exit(long ret,) unsigned long addr, size_t len),
	TP_ARGS(sc_exit(ret,) addr, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, addr, addr)) sc_in(ctf_integer(size_t, len, len)))
)
#endif
#ifndef OVERRIDE_32_ftruncate
SC_LTTNG_TRACEPOINT_EVENT(ftruncate,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned long length),
	TP_ARGS(sc_exit(ret,) fd, length),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(unsigned long, length, length)))
)
#endif
#ifndef OVERRIDE_32_fchmod
SC_LTTNG_TRACEPOINT_EVENT(fchmod,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, umode_t mode),
	TP_ARGS(sc_exit(ret,) fd, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_getpriority
SC_LTTNG_TRACEPOINT_EVENT(getpriority,
	TP_PROTO(sc_exit(long ret,) int which, int who),
	TP_ARGS(sc_exit(ret,) which, who),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(int, who, who)))
)
#endif
#ifndef OVERRIDE_32_bdflush
SC_LTTNG_TRACEPOINT_EVENT(bdflush,
	TP_PROTO(sc_exit(long ret,) int func, long data),
	TP_ARGS(sc_exit(ret,) func, data),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, func, func)) sc_inout(ctf_integer(long, data, data)))
)
#endif
#ifndef OVERRIDE_32_flock
SC_LTTNG_TRACEPOINT_EVENT(flock,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd),
	TP_ARGS(sc_exit(ret,) fd, cmd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(unsigned int, cmd, cmd)))
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
#ifndef OVERRIDE_32_setreuid
SC_LTTNG_TRACEPOINT_EVENT(setreuid,
	TP_PROTO(sc_exit(long ret,) uid_t ruid, uid_t euid),
	TP_ARGS(sc_exit(ret,) ruid, euid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(uid_t, ruid, ruid)) sc_in(ctf_integer(uid_t, euid, euid)))
)
#endif
#ifndef OVERRIDE_32_setregid
SC_LTTNG_TRACEPOINT_EVENT(setregid,
	TP_PROTO(sc_exit(long ret,) gid_t rgid, gid_t egid),
	TP_ARGS(sc_exit(ret,) rgid, egid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(gid_t, rgid, rgid)) sc_in(ctf_integer(gid_t, egid, egid)))
)
#endif
#ifndef OVERRIDE_32_tkill
SC_LTTNG_TRACEPOINT_EVENT(tkill,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int sig),
	TP_ARGS(sc_exit(ret,) pid, sig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, sig, sig)))
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
#ifndef OVERRIDE_32_msgget
SC_LTTNG_TRACEPOINT_EVENT(msgget,
	TP_PROTO(sc_exit(long ret,) key_t key, int msgflg),
	TP_ARGS(sc_exit(ret,) key, msgflg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(key_t, key, key)) sc_in(ctf_integer(int, msgflg, msgflg)))
)
#endif
#ifndef OVERRIDE_32_ioprio_get
SC_LTTNG_TRACEPOINT_EVENT(ioprio_get,
	TP_PROTO(sc_exit(long ret,) int which, int who),
	TP_ARGS(sc_exit(ret,) which, who),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(int, who, who)))
)
#endif
#ifndef OVERRIDE_32_inotify_rm_watch
SC_LTTNG_TRACEPOINT_EVENT(inotify_rm_watch,
	TP_PROTO(sc_exit(long ret,) int fd, __s32 wd),
	TP_ARGS(sc_exit(ret,) fd, wd),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(__s32, wd, wd)))
)
#endif
#ifndef OVERRIDE_32_timerfd_create
SC_LTTNG_TRACEPOINT_EVENT(timerfd_create,
	TP_PROTO(sc_exit(long ret,) int clockid, int flags),
	TP_ARGS(sc_exit(ret,) clockid, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, clockid, clockid)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_eventfd2
SC_LTTNG_TRACEPOINT_EVENT(eventfd2,
	TP_PROTO(sc_exit(long ret,) unsigned int count, int flags),
	TP_ARGS(sc_exit(ret,) count, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, count, count)) sc_in(ctf_integer(int, flags, flags)))
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
	TP_PROTO(sc_exit(long ret,) int fd, int nstype),
	TP_ARGS(sc_exit(ret,) fd, nstype),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(int, nstype, nstype)))
)
#endif
#ifndef OVERRIDE_32_lseek
SC_LTTNG_TRACEPOINT_EVENT(lseek,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, off_t offset, unsigned int origin),
	TP_ARGS(sc_exit(ret,) fd, offset, origin),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(off_t, offset, offset)) sc_in(ctf_integer(unsigned int, origin, origin)))
)
#endif
#ifndef OVERRIDE_32_ioctl
SC_LTTNG_TRACEPOINT_EVENT(ioctl,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) fd, cmd, arg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(unsigned int, cmd, cmd)) sc_inout(ctf_integer(unsigned long, arg, arg)))
)
#endif
#ifndef OVERRIDE_32_fcntl
SC_LTTNG_TRACEPOINT_EVENT(fcntl,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) fd, cmd, arg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(unsigned int, cmd, cmd)) sc_inout(ctf_integer(unsigned long, arg, arg)))
)
#endif
#ifndef OVERRIDE_32_fchown16
SC_LTTNG_TRACEPOINT_EVENT(fchown16,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, old_uid_t user, old_gid_t group),
	TP_ARGS(sc_exit(ret,) fd, user, group),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, fd, fd)) sc_inout(ctf_integer(old_uid_t, user, user)) sc_inout(ctf_integer(old_gid_t, group, group)))
)
#endif
#ifndef OVERRIDE_32_setpriority
SC_LTTNG_TRACEPOINT_EVENT(setpriority,
	TP_PROTO(sc_exit(long ret,) int which, int who, int niceval),
	TP_ARGS(sc_exit(ret,) which, who, niceval),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(int, who, who)) sc_in(ctf_integer(int, niceval, niceval)))
)
#endif
#ifndef OVERRIDE_32_mprotect
SC_LTTNG_TRACEPOINT_EVENT(mprotect,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, unsigned long prot),
	TP_ARGS(sc_exit(ret,) start, len, prot),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(size_t, len, len)) sc_in(ctf_integer(unsigned long, prot, prot)))
)
#endif
#ifndef OVERRIDE_32_sysfs
SC_LTTNG_TRACEPOINT_EVENT(sysfs,
	TP_PROTO(sc_exit(long ret,) int option, unsigned long arg1, unsigned long arg2),
	TP_ARGS(sc_exit(ret,) option, arg1, arg2),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, option, option)) sc_in(ctf_integer(unsigned long, arg1, arg1)) sc_in(ctf_integer(unsigned long, arg2, arg2)))
)
#endif
#ifndef OVERRIDE_32_msync
SC_LTTNG_TRACEPOINT_EVENT(msync,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, int flags),
	TP_ARGS(sc_exit(ret,) start, len, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(size_t, len, len)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_setresuid16
SC_LTTNG_TRACEPOINT_EVENT(setresuid16,
	TP_PROTO(sc_exit(long ret,) old_uid_t ruid, old_uid_t euid, old_uid_t suid),
	TP_ARGS(sc_exit(ret,) ruid, euid, suid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_uid_t, ruid, ruid)) sc_inout(ctf_integer(old_uid_t, euid, euid)) sc_inout(ctf_integer(old_uid_t, suid, suid)))
)
#endif
#ifndef OVERRIDE_32_setresgid16
SC_LTTNG_TRACEPOINT_EVENT(setresgid16,
	TP_PROTO(sc_exit(long ret,) old_gid_t rgid, old_gid_t egid, old_gid_t sgid),
	TP_ARGS(sc_exit(ret,) rgid, egid, sgid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_gid_t, rgid, rgid)) sc_inout(ctf_integer(old_gid_t, egid, egid)) sc_inout(ctf_integer(old_gid_t, sgid, sgid)))
)
#endif
#ifndef OVERRIDE_32_fchown
SC_LTTNG_TRACEPOINT_EVENT(fchown,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, uid_t user, gid_t group),
	TP_ARGS(sc_exit(ret,) fd, user, group),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(uid_t, user, user)) sc_in(ctf_integer(gid_t, group, group)))
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
#ifndef OVERRIDE_32_madvise
SC_LTTNG_TRACEPOINT_EVENT(madvise,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len_in, int behavior),
	TP_ARGS(sc_exit(ret,) start, len_in, behavior),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(size_t, len_in, len_in)) sc_in(ctf_integer(int, behavior, behavior)))
)
#endif
#ifndef OVERRIDE_32_fcntl64
SC_LTTNG_TRACEPOINT_EVENT(fcntl64,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) fd, cmd, arg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, fd, fd)) sc_inout(ctf_integer(unsigned int, cmd, cmd)) sc_inout(ctf_integer(unsigned long, arg, arg)))
)
#endif
#ifndef OVERRIDE_32_tgkill
SC_LTTNG_TRACEPOINT_EVENT(tgkill,
	TP_PROTO(sc_exit(long ret,) pid_t tgid, pid_t pid, int sig),
	TP_ARGS(sc_exit(ret,) tgid, pid, sig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, tgid, tgid)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, sig, sig)))
)
#endif
#ifndef OVERRIDE_32_socket
SC_LTTNG_TRACEPOINT_EVENT(socket,
	TP_PROTO(sc_exit(long ret,) int family, int type, int protocol),
	TP_ARGS(sc_exit(ret,) family, type, protocol),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, family, family)) sc_in(ctf_integer(int, type, type)) sc_in(ctf_integer(int, protocol, protocol)))
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
#ifndef OVERRIDE_32_ioprio_set
SC_LTTNG_TRACEPOINT_EVENT(ioprio_set,
	TP_PROTO(sc_exit(long ret,) int which, int who, int ioprio),
	TP_ARGS(sc_exit(ret,) which, who, ioprio),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(int, who, who)) sc_in(ctf_integer(int, ioprio, ioprio)))
)
#endif
#ifndef OVERRIDE_32_dup3
SC_LTTNG_TRACEPOINT_EVENT(dup3,
	TP_PROTO(sc_exit(long ret,) unsigned int oldfd, unsigned int newfd, int flags),
	TP_ARGS(sc_exit(ret,) oldfd, newfd, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, oldfd, oldfd)) sc_in(ctf_integer(unsigned int, newfd, newfd)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_ptrace
SC_LTTNG_TRACEPOINT_EVENT(ptrace,
	TP_PROTO(sc_exit(long ret,) long request, long pid, unsigned long addr, unsigned long data),
	TP_ARGS(sc_exit(ret,) request, pid, addr, data),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(long, request, request)) sc_in(ctf_integer(long, pid, pid)) sc_inout(ctf_integer(unsigned long, addr, addr)) sc_inout(ctf_integer(unsigned long, data, data)))
)
#endif
#ifndef OVERRIDE_32_tee
SC_LTTNG_TRACEPOINT_EVENT(tee,
	TP_PROTO(sc_exit(long ret,) int fdin, int fdout, size_t len, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fdin, fdout, len, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fdin, fdin)) sc_in(ctf_integer(int, fdout, fdout)) sc_in(ctf_integer(size_t, len, len)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_mremap
SC_LTTNG_TRACEPOINT_EVENT(mremap,
	TP_PROTO(sc_exit(long ret,) unsigned long addr, unsigned long old_len, unsigned long new_len, unsigned long flags, unsigned long new_addr),
	TP_ARGS(sc_exit(ret,) addr, old_len, new_len, flags, new_addr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, addr, addr)) sc_in(ctf_integer(unsigned long, old_len, old_len)) sc_in(ctf_integer(unsigned long, new_len, new_len)) sc_in(ctf_integer(unsigned long, flags, flags)) sc_in(ctf_integer(unsigned long, new_addr, new_addr)))
)
#endif
#ifndef OVERRIDE_32_prctl
SC_LTTNG_TRACEPOINT_EVENT(prctl,
	TP_PROTO(sc_exit(long ret,) int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5),
	TP_ARGS(sc_exit(ret,) option, arg2, arg3, arg4, arg5),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, option, option)) sc_inout(ctf_integer(unsigned long, arg2, arg2)) sc_in(ctf_integer(unsigned long, arg3, arg3)) sc_in(ctf_integer(unsigned long, arg4, arg4)) sc_in(ctf_integer(unsigned long, arg5, arg5)))
)
#endif
#ifndef OVERRIDE_32_remap_file_pages
SC_LTTNG_TRACEPOINT_EVENT(remap_file_pages,
	TP_PROTO(sc_exit(long ret,) unsigned long start, unsigned long size, unsigned long prot, unsigned long pgoff, unsigned long flags),
	TP_ARGS(sc_exit(ret,) start, size, prot, pgoff, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(unsigned long, size, size)) sc_in(ctf_integer(unsigned long, prot, prot)) sc_in(ctf_integer(unsigned long, pgoff, pgoff)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_keyctl
SC_LTTNG_TRACEPOINT_EVENT(keyctl,
	TP_PROTO(sc_exit(long ret,) int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5),
	TP_ARGS(sc_exit(ret,) option, arg2, arg3, arg4, arg5),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, option, option)) sc_inout(ctf_integer(unsigned long, arg2, arg2)) sc_inout(ctf_integer(unsigned long, arg3, arg3)) sc_inout(ctf_integer(unsigned long, arg4, arg4)) sc_inout(ctf_integer(unsigned long, arg5, arg5)))
)
#endif

#endif /*  _TRACE_SYSCALLS_INTEGERS_H */

/* This part must be outside protection */
#include <probes/define_trace.h>

#else /* CREATE_SYSCALL_TABLE */

#include "arm-32-syscalls-3.4.25_integers_override.h"
#include "syscalls_integers_override.h"

#ifdef SC_ENTER
#ifndef OVERRIDE_TABLE_32_restart_syscall
TRACE_SYSCALL_TABLE(syscalls_noargs, restart_syscall, 0, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getpid
TRACE_SYSCALL_TABLE(syscalls_noargs, getpid, 20, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getuid16
TRACE_SYSCALL_TABLE(syscalls_noargs, getuid16, 24, 0)
#endif
#ifndef OVERRIDE_TABLE_32_pause
TRACE_SYSCALL_TABLE(syscalls_noargs, pause, 29, 0)
#endif
#ifndef OVERRIDE_TABLE_32_sync
TRACE_SYSCALL_TABLE(syscalls_noargs, sync, 36, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getgid16
TRACE_SYSCALL_TABLE(syscalls_noargs, getgid16, 47, 0)
#endif
#ifndef OVERRIDE_TABLE_32_geteuid16
TRACE_SYSCALL_TABLE(syscalls_noargs, geteuid16, 49, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getegid16
TRACE_SYSCALL_TABLE(syscalls_noargs, getegid16, 50, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getppid
TRACE_SYSCALL_TABLE(syscalls_noargs, getppid, 64, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getpgrp
TRACE_SYSCALL_TABLE(syscalls_noargs, getpgrp, 65, 0)
#endif
#ifndef OVERRIDE_TABLE_32_setsid
TRACE_SYSCALL_TABLE(syscalls_noargs, setsid, 66, 0)
#endif
#ifndef OVERRIDE_TABLE_32_vhangup
TRACE_SYSCALL_TABLE(syscalls_noargs, vhangup, 111, 0)
#endif
#ifndef OVERRIDE_TABLE_32_munlockall
TRACE_SYSCALL_TABLE(syscalls_noargs, munlockall, 153, 0)
#endif
#ifndef OVERRIDE_TABLE_32_sched_yield
TRACE_SYSCALL_TABLE(syscalls_noargs, sched_yield, 158, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getuid
TRACE_SYSCALL_TABLE(syscalls_noargs, getuid, 199, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getgid
TRACE_SYSCALL_TABLE(syscalls_noargs, getgid, 200, 0)
#endif
#ifndef OVERRIDE_TABLE_32_geteuid
TRACE_SYSCALL_TABLE(syscalls_noargs, geteuid, 201, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getegid
TRACE_SYSCALL_TABLE(syscalls_noargs, getegid, 202, 0)
#endif
#ifndef OVERRIDE_TABLE_32_gettid
TRACE_SYSCALL_TABLE(syscalls_noargs, gettid, 224, 0)
#endif
#ifndef OVERRIDE_TABLE_32_inotify_init
TRACE_SYSCALL_TABLE(syscalls_noargs, inotify_init, 316, 0)
#endif
#else /* #ifdef SC_ENTER */
#ifndef OVERRIDE_TABLE_32_restart_syscall
TRACE_SYSCALL_TABLE(restart_syscall, restart_syscall, 0, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getpid
TRACE_SYSCALL_TABLE(getpid, getpid, 20, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getuid16
TRACE_SYSCALL_TABLE(getuid16, getuid16, 24, 0)
#endif
#ifndef OVERRIDE_TABLE_32_pause
TRACE_SYSCALL_TABLE(pause, pause, 29, 0)
#endif
#ifndef OVERRIDE_TABLE_32_sync
TRACE_SYSCALL_TABLE(sync, sync, 36, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getgid16
TRACE_SYSCALL_TABLE(getgid16, getgid16, 47, 0)
#endif
#ifndef OVERRIDE_TABLE_32_geteuid16
TRACE_SYSCALL_TABLE(geteuid16, geteuid16, 49, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getegid16
TRACE_SYSCALL_TABLE(getegid16, getegid16, 50, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getppid
TRACE_SYSCALL_TABLE(getppid, getppid, 64, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getpgrp
TRACE_SYSCALL_TABLE(getpgrp, getpgrp, 65, 0)
#endif
#ifndef OVERRIDE_TABLE_32_setsid
TRACE_SYSCALL_TABLE(setsid, setsid, 66, 0)
#endif
#ifndef OVERRIDE_TABLE_32_vhangup
TRACE_SYSCALL_TABLE(vhangup, vhangup, 111, 0)
#endif
#ifndef OVERRIDE_TABLE_32_munlockall
TRACE_SYSCALL_TABLE(munlockall, munlockall, 153, 0)
#endif
#ifndef OVERRIDE_TABLE_32_sched_yield
TRACE_SYSCALL_TABLE(sched_yield, sched_yield, 158, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getuid
TRACE_SYSCALL_TABLE(getuid, getuid, 199, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getgid
TRACE_SYSCALL_TABLE(getgid, getgid, 200, 0)
#endif
#ifndef OVERRIDE_TABLE_32_geteuid
TRACE_SYSCALL_TABLE(geteuid, geteuid, 201, 0)
#endif
#ifndef OVERRIDE_TABLE_32_getegid
TRACE_SYSCALL_TABLE(getegid, getegid, 202, 0)
#endif
#ifndef OVERRIDE_TABLE_32_gettid
TRACE_SYSCALL_TABLE(gettid, gettid, 224, 0)
#endif
#ifndef OVERRIDE_TABLE_32_inotify_init
TRACE_SYSCALL_TABLE(inotify_init, inotify_init, 316, 0)
#endif
#endif /* else #ifdef SC_ENTER */
#ifndef OVERRIDE_TABLE_32_exit
TRACE_SYSCALL_TABLE(exit, exit, 1, 1)
#endif
#ifndef OVERRIDE_TABLE_32_close
TRACE_SYSCALL_TABLE(close, close, 6, 1)
#endif
#ifndef OVERRIDE_TABLE_32_lseek
TRACE_SYSCALL_TABLE(lseek, lseek, 19, 3)
#endif
#ifndef OVERRIDE_TABLE_32_setuid16
TRACE_SYSCALL_TABLE(setuid16, setuid16, 23, 1)
#endif
#ifndef OVERRIDE_TABLE_32_ptrace
TRACE_SYSCALL_TABLE(ptrace, ptrace, 26, 4)
#endif
#ifndef OVERRIDE_TABLE_32_nice
TRACE_SYSCALL_TABLE(nice, nice, 34, 1)
#endif
#ifndef OVERRIDE_TABLE_32_kill
TRACE_SYSCALL_TABLE(kill, kill, 37, 2)
#endif
#ifndef OVERRIDE_TABLE_32_dup
TRACE_SYSCALL_TABLE(dup, dup, 41, 1)
#endif
#ifndef OVERRIDE_TABLE_32_brk
TRACE_SYSCALL_TABLE(brk, brk, 45, 1)
#endif
#ifndef OVERRIDE_TABLE_32_setgid16
TRACE_SYSCALL_TABLE(setgid16, setgid16, 46, 1)
#endif
#ifndef OVERRIDE_TABLE_32_ioctl
TRACE_SYSCALL_TABLE(ioctl, ioctl, 54, 3)
#endif
#ifndef OVERRIDE_TABLE_32_fcntl
TRACE_SYSCALL_TABLE(fcntl, fcntl, 55, 3)
#endif
#ifndef OVERRIDE_TABLE_32_setpgid
TRACE_SYSCALL_TABLE(setpgid, setpgid, 57, 2)
#endif
#ifndef OVERRIDE_TABLE_32_umask
TRACE_SYSCALL_TABLE(umask, umask, 60, 1)
#endif
#ifndef OVERRIDE_TABLE_32_dup2
TRACE_SYSCALL_TABLE(dup2, dup2, 63, 2)
#endif
#ifndef OVERRIDE_TABLE_32_setreuid16
TRACE_SYSCALL_TABLE(setreuid16, setreuid16, 70, 2)
#endif
#ifndef OVERRIDE_TABLE_32_setregid16
TRACE_SYSCALL_TABLE(setregid16, setregid16, 71, 2)
#endif
#ifndef OVERRIDE_TABLE_32_munmap
TRACE_SYSCALL_TABLE(munmap, munmap, 91, 2)
#endif
#ifndef OVERRIDE_TABLE_32_ftruncate
TRACE_SYSCALL_TABLE(ftruncate, ftruncate, 93, 2)
#endif
#ifndef OVERRIDE_TABLE_32_fchmod
TRACE_SYSCALL_TABLE(fchmod, fchmod, 94, 2)
#endif
#ifndef OVERRIDE_TABLE_32_fchown16
TRACE_SYSCALL_TABLE(fchown16, fchown16, 95, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getpriority
TRACE_SYSCALL_TABLE(getpriority, getpriority, 96, 2)
#endif
#ifndef OVERRIDE_TABLE_32_setpriority
TRACE_SYSCALL_TABLE(setpriority, setpriority, 97, 3)
#endif
#ifndef OVERRIDE_TABLE_32_fsync
TRACE_SYSCALL_TABLE(fsync, fsync, 118, 1)
#endif
#ifndef OVERRIDE_TABLE_32_mprotect
TRACE_SYSCALL_TABLE(mprotect, mprotect, 125, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getpgid
TRACE_SYSCALL_TABLE(getpgid, getpgid, 132, 1)
#endif
#ifndef OVERRIDE_TABLE_32_fchdir
TRACE_SYSCALL_TABLE(fchdir, fchdir, 133, 1)
#endif
#ifndef OVERRIDE_TABLE_32_bdflush
TRACE_SYSCALL_TABLE(bdflush, bdflush, 134, 2)
#endif
#ifndef OVERRIDE_TABLE_32_sysfs
TRACE_SYSCALL_TABLE(sysfs, sysfs, 135, 3)
#endif
#ifndef OVERRIDE_TABLE_32_personality
TRACE_SYSCALL_TABLE(personality, personality, 136, 1)
#endif
#ifndef OVERRIDE_TABLE_32_setfsuid16
TRACE_SYSCALL_TABLE(setfsuid16, setfsuid16, 138, 1)
#endif
#ifndef OVERRIDE_TABLE_32_setfsgid16
TRACE_SYSCALL_TABLE(setfsgid16, setfsgid16, 139, 1)
#endif
#ifndef OVERRIDE_TABLE_32_flock
TRACE_SYSCALL_TABLE(flock, flock, 143, 2)
#endif
#ifndef OVERRIDE_TABLE_32_msync
TRACE_SYSCALL_TABLE(msync, msync, 144, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getsid
TRACE_SYSCALL_TABLE(getsid, getsid, 147, 1)
#endif
#ifndef OVERRIDE_TABLE_32_fdatasync
TRACE_SYSCALL_TABLE(fdatasync, fdatasync, 148, 1)
#endif
#ifndef OVERRIDE_TABLE_32_mlock
TRACE_SYSCALL_TABLE(mlock, mlock, 150, 2)
#endif
#ifndef OVERRIDE_TABLE_32_munlock
TRACE_SYSCALL_TABLE(munlock, munlock, 151, 2)
#endif
#ifndef OVERRIDE_TABLE_32_mlockall
TRACE_SYSCALL_TABLE(mlockall, mlockall, 152, 1)
#endif
#ifndef OVERRIDE_TABLE_32_sched_getscheduler
TRACE_SYSCALL_TABLE(sched_getscheduler, sched_getscheduler, 157, 1)
#endif
#ifndef OVERRIDE_TABLE_32_sched_get_priority_max
TRACE_SYSCALL_TABLE(sched_get_priority_max, sched_get_priority_max, 159, 1)
#endif
#ifndef OVERRIDE_TABLE_32_sched_get_priority_min
TRACE_SYSCALL_TABLE(sched_get_priority_min, sched_get_priority_min, 160, 1)
#endif
#ifndef OVERRIDE_TABLE_32_mremap
TRACE_SYSCALL_TABLE(mremap, mremap, 163, 5)
#endif
#ifndef OVERRIDE_TABLE_32_setresuid16
TRACE_SYSCALL_TABLE(setresuid16, setresuid16, 164, 3)
#endif
#ifndef OVERRIDE_TABLE_32_setresgid16
TRACE_SYSCALL_TABLE(setresgid16, setresgid16, 170, 3)
#endif
#ifndef OVERRIDE_TABLE_32_prctl
TRACE_SYSCALL_TABLE(prctl, prctl, 172, 5)
#endif
#ifndef OVERRIDE_TABLE_32_setreuid
TRACE_SYSCALL_TABLE(setreuid, setreuid, 203, 2)
#endif
#ifndef OVERRIDE_TABLE_32_setregid
TRACE_SYSCALL_TABLE(setregid, setregid, 204, 2)
#endif
#ifndef OVERRIDE_TABLE_32_fchown
TRACE_SYSCALL_TABLE(fchown, fchown, 207, 3)
#endif
#ifndef OVERRIDE_TABLE_32_setresuid
TRACE_SYSCALL_TABLE(setresuid, setresuid, 208, 3)
#endif
#ifndef OVERRIDE_TABLE_32_setresgid
TRACE_SYSCALL_TABLE(setresgid, setresgid, 210, 3)
#endif
#ifndef OVERRIDE_TABLE_32_setuid
TRACE_SYSCALL_TABLE(setuid, setuid, 213, 1)
#endif
#ifndef OVERRIDE_TABLE_32_setgid
TRACE_SYSCALL_TABLE(setgid, setgid, 214, 1)
#endif
#ifndef OVERRIDE_TABLE_32_setfsuid
TRACE_SYSCALL_TABLE(setfsuid, setfsuid, 215, 1)
#endif
#ifndef OVERRIDE_TABLE_32_setfsgid
TRACE_SYSCALL_TABLE(setfsgid, setfsgid, 216, 1)
#endif
#ifndef OVERRIDE_TABLE_32_madvise
TRACE_SYSCALL_TABLE(madvise, madvise, 220, 3)
#endif
#ifndef OVERRIDE_TABLE_32_fcntl64
TRACE_SYSCALL_TABLE(fcntl64, fcntl64, 221, 3)
#endif
#ifndef OVERRIDE_TABLE_32_tkill
TRACE_SYSCALL_TABLE(tkill, tkill, 238, 2)
#endif
#ifndef OVERRIDE_TABLE_32_io_destroy
TRACE_SYSCALL_TABLE(io_destroy, io_destroy, 244, 1)
#endif
#ifndef OVERRIDE_TABLE_32_exit_group
TRACE_SYSCALL_TABLE(exit_group, exit_group, 248, 1)
#endif
#ifndef OVERRIDE_TABLE_32_epoll_create
TRACE_SYSCALL_TABLE(epoll_create, epoll_create, 250, 1)
#endif
#ifndef OVERRIDE_TABLE_32_remap_file_pages
TRACE_SYSCALL_TABLE(remap_file_pages, remap_file_pages, 253, 5)
#endif
#ifndef OVERRIDE_TABLE_32_timer_getoverrun
TRACE_SYSCALL_TABLE(timer_getoverrun, timer_getoverrun, 260, 1)
#endif
#ifndef OVERRIDE_TABLE_32_timer_delete
TRACE_SYSCALL_TABLE(timer_delete, timer_delete, 261, 1)
#endif
#ifndef OVERRIDE_TABLE_32_tgkill
TRACE_SYSCALL_TABLE(tgkill, tgkill, 268, 3)
#endif
#ifndef OVERRIDE_TABLE_32_socket
TRACE_SYSCALL_TABLE(socket, socket, 281, 3)
#endif
#ifndef OVERRIDE_TABLE_32_listen
TRACE_SYSCALL_TABLE(listen, listen, 284, 2)
#endif
#ifndef OVERRIDE_TABLE_32_shutdown
TRACE_SYSCALL_TABLE(shutdown, shutdown, 293, 2)
#endif
#ifndef OVERRIDE_TABLE_32_semget
TRACE_SYSCALL_TABLE(semget, semget, 299, 3)
#endif
#ifndef OVERRIDE_TABLE_32_msgget
TRACE_SYSCALL_TABLE(msgget, msgget, 303, 2)
#endif
#ifndef OVERRIDE_TABLE_32_shmget
TRACE_SYSCALL_TABLE(shmget, shmget, 307, 3)
#endif
#ifndef OVERRIDE_TABLE_32_keyctl
TRACE_SYSCALL_TABLE(keyctl, keyctl, 311, 5)
#endif
#ifndef OVERRIDE_TABLE_32_ioprio_set
TRACE_SYSCALL_TABLE(ioprio_set, ioprio_set, 314, 3)
#endif
#ifndef OVERRIDE_TABLE_32_ioprio_get
TRACE_SYSCALL_TABLE(ioprio_get, ioprio_get, 315, 2)
#endif
#ifndef OVERRIDE_TABLE_32_inotify_rm_watch
TRACE_SYSCALL_TABLE(inotify_rm_watch, inotify_rm_watch, 318, 2)
#endif
#ifndef OVERRIDE_TABLE_32_unshare
TRACE_SYSCALL_TABLE(unshare, unshare, 337, 1)
#endif
#ifndef OVERRIDE_TABLE_32_tee
TRACE_SYSCALL_TABLE(tee, tee, 342, 4)
#endif
#ifndef OVERRIDE_TABLE_32_timerfd_create
TRACE_SYSCALL_TABLE(timerfd_create, timerfd_create, 350, 2)
#endif
#ifndef OVERRIDE_TABLE_32_eventfd
TRACE_SYSCALL_TABLE(eventfd, eventfd, 351, 1)
#endif
#ifndef OVERRIDE_TABLE_32_eventfd2
TRACE_SYSCALL_TABLE(eventfd2, eventfd2, 356, 2)
#endif
#ifndef OVERRIDE_TABLE_32_epoll_create1
TRACE_SYSCALL_TABLE(epoll_create1, epoll_create1, 357, 1)
#endif
#ifndef OVERRIDE_TABLE_32_dup3
TRACE_SYSCALL_TABLE(dup3, dup3, 358, 3)
#endif
#ifndef OVERRIDE_TABLE_32_inotify_init1
TRACE_SYSCALL_TABLE(inotify_init1, inotify_init1, 360, 1)
#endif
#ifndef OVERRIDE_TABLE_32_fanotify_init
TRACE_SYSCALL_TABLE(fanotify_init, fanotify_init, 367, 2)
#endif
#ifndef OVERRIDE_TABLE_32_syncfs
TRACE_SYSCALL_TABLE(syncfs, syncfs, 373, 1)
#endif
#ifndef OVERRIDE_TABLE_32_setns
TRACE_SYSCALL_TABLE(setns, setns, 375, 2)
#endif

#endif /* CREATE_SYSCALL_TABLE */
