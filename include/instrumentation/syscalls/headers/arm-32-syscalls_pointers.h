/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only) */

/* THIS FILE IS AUTO-GENERATED. DO NOT EDIT */

/* Generated from arm-32-syscalls 6.0.7 */

#ifndef CREATE_SYSCALL_TABLE

#if !defined(_TRACE_SYSCALLS_POINTERS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SYSCALLS_POINTERS_H

#include <lttng/tracepoint-event.h>
#include <linux/syscalls.h>
#include "arm-32-syscalls_pointers_override.h"
#include "syscalls_pointers_override.h"

#ifndef OVERRIDE_32_unlink
SC_LTTNG_TRACEPOINT_EVENT(unlink,
	TP_PROTO(sc_exit(long ret,) const char * pathname),
	TP_ARGS(sc_exit(ret,) pathname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)))
)
#endif
#ifndef OVERRIDE_32_chdir
SC_LTTNG_TRACEPOINT_EVENT(chdir,
	TP_PROTO(sc_exit(long ret,) const char * filename),
	TP_ARGS(sc_exit(ret,) filename),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)))
)
#endif
#ifndef OVERRIDE_32_rmdir
SC_LTTNG_TRACEPOINT_EVENT(rmdir,
	TP_PROTO(sc_exit(long ret,) const char * pathname),
	TP_ARGS(sc_exit(ret,) pathname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)))
)
#endif
#ifndef OVERRIDE_32_pipe
SC_LTTNG_TRACEPOINT_EVENT(pipe,
	TP_PROTO(sc_exit(long ret,) int * fildes),
	TP_ARGS(sc_exit(ret,) fildes),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(int *, fildes, fildes)))
)
#endif
#ifndef OVERRIDE_32_times
SC_LTTNG_TRACEPOINT_EVENT(times,
	TP_PROTO(sc_exit(long ret,) struct tms * tbuf),
	TP_ARGS(sc_exit(ret,) tbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(struct tms *, tbuf, tbuf)))
)
#endif
#ifndef OVERRIDE_32_acct
SC_LTTNG_TRACEPOINT_EVENT(acct,
	TP_PROTO(sc_exit(long ret,) const char * name),
	TP_ARGS(sc_exit(ret,) name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(name, name)))
)
#endif
#ifndef OVERRIDE_32_chroot
SC_LTTNG_TRACEPOINT_EVENT(chroot,
	TP_PROTO(sc_exit(long ret,) const char * filename),
	TP_ARGS(sc_exit(ret,) filename),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)))
)
#endif
#ifndef OVERRIDE_32_sigpending
SC_LTTNG_TRACEPOINT_EVENT(sigpending,
	TP_PROTO(sc_exit(long ret,) old_sigset_t * uset),
	TP_ARGS(sc_exit(ret,) uset),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_sigset_t *, uset, uset)))
)
#endif
#ifndef OVERRIDE_32_swapoff
SC_LTTNG_TRACEPOINT_EVENT(swapoff,
	TP_PROTO(sc_exit(long ret,) const char * specialfile),
	TP_ARGS(sc_exit(ret,) specialfile),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(specialfile, specialfile)))
)
#endif
#ifndef OVERRIDE_32_sysinfo
SC_LTTNG_TRACEPOINT_EVENT(sysinfo,
	TP_PROTO(sc_exit(long ret,) struct sysinfo * info),
	TP_ARGS(sc_exit(ret,) info),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(struct sysinfo *, info, info)))
)
#endif
#ifndef OVERRIDE_32_newuname
SC_LTTNG_TRACEPOINT_EVENT(newuname,
	TP_PROTO(sc_exit(long ret,) struct new_utsname * name),
	TP_ARGS(sc_exit(ret,) name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(struct new_utsname *, name, name)))
)
#endif
#ifndef OVERRIDE_32_adjtimex_time32
SC_LTTNG_TRACEPOINT_EVENT(adjtimex_time32,
	TP_PROTO(sc_exit(long ret,) struct old_timex32 * utp),
	TP_ARGS(sc_exit(ret,) utp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct old_timex32 *, utp, utp)))
)
#endif
#ifndef OVERRIDE_32_set_tid_address
SC_LTTNG_TRACEPOINT_EVENT(set_tid_address,
	TP_PROTO(sc_exit(long ret,) int * tidptr),
	TP_ARGS(sc_exit(ret,) tidptr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int *, tidptr, tidptr)))
)
#endif
#ifndef OVERRIDE_32_mq_unlink
SC_LTTNG_TRACEPOINT_EVENT(mq_unlink,
	TP_PROTO(sc_exit(long ret,) const char * u_name),
	TP_ARGS(sc_exit(ret,) u_name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(u_name, u_name)))
)
#endif
#ifndef OVERRIDE_32_shmdt
SC_LTTNG_TRACEPOINT_EVENT(shmdt,
	TP_PROTO(sc_exit(long ret,) char * shmaddr),
	TP_ARGS(sc_exit(ret,) shmaddr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(char *, shmaddr, shmaddr)))
)
#endif
#ifndef OVERRIDE_32_creat
SC_LTTNG_TRACEPOINT_EVENT(creat,
	TP_PROTO(sc_exit(long ret,) const char * pathname, umode_t mode),
	TP_ARGS(sc_exit(ret,) pathname, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_link
SC_LTTNG_TRACEPOINT_EVENT(link,
	TP_PROTO(sc_exit(long ret,) const char * oldname, const char * newname),
	TP_ARGS(sc_exit(ret,) oldname, newname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(oldname, oldname)) sc_in(ctf_user_string(newname, newname)))
)
#endif
#ifndef OVERRIDE_32_chmod
SC_LTTNG_TRACEPOINT_EVENT(chmod,
	TP_PROTO(sc_exit(long ret,) const char * filename, umode_t mode),
	TP_ARGS(sc_exit(ret,) filename, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_access
SC_LTTNG_TRACEPOINT_EVENT(access,
	TP_PROTO(sc_exit(long ret,) const char * filename, int mode),
	TP_ARGS(sc_exit(ret,) filename, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(int, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_rename
SC_LTTNG_TRACEPOINT_EVENT(rename,
	TP_PROTO(sc_exit(long ret,) const char * oldname, const char * newname),
	TP_ARGS(sc_exit(ret,) oldname, newname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(oldname, oldname)) sc_in(ctf_user_string(newname, newname)))
)
#endif
#ifndef OVERRIDE_32_mkdir
SC_LTTNG_TRACEPOINT_EVENT(mkdir,
	TP_PROTO(sc_exit(long ret,) const char * pathname, umode_t mode),
	TP_ARGS(sc_exit(ret,) pathname, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_umount
SC_LTTNG_TRACEPOINT_EVENT(umount,
	TP_PROTO(sc_exit(long ret,) char * name, int flags),
	TP_ARGS(sc_exit(ret,) name, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(name, name)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_ustat
SC_LTTNG_TRACEPOINT_EVENT(ustat,
	TP_PROTO(sc_exit(long ret,) unsigned dev, struct ustat * ubuf),
	TP_ARGS(sc_exit(ret,) dev, ubuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned, dev, dev)) sc_out(ctf_integer(struct ustat *, ubuf, ubuf)))
)
#endif
#ifndef OVERRIDE_32_sethostname
SC_LTTNG_TRACEPOINT_EVENT(sethostname,
	TP_PROTO(sc_exit(long ret,) char * name, int len),
	TP_ARGS(sc_exit(ret,) name, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(name, name)) sc_in(ctf_integer(int, len, len)))
)
#endif
#ifndef OVERRIDE_32_setrlimit
SC_LTTNG_TRACEPOINT_EVENT(setrlimit,
	TP_PROTO(sc_exit(long ret,) unsigned int resource, struct rlimit * rlim),
	TP_ARGS(sc_exit(ret,) resource, rlim),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, resource, resource)) sc_in(ctf_integer(struct rlimit *, rlim, rlim)))
)
#endif
#ifndef OVERRIDE_32_getrusage
SC_LTTNG_TRACEPOINT_EVENT(getrusage,
	TP_PROTO(sc_exit(long ret,) int who, struct rusage * ru),
	TP_ARGS(sc_exit(ret,) who, ru),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, who, who)) sc_out(ctf_integer(struct rusage *, ru, ru)))
)
#endif
#ifndef OVERRIDE_32_gettimeofday
SC_LTTNG_TRACEPOINT_EVENT(gettimeofday,
	TP_PROTO(sc_exit(long ret,) struct __kernel_old_timeval * tv, struct timezone * tz),
	TP_ARGS(sc_exit(ret,) tv, tz),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(struct __kernel_old_timeval *, tv, tv)) sc_out(ctf_integer(struct timezone *, tz, tz)))
)
#endif
#ifndef OVERRIDE_32_settimeofday
SC_LTTNG_TRACEPOINT_EVENT(settimeofday,
	TP_PROTO(sc_exit(long ret,) struct __kernel_old_timeval * tv, struct timezone * tz),
	TP_ARGS(sc_exit(ret,) tv, tz),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(struct __kernel_old_timeval *, tv, tv)) sc_in(ctf_integer(struct timezone *, tz, tz)))
)
#endif
#ifndef OVERRIDE_32_getgroups16
SC_LTTNG_TRACEPOINT_EVENT(getgroups16,
	TP_PROTO(sc_exit(long ret,) int gidsetsize, old_gid_t * grouplist),
	TP_ARGS(sc_exit(ret,) gidsetsize, grouplist),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, gidsetsize, gidsetsize)) sc_inout(ctf_integer(old_gid_t *, grouplist, grouplist)))
)
#endif
#ifndef OVERRIDE_32_setgroups16
SC_LTTNG_TRACEPOINT_EVENT(setgroups16,
	TP_PROTO(sc_exit(long ret,) int gidsetsize, old_gid_t * grouplist),
	TP_ARGS(sc_exit(ret,) gidsetsize, grouplist),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, gidsetsize, gidsetsize)) sc_inout(ctf_integer(old_gid_t *, grouplist, grouplist)))
)
#endif
#ifndef OVERRIDE_32_symlink
SC_LTTNG_TRACEPOINT_EVENT(symlink,
	TP_PROTO(sc_exit(long ret,) const char * oldname, const char * newname),
	TP_ARGS(sc_exit(ret,) oldname, newname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(oldname, oldname)) sc_in(ctf_user_string(newname, newname)))
)
#endif
#ifndef OVERRIDE_32_swapon
SC_LTTNG_TRACEPOINT_EVENT(swapon,
	TP_PROTO(sc_exit(long ret,) const char * specialfile, int swap_flags),
	TP_ARGS(sc_exit(ret,) specialfile, swap_flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(specialfile, specialfile)) sc_in(ctf_integer(int, swap_flags, swap_flags)))
)
#endif
#ifndef OVERRIDE_32_truncate
SC_LTTNG_TRACEPOINT_EVENT(truncate,
	TP_PROTO(sc_exit(long ret,) const char * path, long length),
	TP_ARGS(sc_exit(ret,) path, length),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(path, path)) sc_in(ctf_integer(long, length, length)))
)
#endif
#ifndef OVERRIDE_32_statfs
SC_LTTNG_TRACEPOINT_EVENT(statfs,
	TP_PROTO(sc_exit(long ret,) const char * pathname, struct statfs * buf),
	TP_ARGS(sc_exit(ret,) pathname, buf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_out(ctf_integer(struct statfs *, buf, buf)))
)
#endif
#ifndef OVERRIDE_32_fstatfs
SC_LTTNG_TRACEPOINT_EVENT(fstatfs,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct statfs * buf),
	TP_ARGS(sc_exit(ret,) fd, buf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(struct statfs *, buf, buf)))
)
#endif
#ifndef OVERRIDE_32_getitimer
SC_LTTNG_TRACEPOINT_EVENT(getitimer,
	TP_PROTO(sc_exit(long ret,) int which, struct __kernel_old_itimerval * value),
	TP_ARGS(sc_exit(ret,) which, value),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_out(ctf_integer(struct __kernel_old_itimerval *, value, value)))
)
#endif
#ifndef OVERRIDE_32_newstat
SC_LTTNG_TRACEPOINT_EVENT(newstat,
	TP_PROTO(sc_exit(long ret,) const char * filename, struct stat * statbuf),
	TP_ARGS(sc_exit(ret,) filename, statbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_out(ctf_integer(struct stat *, statbuf, statbuf)))
)
#endif
#ifndef OVERRIDE_32_newlstat
SC_LTTNG_TRACEPOINT_EVENT(newlstat,
	TP_PROTO(sc_exit(long ret,) const char * filename, struct stat * statbuf),
	TP_ARGS(sc_exit(ret,) filename, statbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_out(ctf_integer(struct stat *, statbuf, statbuf)))
)
#endif
#ifndef OVERRIDE_32_newfstat
SC_LTTNG_TRACEPOINT_EVENT(newfstat,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct stat * statbuf),
	TP_ARGS(sc_exit(ret,) fd, statbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(struct stat *, statbuf, statbuf)))
)
#endif
#ifndef OVERRIDE_32_setdomainname
SC_LTTNG_TRACEPOINT_EVENT(setdomainname,
	TP_PROTO(sc_exit(long ret,) char * name, int len),
	TP_ARGS(sc_exit(ret,) name, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(name, name)) sc_in(ctf_integer(int, len, len)))
)
#endif
#ifndef OVERRIDE_32_delete_module
SC_LTTNG_TRACEPOINT_EVENT(delete_module,
	TP_PROTO(sc_exit(long ret,) const char * name_user, unsigned int flags),
	TP_ARGS(sc_exit(ret,) name_user, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(name_user, name_user)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_sched_setparam
SC_LTTNG_TRACEPOINT_EVENT(sched_setparam,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct sched_param * param),
	TP_ARGS(sc_exit(ret,) pid, param),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(struct sched_param *, param, param)))
)
#endif
#ifndef OVERRIDE_32_sched_getparam
SC_LTTNG_TRACEPOINT_EVENT(sched_getparam,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct sched_param * param),
	TP_ARGS(sc_exit(ret,) pid, param),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_out(ctf_integer(struct sched_param *, param, param)))
)
#endif
#ifndef OVERRIDE_32_sched_rr_get_interval_time32
SC_LTTNG_TRACEPOINT_EVENT(sched_rr_get_interval_time32,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct old_timespec32 * interval),
	TP_ARGS(sc_exit(ret,) pid, interval),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_out(ctf_integer(struct old_timespec32 *, interval, interval)))
)
#endif
#ifndef OVERRIDE_32_nanosleep_time32
SC_LTTNG_TRACEPOINT_EVENT(nanosleep_time32,
	TP_PROTO(sc_exit(long ret,) struct old_timespec32 * rqtp, struct old_timespec32 * rmtp),
	TP_ARGS(sc_exit(ret,) rqtp, rmtp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(struct old_timespec32 *, rqtp, rqtp)) sc_out(ctf_integer(struct old_timespec32 *, rmtp, rmtp)))
)
#endif
#ifndef OVERRIDE_32_rt_sigpending
SC_LTTNG_TRACEPOINT_EVENT(rt_sigpending,
	TP_PROTO(sc_exit(long ret,) sigset_t * uset, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) uset, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(sigset_t *, uset, uset)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_32_rt_sigsuspend
SC_LTTNG_TRACEPOINT_EVENT(rt_sigsuspend,
	TP_PROTO(sc_exit(long ret,) sigset_t * unewset, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) unewset, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(sigset_t *, unewset, unewset)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_32_getcwd
SC_LTTNG_TRACEPOINT_EVENT(getcwd,
	TP_PROTO(sc_exit(long ret,) char * buf, unsigned long size),
	TP_ARGS(sc_exit(ret,) buf, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(unsigned long, size, size)))
)
#endif
#ifndef OVERRIDE_32_sigaltstack
SC_LTTNG_TRACEPOINT_EVENT(sigaltstack,
	TP_PROTO(sc_exit(long ret,) const stack_t * uss, stack_t * uoss),
	TP_ARGS(sc_exit(ret,) uss, uoss),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const stack_t *, uss, uss)) sc_inout(ctf_integer(stack_t *, uoss, uoss)))
)
#endif
#ifndef OVERRIDE_32_getrlimit
SC_LTTNG_TRACEPOINT_EVENT(getrlimit,
	TP_PROTO(sc_exit(long ret,) unsigned int resource, struct rlimit * rlim),
	TP_ARGS(sc_exit(ret,) resource, rlim),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, resource, resource)) sc_out(ctf_integer(struct rlimit *, rlim, rlim)))
)
#endif
#ifndef OVERRIDE_32_truncate64
SC_LTTNG_TRACEPOINT_EVENT(truncate64,
	TP_PROTO(sc_exit(long ret,) const char * path, loff_t length),
	TP_ARGS(sc_exit(ret,) path, length),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(path, path)) sc_in(ctf_integer(loff_t, length, length)))
)
#endif
#ifndef OVERRIDE_32_stat64
SC_LTTNG_TRACEPOINT_EVENT(stat64,
	TP_PROTO(sc_exit(long ret,) const char * filename, struct stat64 * statbuf),
	TP_ARGS(sc_exit(ret,) filename, statbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(struct stat64 *, statbuf, statbuf)))
)
#endif
#ifndef OVERRIDE_32_lstat64
SC_LTTNG_TRACEPOINT_EVENT(lstat64,
	TP_PROTO(sc_exit(long ret,) const char * filename, struct stat64 * statbuf),
	TP_ARGS(sc_exit(ret,) filename, statbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(struct stat64 *, statbuf, statbuf)))
)
#endif
#ifndef OVERRIDE_32_fstat64
SC_LTTNG_TRACEPOINT_EVENT(fstat64,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, struct stat64 * statbuf),
	TP_ARGS(sc_exit(ret,) fd, statbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned long, fd, fd)) sc_inout(ctf_integer(struct stat64 *, statbuf, statbuf)))
)
#endif
#ifndef OVERRIDE_32_getgroups
SC_LTTNG_TRACEPOINT_EVENT(getgroups,
	TP_PROTO(sc_exit(long ret,) int gidsetsize, gid_t * grouplist),
	TP_ARGS(sc_exit(ret,) gidsetsize, grouplist),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, gidsetsize, gidsetsize)) sc_out(ctf_integer(gid_t *, grouplist, grouplist)))
)
#endif
#ifndef OVERRIDE_32_setgroups
SC_LTTNG_TRACEPOINT_EVENT(setgroups,
	TP_PROTO(sc_exit(long ret,) int gidsetsize, gid_t * grouplist),
	TP_ARGS(sc_exit(ret,) gidsetsize, grouplist),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, gidsetsize, gidsetsize)) sc_in(ctf_integer(gid_t *, grouplist, grouplist)))
)
#endif
#ifndef OVERRIDE_32_pivot_root
SC_LTTNG_TRACEPOINT_EVENT(pivot_root,
	TP_PROTO(sc_exit(long ret,) const char * new_root, const char * put_old),
	TP_ARGS(sc_exit(ret,) new_root, put_old),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(new_root, new_root)) sc_in(ctf_user_string(put_old, put_old)))
)
#endif
#ifndef OVERRIDE_32_removexattr
SC_LTTNG_TRACEPOINT_EVENT(removexattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name),
	TP_ARGS(sc_exit(ret,) pathname, name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_user_string(name, name)))
)
#endif
#ifndef OVERRIDE_32_lremovexattr
SC_LTTNG_TRACEPOINT_EVENT(lremovexattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name),
	TP_ARGS(sc_exit(ret,) pathname, name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_user_string(name, name)))
)
#endif
#ifndef OVERRIDE_32_fremovexattr
SC_LTTNG_TRACEPOINT_EVENT(fremovexattr,
	TP_PROTO(sc_exit(long ret,) int fd, const char * name),
	TP_ARGS(sc_exit(ret,) fd, name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_user_string(name, name)))
)
#endif
#ifndef OVERRIDE_32_io_setup
SC_LTTNG_TRACEPOINT_EVENT(io_setup,
	TP_PROTO(sc_exit(long ret,) unsigned nr_events, aio_context_t * ctxp),
	TP_ARGS(sc_exit(ret,) nr_events, ctxp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned, nr_events, nr_events)) sc_in(ctf_integer(aio_context_t *, ctxp, ctxp)))
)
#endif
#ifndef OVERRIDE_32_timer_gettime32
SC_LTTNG_TRACEPOINT_EVENT(timer_gettime32,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id, struct old_itimerspec32 * setting),
	TP_ARGS(sc_exit(ret,) timer_id, setting),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(timer_t, timer_id, timer_id)) sc_out(ctf_integer(struct old_itimerspec32 *, setting, setting)))
)
#endif
#ifndef OVERRIDE_32_clock_settime32
SC_LTTNG_TRACEPOINT_EVENT(clock_settime32,
	TP_PROTO(sc_exit(long ret,) clockid_t which_clock, struct old_timespec32 * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(clockid_t, which_clock, which_clock)) sc_in(ctf_integer(struct old_timespec32 *, tp, tp)))
)
#endif
#ifndef OVERRIDE_32_clock_gettime32
SC_LTTNG_TRACEPOINT_EVENT(clock_gettime32,
	TP_PROTO(sc_exit(long ret,) clockid_t which_clock, struct old_timespec32 * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(clockid_t, which_clock, which_clock)) sc_out(ctf_integer(struct old_timespec32 *, tp, tp)))
)
#endif
#ifndef OVERRIDE_32_clock_getres_time32
SC_LTTNG_TRACEPOINT_EVENT(clock_getres_time32,
	TP_PROTO(sc_exit(long ret,) clockid_t which_clock, struct old_timespec32 * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(clockid_t, which_clock, which_clock)) sc_out(ctf_integer(struct old_timespec32 *, tp, tp)))
)
#endif
#ifndef OVERRIDE_32_utimes_time32
SC_LTTNG_TRACEPOINT_EVENT(utimes_time32,
	TP_PROTO(sc_exit(long ret,) const char * filename, struct old_timeval32 * t),
	TP_ARGS(sc_exit(ret,) filename, t),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(struct old_timeval32 *, t, t)))
)
#endif
#ifndef OVERRIDE_32_mq_notify
SC_LTTNG_TRACEPOINT_EVENT(mq_notify,
	TP_PROTO(sc_exit(long ret,) mqd_t mqdes, const struct sigevent * u_notification),
	TP_ARGS(sc_exit(ret,) mqdes, u_notification),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(mqd_t, mqdes, mqdes)) sc_in(ctf_integer(const struct sigevent *, u_notification, u_notification)))
)
#endif
#ifndef OVERRIDE_32_set_robust_list
SC_LTTNG_TRACEPOINT_EVENT(set_robust_list,
	TP_PROTO(sc_exit(long ret,) struct robust_list_head * head, size_t len),
	TP_ARGS(sc_exit(ret,) head, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(struct robust_list_head *, head, head)) sc_in(ctf_integer(size_t, len, len)))
)
#endif
#ifndef OVERRIDE_32_timerfd_gettime32
SC_LTTNG_TRACEPOINT_EVENT(timerfd_gettime32,
	TP_PROTO(sc_exit(long ret,) int ufd, struct old_itimerspec32 * otmr),
	TP_ARGS(sc_exit(ret,) ufd, otmr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, ufd, ufd)) sc_out(ctf_integer(struct old_itimerspec32 *, otmr, otmr)))
)
#endif
#ifndef OVERRIDE_32_pipe2
SC_LTTNG_TRACEPOINT_EVENT(pipe2,
	TP_PROTO(sc_exit(long ret,) int * fildes, int flags),
	TP_ARGS(sc_exit(ret,) fildes, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(int *, fildes, fildes)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_clock_adjtime32
SC_LTTNG_TRACEPOINT_EVENT(clock_adjtime32,
	TP_PROTO(sc_exit(long ret,) clockid_t which_clock, struct old_timex32 * utp),
	TP_ARGS(sc_exit(ret,) which_clock, utp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(clockid_t, which_clock, which_clock)) sc_inout(ctf_integer(struct old_timex32 *, utp, utp)))
)
#endif
#ifndef OVERRIDE_32_memfd_create
SC_LTTNG_TRACEPOINT_EVENT(memfd_create,
	TP_PROTO(sc_exit(long ret,) const char * uname, unsigned int flags),
	TP_ARGS(sc_exit(ret,) uname, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_user_string(uname, uname)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_clock_gettime
SC_LTTNG_TRACEPOINT_EVENT(clock_gettime,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct __kernel_timespec * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_out(ctf_integer(struct __kernel_timespec *, tp, tp)))
)
#endif
#ifndef OVERRIDE_32_clock_settime
SC_LTTNG_TRACEPOINT_EVENT(clock_settime,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, const struct __kernel_timespec * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_in(ctf_integer(const struct __kernel_timespec *, tp, tp)))
)
#endif
#ifndef OVERRIDE_32_clock_adjtime
SC_LTTNG_TRACEPOINT_EVENT(clock_adjtime,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct __kernel_timex * utx),
	TP_ARGS(sc_exit(ret,) which_clock, utx),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_inout(ctf_integer(struct __kernel_timex *, utx, utx)))
)
#endif
#ifndef OVERRIDE_32_clock_getres
SC_LTTNG_TRACEPOINT_EVENT(clock_getres,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct __kernel_timespec * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_out(ctf_integer(struct __kernel_timespec *, tp, tp)))
)
#endif
#ifndef OVERRIDE_32_timer_gettime
SC_LTTNG_TRACEPOINT_EVENT(timer_gettime,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id, struct __kernel_itimerspec * setting),
	TP_ARGS(sc_exit(ret,) timer_id, setting),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(timer_t, timer_id, timer_id)) sc_out(ctf_integer(struct __kernel_itimerspec *, setting, setting)))
)
#endif
#ifndef OVERRIDE_32_timerfd_gettime
SC_LTTNG_TRACEPOINT_EVENT(timerfd_gettime,
	TP_PROTO(sc_exit(long ret,) int ufd, struct __kernel_itimerspec * otmr),
	TP_ARGS(sc_exit(ret,) ufd, otmr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, ufd, ufd)) sc_out(ctf_integer(struct __kernel_itimerspec *, otmr, otmr)))
)
#endif
#ifndef OVERRIDE_32_sched_rr_get_interval
SC_LTTNG_TRACEPOINT_EVENT(sched_rr_get_interval,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct __kernel_timespec * interval),
	TP_ARGS(sc_exit(ret,) pid, interval),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_out(ctf_integer(struct __kernel_timespec *, interval, interval)))
)
#endif
#ifndef OVERRIDE_32_io_uring_setup
SC_LTTNG_TRACEPOINT_EVENT(io_uring_setup,
	TP_PROTO(sc_exit(long ret,) u32 entries, struct io_uring_params * params),
	TP_ARGS(sc_exit(ret,) entries, params),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(u32, entries, entries)) sc_inout(ctf_integer(struct io_uring_params *, params, params)))
)
#endif
#ifndef OVERRIDE_32_fsopen
SC_LTTNG_TRACEPOINT_EVENT(fsopen,
	TP_PROTO(sc_exit(long ret,) const char * _fs_name, unsigned int flags),
	TP_ARGS(sc_exit(ret,) _fs_name, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_user_string(_fs_name, _fs_name)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_clone3
SC_LTTNG_TRACEPOINT_EVENT(clone3,
	TP_PROTO(sc_exit(long ret,) struct clone_args * uargs, size_t size),
	TP_ARGS(sc_exit(ret,) uargs, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct clone_args *, uargs, uargs)) sc_inout(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_32_read
SC_LTTNG_TRACEPOINT_EVENT(read,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, char * buf, size_t count),
	TP_ARGS(sc_exit(ret,) fd, buf, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(size_t, count, count)))
)
#endif
#ifndef OVERRIDE_32_write
SC_LTTNG_TRACEPOINT_EVENT(write,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, const char * buf, size_t count),
	TP_ARGS(sc_exit(ret,) fd, buf, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(const char *, buf, buf)) sc_in(ctf_integer(size_t, count, count)))
)
#endif
#ifndef OVERRIDE_32_open
SC_LTTNG_TRACEPOINT_EVENT(open,
	TP_PROTO(sc_exit(long ret,) const char * filename, int flags, umode_t mode),
	TP_ARGS(sc_exit(ret,) filename, flags, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_execve
SC_LTTNG_TRACEPOINT_EVENT(execve,
	TP_PROTO(sc_exit(long ret,) const char * filename, const char *const * argv, const char *const * envp),
	TP_ARGS(sc_exit(ret,) filename, argv, envp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(const char *const *, argv, argv)) sc_inout(ctf_integer(const char *const *, envp, envp)))
)
#endif
#ifndef OVERRIDE_32_mknod
SC_LTTNG_TRACEPOINT_EVENT(mknod,
	TP_PROTO(sc_exit(long ret,) const char * filename, umode_t mode, unsigned dev),
	TP_ARGS(sc_exit(ret,) filename, mode, dev),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(umode_t, mode, mode)) sc_in(ctf_integer(unsigned, dev, dev)))
)
#endif
#ifndef OVERRIDE_32_lchown16
SC_LTTNG_TRACEPOINT_EVENT(lchown16,
	TP_PROTO(sc_exit(long ret,) const char * filename, old_uid_t user, old_gid_t group),
	TP_ARGS(sc_exit(ret,) filename, user, group),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(old_uid_t, user, user)) sc_inout(ctf_integer(old_gid_t, group, group)))
)
#endif
#ifndef OVERRIDE_32_sigaction
SC_LTTNG_TRACEPOINT_EVENT(sigaction,
	TP_PROTO(sc_exit(long ret,) int sig, const struct old_sigaction * act, struct old_sigaction * oact),
	TP_ARGS(sc_exit(ret,) sig, act, oact),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, sig, sig)) sc_inout(ctf_integer(const struct old_sigaction *, act, act)) sc_inout(ctf_integer(struct old_sigaction *, oact, oact)))
)
#endif
#ifndef OVERRIDE_32_readlink
SC_LTTNG_TRACEPOINT_EVENT(readlink,
	TP_PROTO(sc_exit(long ret,) const char * path, char * buf, int bufsiz),
	TP_ARGS(sc_exit(ret,) path, buf, bufsiz),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(path, path)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(int, bufsiz, bufsiz)))
)
#endif
#ifndef OVERRIDE_32_syslog
SC_LTTNG_TRACEPOINT_EVENT(syslog,
	TP_PROTO(sc_exit(long ret,) int type, char * buf, int len),
	TP_ARGS(sc_exit(ret,) type, buf, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, type, type)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(int, len, len)))
)
#endif
#ifndef OVERRIDE_32_setitimer
SC_LTTNG_TRACEPOINT_EVENT(setitimer,
	TP_PROTO(sc_exit(long ret,) int which, struct __kernel_old_itimerval * value, struct __kernel_old_itimerval * ovalue),
	TP_ARGS(sc_exit(ret,) which, value, ovalue),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(struct __kernel_old_itimerval *, value, value)) sc_out(ctf_integer(struct __kernel_old_itimerval *, ovalue, ovalue)))
)
#endif
#ifndef OVERRIDE_32_sigprocmask
SC_LTTNG_TRACEPOINT_EVENT(sigprocmask,
	TP_PROTO(sc_exit(long ret,) int how, old_sigset_t * nset, old_sigset_t * oset),
	TP_ARGS(sc_exit(ret,) how, nset, oset),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, how, how)) sc_inout(ctf_integer(old_sigset_t *, nset, nset)) sc_inout(ctf_integer(old_sigset_t *, oset, oset)))
)
#endif
#ifndef OVERRIDE_32_init_module
SC_LTTNG_TRACEPOINT_EVENT(init_module,
	TP_PROTO(sc_exit(long ret,) void * umod, unsigned long len, const char * uargs),
	TP_ARGS(sc_exit(ret,) umod, len, uargs),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(void *, umod, umod)) sc_in(ctf_integer(unsigned long, len, len)) sc_in(ctf_integer(const char *, uargs, uargs)))
)
#endif
#ifndef OVERRIDE_32_getdents
SC_LTTNG_TRACEPOINT_EVENT(getdents,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct linux_dirent * dirent, unsigned int count),
	TP_ARGS(sc_exit(ret,) fd, dirent, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(struct linux_dirent *, dirent, dirent)) sc_in(ctf_integer(unsigned int, count, count)))
)
#endif
#ifndef OVERRIDE_32_readv
SC_LTTNG_TRACEPOINT_EVENT(readv,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, fd, fd)) sc_inout(ctf_integer(const struct iovec *, vec, vec)) sc_in(ctf_integer(unsigned long, vlen, vlen)))
)
#endif
#ifndef OVERRIDE_32_writev
SC_LTTNG_TRACEPOINT_EVENT(writev,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, fd, fd)) sc_inout(ctf_integer(const struct iovec *, vec, vec)) sc_in(ctf_integer(unsigned long, vlen, vlen)))
)
#endif
#ifndef OVERRIDE_32_sched_setscheduler
SC_LTTNG_TRACEPOINT_EVENT(sched_setscheduler,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int policy, struct sched_param * param),
	TP_ARGS(sc_exit(ret,) pid, policy, param),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, policy, policy)) sc_in(ctf_integer(struct sched_param *, param, param)))
)
#endif
#ifndef OVERRIDE_32_getresuid16
SC_LTTNG_TRACEPOINT_EVENT(getresuid16,
	TP_PROTO(sc_exit(long ret,) old_uid_t * ruidp, old_uid_t * euidp, old_uid_t * suidp),
	TP_ARGS(sc_exit(ret,) ruidp, euidp, suidp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_uid_t *, ruidp, ruidp)) sc_inout(ctf_integer(old_uid_t *, euidp, euidp)) sc_inout(ctf_integer(old_uid_t *, suidp, suidp)))
)
#endif
#ifndef OVERRIDE_32_poll
SC_LTTNG_TRACEPOINT_EVENT(poll,
	TP_PROTO(sc_exit(long ret,) struct pollfd * ufds, unsigned int nfds, int timeout_msecs),
	TP_ARGS(sc_exit(ret,) ufds, nfds, timeout_msecs),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct pollfd *, ufds, ufds)) sc_in(ctf_integer(unsigned int, nfds, nfds)) sc_in(ctf_integer(int, timeout_msecs, timeout_msecs)))
)
#endif
#ifndef OVERRIDE_32_getresgid16
SC_LTTNG_TRACEPOINT_EVENT(getresgid16,
	TP_PROTO(sc_exit(long ret,) old_gid_t * rgidp, old_gid_t * egidp, old_gid_t * sgidp),
	TP_ARGS(sc_exit(ret,) rgidp, egidp, sgidp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_gid_t *, rgidp, rgidp)) sc_inout(ctf_integer(old_gid_t *, egidp, egidp)) sc_inout(ctf_integer(old_gid_t *, sgidp, sgidp)))
)
#endif
#ifndef OVERRIDE_32_rt_sigqueueinfo
SC_LTTNG_TRACEPOINT_EVENT(rt_sigqueueinfo,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int sig, siginfo_t * uinfo),
	TP_ARGS(sc_exit(ret,) pid, sig, uinfo),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, sig, sig)) sc_in(ctf_integer(siginfo_t *, uinfo, uinfo)))
)
#endif
#ifndef OVERRIDE_32_chown16
SC_LTTNG_TRACEPOINT_EVENT(chown16,
	TP_PROTO(sc_exit(long ret,) const char * filename, old_uid_t user, old_gid_t group),
	TP_ARGS(sc_exit(ret,) filename, user, group),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(old_uid_t, user, user)) sc_inout(ctf_integer(old_gid_t, group, group)))
)
#endif
#ifndef OVERRIDE_32_lchown
SC_LTTNG_TRACEPOINT_EVENT(lchown,
	TP_PROTO(sc_exit(long ret,) const char * filename, uid_t user, gid_t group),
	TP_ARGS(sc_exit(ret,) filename, user, group),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(uid_t, user, user)) sc_in(ctf_integer(gid_t, group, group)))
)
#endif
#ifndef OVERRIDE_32_getresuid
SC_LTTNG_TRACEPOINT_EVENT(getresuid,
	TP_PROTO(sc_exit(long ret,) uid_t * ruidp, uid_t * euidp, uid_t * suidp),
	TP_ARGS(sc_exit(ret,) ruidp, euidp, suidp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(uid_t *, ruidp, ruidp)) sc_out(ctf_integer(uid_t *, euidp, euidp)) sc_out(ctf_integer(uid_t *, suidp, suidp)))
)
#endif
#ifndef OVERRIDE_32_getresgid
SC_LTTNG_TRACEPOINT_EVENT(getresgid,
	TP_PROTO(sc_exit(long ret,) gid_t * rgidp, gid_t * egidp, gid_t * sgidp),
	TP_ARGS(sc_exit(ret,) rgidp, egidp, sgidp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(gid_t *, rgidp, rgidp)) sc_out(ctf_integer(gid_t *, egidp, egidp)) sc_out(ctf_integer(gid_t *, sgidp, sgidp)))
)
#endif
#ifndef OVERRIDE_32_chown
SC_LTTNG_TRACEPOINT_EVENT(chown,
	TP_PROTO(sc_exit(long ret,) const char * filename, uid_t user, gid_t group),
	TP_ARGS(sc_exit(ret,) filename, user, group),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(uid_t, user, user)) sc_in(ctf_integer(gid_t, group, group)))
)
#endif
#ifndef OVERRIDE_32_getdents64
SC_LTTNG_TRACEPOINT_EVENT(getdents64,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct linux_dirent64 * dirent, unsigned int count),
	TP_ARGS(sc_exit(ret,) fd, dirent, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(struct linux_dirent64 *, dirent, dirent)) sc_in(ctf_integer(unsigned int, count, count)))
)
#endif
#ifndef OVERRIDE_32_mincore
SC_LTTNG_TRACEPOINT_EVENT(mincore,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, unsigned char * vec),
	TP_ARGS(sc_exit(ret,) start, len, vec),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(size_t, len, len)) sc_out(ctf_integer(unsigned char *, vec, vec)))
)
#endif
#ifndef OVERRIDE_32_listxattr
SC_LTTNG_TRACEPOINT_EVENT(listxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, char * list, size_t size),
	TP_ARGS(sc_exit(ret,) pathname, list, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_out(ctf_integer(char *, list, list)) sc_in(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_32_llistxattr
SC_LTTNG_TRACEPOINT_EVENT(llistxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, char * list, size_t size),
	TP_ARGS(sc_exit(ret,) pathname, list, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_out(ctf_integer(char *, list, list)) sc_in(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_32_flistxattr
SC_LTTNG_TRACEPOINT_EVENT(flistxattr,
	TP_PROTO(sc_exit(long ret,) int fd, char * list, size_t size),
	TP_ARGS(sc_exit(ret,) fd, list, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(char *, list, list)) sc_in(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_32_sched_setaffinity
SC_LTTNG_TRACEPOINT_EVENT(sched_setaffinity,
	TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned int len, unsigned long * user_mask_ptr),
	TP_ARGS(sc_exit(ret,) pid, len, user_mask_ptr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(unsigned int, len, len)) sc_in(ctf_integer(unsigned long *, user_mask_ptr, user_mask_ptr)))
)
#endif
#ifndef OVERRIDE_32_sched_getaffinity
SC_LTTNG_TRACEPOINT_EVENT(sched_getaffinity,
	TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned int len, unsigned long * user_mask_ptr),
	TP_ARGS(sc_exit(ret,) pid, len, user_mask_ptr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(unsigned int, len, len)) sc_out(ctf_integer(unsigned long *, user_mask_ptr, user_mask_ptr)))
)
#endif
#ifndef OVERRIDE_32_io_submit
SC_LTTNG_TRACEPOINT_EVENT(io_submit,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx_id, long nr, struct iocb * * iocbpp),
	TP_ARGS(sc_exit(ret,) ctx_id, nr, iocbpp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(aio_context_t, ctx_id, ctx_id)) sc_in(ctf_integer(long, nr, nr)) sc_in(ctf_integer(struct iocb * *, iocbpp, iocbpp)))
)
#endif
#ifndef OVERRIDE_32_io_cancel
SC_LTTNG_TRACEPOINT_EVENT(io_cancel,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx_id, struct iocb * iocb, struct io_event * result),
	TP_ARGS(sc_exit(ret,) ctx_id, iocb, result),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(aio_context_t, ctx_id, ctx_id)) sc_in(ctf_integer(struct iocb *, iocb, iocb)) sc_out(ctf_integer(struct io_event *, result, result)))
)
#endif
#ifndef OVERRIDE_32_timer_create
SC_LTTNG_TRACEPOINT_EVENT(timer_create,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct sigevent * timer_event_spec, timer_t * created_timer_id),
	TP_ARGS(sc_exit(ret,) which_clock, timer_event_spec, created_timer_id),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_in(ctf_integer(struct sigevent *, timer_event_spec, timer_event_spec)) sc_out(ctf_integer(timer_t *, created_timer_id, created_timer_id)))
)
#endif
#ifndef OVERRIDE_32_statfs64
SC_LTTNG_TRACEPOINT_EVENT(statfs64,
	TP_PROTO(sc_exit(long ret,) const char * pathname, size_t sz, struct statfs64 * buf),
	TP_ARGS(sc_exit(ret,) pathname, sz, buf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_user_string(pathname, pathname)) sc_inout(ctf_integer(size_t, sz, sz)) sc_inout(ctf_integer(struct statfs64 *, buf, buf)))
)
#endif
#ifndef OVERRIDE_32_fstatfs64
SC_LTTNG_TRACEPOINT_EVENT(fstatfs64,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, size_t sz, struct statfs64 * buf),
	TP_ARGS(sc_exit(ret,) fd, sz, buf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, fd, fd)) sc_inout(ctf_integer(size_t, sz, sz)) sc_inout(ctf_integer(struct statfs64 *, buf, buf)))
)
#endif
#ifndef OVERRIDE_32_mq_getsetattr
SC_LTTNG_TRACEPOINT_EVENT(mq_getsetattr,
	TP_PROTO(sc_exit(long ret,) mqd_t mqdes, const struct mq_attr * u_mqstat, struct mq_attr * u_omqstat),
	TP_ARGS(sc_exit(ret,) mqdes, u_mqstat, u_omqstat),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(mqd_t, mqdes, mqdes)) sc_in(ctf_integer(const struct mq_attr *, u_mqstat, u_mqstat)) sc_out(ctf_integer(struct mq_attr *, u_omqstat, u_omqstat)))
)
#endif
#ifndef OVERRIDE_32_bind
SC_LTTNG_TRACEPOINT_EVENT(bind,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * umyaddr, int addrlen),
	TP_ARGS(sc_exit(ret,) fd, umyaddr, addrlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(struct sockaddr *, umyaddr, umyaddr)) sc_in(ctf_integer(int, addrlen, addrlen)))
)
#endif
#ifndef OVERRIDE_32_connect
SC_LTTNG_TRACEPOINT_EVENT(connect,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * uservaddr, int addrlen),
	TP_ARGS(sc_exit(ret,) fd, uservaddr, addrlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(struct sockaddr *, uservaddr, uservaddr)) sc_in(ctf_integer(int, addrlen, addrlen)))
)
#endif
#ifndef OVERRIDE_32_accept
SC_LTTNG_TRACEPOINT_EVENT(accept,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * upeer_sockaddr, int * upeer_addrlen),
	TP_ARGS(sc_exit(ret,) fd, upeer_sockaddr, upeer_addrlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(struct sockaddr *, upeer_sockaddr, upeer_sockaddr)) sc_inout(ctf_integer(int *, upeer_addrlen, upeer_addrlen)))
)
#endif
#ifndef OVERRIDE_32_getsockname
SC_LTTNG_TRACEPOINT_EVENT(getsockname,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * usockaddr, int * usockaddr_len),
	TP_ARGS(sc_exit(ret,) fd, usockaddr, usockaddr_len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(struct sockaddr *, usockaddr, usockaddr)) sc_inout(ctf_integer(int *, usockaddr_len, usockaddr_len)))
)
#endif
#ifndef OVERRIDE_32_getpeername
SC_LTTNG_TRACEPOINT_EVENT(getpeername,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * usockaddr, int * usockaddr_len),
	TP_ARGS(sc_exit(ret,) fd, usockaddr, usockaddr_len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(struct sockaddr *, usockaddr, usockaddr)) sc_inout(ctf_integer(int *, usockaddr_len, usockaddr_len)))
)
#endif
#ifndef OVERRIDE_32_sendmsg
SC_LTTNG_TRACEPOINT_EVENT(sendmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct user_msghdr * msg, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, msg, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(struct user_msghdr *, msg, msg)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_recvmsg
SC_LTTNG_TRACEPOINT_EVENT(recvmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct user_msghdr * msg, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, msg, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_inout(ctf_integer(struct user_msghdr *, msg, msg)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_semop
SC_LTTNG_TRACEPOINT_EVENT(semop,
	TP_PROTO(sc_exit(long ret,) int semid, struct sembuf * tsops, unsigned nsops),
	TP_ARGS(sc_exit(ret,) semid, tsops, nsops),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, semid, semid)) sc_in(ctf_integer(struct sembuf *, tsops, tsops)) sc_in(ctf_integer(unsigned, nsops, nsops)))
)
#endif
#ifndef OVERRIDE_32_old_msgctl
SC_LTTNG_TRACEPOINT_EVENT(old_msgctl,
	TP_PROTO(sc_exit(long ret,) int msqid, int cmd, struct msqid_ds * buf),
	TP_ARGS(sc_exit(ret,) msqid, cmd, buf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, msqid, msqid)) sc_inout(ctf_integer(int, cmd, cmd)) sc_inout(ctf_integer(struct msqid_ds *, buf, buf)))
)
#endif
#ifndef OVERRIDE_32_shmat
SC_LTTNG_TRACEPOINT_EVENT(shmat,
	TP_PROTO(sc_exit(long ret,) int shmid, char * shmaddr, int shmflg),
	TP_ARGS(sc_exit(ret,) shmid, shmaddr, shmflg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, shmid, shmid)) sc_in(ctf_integer(char *, shmaddr, shmaddr)) sc_in(ctf_integer(int, shmflg, shmflg)))
)
#endif
#ifndef OVERRIDE_32_old_shmctl
SC_LTTNG_TRACEPOINT_EVENT(old_shmctl,
	TP_PROTO(sc_exit(long ret,) int shmid, int cmd, struct shmid_ds * buf),
	TP_ARGS(sc_exit(ret,) shmid, cmd, buf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, shmid, shmid)) sc_inout(ctf_integer(int, cmd, cmd)) sc_inout(ctf_integer(struct shmid_ds *, buf, buf)))
)
#endif
#ifndef OVERRIDE_32_inotify_add_watch
SC_LTTNG_TRACEPOINT_EVENT(inotify_add_watch,
	TP_PROTO(sc_exit(long ret,) int fd, const char * pathname, u32 mask),
	TP_ARGS(sc_exit(ret,) fd, pathname, mask),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(u32, mask, mask)))
)
#endif
#ifndef OVERRIDE_32_mkdirat
SC_LTTNG_TRACEPOINT_EVENT(mkdirat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * pathname, umode_t mode),
	TP_ARGS(sc_exit(ret,) dfd, pathname, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_futimesat_time32
SC_LTTNG_TRACEPOINT_EVENT(futimesat_time32,
	TP_PROTO(sc_exit(long ret,) unsigned int dfd, const char * filename, struct old_timeval32 * t),
	TP_ARGS(sc_exit(ret,) dfd, filename, t),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(struct old_timeval32 *, t, t)))
)
#endif
#ifndef OVERRIDE_32_unlinkat
SC_LTTNG_TRACEPOINT_EVENT(unlinkat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * pathname, int flag),
	TP_ARGS(sc_exit(ret,) dfd, pathname, flag),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(int, flag, flag)))
)
#endif
#ifndef OVERRIDE_32_symlinkat
SC_LTTNG_TRACEPOINT_EVENT(symlinkat,
	TP_PROTO(sc_exit(long ret,) const char * oldname, int newdfd, const char * newname),
	TP_ARGS(sc_exit(ret,) oldname, newdfd, newname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(oldname, oldname)) sc_in(ctf_integer(int, newdfd, newdfd)) sc_in(ctf_user_string(newname, newname)))
)
#endif
#ifndef OVERRIDE_32_fchmodat
SC_LTTNG_TRACEPOINT_EVENT(fchmodat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, umode_t mode),
	TP_ARGS(sc_exit(ret,) dfd, filename, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_faccessat
SC_LTTNG_TRACEPOINT_EVENT(faccessat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, int mode),
	TP_ARGS(sc_exit(ret,) dfd, filename, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(int, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_get_robust_list
SC_LTTNG_TRACEPOINT_EVENT(get_robust_list,
	TP_PROTO(sc_exit(long ret,) int pid, struct robust_list_head * * head_ptr, size_t * len_ptr),
	TP_ARGS(sc_exit(ret,) pid, head_ptr, len_ptr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, pid, pid)) sc_out(ctf_integer(struct robust_list_head * *, head_ptr, head_ptr)) sc_out(ctf_integer(size_t *, len_ptr, len_ptr)))
)
#endif
#ifndef OVERRIDE_32_getcpu
SC_LTTNG_TRACEPOINT_EVENT(getcpu,
	TP_PROTO(sc_exit(long ret,) unsigned * cpup, unsigned * nodep, struct getcpu_cache * unused),
	TP_ARGS(sc_exit(ret,) cpup, nodep, unused),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(unsigned *, cpup, cpup)) sc_out(ctf_integer(unsigned *, nodep, nodep)) sc_inout(ctf_integer(struct getcpu_cache *, unused, unused)))
)
#endif
#ifndef OVERRIDE_32_signalfd
SC_LTTNG_TRACEPOINT_EVENT(signalfd,
	TP_PROTO(sc_exit(long ret,) int ufd, sigset_t * user_mask, size_t sizemask),
	TP_ARGS(sc_exit(ret,) ufd, user_mask, sizemask),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, ufd, ufd)) sc_in(ctf_integer(sigset_t *, user_mask, user_mask)) sc_in(ctf_integer(size_t, sizemask, sizemask)))
)
#endif
#ifndef OVERRIDE_32_open_by_handle_at
SC_LTTNG_TRACEPOINT_EVENT(open_by_handle_at,
	TP_PROTO(sc_exit(long ret,) int mountdirfd, struct file_handle * handle, int flags),
	TP_ARGS(sc_exit(ret,) mountdirfd, handle, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, mountdirfd, mountdirfd)) sc_in(ctf_integer(struct file_handle *, handle, handle)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_finit_module
SC_LTTNG_TRACEPOINT_EVENT(finit_module,
	TP_PROTO(sc_exit(long ret,) int fd, const char * uargs, int flags),
	TP_ARGS(sc_exit(ret,) fd, uargs, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(const char *, uargs, uargs)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_sched_setattr
SC_LTTNG_TRACEPOINT_EVENT(sched_setattr,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct sched_attr * uattr, unsigned int flags),
	TP_ARGS(sc_exit(ret,) pid, uattr, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(pid_t, pid, pid)) sc_inout(ctf_integer(struct sched_attr *, uattr, uattr)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_seccomp
SC_LTTNG_TRACEPOINT_EVENT(seccomp,
	TP_PROTO(sc_exit(long ret,) unsigned int op, unsigned int flags, void * uargs),
	TP_ARGS(sc_exit(ret,) op, flags, uargs),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, op, op)) sc_inout(ctf_integer(unsigned int, flags, flags)) sc_inout(ctf_integer(void *, uargs, uargs)))
)
#endif
#ifndef OVERRIDE_32_getrandom
SC_LTTNG_TRACEPOINT_EVENT(getrandom,
	TP_PROTO(sc_exit(long ret,) char * ubuf, size_t len, unsigned int flags),
	TP_ARGS(sc_exit(ret,) ubuf, len, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(char *, ubuf, ubuf)) sc_inout(ctf_integer(size_t, len, len)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_bpf
SC_LTTNG_TRACEPOINT_EVENT(bpf,
	TP_PROTO(sc_exit(long ret,) int cmd, union bpf_attr * uattr, unsigned int size),
	TP_ARGS(sc_exit(ret,) cmd, uattr, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, cmd, cmd)) sc_inout(ctf_integer(union bpf_attr *, uattr, uattr)) sc_inout(ctf_integer(unsigned int, size, size)))
)
#endif
#ifndef OVERRIDE_32_open_tree
SC_LTTNG_TRACEPOINT_EVENT(open_tree,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, unsigned flags),
	TP_ARGS(sc_exit(ret,) dfd, filename, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, dfd, dfd)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(unsigned, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_fspick
SC_LTTNG_TRACEPOINT_EVENT(fspick,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * path, unsigned int flags),
	TP_ARGS(sc_exit(ret,) dfd, path, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, dfd, dfd)) sc_inout(ctf_user_string(path, path)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_landlock_create_ruleset
SC_LTTNG_TRACEPOINT_EVENT(landlock_create_ruleset,
	TP_PROTO(sc_exit(long ret,) const struct landlock_ruleset_attr *const attr, const size_t size, const __u32 flags),
	TP_ARGS(sc_exit(ret,) attr, size, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(const struct landlock_ruleset_attr *const, attr, attr)) sc_inout(ctf_integer(const size_t, size, size)) sc_inout(ctf_integer(const __u32, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_reboot
SC_LTTNG_TRACEPOINT_EVENT(reboot,
	TP_PROTO(sc_exit(long ret,) int magic1, int magic2, unsigned int cmd, void * arg),
	TP_ARGS(sc_exit(ret,) magic1, magic2, cmd, arg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, magic1, magic1)) sc_in(ctf_integer(int, magic2, magic2)) sc_in(ctf_integer(unsigned int, cmd, cmd)) sc_in(ctf_integer(void *, arg, arg)))
)
#endif
#ifndef OVERRIDE_32_wait4
SC_LTTNG_TRACEPOINT_EVENT(wait4,
	TP_PROTO(sc_exit(long ret,) pid_t upid, int * stat_addr, int options, struct rusage * ru),
	TP_ARGS(sc_exit(ret,) upid, stat_addr, options, ru),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, upid, upid)) sc_out(ctf_integer(int *, stat_addr, stat_addr)) sc_in(ctf_integer(int, options, options)) sc_out(ctf_integer(struct rusage *, ru, ru)))
)
#endif
#ifndef OVERRIDE_32_quotactl
SC_LTTNG_TRACEPOINT_EVENT(quotactl,
	TP_PROTO(sc_exit(long ret,) unsigned int cmd, const char * special, qid_t id, void * addr),
	TP_ARGS(sc_exit(ret,) cmd, special, id, addr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, cmd, cmd)) sc_in(ctf_integer(const char *, special, special)) sc_in(ctf_integer(qid_t, id, id)) sc_inout(ctf_integer(void *, addr, addr)))
)
#endif
#ifndef OVERRIDE_32_rt_sigaction
SC_LTTNG_TRACEPOINT_EVENT(rt_sigaction,
	TP_PROTO(sc_exit(long ret,) int sig, const struct sigaction * act, struct sigaction * oact, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) sig, act, oact, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, sig, sig)) sc_in(ctf_integer(const struct sigaction *, act, act)) sc_out(ctf_integer(struct sigaction *, oact, oact)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_32_rt_sigprocmask
SC_LTTNG_TRACEPOINT_EVENT(rt_sigprocmask,
	TP_PROTO(sc_exit(long ret,) int how, sigset_t * nset, sigset_t * oset, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) how, nset, oset, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, how, how)) sc_in(ctf_integer(sigset_t *, nset, nset)) sc_out(ctf_integer(sigset_t *, oset, oset)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_32_rt_sigtimedwait_time32
SC_LTTNG_TRACEPOINT_EVENT(rt_sigtimedwait_time32,
	TP_PROTO(sc_exit(long ret,) const sigset_t * uthese, siginfo_t * uinfo, const struct old_timespec32 * uts, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) uthese, uinfo, uts, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(const sigset_t *, uthese, uthese)) sc_out(ctf_integer(siginfo_t *, uinfo, uinfo)) sc_in(ctf_integer(const struct old_timespec32 *, uts, uts)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_32_pread64
SC_LTTNG_TRACEPOINT_EVENT(pread64,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, char * buf, size_t count, loff_t pos),
	TP_ARGS(sc_exit(ret,) fd, buf, count, pos),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(size_t, count, count)) sc_in(ctf_integer(loff_t, pos, pos)))
)
#endif
#ifndef OVERRIDE_32_pwrite64
SC_LTTNG_TRACEPOINT_EVENT(pwrite64,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, const char * buf, size_t count, loff_t pos),
	TP_ARGS(sc_exit(ret,) fd, buf, count, pos),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(const char *, buf, buf)) sc_in(ctf_integer(size_t, count, count)) sc_in(ctf_integer(loff_t, pos, pos)))
)
#endif
#ifndef OVERRIDE_32_sendfile
SC_LTTNG_TRACEPOINT_EVENT(sendfile,
	TP_PROTO(sc_exit(long ret,) int out_fd, int in_fd, off_t * offset, size_t count),
	TP_ARGS(sc_exit(ret,) out_fd, in_fd, offset, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, out_fd, out_fd)) sc_inout(ctf_integer(int, in_fd, in_fd)) sc_inout(ctf_integer(off_t *, offset, offset)) sc_inout(ctf_integer(size_t, count, count)))
)
#endif
#ifndef OVERRIDE_32_getxattr
SC_LTTNG_TRACEPOINT_EVENT(getxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name, void * value, size_t size),
	TP_ARGS(sc_exit(ret,) pathname, name, value, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_user_string(name, name)) sc_out(ctf_integer(void *, value, value)) sc_in(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_32_lgetxattr
SC_LTTNG_TRACEPOINT_EVENT(lgetxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name, void * value, size_t size),
	TP_ARGS(sc_exit(ret,) pathname, name, value, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_user_string(name, name)) sc_out(ctf_integer(void *, value, value)) sc_in(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_32_fgetxattr
SC_LTTNG_TRACEPOINT_EVENT(fgetxattr,
	TP_PROTO(sc_exit(long ret,) int fd, const char * name, void * value, size_t size),
	TP_ARGS(sc_exit(ret,) fd, name, value, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_user_string(name, name)) sc_out(ctf_integer(void *, value, value)) sc_in(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_32_sendfile64
SC_LTTNG_TRACEPOINT_EVENT(sendfile64,
	TP_PROTO(sc_exit(long ret,) int out_fd, int in_fd, loff_t * offset, size_t count),
	TP_ARGS(sc_exit(ret,) out_fd, in_fd, offset, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, out_fd, out_fd)) sc_in(ctf_integer(int, in_fd, in_fd)) sc_inout(ctf_integer(loff_t *, offset, offset)) sc_in(ctf_integer(size_t, count, count)))
)
#endif
#ifndef OVERRIDE_32_epoll_ctl
SC_LTTNG_TRACEPOINT_EVENT(epoll_ctl,
	TP_PROTO(sc_exit(long ret,) int epfd, int op, int fd, struct epoll_event * event),
	TP_ARGS(sc_exit(ret,) epfd, op, fd, event),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, epfd, epfd)) sc_in(ctf_integer(int, op, op)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(struct epoll_event *, event, event)))
)
#endif
#ifndef OVERRIDE_32_epoll_wait
SC_LTTNG_TRACEPOINT_EVENT(epoll_wait,
	TP_PROTO(sc_exit(long ret,) int epfd, struct epoll_event * events, int maxevents, int timeout),
	TP_ARGS(sc_exit(ret,) epfd, events, maxevents, timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, epfd, epfd)) sc_out(ctf_integer(struct epoll_event *, events, events)) sc_in(ctf_integer(int, maxevents, maxevents)) sc_in(ctf_integer(int, timeout, timeout)))
)
#endif
#ifndef OVERRIDE_32_timer_settime32
SC_LTTNG_TRACEPOINT_EVENT(timer_settime32,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id, int flags, struct old_itimerspec32 * new, struct old_itimerspec32 * old),
	TP_ARGS(sc_exit(ret,) timer_id, flags, new, old),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(timer_t, timer_id, timer_id)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(struct old_itimerspec32 *, new, new)) sc_out(ctf_integer(struct old_itimerspec32 *, old, old)))
)
#endif
#ifndef OVERRIDE_32_clock_nanosleep_time32
SC_LTTNG_TRACEPOINT_EVENT(clock_nanosleep_time32,
	TP_PROTO(sc_exit(long ret,) clockid_t which_clock, int flags, struct old_timespec32 * rqtp, struct old_timespec32 * rmtp),
	TP_ARGS(sc_exit(ret,) which_clock, flags, rqtp, rmtp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(clockid_t, which_clock, which_clock)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(struct old_timespec32 *, rqtp, rqtp)) sc_out(ctf_integer(struct old_timespec32 *, rmtp, rmtp)))
)
#endif
#ifndef OVERRIDE_32_mq_open
SC_LTTNG_TRACEPOINT_EVENT(mq_open,
	TP_PROTO(sc_exit(long ret,) const char * u_name, int oflag, umode_t mode, struct mq_attr * u_attr),
	TP_ARGS(sc_exit(ret,) u_name, oflag, mode, u_attr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(u_name, u_name)) sc_in(ctf_integer(int, oflag, oflag)) sc_in(ctf_integer(umode_t, mode, mode)) sc_in(ctf_integer(struct mq_attr *, u_attr, u_attr)))
)
#endif
#ifndef OVERRIDE_32_socketpair
SC_LTTNG_TRACEPOINT_EVENT(socketpair,
	TP_PROTO(sc_exit(long ret,) int family, int type, int protocol, int * usockvec),
	TP_ARGS(sc_exit(ret,) family, type, protocol, usockvec),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, family, family)) sc_in(ctf_integer(int, type, type)) sc_in(ctf_integer(int, protocol, protocol)) sc_out(ctf_integer(int *, usockvec, usockvec)))
)
#endif
#ifndef OVERRIDE_32_send
SC_LTTNG_TRACEPOINT_EVENT(send,
	TP_PROTO(sc_exit(long ret,) int fd, void * buff, size_t len, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, buff, len, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, fd, fd)) sc_inout(ctf_integer(void *, buff, buff)) sc_inout(ctf_integer(size_t, len, len)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_recv
SC_LTTNG_TRACEPOINT_EVENT(recv,
	TP_PROTO(sc_exit(long ret,) int fd, void * ubuf, size_t size, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, ubuf, size, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, fd, fd)) sc_inout(ctf_integer(void *, ubuf, ubuf)) sc_inout(ctf_integer(size_t, size, size)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_msgsnd
SC_LTTNG_TRACEPOINT_EVENT(msgsnd,
	TP_PROTO(sc_exit(long ret,) int msqid, struct msgbuf * msgp, size_t msgsz, int msgflg),
	TP_ARGS(sc_exit(ret,) msqid, msgp, msgsz, msgflg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, msqid, msqid)) sc_in(ctf_integer(struct msgbuf *, msgp, msgp)) sc_in(ctf_integer(size_t, msgsz, msgsz)) sc_in(ctf_integer(int, msgflg, msgflg)))
)
#endif
#ifndef OVERRIDE_32_request_key
SC_LTTNG_TRACEPOINT_EVENT(request_key,
	TP_PROTO(sc_exit(long ret,) const char * _type, const char * _description, const char * _callout_info, key_serial_t destringid),
	TP_ARGS(sc_exit(ret,) _type, _description, _callout_info, destringid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(_type, _type)) sc_in(ctf_integer(const char *, _description, _description)) sc_in(ctf_integer(const char *, _callout_info, _callout_info)) sc_in(ctf_integer(key_serial_t, destringid, destringid)))
)
#endif
#ifndef OVERRIDE_32_semtimedop_time32
SC_LTTNG_TRACEPOINT_EVENT(semtimedop_time32,
	TP_PROTO(sc_exit(long ret,) int semid, struct sembuf * tsems, unsigned int nsops, const struct old_timespec32 * timeout),
	TP_ARGS(sc_exit(ret,) semid, tsems, nsops, timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, semid, semid)) sc_inout(ctf_integer(struct sembuf *, tsems, tsems)) sc_inout(ctf_integer(unsigned int, nsops, nsops)) sc_inout(ctf_integer(const struct old_timespec32 *, timeout, timeout)))
)
#endif
#ifndef OVERRIDE_32_openat
SC_LTTNG_TRACEPOINT_EVENT(openat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, int flags, umode_t mode),
	TP_ARGS(sc_exit(ret,) dfd, filename, flags, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_mknodat
SC_LTTNG_TRACEPOINT_EVENT(mknodat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, umode_t mode, unsigned int dev),
	TP_ARGS(sc_exit(ret,) dfd, filename, mode, dev),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(umode_t, mode, mode)) sc_in(ctf_integer(unsigned int, dev, dev)))
)
#endif
#ifndef OVERRIDE_32_fstatat64
SC_LTTNG_TRACEPOINT_EVENT(fstatat64,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, struct stat64 * statbuf, int flag),
	TP_ARGS(sc_exit(ret,) dfd, filename, statbuf, flag),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, dfd, dfd)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(struct stat64 *, statbuf, statbuf)) sc_inout(ctf_integer(int, flag, flag)))
)
#endif
#ifndef OVERRIDE_32_renameat
SC_LTTNG_TRACEPOINT_EVENT(renameat,
	TP_PROTO(sc_exit(long ret,) int olddfd, const char * oldname, int newdfd, const char * newname),
	TP_ARGS(sc_exit(ret,) olddfd, oldname, newdfd, newname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, olddfd, olddfd)) sc_in(ctf_user_string(oldname, oldname)) sc_in(ctf_integer(int, newdfd, newdfd)) sc_in(ctf_user_string(newname, newname)))
)
#endif
#ifndef OVERRIDE_32_readlinkat
SC_LTTNG_TRACEPOINT_EVENT(readlinkat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * pathname, char * buf, int bufsiz),
	TP_ARGS(sc_exit(ret,) dfd, pathname, buf, bufsiz),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(pathname, pathname)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(int, bufsiz, bufsiz)))
)
#endif
#ifndef OVERRIDE_32_vmsplice
SC_LTTNG_TRACEPOINT_EVENT(vmsplice,
	TP_PROTO(sc_exit(long ret,) int fd, const struct iovec * uiov, unsigned long nr_segs, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, uiov, nr_segs, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(const struct iovec *, uiov, uiov)) sc_in(ctf_integer(unsigned long, nr_segs, nr_segs)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_kexec_load
SC_LTTNG_TRACEPOINT_EVENT(kexec_load,
	TP_PROTO(sc_exit(long ret,) unsigned long entry, unsigned long nr_segments, struct kexec_segment * segments, unsigned long flags),
	TP_ARGS(sc_exit(ret,) entry, nr_segments, segments, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, entry, entry)) sc_in(ctf_integer(unsigned long, nr_segments, nr_segments)) sc_in(ctf_integer(struct kexec_segment *, segments, segments)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_utimensat_time32
SC_LTTNG_TRACEPOINT_EVENT(utimensat_time32,
	TP_PROTO(sc_exit(long ret,) unsigned int dfd, const char * filename, struct old_timespec32 * t, int flags),
	TP_ARGS(sc_exit(ret,) dfd, filename, t, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(struct old_timespec32 *, t, t)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_timerfd_settime32
SC_LTTNG_TRACEPOINT_EVENT(timerfd_settime32,
	TP_PROTO(sc_exit(long ret,) int ufd, int flags, const struct old_itimerspec32 * utmr, struct old_itimerspec32 * otmr),
	TP_ARGS(sc_exit(ret,) ufd, flags, utmr, otmr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, ufd, ufd)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(const struct old_itimerspec32 *, utmr, utmr)) sc_out(ctf_integer(struct old_itimerspec32 *, otmr, otmr)))
)
#endif
#ifndef OVERRIDE_32_signalfd4
SC_LTTNG_TRACEPOINT_EVENT(signalfd4,
	TP_PROTO(sc_exit(long ret,) int ufd, sigset_t * user_mask, size_t sizemask, int flags),
	TP_ARGS(sc_exit(ret,) ufd, user_mask, sizemask, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, ufd, ufd)) sc_in(ctf_integer(sigset_t *, user_mask, user_mask)) sc_in(ctf_integer(size_t, sizemask, sizemask)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_rt_tgsigqueueinfo
SC_LTTNG_TRACEPOINT_EVENT(rt_tgsigqueueinfo,
	TP_PROTO(sc_exit(long ret,) pid_t tgid, pid_t pid, int sig, siginfo_t * uinfo),
	TP_ARGS(sc_exit(ret,) tgid, pid, sig, uinfo),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, tgid, tgid)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, sig, sig)) sc_in(ctf_integer(siginfo_t *, uinfo, uinfo)))
)
#endif
#ifndef OVERRIDE_32_accept4
SC_LTTNG_TRACEPOINT_EVENT(accept4,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * upeer_sockaddr, int * upeer_addrlen, int flags),
	TP_ARGS(sc_exit(ret,) fd, upeer_sockaddr, upeer_addrlen, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(struct sockaddr *, upeer_sockaddr, upeer_sockaddr)) sc_inout(ctf_integer(int *, upeer_addrlen, upeer_addrlen)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_prlimit64
SC_LTTNG_TRACEPOINT_EVENT(prlimit64,
	TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned int resource, const struct rlimit64 * new_rlim, struct rlimit64 * old_rlim),
	TP_ARGS(sc_exit(ret,) pid, resource, new_rlim, old_rlim),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(unsigned int, resource, resource)) sc_in(ctf_integer(const struct rlimit64 *, new_rlim, new_rlim)) sc_out(ctf_integer(struct rlimit64 *, old_rlim, old_rlim)))
)
#endif
#ifndef OVERRIDE_32_sendmmsg
SC_LTTNG_TRACEPOINT_EVENT(sendmmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct mmsghdr * mmsg, unsigned int vlen, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, mmsg, vlen, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(struct mmsghdr *, mmsg, mmsg)) sc_in(ctf_integer(unsigned int, vlen, vlen)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_sched_getattr
SC_LTTNG_TRACEPOINT_EVENT(sched_getattr,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct sched_attr * uattr, unsigned int usize, unsigned int flags),
	TP_ARGS(sc_exit(ret,) pid, uattr, usize, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(pid_t, pid, pid)) sc_inout(ctf_integer(struct sched_attr *, uattr, uattr)) sc_inout(ctf_integer(unsigned int, usize, usize)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_rseq
SC_LTTNG_TRACEPOINT_EVENT(rseq,
	TP_PROTO(sc_exit(long ret,) struct rseq * rseq, u32 rseq_len, int flags, u32 sig),
	TP_ARGS(sc_exit(ret,) rseq, rseq_len, flags, sig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct rseq *, rseq, rseq)) sc_inout(ctf_integer(u32, rseq_len, rseq_len)) sc_inout(ctf_integer(int, flags, flags)) sc_inout(ctf_integer(u32, sig, sig)))
)
#endif
#ifndef OVERRIDE_32_clock_nanosleep
SC_LTTNG_TRACEPOINT_EVENT(clock_nanosleep,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, int flags, const struct __kernel_timespec * rqtp, struct __kernel_timespec * rmtp),
	TP_ARGS(sc_exit(ret,) which_clock, flags, rqtp, rmtp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(const struct __kernel_timespec *, rqtp, rqtp)) sc_out(ctf_integer(struct __kernel_timespec *, rmtp, rmtp)))
)
#endif
#ifndef OVERRIDE_32_timer_settime
SC_LTTNG_TRACEPOINT_EVENT(timer_settime,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id, int flags, const struct __kernel_itimerspec * new_setting, struct __kernel_itimerspec * old_setting),
	TP_ARGS(sc_exit(ret,) timer_id, flags, new_setting, old_setting),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(timer_t, timer_id, timer_id)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(const struct __kernel_itimerspec *, new_setting, new_setting)) sc_out(ctf_integer(struct __kernel_itimerspec *, old_setting, old_setting)))
)
#endif
#ifndef OVERRIDE_32_timerfd_settime
SC_LTTNG_TRACEPOINT_EVENT(timerfd_settime,
	TP_PROTO(sc_exit(long ret,) int ufd, int flags, const struct __kernel_itimerspec * utmr, struct __kernel_itimerspec * otmr),
	TP_ARGS(sc_exit(ret,) ufd, flags, utmr, otmr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, ufd, ufd)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(const struct __kernel_itimerspec *, utmr, utmr)) sc_out(ctf_integer(struct __kernel_itimerspec *, otmr, otmr)))
)
#endif
#ifndef OVERRIDE_32_utimensat
SC_LTTNG_TRACEPOINT_EVENT(utimensat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, struct __kernel_timespec * utimes, int flags),
	TP_ARGS(sc_exit(ret,) dfd, filename, utimes, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(struct __kernel_timespec *, utimes, utimes)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_semtimedop
SC_LTTNG_TRACEPOINT_EVENT(semtimedop,
	TP_PROTO(sc_exit(long ret,) int semid, struct sembuf * tsops, unsigned int nsops, const struct __kernel_timespec * timeout),
	TP_ARGS(sc_exit(ret,) semid, tsops, nsops, timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, semid, semid)) sc_in(ctf_integer(struct sembuf *, tsops, tsops)) sc_in(ctf_integer(unsigned int, nsops, nsops)) sc_inout(ctf_integer(const struct __kernel_timespec *, timeout, timeout)))
)
#endif
#ifndef OVERRIDE_32_rt_sigtimedwait
SC_LTTNG_TRACEPOINT_EVENT(rt_sigtimedwait,
	TP_PROTO(sc_exit(long ret,) const sigset_t * uthese, siginfo_t * uinfo, const struct __kernel_timespec * uts, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) uthese, uinfo, uts, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(const sigset_t *, uthese, uthese)) sc_out(ctf_integer(siginfo_t *, uinfo, uinfo)) sc_in(ctf_integer(const struct __kernel_timespec *, uts, uts)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_32_pidfd_send_signal
SC_LTTNG_TRACEPOINT_EVENT(pidfd_send_signal,
	TP_PROTO(sc_exit(long ret,) int pidfd, int sig, siginfo_t * info, unsigned int flags),
	TP_ARGS(sc_exit(ret,) pidfd, sig, info, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, pidfd, pidfd)) sc_inout(ctf_integer(int, sig, sig)) sc_inout(ctf_integer(siginfo_t *, info, info)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_io_uring_register
SC_LTTNG_TRACEPOINT_EVENT(io_uring_register,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int opcode, void * arg, unsigned int nr_args),
	TP_ARGS(sc_exit(ret,) fd, opcode, arg, nr_args),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, fd, fd)) sc_inout(ctf_integer(unsigned int, opcode, opcode)) sc_inout(ctf_integer(void *, arg, arg)) sc_inout(ctf_integer(unsigned int, nr_args, nr_args)))
)
#endif
#ifndef OVERRIDE_32_openat2
SC_LTTNG_TRACEPOINT_EVENT(openat2,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, struct open_how * how, size_t usize),
	TP_ARGS(sc_exit(ret,) dfd, filename, how, usize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, dfd, dfd)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(struct open_how *, how, how)) sc_inout(ctf_integer(size_t, usize, usize)))
)
#endif
#ifndef OVERRIDE_32_faccessat2
SC_LTTNG_TRACEPOINT_EVENT(faccessat2,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, int mode, int flags),
	TP_ARGS(sc_exit(ret,) dfd, filename, mode, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, dfd, dfd)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(int, mode, mode)) sc_inout(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_quotactl_fd
SC_LTTNG_TRACEPOINT_EVENT(quotactl_fd,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd, qid_t id, void * addr),
	TP_ARGS(sc_exit(ret,) fd, cmd, id, addr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, fd, fd)) sc_inout(ctf_integer(unsigned int, cmd, cmd)) sc_inout(ctf_integer(qid_t, id, id)) sc_inout(ctf_integer(void *, addr, addr)))
)
#endif
#ifndef OVERRIDE_32_landlock_add_rule
SC_LTTNG_TRACEPOINT_EVENT(landlock_add_rule,
	TP_PROTO(sc_exit(long ret,) const int ruleset_fd, const enum landlock_rule_type rule_type, const void *const rule_attr, const __u32 flags),
	TP_ARGS(sc_exit(ret,) ruleset_fd, rule_type, rule_attr, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(const int, ruleset_fd, ruleset_fd)) sc_inout(ctf_integer(const enum landlock_rule_type, rule_type, rule_type)) sc_inout(ctf_integer(const void *const, rule_attr, rule_attr)) sc_inout(ctf_integer(const __u32, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_mount
SC_LTTNG_TRACEPOINT_EVENT(mount,
	TP_PROTO(sc_exit(long ret,) char * dev_name, char * dir_name, char * type, unsigned long flags, void * data),
	TP_ARGS(sc_exit(ret,) dev_name, dir_name, type, flags, data),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(dev_name, dev_name)) sc_in(ctf_user_string(dir_name, dir_name)) sc_in(ctf_user_string(type, type)) sc_in(ctf_integer(unsigned long, flags, flags)) sc_in(ctf_integer(void *, data, data)))
)
#endif
#ifndef OVERRIDE_32_clone
SC_LTTNG_TRACEPOINT_EVENT(clone,
	TP_PROTO(sc_exit(long ret,) unsigned long clone_flags, unsigned long newsp, int * parent_tidptr, unsigned long tls, int * child_tidptr),
	TP_ARGS(sc_exit(ret,) clone_flags, newsp, parent_tidptr, tls, child_tidptr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned long, clone_flags, clone_flags)) sc_inout(ctf_integer(unsigned long, newsp, newsp)) sc_inout(ctf_integer(int *, parent_tidptr, parent_tidptr)) sc_inout(ctf_integer(unsigned long, tls, tls)) sc_inout(ctf_integer(int *, child_tidptr, child_tidptr)))
)
#endif
#ifndef OVERRIDE_32_llseek
SC_LTTNG_TRACEPOINT_EVENT(llseek,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned long offset_high, unsigned long offset_low, loff_t * result, unsigned int whence),
	TP_ARGS(sc_exit(ret,) fd, offset_high, offset_low, result, whence),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, fd, fd)) sc_inout(ctf_integer(unsigned long, offset_high, offset_high)) sc_inout(ctf_integer(unsigned long, offset_low, offset_low)) sc_inout(ctf_integer(loff_t *, result, result)) sc_inout(ctf_integer(unsigned int, whence, whence)))
)
#endif
#ifndef OVERRIDE_32_select
SC_LTTNG_TRACEPOINT_EVENT(select,
	TP_PROTO(sc_exit(long ret,) int n, fd_set * inp, fd_set * outp, fd_set * exp, struct __kernel_old_timeval * tvp),
	TP_ARGS(sc_exit(ret,) n, inp, outp, exp, tvp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, n, n)) sc_inout(ctf_integer(fd_set *, inp, inp)) sc_inout(ctf_integer(fd_set *, outp, outp)) sc_inout(ctf_integer(fd_set *, exp, exp)) sc_inout(ctf_integer(struct __kernel_old_timeval *, tvp, tvp)))
)
#endif
#ifndef OVERRIDE_32_setxattr
SC_LTTNG_TRACEPOINT_EVENT(setxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name, const void * value, size_t size, int flags),
	TP_ARGS(sc_exit(ret,) pathname, name, value, size, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_user_string(name, name)) sc_in(ctf_integer(const void *, value, value)) sc_in(ctf_integer(size_t, size, size)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_lsetxattr
SC_LTTNG_TRACEPOINT_EVENT(lsetxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name, const void * value, size_t size, int flags),
	TP_ARGS(sc_exit(ret,) pathname, name, value, size, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_user_string(name, name)) sc_in(ctf_integer(const void *, value, value)) sc_in(ctf_integer(size_t, size, size)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_fsetxattr
SC_LTTNG_TRACEPOINT_EVENT(fsetxattr,
	TP_PROTO(sc_exit(long ret,) int fd, const char * name, const void * value, size_t size, int flags),
	TP_ARGS(sc_exit(ret,) fd, name, value, size, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_user_string(name, name)) sc_in(ctf_integer(const void *, value, value)) sc_in(ctf_integer(size_t, size, size)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_io_getevents_time32
SC_LTTNG_TRACEPOINT_EVENT(io_getevents_time32,
	TP_PROTO(sc_exit(long ret,) __u32 ctx_id, __s32 min_nr, __s32 nr, struct io_event * events, struct old_timespec32 * timeout),
	TP_ARGS(sc_exit(ret,) ctx_id, min_nr, nr, events, timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(__u32, ctx_id, ctx_id)) sc_in(ctf_integer(__s32, min_nr, min_nr)) sc_in(ctf_integer(__s32, nr, nr)) sc_out(ctf_integer(struct io_event *, events, events)) sc_inout(ctf_integer(struct old_timespec32 *, timeout, timeout)))
)
#endif
#ifndef OVERRIDE_32_pciconfig_read
SC_LTTNG_TRACEPOINT_EVENT(pciconfig_read,
	TP_PROTO(sc_exit(long ret,) unsigned long bus, unsigned long dfn, unsigned long off, unsigned long len, void * buf),
	TP_ARGS(sc_exit(ret,) bus, dfn, off, len, buf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned long, bus, bus)) sc_inout(ctf_integer(unsigned long, dfn, dfn)) sc_inout(ctf_integer(unsigned long, off, off)) sc_inout(ctf_integer(unsigned long, len, len)) sc_inout(ctf_integer(void *, buf, buf)))
)
#endif
#ifndef OVERRIDE_32_pciconfig_write
SC_LTTNG_TRACEPOINT_EVENT(pciconfig_write,
	TP_PROTO(sc_exit(long ret,) unsigned long bus, unsigned long dfn, unsigned long off, unsigned long len, void * buf),
	TP_ARGS(sc_exit(ret,) bus, dfn, off, len, buf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned long, bus, bus)) sc_inout(ctf_integer(unsigned long, dfn, dfn)) sc_inout(ctf_integer(unsigned long, off, off)) sc_inout(ctf_integer(unsigned long, len, len)) sc_inout(ctf_integer(void *, buf, buf)))
)
#endif
#ifndef OVERRIDE_32_mq_timedsend_time32
SC_LTTNG_TRACEPOINT_EVENT(mq_timedsend_time32,
	TP_PROTO(sc_exit(long ret,) mqd_t mqdes, const char * u_msg_ptr, unsigned int msg_len, unsigned int msg_prio, const struct old_timespec32 * u_abs_timeout),
	TP_ARGS(sc_exit(ret,) mqdes, u_msg_ptr, msg_len, msg_prio, u_abs_timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(mqd_t, mqdes, mqdes)) sc_in(ctf_integer(const char *, u_msg_ptr, u_msg_ptr)) sc_in(ctf_integer(unsigned int, msg_len, msg_len)) sc_in(ctf_integer(unsigned int, msg_prio, msg_prio)) sc_in(ctf_integer(const struct old_timespec32 *, u_abs_timeout, u_abs_timeout)))
)
#endif
#ifndef OVERRIDE_32_mq_timedreceive_time32
SC_LTTNG_TRACEPOINT_EVENT(mq_timedreceive_time32,
	TP_PROTO(sc_exit(long ret,) mqd_t mqdes, char * u_msg_ptr, unsigned int msg_len, unsigned int * u_msg_prio, const struct old_timespec32 * u_abs_timeout),
	TP_ARGS(sc_exit(ret,) mqdes, u_msg_ptr, msg_len, u_msg_prio, u_abs_timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(mqd_t, mqdes, mqdes)) sc_out(ctf_integer(char *, u_msg_ptr, u_msg_ptr)) sc_in(ctf_integer(unsigned int, msg_len, msg_len)) sc_out(ctf_integer(unsigned int *, u_msg_prio, u_msg_prio)) sc_in(ctf_integer(const struct old_timespec32 *, u_abs_timeout, u_abs_timeout)))
)
#endif
#ifndef OVERRIDE_32_waitid
SC_LTTNG_TRACEPOINT_EVENT(waitid,
	TP_PROTO(sc_exit(long ret,) int which, pid_t upid, struct siginfo * infop, int options, struct rusage * ru),
	TP_ARGS(sc_exit(ret,) which, upid, infop, options, ru),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(pid_t, upid, upid)) sc_out(ctf_integer(struct siginfo *, infop, infop)) sc_in(ctf_integer(int, options, options)) sc_out(ctf_integer(struct rusage *, ru, ru)))
)
#endif
#ifndef OVERRIDE_32_setsockopt
SC_LTTNG_TRACEPOINT_EVENT(setsockopt,
	TP_PROTO(sc_exit(long ret,) int fd, int level, int optname, char * optval, int optlen),
	TP_ARGS(sc_exit(ret,) fd, level, optname, optval, optlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(int, level, level)) sc_in(ctf_integer(int, optname, optname)) sc_in(ctf_integer(char *, optval, optval)) sc_in(ctf_integer(int, optlen, optlen)))
)
#endif
#ifndef OVERRIDE_32_getsockopt
SC_LTTNG_TRACEPOINT_EVENT(getsockopt,
	TP_PROTO(sc_exit(long ret,) int fd, int level, int optname, char * optval, int * optlen),
	TP_ARGS(sc_exit(ret,) fd, level, optname, optval, optlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(int, level, level)) sc_in(ctf_integer(int, optname, optname)) sc_out(ctf_integer(char *, optval, optval)) sc_inout(ctf_integer(int *, optlen, optlen)))
)
#endif
#ifndef OVERRIDE_32_msgrcv
SC_LTTNG_TRACEPOINT_EVENT(msgrcv,
	TP_PROTO(sc_exit(long ret,) int msqid, struct msgbuf * msgp, size_t msgsz, long msgtyp, int msgflg),
	TP_ARGS(sc_exit(ret,) msqid, msgp, msgsz, msgtyp, msgflg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, msqid, msqid)) sc_out(ctf_integer(struct msgbuf *, msgp, msgp)) sc_in(ctf_integer(size_t, msgsz, msgsz)) sc_in(ctf_integer(long, msgtyp, msgtyp)) sc_in(ctf_integer(int, msgflg, msgflg)))
)
#endif
#ifndef OVERRIDE_32_add_key
SC_LTTNG_TRACEPOINT_EVENT(add_key,
	TP_PROTO(sc_exit(long ret,) const char * _type, const char * _description, const void * _payload, size_t plen, key_serial_t ringid),
	TP_ARGS(sc_exit(ret,) _type, _description, _payload, plen, ringid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(_type, _type)) sc_in(ctf_integer(const char *, _description, _description)) sc_in(ctf_integer(const void *, _payload, _payload)) sc_in(ctf_integer(size_t, plen, plen)) sc_in(ctf_integer(key_serial_t, ringid, ringid)))
)
#endif
#ifndef OVERRIDE_32_fchownat
SC_LTTNG_TRACEPOINT_EVENT(fchownat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, uid_t user, gid_t group, int flag),
	TP_ARGS(sc_exit(ret,) dfd, filename, user, group, flag),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(uid_t, user, user)) sc_in(ctf_integer(gid_t, group, group)) sc_in(ctf_integer(int, flag, flag)))
)
#endif
#ifndef OVERRIDE_32_linkat
SC_LTTNG_TRACEPOINT_EVENT(linkat,
	TP_PROTO(sc_exit(long ret,) int olddfd, const char * oldname, int newdfd, const char * newname, int flags),
	TP_ARGS(sc_exit(ret,) olddfd, oldname, newdfd, newname, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, olddfd, olddfd)) sc_in(ctf_user_string(oldname, oldname)) sc_in(ctf_integer(int, newdfd, newdfd)) sc_in(ctf_user_string(newname, newname)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_ppoll_time32
SC_LTTNG_TRACEPOINT_EVENT(ppoll_time32,
	TP_PROTO(sc_exit(long ret,) struct pollfd * ufds, unsigned int nfds, struct old_timespec32 * tsp, const sigset_t * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) ufds, nfds, tsp, sigmask, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct pollfd *, ufds, ufds)) sc_in(ctf_integer(unsigned int, nfds, nfds)) sc_in(ctf_integer(struct old_timespec32 *, tsp, tsp)) sc_in(ctf_integer(const sigset_t *, sigmask, sigmask)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_32_preadv
SC_LTTNG_TRACEPOINT_EVENT(preadv,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen, unsigned long pos_l, unsigned long pos_h),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen, pos_l, pos_h),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, fd, fd)) sc_out(ctf_integer(const struct iovec *, vec, vec)) sc_in(ctf_integer(unsigned long, vlen, vlen)) sc_in(ctf_integer(unsigned long, pos_l, pos_l)) sc_in(ctf_integer(unsigned long, pos_h, pos_h)))
)
#endif
#ifndef OVERRIDE_32_pwritev
SC_LTTNG_TRACEPOINT_EVENT(pwritev,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen, unsigned long pos_l, unsigned long pos_h),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen, pos_l, pos_h),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, fd, fd)) sc_in(ctf_integer(const struct iovec *, vec, vec)) sc_in(ctf_integer(unsigned long, vlen, vlen)) sc_in(ctf_integer(unsigned long, pos_l, pos_l)) sc_in(ctf_integer(unsigned long, pos_h, pos_h)))
)
#endif
#ifndef OVERRIDE_32_perf_event_open
SC_LTTNG_TRACEPOINT_EVENT(perf_event_open,
	TP_PROTO(sc_exit(long ret,) struct perf_event_attr * attr_uptr, pid_t pid, int cpu, int group_fd, unsigned long flags),
	TP_ARGS(sc_exit(ret,) attr_uptr, pid, cpu, group_fd, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(struct perf_event_attr *, attr_uptr, attr_uptr)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, cpu, cpu)) sc_in(ctf_integer(int, group_fd, group_fd)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_recvmmsg_time32
SC_LTTNG_TRACEPOINT_EVENT(recvmmsg_time32,
	TP_PROTO(sc_exit(long ret,) int fd, struct mmsghdr * mmsg, unsigned int vlen, unsigned int flags, struct old_timespec32 * timeout),
	TP_ARGS(sc_exit(ret,) fd, mmsg, vlen, flags, timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(struct mmsghdr *, mmsg, mmsg)) sc_in(ctf_integer(unsigned int, vlen, vlen)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_inout(ctf_integer(struct old_timespec32 *, timeout, timeout)))
)
#endif
#ifndef OVERRIDE_32_fanotify_mark
SC_LTTNG_TRACEPOINT_EVENT(fanotify_mark,
	TP_PROTO(sc_exit(long ret,) int fanotify_fd, unsigned int flags, __u64 mask, int dfd, const char * pathname),
	TP_ARGS(sc_exit(ret,) fanotify_fd, flags, mask, dfd, pathname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fanotify_fd, fanotify_fd)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_in(ctf_integer(__u64, mask, mask)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(pathname, pathname)))
)
#endif
#ifndef OVERRIDE_32_name_to_handle_at
SC_LTTNG_TRACEPOINT_EVENT(name_to_handle_at,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * name, struct file_handle * handle, int * mnt_id, int flag),
	TP_ARGS(sc_exit(ret,) dfd, name, handle, mnt_id, flag),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(name, name)) sc_inout(ctf_integer(struct file_handle *, handle, handle)) sc_out(ctf_integer(int *, mnt_id, mnt_id)) sc_in(ctf_integer(int, flag, flag)))
)
#endif
#ifndef OVERRIDE_32_renameat2
SC_LTTNG_TRACEPOINT_EVENT(renameat2,
	TP_PROTO(sc_exit(long ret,) int olddfd, const char * oldname, int newdfd, const char * newname, unsigned int flags),
	TP_ARGS(sc_exit(ret,) olddfd, oldname, newdfd, newname, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, olddfd, olddfd)) sc_inout(ctf_user_string(oldname, oldname)) sc_inout(ctf_integer(int, newdfd, newdfd)) sc_inout(ctf_user_string(newname, newname)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_execveat
SC_LTTNG_TRACEPOINT_EVENT(execveat,
	TP_PROTO(sc_exit(long ret,) int fd, const char * filename, const char *const * argv, const char *const * envp, int flags),
	TP_ARGS(sc_exit(ret,) fd, filename, argv, envp, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, fd, fd)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(const char *const *, argv, argv)) sc_inout(ctf_integer(const char *const *, envp, envp)) sc_inout(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_statx
SC_LTTNG_TRACEPOINT_EVENT(statx,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, unsigned flags, unsigned int mask, struct statx * buffer),
	TP_ARGS(sc_exit(ret,) dfd, filename, flags, mask, buffer),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, dfd, dfd)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(unsigned, flags, flags)) sc_inout(ctf_integer(unsigned int, mask, mask)) sc_inout(ctf_integer(struct statx *, buffer, buffer)))
)
#endif
#ifndef OVERRIDE_32_ppoll
SC_LTTNG_TRACEPOINT_EVENT(ppoll,
	TP_PROTO(sc_exit(long ret,) struct pollfd * ufds, unsigned int nfds, struct __kernel_timespec * tsp, const sigset_t * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) ufds, nfds, tsp, sigmask, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct pollfd *, ufds, ufds)) sc_in(ctf_integer(unsigned int, nfds, nfds)) sc_in(ctf_integer(struct __kernel_timespec *, tsp, tsp)) sc_in(ctf_integer(const sigset_t *, sigmask, sigmask)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_32_recvmmsg
SC_LTTNG_TRACEPOINT_EVENT(recvmmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct mmsghdr * mmsg, unsigned int vlen, unsigned int flags, struct __kernel_timespec * timeout),
	TP_ARGS(sc_exit(ret,) fd, mmsg, vlen, flags, timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(struct mmsghdr *, mmsg, mmsg)) sc_in(ctf_integer(unsigned int, vlen, vlen)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_inout(ctf_integer(struct __kernel_timespec *, timeout, timeout)))
)
#endif
#ifndef OVERRIDE_32_mq_timedsend
SC_LTTNG_TRACEPOINT_EVENT(mq_timedsend,
	TP_PROTO(sc_exit(long ret,) mqd_t mqdes, const char * u_msg_ptr, size_t msg_len, unsigned int msg_prio, const struct __kernel_timespec * u_abs_timeout),
	TP_ARGS(sc_exit(ret,) mqdes, u_msg_ptr, msg_len, msg_prio, u_abs_timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(mqd_t, mqdes, mqdes)) sc_in(ctf_integer(const char *, u_msg_ptr, u_msg_ptr)) sc_in(ctf_integer(size_t, msg_len, msg_len)) sc_in(ctf_integer(unsigned int, msg_prio, msg_prio)) sc_in(ctf_integer(const struct __kernel_timespec *, u_abs_timeout, u_abs_timeout)))
)
#endif
#ifndef OVERRIDE_32_mq_timedreceive
SC_LTTNG_TRACEPOINT_EVENT(mq_timedreceive,
	TP_PROTO(sc_exit(long ret,) mqd_t mqdes, char * u_msg_ptr, size_t msg_len, unsigned int * u_msg_prio, const struct __kernel_timespec * u_abs_timeout),
	TP_ARGS(sc_exit(ret,) mqdes, u_msg_ptr, msg_len, u_msg_prio, u_abs_timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(mqd_t, mqdes, mqdes)) sc_out(ctf_integer(char *, u_msg_ptr, u_msg_ptr)) sc_in(ctf_integer(size_t, msg_len, msg_len)) sc_out(ctf_integer(unsigned int *, u_msg_prio, u_msg_prio)) sc_in(ctf_integer(const struct __kernel_timespec *, u_abs_timeout, u_abs_timeout)))
)
#endif
#ifndef OVERRIDE_32_move_mount
SC_LTTNG_TRACEPOINT_EVENT(move_mount,
	TP_PROTO(sc_exit(long ret,) int from_dfd, const char * from_pathname, int to_dfd, const char * to_pathname, unsigned int flags),
	TP_ARGS(sc_exit(ret,) from_dfd, from_pathname, to_dfd, to_pathname, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, from_dfd, from_dfd)) sc_inout(ctf_user_string(from_pathname, from_pathname)) sc_inout(ctf_integer(int, to_dfd, to_dfd)) sc_inout(ctf_user_string(to_pathname, to_pathname)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_fsconfig
SC_LTTNG_TRACEPOINT_EVENT(fsconfig,
	TP_PROTO(sc_exit(long ret,) int fd, unsigned int cmd, const char * _key, const void * _value, int aux),
	TP_ARGS(sc_exit(ret,) fd, cmd, _key, _value, aux),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, fd, fd)) sc_inout(ctf_integer(unsigned int, cmd, cmd)) sc_inout(ctf_integer(const char *, _key, _key)) sc_inout(ctf_integer(const void *, _value, _value)) sc_inout(ctf_integer(int, aux, aux)))
)
#endif
#ifndef OVERRIDE_32_process_madvise
SC_LTTNG_TRACEPOINT_EVENT(process_madvise,
	TP_PROTO(sc_exit(long ret,) int pidfd, const struct iovec * vec, size_t vlen, int behavior, unsigned int flags),
	TP_ARGS(sc_exit(ret,) pidfd, vec, vlen, behavior, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, pidfd, pidfd)) sc_inout(ctf_integer(const struct iovec *, vec, vec)) sc_inout(ctf_integer(size_t, vlen, vlen)) sc_inout(ctf_integer(int, behavior, behavior)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_mount_setattr
SC_LTTNG_TRACEPOINT_EVENT(mount_setattr,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * path, unsigned int flags, struct mount_attr * uattr, size_t usize),
	TP_ARGS(sc_exit(ret,) dfd, path, flags, uattr, usize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, dfd, dfd)) sc_inout(ctf_user_string(path, path)) sc_inout(ctf_integer(unsigned int, flags, flags)) sc_inout(ctf_integer(struct mount_attr *, uattr, uattr)) sc_inout(ctf_integer(size_t, usize, usize)))
)
#endif
#ifndef OVERRIDE_32_futex_waitv
SC_LTTNG_TRACEPOINT_EVENT(futex_waitv,
	TP_PROTO(sc_exit(long ret,) struct futex_waitv * waiters, unsigned int nr_futexes, unsigned int flags, struct __kernel_timespec * timeout, clockid_t clockid),
	TP_ARGS(sc_exit(ret,) waiters, nr_futexes, flags, timeout, clockid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct futex_waitv *, waiters, waiters)) sc_inout(ctf_integer(unsigned int, nr_futexes, nr_futexes)) sc_inout(ctf_integer(unsigned int, flags, flags)) sc_inout(ctf_integer(struct __kernel_timespec *, timeout, timeout)) sc_inout(ctf_integer(clockid_t, clockid, clockid)))
)
#endif
#ifndef OVERRIDE_32_futex_time32
SC_LTTNG_TRACEPOINT_EVENT(futex_time32,
	TP_PROTO(sc_exit(long ret,) u32 * uaddr, int op, u32 val, const struct old_timespec32 * utime, u32 * uaddr2, u32 val3),
	TP_ARGS(sc_exit(ret,) uaddr, op, val, utime, uaddr2, val3),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(u32 *, uaddr, uaddr)) sc_in(ctf_integer(int, op, op)) sc_in(ctf_integer(u32, val, val)) sc_in(ctf_integer(const struct old_timespec32 *, utime, utime)) sc_inout(ctf_integer(u32 *, uaddr2, uaddr2)) sc_in(ctf_integer(u32, val3, val3)))
)
#endif
#ifndef OVERRIDE_32_sendto
SC_LTTNG_TRACEPOINT_EVENT(sendto,
	TP_PROTO(sc_exit(long ret,) int fd, void * buff, size_t len, unsigned int flags, struct sockaddr * addr, int addr_len),
	TP_ARGS(sc_exit(ret,) fd, buff, len, flags, addr, addr_len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(void *, buff, buff)) sc_in(ctf_integer(size_t, len, len)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_in(ctf_integer(struct sockaddr *, addr, addr)) sc_in(ctf_integer(int, addr_len, addr_len)))
)
#endif
#ifndef OVERRIDE_32_recvfrom
SC_LTTNG_TRACEPOINT_EVENT(recvfrom,
	TP_PROTO(sc_exit(long ret,) int fd, void * ubuf, size_t size, unsigned int flags, struct sockaddr * addr, int * addr_len),
	TP_ARGS(sc_exit(ret,) fd, ubuf, size, flags, addr, addr_len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(void *, ubuf, ubuf)) sc_in(ctf_integer(size_t, size, size)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_out(ctf_integer(struct sockaddr *, addr, addr)) sc_inout(ctf_integer(int *, addr_len, addr_len)))
)
#endif
#ifndef OVERRIDE_32_pselect6_time32
SC_LTTNG_TRACEPOINT_EVENT(pselect6_time32,
	TP_PROTO(sc_exit(long ret,) int n, fd_set * inp, fd_set * outp, fd_set * exp, struct old_timespec32 * tsp, void * sig),
	TP_ARGS(sc_exit(ret,) n, inp, outp, exp, tsp, sig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, n, n)) sc_inout(ctf_integer(fd_set *, inp, inp)) sc_inout(ctf_integer(fd_set *, outp, outp)) sc_inout(ctf_integer(fd_set *, exp, exp)) sc_inout(ctf_integer(struct old_timespec32 *, tsp, tsp)) sc_in(ctf_integer(void *, sig, sig)))
)
#endif
#ifndef OVERRIDE_32_splice
SC_LTTNG_TRACEPOINT_EVENT(splice,
	TP_PROTO(sc_exit(long ret,) int fd_in, loff_t * off_in, int fd_out, loff_t * off_out, size_t len, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd_in, off_in, fd_out, off_out, len, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd_in, fd_in)) sc_in(ctf_integer(loff_t *, off_in, off_in)) sc_in(ctf_integer(int, fd_out, fd_out)) sc_in(ctf_integer(loff_t *, off_out, off_out)) sc_in(ctf_integer(size_t, len, len)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_epoll_pwait
SC_LTTNG_TRACEPOINT_EVENT(epoll_pwait,
	TP_PROTO(sc_exit(long ret,) int epfd, struct epoll_event * events, int maxevents, int timeout, const sigset_t * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) epfd, events, maxevents, timeout, sigmask, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, epfd, epfd)) sc_out(ctf_integer(struct epoll_event *, events, events)) sc_in(ctf_integer(int, maxevents, maxevents)) sc_in(ctf_integer(int, timeout, timeout)) sc_in(ctf_integer(const sigset_t *, sigmask, sigmask)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_32_process_vm_readv
SC_LTTNG_TRACEPOINT_EVENT(process_vm_readv,
	TP_PROTO(sc_exit(long ret,) pid_t pid, const struct iovec * lvec, unsigned long liovcnt, const struct iovec * rvec, unsigned long riovcnt, unsigned long flags),
	TP_ARGS(sc_exit(ret,) pid, lvec, liovcnt, rvec, riovcnt, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_out(ctf_integer(const struct iovec *, lvec, lvec)) sc_in(ctf_integer(unsigned long, liovcnt, liovcnt)) sc_in(ctf_integer(const struct iovec *, rvec, rvec)) sc_in(ctf_integer(unsigned long, riovcnt, riovcnt)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_process_vm_writev
SC_LTTNG_TRACEPOINT_EVENT(process_vm_writev,
	TP_PROTO(sc_exit(long ret,) pid_t pid, const struct iovec * lvec, unsigned long liovcnt, const struct iovec * rvec, unsigned long riovcnt, unsigned long flags),
	TP_ARGS(sc_exit(ret,) pid, lvec, liovcnt, rvec, riovcnt, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(const struct iovec *, lvec, lvec)) sc_in(ctf_integer(unsigned long, liovcnt, liovcnt)) sc_in(ctf_integer(const struct iovec *, rvec, rvec)) sc_in(ctf_integer(unsigned long, riovcnt, riovcnt)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_copy_file_range
SC_LTTNG_TRACEPOINT_EVENT(copy_file_range,
	TP_PROTO(sc_exit(long ret,) int fd_in, loff_t * off_in, int fd_out, loff_t * off_out, size_t len, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd_in, off_in, fd_out, off_out, len, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, fd_in, fd_in)) sc_inout(ctf_integer(loff_t *, off_in, off_in)) sc_inout(ctf_integer(int, fd_out, fd_out)) sc_inout(ctf_integer(loff_t *, off_out, off_out)) sc_inout(ctf_integer(size_t, len, len)) sc_inout(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_preadv2
SC_LTTNG_TRACEPOINT_EVENT(preadv2,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen, unsigned long pos_l, unsigned long pos_h, rwf_t flags),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen, pos_l, pos_h, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned long, fd, fd)) sc_inout(ctf_integer(const struct iovec *, vec, vec)) sc_inout(ctf_integer(unsigned long, vlen, vlen)) sc_inout(ctf_integer(unsigned long, pos_l, pos_l)) sc_inout(ctf_integer(unsigned long, pos_h, pos_h)) sc_inout(ctf_integer(rwf_t, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_pwritev2
SC_LTTNG_TRACEPOINT_EVENT(pwritev2,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen, unsigned long pos_l, unsigned long pos_h, rwf_t flags),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen, pos_l, pos_h, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned long, fd, fd)) sc_inout(ctf_integer(const struct iovec *, vec, vec)) sc_inout(ctf_integer(unsigned long, vlen, vlen)) sc_inout(ctf_integer(unsigned long, pos_l, pos_l)) sc_inout(ctf_integer(unsigned long, pos_h, pos_h)) sc_inout(ctf_integer(rwf_t, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_io_pgetevents_time32
SC_LTTNG_TRACEPOINT_EVENT(io_pgetevents_time32,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx_id, long min_nr, long nr, struct io_event * events, struct old_timespec32 * timeout, const struct __aio_sigset * usig),
	TP_ARGS(sc_exit(ret,) ctx_id, min_nr, nr, events, timeout, usig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(aio_context_t, ctx_id, ctx_id)) sc_inout(ctf_integer(long, min_nr, min_nr)) sc_inout(ctf_integer(long, nr, nr)) sc_inout(ctf_integer(struct io_event *, events, events)) sc_inout(ctf_integer(struct old_timespec32 *, timeout, timeout)) sc_inout(ctf_integer(const struct __aio_sigset *, usig, usig)))
)
#endif
#ifndef OVERRIDE_32_pselect6
SC_LTTNG_TRACEPOINT_EVENT(pselect6,
	TP_PROTO(sc_exit(long ret,) int n, fd_set * inp, fd_set * outp, fd_set * exp, struct __kernel_timespec * tsp, void * sig),
	TP_ARGS(sc_exit(ret,) n, inp, outp, exp, tsp, sig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, n, n)) sc_inout(ctf_integer(fd_set *, inp, inp)) sc_inout(ctf_integer(fd_set *, outp, outp)) sc_inout(ctf_integer(fd_set *, exp, exp)) sc_inout(ctf_integer(struct __kernel_timespec *, tsp, tsp)) sc_in(ctf_integer(void *, sig, sig)))
)
#endif
#ifndef OVERRIDE_32_io_pgetevents
SC_LTTNG_TRACEPOINT_EVENT(io_pgetevents,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx_id, long min_nr, long nr, struct io_event * events, struct __kernel_timespec * timeout, const struct __aio_sigset * usig),
	TP_ARGS(sc_exit(ret,) ctx_id, min_nr, nr, events, timeout, usig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(aio_context_t, ctx_id, ctx_id)) sc_inout(ctf_integer(long, min_nr, min_nr)) sc_inout(ctf_integer(long, nr, nr)) sc_inout(ctf_integer(struct io_event *, events, events)) sc_inout(ctf_integer(struct __kernel_timespec *, timeout, timeout)) sc_inout(ctf_integer(const struct __aio_sigset *, usig, usig)))
)
#endif
#ifndef OVERRIDE_32_futex
SC_LTTNG_TRACEPOINT_EVENT(futex,
	TP_PROTO(sc_exit(long ret,) u32 * uaddr, int op, u32 val, const struct __kernel_timespec * utime, u32 * uaddr2, u32 val3),
	TP_ARGS(sc_exit(ret,) uaddr, op, val, utime, uaddr2, val3),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(u32 *, uaddr, uaddr)) sc_in(ctf_integer(int, op, op)) sc_in(ctf_integer(u32, val, val)) sc_in(ctf_integer(const struct __kernel_timespec *, utime, utime)) sc_inout(ctf_integer(u32 *, uaddr2, uaddr2)) sc_in(ctf_integer(u32, val3, val3)))
)
#endif
#ifndef OVERRIDE_32_io_uring_enter
SC_LTTNG_TRACEPOINT_EVENT(io_uring_enter,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, u32 to_submit, u32 min_complete, u32 flags, const void * argp, size_t argsz),
	TP_ARGS(sc_exit(ret,) fd, to_submit, min_complete, flags, argp, argsz),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, fd, fd)) sc_inout(ctf_integer(u32, to_submit, to_submit)) sc_inout(ctf_integer(u32, min_complete, min_complete)) sc_inout(ctf_integer(u32, flags, flags)) sc_inout(ctf_integer(const void *, argp, argp)) sc_inout(ctf_integer(size_t, argsz, argsz)))
)
#endif
#ifndef OVERRIDE_32_epoll_pwait2
SC_LTTNG_TRACEPOINT_EVENT(epoll_pwait2,
	TP_PROTO(sc_exit(long ret,) int epfd, struct epoll_event * events, int maxevents, const struct __kernel_timespec * timeout, const sigset_t * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) epfd, events, maxevents, timeout, sigmask, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, epfd, epfd)) sc_inout(ctf_integer(struct epoll_event *, events, events)) sc_inout(ctf_integer(int, maxevents, maxevents)) sc_inout(ctf_integer(const struct __kernel_timespec *, timeout, timeout)) sc_inout(ctf_integer(const sigset_t *, sigmask, sigmask)) sc_inout(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif

#endif /*  _TRACE_SYSCALLS_POINTERS_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>

#else /* CREATE_SYSCALL_TABLE */

#include "arm-32-syscalls_pointers_override.h"
#include "syscalls_pointers_override.h"

#ifndef OVERRIDE_TABLE_32_read
TRACE_SYSCALL_TABLE(read, read, 3, 3)
#endif
#ifndef OVERRIDE_TABLE_32_write
TRACE_SYSCALL_TABLE(write, write, 4, 3)
#endif
#ifndef OVERRIDE_TABLE_32_open
TRACE_SYSCALL_TABLE(open, open, 5, 3)
#endif
#ifndef OVERRIDE_TABLE_32_creat
TRACE_SYSCALL_TABLE(creat, creat, 8, 2)
#endif
#ifndef OVERRIDE_TABLE_32_link
TRACE_SYSCALL_TABLE(link, link, 9, 2)
#endif
#ifndef OVERRIDE_TABLE_32_unlink
TRACE_SYSCALL_TABLE(unlink, unlink, 10, 1)
#endif
#ifndef OVERRIDE_TABLE_32_execve
TRACE_SYSCALL_TABLE(execve, execve, 11, 3)
#endif
#ifndef OVERRIDE_TABLE_32_chdir
TRACE_SYSCALL_TABLE(chdir, chdir, 12, 1)
#endif
#ifndef OVERRIDE_TABLE_32_mknod
TRACE_SYSCALL_TABLE(mknod, mknod, 14, 3)
#endif
#ifndef OVERRIDE_TABLE_32_chmod
TRACE_SYSCALL_TABLE(chmod, chmod, 15, 2)
#endif
#ifndef OVERRIDE_TABLE_32_lchown16
TRACE_SYSCALL_TABLE(lchown16, lchown16, 16, 3)
#endif
#ifndef OVERRIDE_TABLE_32_mount
TRACE_SYSCALL_TABLE(mount, mount, 21, 5)
#endif
#ifndef OVERRIDE_TABLE_32_access
TRACE_SYSCALL_TABLE(access, access, 33, 2)
#endif
#ifndef OVERRIDE_TABLE_32_rename
TRACE_SYSCALL_TABLE(rename, rename, 38, 2)
#endif
#ifndef OVERRIDE_TABLE_32_mkdir
TRACE_SYSCALL_TABLE(mkdir, mkdir, 39, 2)
#endif
#ifndef OVERRIDE_TABLE_32_rmdir
TRACE_SYSCALL_TABLE(rmdir, rmdir, 40, 1)
#endif
#ifndef OVERRIDE_TABLE_32_pipe
TRACE_SYSCALL_TABLE(pipe, pipe, 42, 1)
#endif
#ifndef OVERRIDE_TABLE_32_times
TRACE_SYSCALL_TABLE(times, times, 43, 1)
#endif
#ifndef OVERRIDE_TABLE_32_acct
TRACE_SYSCALL_TABLE(acct, acct, 51, 1)
#endif
#ifndef OVERRIDE_TABLE_32_umount
TRACE_SYSCALL_TABLE(umount, umount, 52, 2)
#endif
#ifndef OVERRIDE_TABLE_32_chroot
TRACE_SYSCALL_TABLE(chroot, chroot, 61, 1)
#endif
#ifndef OVERRIDE_TABLE_32_ustat
TRACE_SYSCALL_TABLE(ustat, ustat, 62, 2)
#endif
#ifndef OVERRIDE_TABLE_32_sigaction
TRACE_SYSCALL_TABLE(sigaction, sigaction, 67, 3)
#endif
#ifndef OVERRIDE_TABLE_32_sigpending
TRACE_SYSCALL_TABLE(sigpending, sigpending, 73, 1)
#endif
#ifndef OVERRIDE_TABLE_32_sethostname
TRACE_SYSCALL_TABLE(sethostname, sethostname, 74, 2)
#endif
#ifndef OVERRIDE_TABLE_32_setrlimit
TRACE_SYSCALL_TABLE(setrlimit, setrlimit, 75, 2)
#endif
#ifndef OVERRIDE_TABLE_32_getrusage
TRACE_SYSCALL_TABLE(getrusage, getrusage, 77, 2)
#endif
#ifndef OVERRIDE_TABLE_32_gettimeofday
TRACE_SYSCALL_TABLE(gettimeofday, gettimeofday, 78, 2)
#endif
#ifndef OVERRIDE_TABLE_32_settimeofday
TRACE_SYSCALL_TABLE(settimeofday, settimeofday, 79, 2)
#endif
#ifndef OVERRIDE_TABLE_32_getgroups16
TRACE_SYSCALL_TABLE(getgroups16, getgroups16, 80, 2)
#endif
#ifndef OVERRIDE_TABLE_32_setgroups16
TRACE_SYSCALL_TABLE(setgroups16, setgroups16, 81, 2)
#endif
#ifndef OVERRIDE_TABLE_32_symlink
TRACE_SYSCALL_TABLE(symlink, symlink, 83, 2)
#endif
#ifndef OVERRIDE_TABLE_32_readlink
TRACE_SYSCALL_TABLE(readlink, readlink, 85, 3)
#endif
#ifndef OVERRIDE_TABLE_32_swapon
TRACE_SYSCALL_TABLE(swapon, swapon, 87, 2)
#endif
#ifndef OVERRIDE_TABLE_32_reboot
TRACE_SYSCALL_TABLE(reboot, reboot, 88, 4)
#endif
#ifndef OVERRIDE_TABLE_32_truncate
TRACE_SYSCALL_TABLE(truncate, truncate, 92, 2)
#endif
#ifndef OVERRIDE_TABLE_32_statfs
TRACE_SYSCALL_TABLE(statfs, statfs, 99, 2)
#endif
#ifndef OVERRIDE_TABLE_32_fstatfs
TRACE_SYSCALL_TABLE(fstatfs, fstatfs, 100, 2)
#endif
#ifndef OVERRIDE_TABLE_32_syslog
TRACE_SYSCALL_TABLE(syslog, syslog, 103, 3)
#endif
#ifndef OVERRIDE_TABLE_32_setitimer
TRACE_SYSCALL_TABLE(setitimer, setitimer, 104, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getitimer
TRACE_SYSCALL_TABLE(getitimer, getitimer, 105, 2)
#endif
#ifndef OVERRIDE_TABLE_32_newstat
TRACE_SYSCALL_TABLE(newstat, newstat, 106, 2)
#endif
#ifndef OVERRIDE_TABLE_32_newlstat
TRACE_SYSCALL_TABLE(newlstat, newlstat, 107, 2)
#endif
#ifndef OVERRIDE_TABLE_32_newfstat
TRACE_SYSCALL_TABLE(newfstat, newfstat, 108, 2)
#endif
#ifndef OVERRIDE_TABLE_32_wait4
TRACE_SYSCALL_TABLE(wait4, wait4, 114, 4)
#endif
#ifndef OVERRIDE_TABLE_32_swapoff
TRACE_SYSCALL_TABLE(swapoff, swapoff, 115, 1)
#endif
#ifndef OVERRIDE_TABLE_32_sysinfo
TRACE_SYSCALL_TABLE(sysinfo, sysinfo, 116, 1)
#endif
#ifndef OVERRIDE_TABLE_32_clone
TRACE_SYSCALL_TABLE(clone, clone, 120, 5)
#endif
#ifndef OVERRIDE_TABLE_32_setdomainname
TRACE_SYSCALL_TABLE(setdomainname, setdomainname, 121, 2)
#endif
#ifndef OVERRIDE_TABLE_32_newuname
TRACE_SYSCALL_TABLE(newuname, newuname, 122, 1)
#endif
#ifndef OVERRIDE_TABLE_32_adjtimex_time32
TRACE_SYSCALL_TABLE(adjtimex_time32, adjtimex_time32, 124, 1)
#endif
#ifndef OVERRIDE_TABLE_32_sigprocmask
TRACE_SYSCALL_TABLE(sigprocmask, sigprocmask, 126, 3)
#endif
#ifndef OVERRIDE_TABLE_32_init_module
TRACE_SYSCALL_TABLE(init_module, init_module, 128, 3)
#endif
#ifndef OVERRIDE_TABLE_32_delete_module
TRACE_SYSCALL_TABLE(delete_module, delete_module, 129, 2)
#endif
#ifndef OVERRIDE_TABLE_32_quotactl
TRACE_SYSCALL_TABLE(quotactl, quotactl, 131, 4)
#endif
#ifndef OVERRIDE_TABLE_32_llseek
TRACE_SYSCALL_TABLE(llseek, llseek, 140, 5)
#endif
#ifndef OVERRIDE_TABLE_32_getdents
TRACE_SYSCALL_TABLE(getdents, getdents, 141, 3)
#endif
#ifndef OVERRIDE_TABLE_32_select
TRACE_SYSCALL_TABLE(select, select, 142, 5)
#endif
#ifndef OVERRIDE_TABLE_32_readv
TRACE_SYSCALL_TABLE(readv, readv, 145, 3)
#endif
#ifndef OVERRIDE_TABLE_32_writev
TRACE_SYSCALL_TABLE(writev, writev, 146, 3)
#endif
#ifndef OVERRIDE_TABLE_32_sched_setparam
TRACE_SYSCALL_TABLE(sched_setparam, sched_setparam, 154, 2)
#endif
#ifndef OVERRIDE_TABLE_32_sched_getparam
TRACE_SYSCALL_TABLE(sched_getparam, sched_getparam, 155, 2)
#endif
#ifndef OVERRIDE_TABLE_32_sched_setscheduler
TRACE_SYSCALL_TABLE(sched_setscheduler, sched_setscheduler, 156, 3)
#endif
#ifndef OVERRIDE_TABLE_32_sched_rr_get_interval_time32
TRACE_SYSCALL_TABLE(sched_rr_get_interval_time32, sched_rr_get_interval_time32, 161, 2)
#endif
#ifndef OVERRIDE_TABLE_32_nanosleep_time32
TRACE_SYSCALL_TABLE(nanosleep_time32, nanosleep_time32, 162, 2)
#endif
#ifndef OVERRIDE_TABLE_32_getresuid16
TRACE_SYSCALL_TABLE(getresuid16, getresuid16, 165, 3)
#endif
#ifndef OVERRIDE_TABLE_32_poll
TRACE_SYSCALL_TABLE(poll, poll, 168, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getresgid16
TRACE_SYSCALL_TABLE(getresgid16, getresgid16, 171, 3)
#endif
#ifndef OVERRIDE_TABLE_32_rt_sigaction
TRACE_SYSCALL_TABLE(rt_sigaction, rt_sigaction, 174, 4)
#endif
#ifndef OVERRIDE_TABLE_32_rt_sigprocmask
TRACE_SYSCALL_TABLE(rt_sigprocmask, rt_sigprocmask, 175, 4)
#endif
#ifndef OVERRIDE_TABLE_32_rt_sigpending
TRACE_SYSCALL_TABLE(rt_sigpending, rt_sigpending, 176, 2)
#endif
#ifndef OVERRIDE_TABLE_32_rt_sigtimedwait_time32
TRACE_SYSCALL_TABLE(rt_sigtimedwait_time32, rt_sigtimedwait_time32, 177, 4)
#endif
#ifndef OVERRIDE_TABLE_32_rt_sigqueueinfo
TRACE_SYSCALL_TABLE(rt_sigqueueinfo, rt_sigqueueinfo, 178, 3)
#endif
#ifndef OVERRIDE_TABLE_32_rt_sigsuspend
TRACE_SYSCALL_TABLE(rt_sigsuspend, rt_sigsuspend, 179, 2)
#endif
#ifndef OVERRIDE_TABLE_32_pread64
TRACE_SYSCALL_TABLE(pread64, pread64, 180, 4)
#endif
#ifndef OVERRIDE_TABLE_32_pwrite64
TRACE_SYSCALL_TABLE(pwrite64, pwrite64, 181, 4)
#endif
#ifndef OVERRIDE_TABLE_32_chown16
TRACE_SYSCALL_TABLE(chown16, chown16, 182, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getcwd
TRACE_SYSCALL_TABLE(getcwd, getcwd, 183, 2)
#endif
#ifndef OVERRIDE_TABLE_32_sigaltstack
TRACE_SYSCALL_TABLE(sigaltstack, sigaltstack, 186, 2)
#endif
#ifndef OVERRIDE_TABLE_32_sendfile
TRACE_SYSCALL_TABLE(sendfile, sendfile, 187, 4)
#endif
#ifndef OVERRIDE_TABLE_32_getrlimit
TRACE_SYSCALL_TABLE(getrlimit, getrlimit, 191, 2)
#endif
#ifndef OVERRIDE_TABLE_32_truncate64
TRACE_SYSCALL_TABLE(truncate64, truncate64, 193, 2)
#endif
#ifndef OVERRIDE_TABLE_32_stat64
TRACE_SYSCALL_TABLE(stat64, stat64, 195, 2)
#endif
#ifndef OVERRIDE_TABLE_32_lstat64
TRACE_SYSCALL_TABLE(lstat64, lstat64, 196, 2)
#endif
#ifndef OVERRIDE_TABLE_32_fstat64
TRACE_SYSCALL_TABLE(fstat64, fstat64, 197, 2)
#endif
#ifndef OVERRIDE_TABLE_32_lchown
TRACE_SYSCALL_TABLE(lchown, lchown, 198, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getgroups
TRACE_SYSCALL_TABLE(getgroups, getgroups, 205, 2)
#endif
#ifndef OVERRIDE_TABLE_32_setgroups
TRACE_SYSCALL_TABLE(setgroups, setgroups, 206, 2)
#endif
#ifndef OVERRIDE_TABLE_32_getresuid
TRACE_SYSCALL_TABLE(getresuid, getresuid, 209, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getresgid
TRACE_SYSCALL_TABLE(getresgid, getresgid, 211, 3)
#endif
#ifndef OVERRIDE_TABLE_32_chown
TRACE_SYSCALL_TABLE(chown, chown, 212, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getdents64
TRACE_SYSCALL_TABLE(getdents64, getdents64, 217, 3)
#endif
#ifndef OVERRIDE_TABLE_32_pivot_root
TRACE_SYSCALL_TABLE(pivot_root, pivot_root, 218, 2)
#endif
#ifndef OVERRIDE_TABLE_32_mincore
TRACE_SYSCALL_TABLE(mincore, mincore, 219, 3)
#endif
#ifndef OVERRIDE_TABLE_32_setxattr
TRACE_SYSCALL_TABLE(setxattr, setxattr, 226, 5)
#endif
#ifndef OVERRIDE_TABLE_32_lsetxattr
TRACE_SYSCALL_TABLE(lsetxattr, lsetxattr, 227, 5)
#endif
#ifndef OVERRIDE_TABLE_32_fsetxattr
TRACE_SYSCALL_TABLE(fsetxattr, fsetxattr, 228, 5)
#endif
#ifndef OVERRIDE_TABLE_32_getxattr
TRACE_SYSCALL_TABLE(getxattr, getxattr, 229, 4)
#endif
#ifndef OVERRIDE_TABLE_32_lgetxattr
TRACE_SYSCALL_TABLE(lgetxattr, lgetxattr, 230, 4)
#endif
#ifndef OVERRIDE_TABLE_32_fgetxattr
TRACE_SYSCALL_TABLE(fgetxattr, fgetxattr, 231, 4)
#endif
#ifndef OVERRIDE_TABLE_32_listxattr
TRACE_SYSCALL_TABLE(listxattr, listxattr, 232, 3)
#endif
#ifndef OVERRIDE_TABLE_32_llistxattr
TRACE_SYSCALL_TABLE(llistxattr, llistxattr, 233, 3)
#endif
#ifndef OVERRIDE_TABLE_32_flistxattr
TRACE_SYSCALL_TABLE(flistxattr, flistxattr, 234, 3)
#endif
#ifndef OVERRIDE_TABLE_32_removexattr
TRACE_SYSCALL_TABLE(removexattr, removexattr, 235, 2)
#endif
#ifndef OVERRIDE_TABLE_32_lremovexattr
TRACE_SYSCALL_TABLE(lremovexattr, lremovexattr, 236, 2)
#endif
#ifndef OVERRIDE_TABLE_32_fremovexattr
TRACE_SYSCALL_TABLE(fremovexattr, fremovexattr, 237, 2)
#endif
#ifndef OVERRIDE_TABLE_32_sendfile64
TRACE_SYSCALL_TABLE(sendfile64, sendfile64, 239, 4)
#endif
#ifndef OVERRIDE_TABLE_32_futex_time32
TRACE_SYSCALL_TABLE(futex_time32, futex_time32, 240, 6)
#endif
#ifndef OVERRIDE_TABLE_32_sched_setaffinity
TRACE_SYSCALL_TABLE(sched_setaffinity, sched_setaffinity, 241, 3)
#endif
#ifndef OVERRIDE_TABLE_32_sched_getaffinity
TRACE_SYSCALL_TABLE(sched_getaffinity, sched_getaffinity, 242, 3)
#endif
#ifndef OVERRIDE_TABLE_32_io_setup
TRACE_SYSCALL_TABLE(io_setup, io_setup, 243, 2)
#endif
#ifndef OVERRIDE_TABLE_32_io_getevents_time32
TRACE_SYSCALL_TABLE(io_getevents_time32, io_getevents_time32, 245, 5)
#endif
#ifndef OVERRIDE_TABLE_32_io_submit
TRACE_SYSCALL_TABLE(io_submit, io_submit, 246, 3)
#endif
#ifndef OVERRIDE_TABLE_32_io_cancel
TRACE_SYSCALL_TABLE(io_cancel, io_cancel, 247, 3)
#endif
#ifndef OVERRIDE_TABLE_32_epoll_ctl
TRACE_SYSCALL_TABLE(epoll_ctl, epoll_ctl, 251, 4)
#endif
#ifndef OVERRIDE_TABLE_32_epoll_wait
TRACE_SYSCALL_TABLE(epoll_wait, epoll_wait, 252, 4)
#endif
#ifndef OVERRIDE_TABLE_32_set_tid_address
TRACE_SYSCALL_TABLE(set_tid_address, set_tid_address, 256, 1)
#endif
#ifndef OVERRIDE_TABLE_32_timer_create
TRACE_SYSCALL_TABLE(timer_create, timer_create, 257, 3)
#endif
#ifndef OVERRIDE_TABLE_32_timer_settime32
TRACE_SYSCALL_TABLE(timer_settime32, timer_settime32, 258, 4)
#endif
#ifndef OVERRIDE_TABLE_32_timer_gettime32
TRACE_SYSCALL_TABLE(timer_gettime32, timer_gettime32, 259, 2)
#endif
#ifndef OVERRIDE_TABLE_32_clock_settime32
TRACE_SYSCALL_TABLE(clock_settime32, clock_settime32, 262, 2)
#endif
#ifndef OVERRIDE_TABLE_32_clock_gettime32
TRACE_SYSCALL_TABLE(clock_gettime32, clock_gettime32, 263, 2)
#endif
#ifndef OVERRIDE_TABLE_32_clock_getres_time32
TRACE_SYSCALL_TABLE(clock_getres_time32, clock_getres_time32, 264, 2)
#endif
#ifndef OVERRIDE_TABLE_32_clock_nanosleep_time32
TRACE_SYSCALL_TABLE(clock_nanosleep_time32, clock_nanosleep_time32, 265, 4)
#endif
#ifndef OVERRIDE_TABLE_32_statfs64
TRACE_SYSCALL_TABLE(statfs64, statfs64, 266, 3)
#endif
#ifndef OVERRIDE_TABLE_32_fstatfs64
TRACE_SYSCALL_TABLE(fstatfs64, fstatfs64, 267, 3)
#endif
#ifndef OVERRIDE_TABLE_32_utimes_time32
TRACE_SYSCALL_TABLE(utimes_time32, utimes_time32, 269, 2)
#endif
#ifndef OVERRIDE_TABLE_32_pciconfig_read
TRACE_SYSCALL_TABLE(pciconfig_read, pciconfig_read, 272, 5)
#endif
#ifndef OVERRIDE_TABLE_32_pciconfig_write
TRACE_SYSCALL_TABLE(pciconfig_write, pciconfig_write, 273, 5)
#endif
#ifndef OVERRIDE_TABLE_32_mq_open
TRACE_SYSCALL_TABLE(mq_open, mq_open, 274, 4)
#endif
#ifndef OVERRIDE_TABLE_32_mq_unlink
TRACE_SYSCALL_TABLE(mq_unlink, mq_unlink, 275, 1)
#endif
#ifndef OVERRIDE_TABLE_32_mq_timedsend_time32
TRACE_SYSCALL_TABLE(mq_timedsend_time32, mq_timedsend_time32, 276, 5)
#endif
#ifndef OVERRIDE_TABLE_32_mq_timedreceive_time32
TRACE_SYSCALL_TABLE(mq_timedreceive_time32, mq_timedreceive_time32, 277, 5)
#endif
#ifndef OVERRIDE_TABLE_32_mq_notify
TRACE_SYSCALL_TABLE(mq_notify, mq_notify, 278, 2)
#endif
#ifndef OVERRIDE_TABLE_32_mq_getsetattr
TRACE_SYSCALL_TABLE(mq_getsetattr, mq_getsetattr, 279, 3)
#endif
#ifndef OVERRIDE_TABLE_32_waitid
TRACE_SYSCALL_TABLE(waitid, waitid, 280, 5)
#endif
#ifndef OVERRIDE_TABLE_32_bind
TRACE_SYSCALL_TABLE(bind, bind, 282, 3)
#endif
#ifndef OVERRIDE_TABLE_32_connect
TRACE_SYSCALL_TABLE(connect, connect, 283, 3)
#endif
#ifndef OVERRIDE_TABLE_32_accept
TRACE_SYSCALL_TABLE(accept, accept, 285, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getsockname
TRACE_SYSCALL_TABLE(getsockname, getsockname, 286, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getpeername
TRACE_SYSCALL_TABLE(getpeername, getpeername, 287, 3)
#endif
#ifndef OVERRIDE_TABLE_32_socketpair
TRACE_SYSCALL_TABLE(socketpair, socketpair, 288, 4)
#endif
#ifndef OVERRIDE_TABLE_32_send
TRACE_SYSCALL_TABLE(send, send, 289, 4)
#endif
#ifndef OVERRIDE_TABLE_32_sendto
TRACE_SYSCALL_TABLE(sendto, sendto, 290, 6)
#endif
#ifndef OVERRIDE_TABLE_32_recv
TRACE_SYSCALL_TABLE(recv, recv, 291, 4)
#endif
#ifndef OVERRIDE_TABLE_32_recvfrom
TRACE_SYSCALL_TABLE(recvfrom, recvfrom, 292, 6)
#endif
#ifndef OVERRIDE_TABLE_32_setsockopt
TRACE_SYSCALL_TABLE(setsockopt, setsockopt, 294, 5)
#endif
#ifndef OVERRIDE_TABLE_32_getsockopt
TRACE_SYSCALL_TABLE(getsockopt, getsockopt, 295, 5)
#endif
#ifndef OVERRIDE_TABLE_32_sendmsg
TRACE_SYSCALL_TABLE(sendmsg, sendmsg, 296, 3)
#endif
#ifndef OVERRIDE_TABLE_32_recvmsg
TRACE_SYSCALL_TABLE(recvmsg, recvmsg, 297, 3)
#endif
#ifndef OVERRIDE_TABLE_32_semop
TRACE_SYSCALL_TABLE(semop, semop, 298, 3)
#endif
#ifndef OVERRIDE_TABLE_32_msgsnd
TRACE_SYSCALL_TABLE(msgsnd, msgsnd, 301, 4)
#endif
#ifndef OVERRIDE_TABLE_32_msgrcv
TRACE_SYSCALL_TABLE(msgrcv, msgrcv, 302, 5)
#endif
#ifndef OVERRIDE_TABLE_32_old_msgctl
TRACE_SYSCALL_TABLE(old_msgctl, old_msgctl, 304, 3)
#endif
#ifndef OVERRIDE_TABLE_32_shmat
TRACE_SYSCALL_TABLE(shmat, shmat, 305, 3)
#endif
#ifndef OVERRIDE_TABLE_32_shmdt
TRACE_SYSCALL_TABLE(shmdt, shmdt, 306, 1)
#endif
#ifndef OVERRIDE_TABLE_32_old_shmctl
TRACE_SYSCALL_TABLE(old_shmctl, old_shmctl, 308, 3)
#endif
#ifndef OVERRIDE_TABLE_32_add_key
TRACE_SYSCALL_TABLE(add_key, add_key, 309, 5)
#endif
#ifndef OVERRIDE_TABLE_32_request_key
TRACE_SYSCALL_TABLE(request_key, request_key, 310, 4)
#endif
#ifndef OVERRIDE_TABLE_32_semtimedop_time32
TRACE_SYSCALL_TABLE(semtimedop_time32, semtimedop_time32, 312, 4)
#endif
#ifndef OVERRIDE_TABLE_32_inotify_add_watch
TRACE_SYSCALL_TABLE(inotify_add_watch, inotify_add_watch, 317, 3)
#endif
#ifndef OVERRIDE_TABLE_32_openat
TRACE_SYSCALL_TABLE(openat, openat, 322, 4)
#endif
#ifndef OVERRIDE_TABLE_32_mkdirat
TRACE_SYSCALL_TABLE(mkdirat, mkdirat, 323, 3)
#endif
#ifndef OVERRIDE_TABLE_32_mknodat
TRACE_SYSCALL_TABLE(mknodat, mknodat, 324, 4)
#endif
#ifndef OVERRIDE_TABLE_32_fchownat
TRACE_SYSCALL_TABLE(fchownat, fchownat, 325, 5)
#endif
#ifndef OVERRIDE_TABLE_32_futimesat_time32
TRACE_SYSCALL_TABLE(futimesat_time32, futimesat_time32, 326, 3)
#endif
#ifndef OVERRIDE_TABLE_32_fstatat64
TRACE_SYSCALL_TABLE(fstatat64, fstatat64, 327, 4)
#endif
#ifndef OVERRIDE_TABLE_32_unlinkat
TRACE_SYSCALL_TABLE(unlinkat, unlinkat, 328, 3)
#endif
#ifndef OVERRIDE_TABLE_32_renameat
TRACE_SYSCALL_TABLE(renameat, renameat, 329, 4)
#endif
#ifndef OVERRIDE_TABLE_32_linkat
TRACE_SYSCALL_TABLE(linkat, linkat, 330, 5)
#endif
#ifndef OVERRIDE_TABLE_32_symlinkat
TRACE_SYSCALL_TABLE(symlinkat, symlinkat, 331, 3)
#endif
#ifndef OVERRIDE_TABLE_32_readlinkat
TRACE_SYSCALL_TABLE(readlinkat, readlinkat, 332, 4)
#endif
#ifndef OVERRIDE_TABLE_32_fchmodat
TRACE_SYSCALL_TABLE(fchmodat, fchmodat, 333, 3)
#endif
#ifndef OVERRIDE_TABLE_32_faccessat
TRACE_SYSCALL_TABLE(faccessat, faccessat, 334, 3)
#endif
#ifndef OVERRIDE_TABLE_32_pselect6_time32
TRACE_SYSCALL_TABLE(pselect6_time32, pselect6_time32, 335, 6)
#endif
#ifndef OVERRIDE_TABLE_32_ppoll_time32
TRACE_SYSCALL_TABLE(ppoll_time32, ppoll_time32, 336, 5)
#endif
#ifndef OVERRIDE_TABLE_32_set_robust_list
TRACE_SYSCALL_TABLE(set_robust_list, set_robust_list, 338, 2)
#endif
#ifndef OVERRIDE_TABLE_32_get_robust_list
TRACE_SYSCALL_TABLE(get_robust_list, get_robust_list, 339, 3)
#endif
#ifndef OVERRIDE_TABLE_32_splice
TRACE_SYSCALL_TABLE(splice, splice, 340, 6)
#endif
#ifndef OVERRIDE_TABLE_32_vmsplice
TRACE_SYSCALL_TABLE(vmsplice, vmsplice, 343, 4)
#endif
#ifndef OVERRIDE_TABLE_32_getcpu
TRACE_SYSCALL_TABLE(getcpu, getcpu, 345, 3)
#endif
#ifndef OVERRIDE_TABLE_32_epoll_pwait
TRACE_SYSCALL_TABLE(epoll_pwait, epoll_pwait, 346, 6)
#endif
#ifndef OVERRIDE_TABLE_32_kexec_load
TRACE_SYSCALL_TABLE(kexec_load, kexec_load, 347, 4)
#endif
#ifndef OVERRIDE_TABLE_32_utimensat_time32
TRACE_SYSCALL_TABLE(utimensat_time32, utimensat_time32, 348, 4)
#endif
#ifndef OVERRIDE_TABLE_32_signalfd
TRACE_SYSCALL_TABLE(signalfd, signalfd, 349, 3)
#endif
#ifndef OVERRIDE_TABLE_32_timerfd_settime32
TRACE_SYSCALL_TABLE(timerfd_settime32, timerfd_settime32, 353, 4)
#endif
#ifndef OVERRIDE_TABLE_32_timerfd_gettime32
TRACE_SYSCALL_TABLE(timerfd_gettime32, timerfd_gettime32, 354, 2)
#endif
#ifndef OVERRIDE_TABLE_32_signalfd4
TRACE_SYSCALL_TABLE(signalfd4, signalfd4, 355, 4)
#endif
#ifndef OVERRIDE_TABLE_32_pipe2
TRACE_SYSCALL_TABLE(pipe2, pipe2, 359, 2)
#endif
#ifndef OVERRIDE_TABLE_32_preadv
TRACE_SYSCALL_TABLE(preadv, preadv, 361, 5)
#endif
#ifndef OVERRIDE_TABLE_32_pwritev
TRACE_SYSCALL_TABLE(pwritev, pwritev, 362, 5)
#endif
#ifndef OVERRIDE_TABLE_32_rt_tgsigqueueinfo
TRACE_SYSCALL_TABLE(rt_tgsigqueueinfo, rt_tgsigqueueinfo, 363, 4)
#endif
#ifndef OVERRIDE_TABLE_32_perf_event_open
TRACE_SYSCALL_TABLE(perf_event_open, perf_event_open, 364, 5)
#endif
#ifndef OVERRIDE_TABLE_32_recvmmsg_time32
TRACE_SYSCALL_TABLE(recvmmsg_time32, recvmmsg_time32, 365, 5)
#endif
#ifndef OVERRIDE_TABLE_32_accept4
TRACE_SYSCALL_TABLE(accept4, accept4, 366, 4)
#endif
#ifndef OVERRIDE_TABLE_32_fanotify_mark
TRACE_SYSCALL_TABLE(fanotify_mark, fanotify_mark, 368, 5)
#endif
#ifndef OVERRIDE_TABLE_32_prlimit64
TRACE_SYSCALL_TABLE(prlimit64, prlimit64, 369, 4)
#endif
#ifndef OVERRIDE_TABLE_32_name_to_handle_at
TRACE_SYSCALL_TABLE(name_to_handle_at, name_to_handle_at, 370, 5)
#endif
#ifndef OVERRIDE_TABLE_32_open_by_handle_at
TRACE_SYSCALL_TABLE(open_by_handle_at, open_by_handle_at, 371, 3)
#endif
#ifndef OVERRIDE_TABLE_32_clock_adjtime32
TRACE_SYSCALL_TABLE(clock_adjtime32, clock_adjtime32, 372, 2)
#endif
#ifndef OVERRIDE_TABLE_32_sendmmsg
TRACE_SYSCALL_TABLE(sendmmsg, sendmmsg, 374, 4)
#endif
#ifndef OVERRIDE_TABLE_32_process_vm_readv
TRACE_SYSCALL_TABLE(process_vm_readv, process_vm_readv, 376, 6)
#endif
#ifndef OVERRIDE_TABLE_32_process_vm_writev
TRACE_SYSCALL_TABLE(process_vm_writev, process_vm_writev, 377, 6)
#endif
#ifndef OVERRIDE_TABLE_32_finit_module
TRACE_SYSCALL_TABLE(finit_module, finit_module, 379, 3)
#endif
#ifndef OVERRIDE_TABLE_32_sched_setattr
TRACE_SYSCALL_TABLE(sched_setattr, sched_setattr, 380, 3)
#endif
#ifndef OVERRIDE_TABLE_32_sched_getattr
TRACE_SYSCALL_TABLE(sched_getattr, sched_getattr, 381, 4)
#endif
#ifndef OVERRIDE_TABLE_32_renameat2
TRACE_SYSCALL_TABLE(renameat2, renameat2, 382, 5)
#endif
#ifndef OVERRIDE_TABLE_32_seccomp
TRACE_SYSCALL_TABLE(seccomp, seccomp, 383, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getrandom
TRACE_SYSCALL_TABLE(getrandom, getrandom, 384, 3)
#endif
#ifndef OVERRIDE_TABLE_32_memfd_create
TRACE_SYSCALL_TABLE(memfd_create, memfd_create, 385, 2)
#endif
#ifndef OVERRIDE_TABLE_32_bpf
TRACE_SYSCALL_TABLE(bpf, bpf, 386, 3)
#endif
#ifndef OVERRIDE_TABLE_32_execveat
TRACE_SYSCALL_TABLE(execveat, execveat, 387, 5)
#endif
#ifndef OVERRIDE_TABLE_32_copy_file_range
TRACE_SYSCALL_TABLE(copy_file_range, copy_file_range, 391, 6)
#endif
#ifndef OVERRIDE_TABLE_32_preadv2
TRACE_SYSCALL_TABLE(preadv2, preadv2, 392, 6)
#endif
#ifndef OVERRIDE_TABLE_32_pwritev2
TRACE_SYSCALL_TABLE(pwritev2, pwritev2, 393, 6)
#endif
#ifndef OVERRIDE_TABLE_32_statx
TRACE_SYSCALL_TABLE(statx, statx, 397, 5)
#endif
#ifndef OVERRIDE_TABLE_32_rseq
TRACE_SYSCALL_TABLE(rseq, rseq, 398, 4)
#endif
#ifndef OVERRIDE_TABLE_32_io_pgetevents_time32
TRACE_SYSCALL_TABLE(io_pgetevents_time32, io_pgetevents_time32, 399, 6)
#endif
#ifndef OVERRIDE_TABLE_32_clock_gettime
TRACE_SYSCALL_TABLE(clock_gettime, clock_gettime, 403, 2)
#endif
#ifndef OVERRIDE_TABLE_32_clock_settime
TRACE_SYSCALL_TABLE(clock_settime, clock_settime, 404, 2)
#endif
#ifndef OVERRIDE_TABLE_32_clock_adjtime
TRACE_SYSCALL_TABLE(clock_adjtime, clock_adjtime, 405, 2)
#endif
#ifndef OVERRIDE_TABLE_32_clock_getres
TRACE_SYSCALL_TABLE(clock_getres, clock_getres, 406, 2)
#endif
#ifndef OVERRIDE_TABLE_32_clock_nanosleep
TRACE_SYSCALL_TABLE(clock_nanosleep, clock_nanosleep, 407, 4)
#endif
#ifndef OVERRIDE_TABLE_32_timer_gettime
TRACE_SYSCALL_TABLE(timer_gettime, timer_gettime, 408, 2)
#endif
#ifndef OVERRIDE_TABLE_32_timer_settime
TRACE_SYSCALL_TABLE(timer_settime, timer_settime, 409, 4)
#endif
#ifndef OVERRIDE_TABLE_32_timerfd_gettime
TRACE_SYSCALL_TABLE(timerfd_gettime, timerfd_gettime, 410, 2)
#endif
#ifndef OVERRIDE_TABLE_32_timerfd_settime
TRACE_SYSCALL_TABLE(timerfd_settime, timerfd_settime, 411, 4)
#endif
#ifndef OVERRIDE_TABLE_32_utimensat
TRACE_SYSCALL_TABLE(utimensat, utimensat, 412, 4)
#endif
#ifndef OVERRIDE_TABLE_32_pselect6
TRACE_SYSCALL_TABLE(pselect6, pselect6, 413, 6)
#endif
#ifndef OVERRIDE_TABLE_32_ppoll
TRACE_SYSCALL_TABLE(ppoll, ppoll, 414, 5)
#endif
#ifndef OVERRIDE_TABLE_32_io_pgetevents
TRACE_SYSCALL_TABLE(io_pgetevents, io_pgetevents, 416, 6)
#endif
#ifndef OVERRIDE_TABLE_32_recvmmsg
TRACE_SYSCALL_TABLE(recvmmsg, recvmmsg, 417, 5)
#endif
#ifndef OVERRIDE_TABLE_32_mq_timedsend
TRACE_SYSCALL_TABLE(mq_timedsend, mq_timedsend, 418, 5)
#endif
#ifndef OVERRIDE_TABLE_32_mq_timedreceive
TRACE_SYSCALL_TABLE(mq_timedreceive, mq_timedreceive, 419, 5)
#endif
#ifndef OVERRIDE_TABLE_32_semtimedop
TRACE_SYSCALL_TABLE(semtimedop, semtimedop, 420, 4)
#endif
#ifndef OVERRIDE_TABLE_32_rt_sigtimedwait
TRACE_SYSCALL_TABLE(rt_sigtimedwait, rt_sigtimedwait, 421, 4)
#endif
#ifndef OVERRIDE_TABLE_32_futex
TRACE_SYSCALL_TABLE(futex, futex, 422, 6)
#endif
#ifndef OVERRIDE_TABLE_32_sched_rr_get_interval
TRACE_SYSCALL_TABLE(sched_rr_get_interval, sched_rr_get_interval, 423, 2)
#endif
#ifndef OVERRIDE_TABLE_32_pidfd_send_signal
TRACE_SYSCALL_TABLE(pidfd_send_signal, pidfd_send_signal, 424, 4)
#endif
#ifndef OVERRIDE_TABLE_32_io_uring_setup
TRACE_SYSCALL_TABLE(io_uring_setup, io_uring_setup, 425, 2)
#endif
#ifndef OVERRIDE_TABLE_32_io_uring_enter
TRACE_SYSCALL_TABLE(io_uring_enter, io_uring_enter, 426, 6)
#endif
#ifndef OVERRIDE_TABLE_32_io_uring_register
TRACE_SYSCALL_TABLE(io_uring_register, io_uring_register, 427, 4)
#endif
#ifndef OVERRIDE_TABLE_32_open_tree
TRACE_SYSCALL_TABLE(open_tree, open_tree, 428, 3)
#endif
#ifndef OVERRIDE_TABLE_32_move_mount
TRACE_SYSCALL_TABLE(move_mount, move_mount, 429, 5)
#endif
#ifndef OVERRIDE_TABLE_32_fsopen
TRACE_SYSCALL_TABLE(fsopen, fsopen, 430, 2)
#endif
#ifndef OVERRIDE_TABLE_32_fsconfig
TRACE_SYSCALL_TABLE(fsconfig, fsconfig, 431, 5)
#endif
#ifndef OVERRIDE_TABLE_32_fspick
TRACE_SYSCALL_TABLE(fspick, fspick, 433, 3)
#endif
#ifndef OVERRIDE_TABLE_32_clone3
TRACE_SYSCALL_TABLE(clone3, clone3, 435, 2)
#endif
#ifndef OVERRIDE_TABLE_32_openat2
TRACE_SYSCALL_TABLE(openat2, openat2, 437, 4)
#endif
#ifndef OVERRIDE_TABLE_32_faccessat2
TRACE_SYSCALL_TABLE(faccessat2, faccessat2, 439, 4)
#endif
#ifndef OVERRIDE_TABLE_32_process_madvise
TRACE_SYSCALL_TABLE(process_madvise, process_madvise, 440, 5)
#endif
#ifndef OVERRIDE_TABLE_32_epoll_pwait2
TRACE_SYSCALL_TABLE(epoll_pwait2, epoll_pwait2, 441, 6)
#endif
#ifndef OVERRIDE_TABLE_32_mount_setattr
TRACE_SYSCALL_TABLE(mount_setattr, mount_setattr, 442, 5)
#endif
#ifndef OVERRIDE_TABLE_32_quotactl_fd
TRACE_SYSCALL_TABLE(quotactl_fd, quotactl_fd, 443, 4)
#endif
#ifndef OVERRIDE_TABLE_32_landlock_create_ruleset
TRACE_SYSCALL_TABLE(landlock_create_ruleset, landlock_create_ruleset, 444, 3)
#endif
#ifndef OVERRIDE_TABLE_32_landlock_add_rule
TRACE_SYSCALL_TABLE(landlock_add_rule, landlock_add_rule, 445, 4)
#endif
#ifndef OVERRIDE_TABLE_32_futex_waitv
TRACE_SYSCALL_TABLE(futex_waitv, futex_waitv, 449, 5)
#endif

#endif /* CREATE_SYSCALL_TABLE */
