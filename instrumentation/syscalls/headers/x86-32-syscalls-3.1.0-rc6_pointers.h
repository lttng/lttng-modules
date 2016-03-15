/* THIS FILE IS AUTO-GENERATED. DO NOT EDIT */
#ifndef CREATE_SYSCALL_TABLE

#if !defined(_TRACE_SYSCALLS_POINTERS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SYSCALLS_POINTERS_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/syscalls.h>
#include "x86-32-syscalls-3.1.0-rc6_pointers_override.h"
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
#ifndef OVERRIDE_32_time
SC_LTTNG_TRACEPOINT_EVENT(time,
	TP_PROTO(sc_exit(long ret,) time_t * tloc),
	TP_ARGS(sc_exit(ret,) tloc),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(time_t *, tloc, tloc)))
)
#endif
#ifndef OVERRIDE_32_oldumount
SC_LTTNG_TRACEPOINT_EVENT(oldumount,
	TP_PROTO(sc_exit(long ret,) char * name),
	TP_ARGS(sc_exit(ret,) name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_user_string(name, name)))
)
#endif
#ifndef OVERRIDE_32_stime
SC_LTTNG_TRACEPOINT_EVENT(stime,
	TP_PROTO(sc_exit(long ret,) time_t * tptr),
	TP_ARGS(sc_exit(ret,) tptr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(time_t *, tptr, tptr)))
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
#ifndef OVERRIDE_32_olduname
SC_LTTNG_TRACEPOINT_EVENT(olduname,
	TP_PROTO(sc_exit(long ret,) struct oldold_utsname * name),
	TP_ARGS(sc_exit(ret,) name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct oldold_utsname *, name, name)))
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
	TP_PROTO(sc_exit(long ret,) old_sigset_t * set),
	TP_ARGS(sc_exit(ret,) set),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_sigset_t *, set, set)))
)
#endif
#ifndef OVERRIDE_32_old_select
SC_LTTNG_TRACEPOINT_EVENT(old_select,
	TP_PROTO(sc_exit(long ret,) struct sel_arg_struct * arg),
	TP_ARGS(sc_exit(ret,) arg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct sel_arg_struct *, arg, arg)))
)
#endif
#ifndef OVERRIDE_32_uselib
SC_LTTNG_TRACEPOINT_EVENT(uselib,
	TP_PROTO(sc_exit(long ret,) const char * library),
	TP_ARGS(sc_exit(ret,) library),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(const char *, library, library)))
)
#endif
#ifndef OVERRIDE_32_old_mmap
SC_LTTNG_TRACEPOINT_EVENT(old_mmap,
	TP_PROTO(sc_exit(long ret,) struct mmap_arg_struct * arg),
	TP_ARGS(sc_exit(ret,) arg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct mmap_arg_struct *, arg, arg)))
)
#endif
#ifndef OVERRIDE_32_uname
SC_LTTNG_TRACEPOINT_EVENT(uname,
	TP_PROTO(sc_exit(long ret,) struct old_utsname * name),
	TP_ARGS(sc_exit(ret,) name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct old_utsname *, name, name)))
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
#ifndef OVERRIDE_32_adjtimex
SC_LTTNG_TRACEPOINT_EVENT(adjtimex,
	TP_PROTO(sc_exit(long ret,) struct timex * txc_p),
	TP_ARGS(sc_exit(ret,) txc_p),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct timex *, txc_p, txc_p)))
)
#endif
#ifndef OVERRIDE_32_sysctl
SC_LTTNG_TRACEPOINT_EVENT(sysctl,
	TP_PROTO(sc_exit(long ret,) struct __sysctl_args * args),
	TP_ARGS(sc_exit(ret,) args),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct __sysctl_args *, args, args)))
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
#ifndef OVERRIDE_32_creat
SC_LTTNG_TRACEPOINT_EVENT(creat,
	TP_PROTO(sc_exit(long ret,) const char * pathname, int mode),
	TP_ARGS(sc_exit(ret,) pathname, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(int, mode, mode)))
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
	TP_PROTO(sc_exit(long ret,) const char * filename, mode_t mode),
	TP_ARGS(sc_exit(ret,) filename, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(mode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_stat
SC_LTTNG_TRACEPOINT_EVENT(stat,
	TP_PROTO(sc_exit(long ret,) const char * filename, struct __old_kernel_stat * statbuf),
	TP_ARGS(sc_exit(ret,) filename, statbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(struct __old_kernel_stat *, statbuf, statbuf)))
)
#endif
#ifndef OVERRIDE_32_fstat
SC_LTTNG_TRACEPOINT_EVENT(fstat,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct __old_kernel_stat * statbuf),
	TP_ARGS(sc_exit(ret,) fd, statbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, fd, fd)) sc_inout(ctf_integer(struct __old_kernel_stat *, statbuf, statbuf)))
)
#endif
#ifndef OVERRIDE_32_utime
SC_LTTNG_TRACEPOINT_EVENT(utime,
	TP_PROTO(sc_exit(long ret,) char * filename, struct utimbuf * times),
	TP_ARGS(sc_exit(ret,) filename, times),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(struct utimbuf *, times, times)))
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
	TP_PROTO(sc_exit(long ret,) const char * pathname, int mode),
	TP_ARGS(sc_exit(ret,) pathname, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(int, mode, mode)))
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
#ifndef OVERRIDE_32_old_getrlimit
SC_LTTNG_TRACEPOINT_EVENT(old_getrlimit,
	TP_PROTO(sc_exit(long ret,) unsigned int resource, struct rlimit * rlim),
	TP_ARGS(sc_exit(ret,) resource, rlim),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, resource, resource)) sc_inout(ctf_integer(struct rlimit *, rlim, rlim)))
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
	TP_PROTO(sc_exit(long ret,) struct timeval * tv, struct timezone * tz),
	TP_ARGS(sc_exit(ret,) tv, tz),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(struct timeval *, tv, tv)) sc_out(ctf_integer(struct timezone *, tz, tz)))
)
#endif
#ifndef OVERRIDE_32_settimeofday
SC_LTTNG_TRACEPOINT_EVENT(settimeofday,
	TP_PROTO(sc_exit(long ret,) struct timeval * tv, struct timezone * tz),
	TP_ARGS(sc_exit(ret,) tv, tz),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(struct timeval *, tv, tv)) sc_in(ctf_integer(struct timezone *, tz, tz)))
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
#ifndef OVERRIDE_32_lstat
SC_LTTNG_TRACEPOINT_EVENT(lstat,
	TP_PROTO(sc_exit(long ret,) const char * filename, struct __old_kernel_stat * statbuf),
	TP_ARGS(sc_exit(ret,) filename, statbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(struct __old_kernel_stat *, statbuf, statbuf)))
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
#ifndef OVERRIDE_32_socketcall
SC_LTTNG_TRACEPOINT_EVENT(socketcall,
	TP_PROTO(sc_exit(long ret,) int call, unsigned long * args),
	TP_ARGS(sc_exit(ret,) call, args),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(int, call, call)) sc_inout(ctf_integer(unsigned long *, args, args)))
)
#endif
#ifndef OVERRIDE_32_getitimer
SC_LTTNG_TRACEPOINT_EVENT(getitimer,
	TP_PROTO(sc_exit(long ret,) int which, struct itimerval * value),
	TP_ARGS(sc_exit(ret,) which, value),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_out(ctf_integer(struct itimerval *, value, value)))
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
#ifndef OVERRIDE_32_sched_rr_get_interval
SC_LTTNG_TRACEPOINT_EVENT(sched_rr_get_interval,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct timespec * interval),
	TP_ARGS(sc_exit(ret,) pid, interval),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_out(ctf_integer(struct timespec *, interval, interval)))
)
#endif
#ifndef OVERRIDE_32_nanosleep
SC_LTTNG_TRACEPOINT_EVENT(nanosleep,
	TP_PROTO(sc_exit(long ret,) struct timespec * rqtp, struct timespec * rmtp),
	TP_ARGS(sc_exit(ret,) rqtp, rmtp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(struct timespec *, rqtp, rqtp)) sc_out(ctf_integer(struct timespec *, rmtp, rmtp)))
)
#endif
#ifndef OVERRIDE_32_rt_sigpending
SC_LTTNG_TRACEPOINT_EVENT(rt_sigpending,
	TP_PROTO(sc_exit(long ret,) sigset_t * set, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) set, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(sigset_t *, set, set)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
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
#ifndef OVERRIDE_32_getrlimit
SC_LTTNG_TRACEPOINT_EVENT(getrlimit,
	TP_PROTO(sc_exit(long ret,) unsigned int resource, struct rlimit * rlim),
	TP_ARGS(sc_exit(ret,) resource, rlim),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, resource, resource)) sc_out(ctf_integer(struct rlimit *, rlim, rlim)))
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
#ifndef OVERRIDE_32_timer_gettime
SC_LTTNG_TRACEPOINT_EVENT(timer_gettime,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id, struct itimerspec * setting),
	TP_ARGS(sc_exit(ret,) timer_id, setting),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(timer_t, timer_id, timer_id)) sc_out(ctf_integer(struct itimerspec *, setting, setting)))
)
#endif
#ifndef OVERRIDE_32_clock_settime
SC_LTTNG_TRACEPOINT_EVENT(clock_settime,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, const struct timespec * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_in(ctf_integer(const struct timespec *, tp, tp)))
)
#endif
#ifndef OVERRIDE_32_clock_gettime
SC_LTTNG_TRACEPOINT_EVENT(clock_gettime,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct timespec * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_out(ctf_integer(struct timespec *, tp, tp)))
)
#endif
#ifndef OVERRIDE_32_clock_getres
SC_LTTNG_TRACEPOINT_EVENT(clock_getres,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct timespec * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_out(ctf_integer(struct timespec *, tp, tp)))
)
#endif
#ifndef OVERRIDE_32_utimes
SC_LTTNG_TRACEPOINT_EVENT(utimes,
	TP_PROTO(sc_exit(long ret,) char * filename, struct timeval * utimes),
	TP_ARGS(sc_exit(ret,) filename, utimes),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(struct timeval *, utimes, utimes)))
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
#ifndef OVERRIDE_32_timerfd_gettime
SC_LTTNG_TRACEPOINT_EVENT(timerfd_gettime,
	TP_PROTO(sc_exit(long ret,) int ufd, struct itimerspec * otmr),
	TP_ARGS(sc_exit(ret,) ufd, otmr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, ufd, ufd)) sc_out(ctf_integer(struct itimerspec *, otmr, otmr)))
)
#endif
#ifndef OVERRIDE_32_pipe2
SC_LTTNG_TRACEPOINT_EVENT(pipe2,
	TP_PROTO(sc_exit(long ret,) int * fildes, int flags),
	TP_ARGS(sc_exit(ret,) fildes, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(int *, fildes, fildes)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_clock_adjtime
SC_LTTNG_TRACEPOINT_EVENT(clock_adjtime,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct timex * utx),
	TP_ARGS(sc_exit(ret,) which_clock, utx),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_inout(ctf_integer(struct timex *, utx, utx)))
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
	TP_PROTO(sc_exit(long ret,) const char * filename, int flags, int mode),
	TP_ARGS(sc_exit(ret,) filename, flags, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(int, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_waitpid
SC_LTTNG_TRACEPOINT_EVENT(waitpid,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int * stat_addr, int options),
	TP_ARGS(sc_exit(ret,) pid, stat_addr, options),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(pid_t, pid, pid)) sc_inout(ctf_integer(int *, stat_addr, stat_addr)) sc_inout(ctf_integer(int, options, options)))
)
#endif
#ifndef OVERRIDE_32_mknod
SC_LTTNG_TRACEPOINT_EVENT(mknod,
	TP_PROTO(sc_exit(long ret,) const char * filename, int mode, unsigned dev),
	TP_ARGS(sc_exit(ret,) filename, mode, dev),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(int, mode, mode)) sc_in(ctf_integer(unsigned, dev, dev)))
)
#endif
#ifndef OVERRIDE_32_lchown16
SC_LTTNG_TRACEPOINT_EVENT(lchown16,
	TP_PROTO(sc_exit(long ret,) const char * filename, old_uid_t user, old_gid_t group),
	TP_ARGS(sc_exit(ret,) filename, user, group),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_user_string(filename, filename)) sc_inout(ctf_integer(old_uid_t, user, user)) sc_inout(ctf_integer(old_gid_t, group, group)))
)
#endif
#ifndef OVERRIDE_32_readlink
SC_LTTNG_TRACEPOINT_EVENT(readlink,
	TP_PROTO(sc_exit(long ret,) const char * path, char * buf, int bufsiz),
	TP_ARGS(sc_exit(ret,) path, buf, bufsiz),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(path, path)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(int, bufsiz, bufsiz)))
)
#endif
#ifndef OVERRIDE_32_old_readdir
SC_LTTNG_TRACEPOINT_EVENT(old_readdir,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct old_linux_dirent * dirent, unsigned int count),
	TP_ARGS(sc_exit(ret,) fd, dirent, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, fd, fd)) sc_inout(ctf_integer(struct old_linux_dirent *, dirent, dirent)) sc_inout(ctf_integer(unsigned int, count, count)))
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
	TP_PROTO(sc_exit(long ret,) int which, struct itimerval * value, struct itimerval * ovalue),
	TP_ARGS(sc_exit(ret,) which, value, ovalue),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(struct itimerval *, value, value)) sc_out(ctf_integer(struct itimerval *, ovalue, ovalue)))
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
	TP_PROTO(sc_exit(long ret,) old_uid_t * ruid, old_uid_t * euid, old_uid_t * suid),
	TP_ARGS(sc_exit(ret,) ruid, euid, suid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_uid_t *, ruid, ruid)) sc_inout(ctf_integer(old_uid_t *, euid, euid)) sc_inout(ctf_integer(old_uid_t *, suid, suid)))
)
#endif
#ifndef OVERRIDE_32_poll
SC_LTTNG_TRACEPOINT_EVENT(poll,
	TP_PROTO(sc_exit(long ret,) struct pollfd * ufds, unsigned int nfds, long timeout_msecs),
	TP_ARGS(sc_exit(ret,) ufds, nfds, timeout_msecs),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct pollfd *, ufds, ufds)) sc_in(ctf_integer(unsigned int, nfds, nfds)) sc_in(ctf_integer(long, timeout_msecs, timeout_msecs)))
)
#endif
#ifndef OVERRIDE_32_getresgid16
SC_LTTNG_TRACEPOINT_EVENT(getresgid16,
	TP_PROTO(sc_exit(long ret,) old_gid_t * rgid, old_gid_t * egid, old_gid_t * sgid),
	TP_ARGS(sc_exit(ret,) rgid, egid, sgid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(old_gid_t *, rgid, rgid)) sc_inout(ctf_integer(old_gid_t *, egid, egid)) sc_inout(ctf_integer(old_gid_t *, sgid, sgid)))
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
	TP_PROTO(sc_exit(long ret,) uid_t * ruid, uid_t * euid, uid_t * suid),
	TP_ARGS(sc_exit(ret,) ruid, euid, suid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(uid_t *, ruid, ruid)) sc_out(ctf_integer(uid_t *, euid, euid)) sc_out(ctf_integer(uid_t *, suid, suid)))
)
#endif
#ifndef OVERRIDE_32_getresgid
SC_LTTNG_TRACEPOINT_EVENT(getresgid,
	TP_PROTO(sc_exit(long ret,) gid_t * rgid, gid_t * egid, gid_t * sgid),
	TP_ARGS(sc_exit(ret,) rgid, egid, sgid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(gid_t *, rgid, rgid)) sc_out(ctf_integer(gid_t *, egid, egid)) sc_out(ctf_integer(gid_t *, sgid, sgid)))
)
#endif
#ifndef OVERRIDE_32_chown
SC_LTTNG_TRACEPOINT_EVENT(chown,
	TP_PROTO(sc_exit(long ret,) const char * filename, uid_t user, gid_t group),
	TP_ARGS(sc_exit(ret,) filename, user, group),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(uid_t, user, user)) sc_in(ctf_integer(gid_t, group, group)))
)
#endif
#ifndef OVERRIDE_32_mincore
SC_LTTNG_TRACEPOINT_EVENT(mincore,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, unsigned char * vec),
	TP_ARGS(sc_exit(ret,) start, len, vec),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(size_t, len, len)) sc_out(ctf_integer(unsigned char *, vec, vec)))
)
#endif
#ifndef OVERRIDE_32_getdents64
SC_LTTNG_TRACEPOINT_EVENT(getdents64,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct linux_dirent64 * dirent, unsigned int count),
	TP_ARGS(sc_exit(ret,) fd, dirent, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(struct linux_dirent64 *, dirent, dirent)) sc_in(ctf_integer(unsigned int, count, count)))
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
#ifndef OVERRIDE_32_inotify_add_watch
SC_LTTNG_TRACEPOINT_EVENT(inotify_add_watch,
	TP_PROTO(sc_exit(long ret,) int fd, const char * pathname, u32 mask),
	TP_ARGS(sc_exit(ret,) fd, pathname, mask),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(u32, mask, mask)))
)
#endif
#ifndef OVERRIDE_32_mkdirat
SC_LTTNG_TRACEPOINT_EVENT(mkdirat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * pathname, int mode),
	TP_ARGS(sc_exit(ret,) dfd, pathname, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(int, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_futimesat
SC_LTTNG_TRACEPOINT_EVENT(futimesat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, struct timeval * utimes),
	TP_ARGS(sc_exit(ret,) dfd, filename, utimes),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(struct timeval *, utimes, utimes)))
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
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, mode_t mode),
	TP_ARGS(sc_exit(ret,) dfd, filename, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(mode_t, mode, mode)))
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
#ifndef OVERRIDE_32_rt_sigtimedwait
SC_LTTNG_TRACEPOINT_EVENT(rt_sigtimedwait,
	TP_PROTO(sc_exit(long ret,) const sigset_t * uthese, siginfo_t * uinfo, const struct timespec * uts, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) uthese, uinfo, uts, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(const sigset_t *, uthese, uthese)) sc_out(ctf_integer(siginfo_t *, uinfo, uinfo)) sc_in(ctf_integer(const struct timespec *, uts, uts)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
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
#ifndef OVERRIDE_32_timer_settime
SC_LTTNG_TRACEPOINT_EVENT(timer_settime,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id, int flags, const struct itimerspec * new_setting, struct itimerspec * old_setting),
	TP_ARGS(sc_exit(ret,) timer_id, flags, new_setting, old_setting),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(timer_t, timer_id, timer_id)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(const struct itimerspec *, new_setting, new_setting)) sc_out(ctf_integer(struct itimerspec *, old_setting, old_setting)))
)
#endif
#ifndef OVERRIDE_32_clock_nanosleep
SC_LTTNG_TRACEPOINT_EVENT(clock_nanosleep,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, int flags, const struct timespec * rqtp, struct timespec * rmtp),
	TP_ARGS(sc_exit(ret,) which_clock, flags, rqtp, rmtp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(const struct timespec *, rqtp, rqtp)) sc_out(ctf_integer(struct timespec *, rmtp, rmtp)))
)
#endif
#ifndef OVERRIDE_32_mq_open
SC_LTTNG_TRACEPOINT_EVENT(mq_open,
	TP_PROTO(sc_exit(long ret,) const char * u_name, int oflag, mode_t mode, struct mq_attr * u_attr),
	TP_ARGS(sc_exit(ret,) u_name, oflag, mode, u_attr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(u_name, u_name)) sc_in(ctf_integer(int, oflag, oflag)) sc_in(ctf_integer(mode_t, mode, mode)) sc_in(ctf_integer(struct mq_attr *, u_attr, u_attr)))
)
#endif
#ifndef OVERRIDE_32_kexec_load
SC_LTTNG_TRACEPOINT_EVENT(kexec_load,
	TP_PROTO(sc_exit(long ret,) unsigned long entry, unsigned long nr_segments, struct kexec_segment * segments, unsigned long flags),
	TP_ARGS(sc_exit(ret,) entry, nr_segments, segments, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, entry, entry)) sc_in(ctf_integer(unsigned long, nr_segments, nr_segments)) sc_in(ctf_integer(struct kexec_segment *, segments, segments)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_request_key
SC_LTTNG_TRACEPOINT_EVENT(request_key,
	TP_PROTO(sc_exit(long ret,) const char * _type, const char * _description, const char * _callout_info, key_serial_t destringid),
	TP_ARGS(sc_exit(ret,) _type, _description, _callout_info, destringid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(_type, _type)) sc_in(ctf_integer(const char *, _description, _description)) sc_in(ctf_integer(const char *, _callout_info, _callout_info)) sc_in(ctf_integer(key_serial_t, destringid, destringid)))
)
#endif
#ifndef OVERRIDE_32_openat
SC_LTTNG_TRACEPOINT_EVENT(openat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, int flags, int mode),
	TP_ARGS(sc_exit(ret,) dfd, filename, flags, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(int, mode, mode)))
)
#endif
#ifndef OVERRIDE_32_mknodat
SC_LTTNG_TRACEPOINT_EVENT(mknodat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, int mode, unsigned dev),
	TP_ARGS(sc_exit(ret,) dfd, filename, mode, dev),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(int, mode, mode)) sc_in(ctf_integer(unsigned, dev, dev)))
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
	TP_PROTO(sc_exit(long ret,) int fd, const struct iovec * iov, unsigned long nr_segs, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, iov, nr_segs, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(const struct iovec *, iov, iov)) sc_in(ctf_integer(unsigned long, nr_segs, nr_segs)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_utimensat
SC_LTTNG_TRACEPOINT_EVENT(utimensat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, struct timespec * utimes, int flags),
	TP_ARGS(sc_exit(ret,) dfd, filename, utimes, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(struct timespec *, utimes, utimes)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_32_timerfd_settime
SC_LTTNG_TRACEPOINT_EVENT(timerfd_settime,
	TP_PROTO(sc_exit(long ret,) int ufd, int flags, const struct itimerspec * utmr, struct itimerspec * otmr),
	TP_ARGS(sc_exit(ret,) ufd, flags, utmr, otmr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, ufd, ufd)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(const struct itimerspec *, utmr, utmr)) sc_out(ctf_integer(struct itimerspec *, otmr, otmr)))
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
#ifndef OVERRIDE_32_mount
SC_LTTNG_TRACEPOINT_EVENT(mount,
	TP_PROTO(sc_exit(long ret,) char * dev_name, char * dir_name, char * type, unsigned long flags, void * data),
	TP_ARGS(sc_exit(ret,) dev_name, dir_name, type, flags, data),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(dev_name, dev_name)) sc_in(ctf_user_string(dir_name, dir_name)) sc_in(ctf_user_string(type, type)) sc_in(ctf_integer(unsigned long, flags, flags)) sc_in(ctf_integer(void *, data, data)))
)
#endif
#ifndef OVERRIDE_32_llseek
SC_LTTNG_TRACEPOINT_EVENT(llseek,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned long offset_high, unsigned long offset_low, loff_t * result, unsigned int origin),
	TP_ARGS(sc_exit(ret,) fd, offset_high, offset_low, result, origin),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, fd, fd)) sc_inout(ctf_integer(unsigned long, offset_high, offset_high)) sc_inout(ctf_integer(unsigned long, offset_low, offset_low)) sc_inout(ctf_integer(loff_t *, result, result)) sc_inout(ctf_integer(unsigned int, origin, origin)))
)
#endif
#ifndef OVERRIDE_32_select
SC_LTTNG_TRACEPOINT_EVENT(select,
	TP_PROTO(sc_exit(long ret,) int n, fd_set * inp, fd_set * outp, fd_set * exp, struct timeval * tvp),
	TP_ARGS(sc_exit(ret,) n, inp, outp, exp, tvp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, n, n)) sc_inout(ctf_integer(fd_set *, inp, inp)) sc_inout(ctf_integer(fd_set *, outp, outp)) sc_inout(ctf_integer(fd_set *, exp, exp)) sc_inout(ctf_integer(struct timeval *, tvp, tvp)))
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
#ifndef OVERRIDE_32_io_getevents
SC_LTTNG_TRACEPOINT_EVENT(io_getevents,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx_id, long min_nr, long nr, struct io_event * events, struct timespec * timeout),
	TP_ARGS(sc_exit(ret,) ctx_id, min_nr, nr, events, timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(aio_context_t, ctx_id, ctx_id)) sc_in(ctf_integer(long, min_nr, min_nr)) sc_in(ctf_integer(long, nr, nr)) sc_out(ctf_integer(struct io_event *, events, events)) sc_inout(ctf_integer(struct timespec *, timeout, timeout)))
)
#endif
#ifndef OVERRIDE_32_mq_timedsend
SC_LTTNG_TRACEPOINT_EVENT(mq_timedsend,
	TP_PROTO(sc_exit(long ret,) mqd_t mqdes, const char * u_msg_ptr, size_t msg_len, unsigned int msg_prio, const struct timespec * u_abs_timeout),
	TP_ARGS(sc_exit(ret,) mqdes, u_msg_ptr, msg_len, msg_prio, u_abs_timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(mqd_t, mqdes, mqdes)) sc_in(ctf_integer(const char *, u_msg_ptr, u_msg_ptr)) sc_in(ctf_integer(size_t, msg_len, msg_len)) sc_in(ctf_integer(unsigned int, msg_prio, msg_prio)) sc_in(ctf_integer(const struct timespec *, u_abs_timeout, u_abs_timeout)))
)
#endif
#ifndef OVERRIDE_32_mq_timedreceive
SC_LTTNG_TRACEPOINT_EVENT(mq_timedreceive,
	TP_PROTO(sc_exit(long ret,) mqd_t mqdes, char * u_msg_ptr, size_t msg_len, unsigned int * u_msg_prio, const struct timespec * u_abs_timeout),
	TP_ARGS(sc_exit(ret,) mqdes, u_msg_ptr, msg_len, u_msg_prio, u_abs_timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(mqd_t, mqdes, mqdes)) sc_out(ctf_integer(char *, u_msg_ptr, u_msg_ptr)) sc_in(ctf_integer(size_t, msg_len, msg_len)) sc_out(ctf_integer(unsigned int *, u_msg_prio, u_msg_prio)) sc_in(ctf_integer(const struct timespec *, u_abs_timeout, u_abs_timeout)))
)
#endif
#ifndef OVERRIDE_32_waitid
SC_LTTNG_TRACEPOINT_EVENT(waitid,
	TP_PROTO(sc_exit(long ret,) int which, pid_t upid, struct siginfo * infop, int options, struct rusage * ru),
	TP_ARGS(sc_exit(ret,) which, upid, infop, options, ru),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(pid_t, upid, upid)) sc_out(ctf_integer(struct siginfo *, infop, infop)) sc_in(ctf_integer(int, options, options)) sc_out(ctf_integer(struct rusage *, ru, ru)))
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
#ifndef OVERRIDE_32_ppoll
SC_LTTNG_TRACEPOINT_EVENT(ppoll,
	TP_PROTO(sc_exit(long ret,) struct pollfd * ufds, unsigned int nfds, struct timespec * tsp, const sigset_t * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) ufds, nfds, tsp, sigmask, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct pollfd *, ufds, ufds)) sc_in(ctf_integer(unsigned int, nfds, nfds)) sc_in(ctf_integer(struct timespec *, tsp, tsp)) sc_in(ctf_integer(const sigset_t *, sigmask, sigmask)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
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
#ifndef OVERRIDE_32_recvmmsg
SC_LTTNG_TRACEPOINT_EVENT(recvmmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct mmsghdr * mmsg, unsigned int vlen, unsigned int flags, struct timespec * timeout),
	TP_ARGS(sc_exit(ret,) fd, mmsg, vlen, flags, timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(struct mmsghdr *, mmsg, mmsg)) sc_in(ctf_integer(unsigned int, vlen, vlen)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_inout(ctf_integer(struct timespec *, timeout, timeout)))
)
#endif
#ifndef OVERRIDE_32_ipc
SC_LTTNG_TRACEPOINT_EVENT(ipc,
	TP_PROTO(sc_exit(long ret,) unsigned int call, int first, unsigned long second, unsigned long third, void * ptr, long fifth),
	TP_ARGS(sc_exit(ret,) call, first, second, third, ptr, fifth),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(unsigned int, call, call)) sc_inout(ctf_integer(int, first, first)) sc_inout(ctf_integer(unsigned long, second, second)) sc_inout(ctf_integer(unsigned long, third, third)) sc_inout(ctf_integer(void *, ptr, ptr)) sc_inout(ctf_integer(long, fifth, fifth)))
)
#endif
#ifndef OVERRIDE_32_futex
SC_LTTNG_TRACEPOINT_EVENT(futex,
	TP_PROTO(sc_exit(long ret,) u32 * uaddr, int op, u32 val, struct timespec * utime, u32 * uaddr2, u32 val3),
	TP_ARGS(sc_exit(ret,) uaddr, op, val, utime, uaddr2, val3),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(u32 *, uaddr, uaddr)) sc_in(ctf_integer(int, op, op)) sc_in(ctf_integer(u32, val, val)) sc_in(ctf_integer(struct timespec *, utime, utime)) sc_inout(ctf_integer(u32 *, uaddr2, uaddr2)) sc_in(ctf_integer(u32, val3, val3)))
)
#endif
#ifndef OVERRIDE_32_pselect6
SC_LTTNG_TRACEPOINT_EVENT(pselect6,
	TP_PROTO(sc_exit(long ret,) int n, fd_set * inp, fd_set * outp, fd_set * exp, struct timespec * tsp, void * sig),
	TP_ARGS(sc_exit(ret,) n, inp, outp, exp, tsp, sig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, n, n)) sc_inout(ctf_integer(fd_set *, inp, inp)) sc_inout(ctf_integer(fd_set *, outp, outp)) sc_inout(ctf_integer(fd_set *, exp, exp)) sc_inout(ctf_integer(struct timespec *, tsp, tsp)) sc_in(ctf_integer(void *, sig, sig)))
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

#endif /*  _TRACE_SYSCALLS_POINTERS_H */

/* This part must be outside protection */
#include <probes/define_trace.h>

#else /* CREATE_SYSCALL_TABLE */

#include "x86-32-syscalls-3.1.0-rc6_pointers_override.h"
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
#ifndef OVERRIDE_TABLE_32_waitpid
TRACE_SYSCALL_TABLE(waitpid, waitpid, 7, 3)
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
#ifndef OVERRIDE_TABLE_32_chdir
TRACE_SYSCALL_TABLE(chdir, chdir, 12, 1)
#endif
#ifndef OVERRIDE_TABLE_32_time
TRACE_SYSCALL_TABLE(time, time, 13, 1)
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
#ifndef OVERRIDE_TABLE_32_stat
TRACE_SYSCALL_TABLE(stat, stat, 18, 2)
#endif
#ifndef OVERRIDE_TABLE_32_mount
TRACE_SYSCALL_TABLE(mount, mount, 21, 5)
#endif
#ifndef OVERRIDE_TABLE_32_oldumount
TRACE_SYSCALL_TABLE(oldumount, oldumount, 22, 1)
#endif
#ifndef OVERRIDE_TABLE_32_stime
TRACE_SYSCALL_TABLE(stime, stime, 25, 1)
#endif
#ifndef OVERRIDE_TABLE_32_fstat
TRACE_SYSCALL_TABLE(fstat, fstat, 28, 2)
#endif
#ifndef OVERRIDE_TABLE_32_utime
TRACE_SYSCALL_TABLE(utime, utime, 30, 2)
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
#ifndef OVERRIDE_TABLE_32_olduname
TRACE_SYSCALL_TABLE(olduname, olduname, 59, 1)
#endif
#ifndef OVERRIDE_TABLE_32_chroot
TRACE_SYSCALL_TABLE(chroot, chroot, 61, 1)
#endif
#ifndef OVERRIDE_TABLE_32_ustat
TRACE_SYSCALL_TABLE(ustat, ustat, 62, 2)
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
#ifndef OVERRIDE_TABLE_32_old_getrlimit
TRACE_SYSCALL_TABLE(old_getrlimit, old_getrlimit, 76, 2)
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
#ifndef OVERRIDE_TABLE_32_old_select
TRACE_SYSCALL_TABLE(old_select, old_select, 82, 1)
#endif
#ifndef OVERRIDE_TABLE_32_symlink
TRACE_SYSCALL_TABLE(symlink, symlink, 83, 2)
#endif
#ifndef OVERRIDE_TABLE_32_lstat
TRACE_SYSCALL_TABLE(lstat, lstat, 84, 2)
#endif
#ifndef OVERRIDE_TABLE_32_readlink
TRACE_SYSCALL_TABLE(readlink, readlink, 85, 3)
#endif
#ifndef OVERRIDE_TABLE_32_uselib
TRACE_SYSCALL_TABLE(uselib, uselib, 86, 1)
#endif
#ifndef OVERRIDE_TABLE_32_swapon
TRACE_SYSCALL_TABLE(swapon, swapon, 87, 2)
#endif
#ifndef OVERRIDE_TABLE_32_reboot
TRACE_SYSCALL_TABLE(reboot, reboot, 88, 4)
#endif
#ifndef OVERRIDE_TABLE_32_old_readdir
TRACE_SYSCALL_TABLE(old_readdir, old_readdir, 89, 3)
#endif
#ifndef OVERRIDE_TABLE_32_old_mmap
TRACE_SYSCALL_TABLE(old_mmap, old_mmap, 90, 1)
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
#ifndef OVERRIDE_TABLE_32_socketcall
TRACE_SYSCALL_TABLE(socketcall, socketcall, 102, 2)
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
#ifndef OVERRIDE_TABLE_32_uname
TRACE_SYSCALL_TABLE(uname, uname, 109, 1)
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
#ifndef OVERRIDE_TABLE_32_ipc
TRACE_SYSCALL_TABLE(ipc, ipc, 117, 6)
#endif
#ifndef OVERRIDE_TABLE_32_setdomainname
TRACE_SYSCALL_TABLE(setdomainname, setdomainname, 121, 2)
#endif
#ifndef OVERRIDE_TABLE_32_newuname
TRACE_SYSCALL_TABLE(newuname, newuname, 122, 1)
#endif
#ifndef OVERRIDE_TABLE_32_adjtimex
TRACE_SYSCALL_TABLE(adjtimex, adjtimex, 124, 1)
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
#ifndef OVERRIDE_TABLE_32_sysctl
TRACE_SYSCALL_TABLE(sysctl, sysctl, 149, 1)
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
#ifndef OVERRIDE_TABLE_32_sched_rr_get_interval
TRACE_SYSCALL_TABLE(sched_rr_get_interval, sched_rr_get_interval, 161, 2)
#endif
#ifndef OVERRIDE_TABLE_32_nanosleep
TRACE_SYSCALL_TABLE(nanosleep, nanosleep, 162, 2)
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
#ifndef OVERRIDE_TABLE_32_rt_sigtimedwait
TRACE_SYSCALL_TABLE(rt_sigtimedwait, rt_sigtimedwait, 177, 4)
#endif
#ifndef OVERRIDE_TABLE_32_rt_sigqueueinfo
TRACE_SYSCALL_TABLE(rt_sigqueueinfo, rt_sigqueueinfo, 178, 3)
#endif
#ifndef OVERRIDE_TABLE_32_rt_sigsuspend
TRACE_SYSCALL_TABLE(rt_sigsuspend, rt_sigsuspend, 179, 2)
#endif
#ifndef OVERRIDE_TABLE_32_chown16
TRACE_SYSCALL_TABLE(chown16, chown16, 182, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getcwd
TRACE_SYSCALL_TABLE(getcwd, getcwd, 183, 2)
#endif
#ifndef OVERRIDE_TABLE_32_sendfile
TRACE_SYSCALL_TABLE(sendfile, sendfile, 187, 4)
#endif
#ifndef OVERRIDE_TABLE_32_getrlimit
TRACE_SYSCALL_TABLE(getrlimit, getrlimit, 191, 2)
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
#ifndef OVERRIDE_TABLE_32_pivot_root
TRACE_SYSCALL_TABLE(pivot_root, pivot_root, 217, 2)
#endif
#ifndef OVERRIDE_TABLE_32_mincore
TRACE_SYSCALL_TABLE(mincore, mincore, 218, 3)
#endif
#ifndef OVERRIDE_TABLE_32_getdents64
TRACE_SYSCALL_TABLE(getdents64, getdents64, 220, 3)
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
#ifndef OVERRIDE_TABLE_32_futex
TRACE_SYSCALL_TABLE(futex, futex, 240, 6)
#endif
#ifndef OVERRIDE_TABLE_32_sched_setaffinity
TRACE_SYSCALL_TABLE(sched_setaffinity, sched_setaffinity, 241, 3)
#endif
#ifndef OVERRIDE_TABLE_32_sched_getaffinity
TRACE_SYSCALL_TABLE(sched_getaffinity, sched_getaffinity, 242, 3)
#endif
#ifndef OVERRIDE_TABLE_32_io_setup
TRACE_SYSCALL_TABLE(io_setup, io_setup, 245, 2)
#endif
#ifndef OVERRIDE_TABLE_32_io_getevents
TRACE_SYSCALL_TABLE(io_getevents, io_getevents, 247, 5)
#endif
#ifndef OVERRIDE_TABLE_32_io_submit
TRACE_SYSCALL_TABLE(io_submit, io_submit, 248, 3)
#endif
#ifndef OVERRIDE_TABLE_32_io_cancel
TRACE_SYSCALL_TABLE(io_cancel, io_cancel, 249, 3)
#endif
#ifndef OVERRIDE_TABLE_32_epoll_ctl
TRACE_SYSCALL_TABLE(epoll_ctl, epoll_ctl, 255, 4)
#endif
#ifndef OVERRIDE_TABLE_32_epoll_wait
TRACE_SYSCALL_TABLE(epoll_wait, epoll_wait, 256, 4)
#endif
#ifndef OVERRIDE_TABLE_32_set_tid_address
TRACE_SYSCALL_TABLE(set_tid_address, set_tid_address, 258, 1)
#endif
#ifndef OVERRIDE_TABLE_32_timer_create
TRACE_SYSCALL_TABLE(timer_create, timer_create, 259, 3)
#endif
#ifndef OVERRIDE_TABLE_32_timer_settime
TRACE_SYSCALL_TABLE(timer_settime, timer_settime, 260, 4)
#endif
#ifndef OVERRIDE_TABLE_32_timer_gettime
TRACE_SYSCALL_TABLE(timer_gettime, timer_gettime, 261, 2)
#endif
#ifndef OVERRIDE_TABLE_32_clock_settime
TRACE_SYSCALL_TABLE(clock_settime, clock_settime, 264, 2)
#endif
#ifndef OVERRIDE_TABLE_32_clock_gettime
TRACE_SYSCALL_TABLE(clock_gettime, clock_gettime, 265, 2)
#endif
#ifndef OVERRIDE_TABLE_32_clock_getres
TRACE_SYSCALL_TABLE(clock_getres, clock_getres, 266, 2)
#endif
#ifndef OVERRIDE_TABLE_32_clock_nanosleep
TRACE_SYSCALL_TABLE(clock_nanosleep, clock_nanosleep, 267, 4)
#endif
#ifndef OVERRIDE_TABLE_32_statfs64
TRACE_SYSCALL_TABLE(statfs64, statfs64, 268, 3)
#endif
#ifndef OVERRIDE_TABLE_32_fstatfs64
TRACE_SYSCALL_TABLE(fstatfs64, fstatfs64, 269, 3)
#endif
#ifndef OVERRIDE_TABLE_32_utimes
TRACE_SYSCALL_TABLE(utimes, utimes, 271, 2)
#endif
#ifndef OVERRIDE_TABLE_32_mq_open
TRACE_SYSCALL_TABLE(mq_open, mq_open, 277, 4)
#endif
#ifndef OVERRIDE_TABLE_32_mq_unlink
TRACE_SYSCALL_TABLE(mq_unlink, mq_unlink, 278, 1)
#endif
#ifndef OVERRIDE_TABLE_32_mq_timedsend
TRACE_SYSCALL_TABLE(mq_timedsend, mq_timedsend, 279, 5)
#endif
#ifndef OVERRIDE_TABLE_32_mq_timedreceive
TRACE_SYSCALL_TABLE(mq_timedreceive, mq_timedreceive, 280, 5)
#endif
#ifndef OVERRIDE_TABLE_32_mq_notify
TRACE_SYSCALL_TABLE(mq_notify, mq_notify, 281, 2)
#endif
#ifndef OVERRIDE_TABLE_32_mq_getsetattr
TRACE_SYSCALL_TABLE(mq_getsetattr, mq_getsetattr, 282, 3)
#endif
#ifndef OVERRIDE_TABLE_32_kexec_load
TRACE_SYSCALL_TABLE(kexec_load, kexec_load, 283, 4)
#endif
#ifndef OVERRIDE_TABLE_32_waitid
TRACE_SYSCALL_TABLE(waitid, waitid, 284, 5)
#endif
#ifndef OVERRIDE_TABLE_32_add_key
TRACE_SYSCALL_TABLE(add_key, add_key, 286, 5)
#endif
#ifndef OVERRIDE_TABLE_32_request_key
TRACE_SYSCALL_TABLE(request_key, request_key, 287, 4)
#endif
#ifndef OVERRIDE_TABLE_32_inotify_add_watch
TRACE_SYSCALL_TABLE(inotify_add_watch, inotify_add_watch, 292, 3)
#endif
#ifndef OVERRIDE_TABLE_32_openat
TRACE_SYSCALL_TABLE(openat, openat, 295, 4)
#endif
#ifndef OVERRIDE_TABLE_32_mkdirat
TRACE_SYSCALL_TABLE(mkdirat, mkdirat, 296, 3)
#endif
#ifndef OVERRIDE_TABLE_32_mknodat
TRACE_SYSCALL_TABLE(mknodat, mknodat, 297, 4)
#endif
#ifndef OVERRIDE_TABLE_32_fchownat
TRACE_SYSCALL_TABLE(fchownat, fchownat, 298, 5)
#endif
#ifndef OVERRIDE_TABLE_32_futimesat
TRACE_SYSCALL_TABLE(futimesat, futimesat, 299, 3)
#endif
#ifndef OVERRIDE_TABLE_32_fstatat64
TRACE_SYSCALL_TABLE(fstatat64, fstatat64, 300, 4)
#endif
#ifndef OVERRIDE_TABLE_32_unlinkat
TRACE_SYSCALL_TABLE(unlinkat, unlinkat, 301, 3)
#endif
#ifndef OVERRIDE_TABLE_32_renameat
TRACE_SYSCALL_TABLE(renameat, renameat, 302, 4)
#endif
#ifndef OVERRIDE_TABLE_32_linkat
TRACE_SYSCALL_TABLE(linkat, linkat, 303, 5)
#endif
#ifndef OVERRIDE_TABLE_32_symlinkat
TRACE_SYSCALL_TABLE(symlinkat, symlinkat, 304, 3)
#endif
#ifndef OVERRIDE_TABLE_32_readlinkat
TRACE_SYSCALL_TABLE(readlinkat, readlinkat, 305, 4)
#endif
#ifndef OVERRIDE_TABLE_32_fchmodat
TRACE_SYSCALL_TABLE(fchmodat, fchmodat, 306, 3)
#endif
#ifndef OVERRIDE_TABLE_32_faccessat
TRACE_SYSCALL_TABLE(faccessat, faccessat, 307, 3)
#endif
#ifndef OVERRIDE_TABLE_32_pselect6
TRACE_SYSCALL_TABLE(pselect6, pselect6, 308, 6)
#endif
#ifndef OVERRIDE_TABLE_32_ppoll
TRACE_SYSCALL_TABLE(ppoll, ppoll, 309, 5)
#endif
#ifndef OVERRIDE_TABLE_32_set_robust_list
TRACE_SYSCALL_TABLE(set_robust_list, set_robust_list, 311, 2)
#endif
#ifndef OVERRIDE_TABLE_32_get_robust_list
TRACE_SYSCALL_TABLE(get_robust_list, get_robust_list, 312, 3)
#endif
#ifndef OVERRIDE_TABLE_32_splice
TRACE_SYSCALL_TABLE(splice, splice, 313, 6)
#endif
#ifndef OVERRIDE_TABLE_32_vmsplice
TRACE_SYSCALL_TABLE(vmsplice, vmsplice, 316, 4)
#endif
#ifndef OVERRIDE_TABLE_32_getcpu
TRACE_SYSCALL_TABLE(getcpu, getcpu, 318, 3)
#endif
#ifndef OVERRIDE_TABLE_32_epoll_pwait
TRACE_SYSCALL_TABLE(epoll_pwait, epoll_pwait, 319, 6)
#endif
#ifndef OVERRIDE_TABLE_32_utimensat
TRACE_SYSCALL_TABLE(utimensat, utimensat, 320, 4)
#endif
#ifndef OVERRIDE_TABLE_32_signalfd
TRACE_SYSCALL_TABLE(signalfd, signalfd, 321, 3)
#endif
#ifndef OVERRIDE_TABLE_32_timerfd_settime
TRACE_SYSCALL_TABLE(timerfd_settime, timerfd_settime, 325, 4)
#endif
#ifndef OVERRIDE_TABLE_32_timerfd_gettime
TRACE_SYSCALL_TABLE(timerfd_gettime, timerfd_gettime, 326, 2)
#endif
#ifndef OVERRIDE_TABLE_32_signalfd4
TRACE_SYSCALL_TABLE(signalfd4, signalfd4, 327, 4)
#endif
#ifndef OVERRIDE_TABLE_32_pipe2
TRACE_SYSCALL_TABLE(pipe2, pipe2, 331, 2)
#endif
#ifndef OVERRIDE_TABLE_32_preadv
TRACE_SYSCALL_TABLE(preadv, preadv, 333, 5)
#endif
#ifndef OVERRIDE_TABLE_32_pwritev
TRACE_SYSCALL_TABLE(pwritev, pwritev, 334, 5)
#endif
#ifndef OVERRIDE_TABLE_32_rt_tgsigqueueinfo
TRACE_SYSCALL_TABLE(rt_tgsigqueueinfo, rt_tgsigqueueinfo, 335, 4)
#endif
#ifndef OVERRIDE_TABLE_32_perf_event_open
TRACE_SYSCALL_TABLE(perf_event_open, perf_event_open, 336, 5)
#endif
#ifndef OVERRIDE_TABLE_32_recvmmsg
TRACE_SYSCALL_TABLE(recvmmsg, recvmmsg, 337, 5)
#endif
#ifndef OVERRIDE_TABLE_32_prlimit64
TRACE_SYSCALL_TABLE(prlimit64, prlimit64, 340, 4)
#endif
#ifndef OVERRIDE_TABLE_32_clock_adjtime
TRACE_SYSCALL_TABLE(clock_adjtime, clock_adjtime, 343, 2)
#endif
#ifndef OVERRIDE_TABLE_32_sendmmsg
TRACE_SYSCALL_TABLE(sendmmsg, sendmmsg, 345, 4)
#endif

#endif /* CREATE_SYSCALL_TABLE */
