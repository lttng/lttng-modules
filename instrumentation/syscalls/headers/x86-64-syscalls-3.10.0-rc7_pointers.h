/* THIS FILE IS AUTO-GENERATED. DO NOT EDIT */
#ifndef CREATE_SYSCALL_TABLE

#if !defined(_TRACE_SYSCALLS_POINTERS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SYSCALLS_POINTERS_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/syscalls.h>
#include "x86-64-syscalls-3.10.0-rc7_pointers_override.h"
#include "syscalls_pointers_override.h"

#ifndef OVERRIDE_64_pipe
SC_LTTNG_TRACEPOINT_EVENT(pipe,
	TP_PROTO(sc_exit(long ret,) int * fildes),
	TP_ARGS(sc_exit(ret,) fildes),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(int *, fildes, fildes)))
)
#endif
#ifndef OVERRIDE_64_newuname
SC_LTTNG_TRACEPOINT_EVENT(newuname,
	TP_PROTO(sc_exit(long ret,) struct new_utsname * name),
	TP_ARGS(sc_exit(ret,) name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(struct new_utsname *, name, name)))
)
#endif
#ifndef OVERRIDE_64_shmdt
SC_LTTNG_TRACEPOINT_EVENT(shmdt,
	TP_PROTO(sc_exit(long ret,) char * shmaddr),
	TP_ARGS(sc_exit(ret,) shmaddr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(char *, shmaddr, shmaddr)))
)
#endif
#ifndef OVERRIDE_64_chdir
SC_LTTNG_TRACEPOINT_EVENT(chdir,
	TP_PROTO(sc_exit(long ret,) const char * filename),
	TP_ARGS(sc_exit(ret,) filename),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)))
)
#endif
#ifndef OVERRIDE_64_rmdir
SC_LTTNG_TRACEPOINT_EVENT(rmdir,
	TP_PROTO(sc_exit(long ret,) const char * pathname),
	TP_ARGS(sc_exit(ret,) pathname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)))
)
#endif
#ifndef OVERRIDE_64_unlink
SC_LTTNG_TRACEPOINT_EVENT(unlink,
	TP_PROTO(sc_exit(long ret,) const char * pathname),
	TP_ARGS(sc_exit(ret,) pathname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)))
)
#endif
#ifndef OVERRIDE_64_sysinfo
SC_LTTNG_TRACEPOINT_EVENT(sysinfo,
	TP_PROTO(sc_exit(long ret,) struct sysinfo * info),
	TP_ARGS(sc_exit(ret,) info),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(struct sysinfo *, info, info)))
)
#endif
#ifndef OVERRIDE_64_times
SC_LTTNG_TRACEPOINT_EVENT(times,
	TP_PROTO(sc_exit(long ret,) struct tms * tbuf),
	TP_ARGS(sc_exit(ret,) tbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(struct tms *, tbuf, tbuf)))
)
#endif
#ifndef OVERRIDE_64_sysctl
SC_LTTNG_TRACEPOINT_EVENT(sysctl,
	TP_PROTO(sc_exit(long ret,) struct __sysctl_args * args),
	TP_ARGS(sc_exit(ret,) args),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct __sysctl_args *, args, args)))
)
#endif
#ifndef OVERRIDE_64_adjtimex
SC_LTTNG_TRACEPOINT_EVENT(adjtimex,
	TP_PROTO(sc_exit(long ret,) struct timex * txc_p),
	TP_ARGS(sc_exit(ret,) txc_p),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct timex *, txc_p, txc_p)))
)
#endif
#ifndef OVERRIDE_64_chroot
SC_LTTNG_TRACEPOINT_EVENT(chroot,
	TP_PROTO(sc_exit(long ret,) const char * filename),
	TP_ARGS(sc_exit(ret,) filename),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)))
)
#endif
#ifndef OVERRIDE_64_acct
SC_LTTNG_TRACEPOINT_EVENT(acct,
	TP_PROTO(sc_exit(long ret,) const char * name),
	TP_ARGS(sc_exit(ret,) name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(name, name)))
)
#endif
#ifndef OVERRIDE_64_swapoff
SC_LTTNG_TRACEPOINT_EVENT(swapoff,
	TP_PROTO(sc_exit(long ret,) const char * specialfile),
	TP_ARGS(sc_exit(ret,) specialfile),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(specialfile, specialfile)))
)
#endif
#ifndef OVERRIDE_64_time
SC_LTTNG_TRACEPOINT_EVENT(time,
	TP_PROTO(sc_exit(long ret,) time_t * tloc),
	TP_ARGS(sc_exit(ret,) tloc),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(time_t *, tloc, tloc)))
)
#endif
#ifndef OVERRIDE_64_set_tid_address
SC_LTTNG_TRACEPOINT_EVENT(set_tid_address,
	TP_PROTO(sc_exit(long ret,) int * tidptr),
	TP_ARGS(sc_exit(ret,) tidptr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int *, tidptr, tidptr)))
)
#endif
#ifndef OVERRIDE_64_mq_unlink
SC_LTTNG_TRACEPOINT_EVENT(mq_unlink,
	TP_PROTO(sc_exit(long ret,) const char * u_name),
	TP_ARGS(sc_exit(ret,) u_name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(u_name, u_name)))
)
#endif
#ifndef OVERRIDE_64_newstat
SC_LTTNG_TRACEPOINT_EVENT(newstat,
	TP_PROTO(sc_exit(long ret,) const char * filename, struct stat * statbuf),
	TP_ARGS(sc_exit(ret,) filename, statbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_out(ctf_integer(struct stat *, statbuf, statbuf)))
)
#endif
#ifndef OVERRIDE_64_newfstat
SC_LTTNG_TRACEPOINT_EVENT(newfstat,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct stat * statbuf),
	TP_ARGS(sc_exit(ret,) fd, statbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(struct stat *, statbuf, statbuf)))
)
#endif
#ifndef OVERRIDE_64_newlstat
SC_LTTNG_TRACEPOINT_EVENT(newlstat,
	TP_PROTO(sc_exit(long ret,) const char * filename, struct stat * statbuf),
	TP_ARGS(sc_exit(ret,) filename, statbuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_out(ctf_integer(struct stat *, statbuf, statbuf)))
)
#endif
#ifndef OVERRIDE_64_access
SC_LTTNG_TRACEPOINT_EVENT(access,
	TP_PROTO(sc_exit(long ret,) const char * filename, int mode),
	TP_ARGS(sc_exit(ret,) filename, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(int, mode, mode)))
)
#endif
#ifndef OVERRIDE_64_nanosleep
SC_LTTNG_TRACEPOINT_EVENT(nanosleep,
	TP_PROTO(sc_exit(long ret,) struct timespec * rqtp, struct timespec * rmtp),
	TP_ARGS(sc_exit(ret,) rqtp, rmtp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(struct timespec *, rqtp, rqtp)) sc_out(ctf_integer(struct timespec *, rmtp, rmtp)))
)
#endif
#ifndef OVERRIDE_64_getitimer
SC_LTTNG_TRACEPOINT_EVENT(getitimer,
	TP_PROTO(sc_exit(long ret,) int which, struct itimerval * value),
	TP_ARGS(sc_exit(ret,) which, value),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_out(ctf_integer(struct itimerval *, value, value)))
)
#endif
#ifndef OVERRIDE_64_truncate
SC_LTTNG_TRACEPOINT_EVENT(truncate,
	TP_PROTO(sc_exit(long ret,) const char * path, long length),
	TP_ARGS(sc_exit(ret,) path, length),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(path, path)) sc_in(ctf_integer(long, length, length)))
)
#endif
#ifndef OVERRIDE_64_getcwd
SC_LTTNG_TRACEPOINT_EVENT(getcwd,
	TP_PROTO(sc_exit(long ret,) char * buf, unsigned long size),
	TP_ARGS(sc_exit(ret,) buf, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(unsigned long, size, size)))
)
#endif
#ifndef OVERRIDE_64_rename
SC_LTTNG_TRACEPOINT_EVENT(rename,
	TP_PROTO(sc_exit(long ret,) const char * oldname, const char * newname),
	TP_ARGS(sc_exit(ret,) oldname, newname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(oldname, oldname)) sc_in(ctf_user_string(newname, newname)))
)
#endif
#ifndef OVERRIDE_64_mkdir
SC_LTTNG_TRACEPOINT_EVENT(mkdir,
	TP_PROTO(sc_exit(long ret,) const char * pathname, umode_t mode),
	TP_ARGS(sc_exit(ret,) pathname, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_64_creat
SC_LTTNG_TRACEPOINT_EVENT(creat,
	TP_PROTO(sc_exit(long ret,) const char * pathname, umode_t mode),
	TP_ARGS(sc_exit(ret,) pathname, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_64_link
SC_LTTNG_TRACEPOINT_EVENT(link,
	TP_PROTO(sc_exit(long ret,) const char * oldname, const char * newname),
	TP_ARGS(sc_exit(ret,) oldname, newname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(oldname, oldname)) sc_in(ctf_user_string(newname, newname)))
)
#endif
#ifndef OVERRIDE_64_symlink
SC_LTTNG_TRACEPOINT_EVENT(symlink,
	TP_PROTO(sc_exit(long ret,) const char * oldname, const char * newname),
	TP_ARGS(sc_exit(ret,) oldname, newname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(oldname, oldname)) sc_in(ctf_user_string(newname, newname)))
)
#endif
#ifndef OVERRIDE_64_chmod
SC_LTTNG_TRACEPOINT_EVENT(chmod,
	TP_PROTO(sc_exit(long ret,) const char * filename, umode_t mode),
	TP_ARGS(sc_exit(ret,) filename, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_64_gettimeofday
SC_LTTNG_TRACEPOINT_EVENT(gettimeofday,
	TP_PROTO(sc_exit(long ret,) struct timeval * tv, struct timezone * tz),
	TP_ARGS(sc_exit(ret,) tv, tz),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(struct timeval *, tv, tv)) sc_out(ctf_integer(struct timezone *, tz, tz)))
)
#endif
#ifndef OVERRIDE_64_getrlimit
SC_LTTNG_TRACEPOINT_EVENT(getrlimit,
	TP_PROTO(sc_exit(long ret,) unsigned int resource, struct rlimit * rlim),
	TP_ARGS(sc_exit(ret,) resource, rlim),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, resource, resource)) sc_out(ctf_integer(struct rlimit *, rlim, rlim)))
)
#endif
#ifndef OVERRIDE_64_getrusage
SC_LTTNG_TRACEPOINT_EVENT(getrusage,
	TP_PROTO(sc_exit(long ret,) int who, struct rusage * ru),
	TP_ARGS(sc_exit(ret,) who, ru),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, who, who)) sc_out(ctf_integer(struct rusage *, ru, ru)))
)
#endif
#ifndef OVERRIDE_64_getgroups
SC_LTTNG_TRACEPOINT_EVENT(getgroups,
	TP_PROTO(sc_exit(long ret,) int gidsetsize, gid_t * grouplist),
	TP_ARGS(sc_exit(ret,) gidsetsize, grouplist),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, gidsetsize, gidsetsize)) sc_out(ctf_integer(gid_t *, grouplist, grouplist)))
)
#endif
#ifndef OVERRIDE_64_setgroups
SC_LTTNG_TRACEPOINT_EVENT(setgroups,
	TP_PROTO(sc_exit(long ret,) int gidsetsize, gid_t * grouplist),
	TP_ARGS(sc_exit(ret,) gidsetsize, grouplist),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, gidsetsize, gidsetsize)) sc_in(ctf_integer(gid_t *, grouplist, grouplist)))
)
#endif
#ifndef OVERRIDE_64_rt_sigpending
SC_LTTNG_TRACEPOINT_EVENT(rt_sigpending,
	TP_PROTO(sc_exit(long ret,) sigset_t * uset, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) uset, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(sigset_t *, uset, uset)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_64_rt_sigsuspend
SC_LTTNG_TRACEPOINT_EVENT(rt_sigsuspend,
	TP_PROTO(sc_exit(long ret,) sigset_t * unewset, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) unewset, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(sigset_t *, unewset, unewset)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_64_sigaltstack
SC_LTTNG_TRACEPOINT_EVENT(sigaltstack,
	TP_PROTO(sc_exit(long ret,) const stack_t * uss, stack_t * uoss),
	TP_ARGS(sc_exit(ret,) uss, uoss),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const stack_t *, uss, uss)) sc_inout(ctf_integer(stack_t *, uoss, uoss)))
)
#endif
#ifndef OVERRIDE_64_utime
SC_LTTNG_TRACEPOINT_EVENT(utime,
	TP_PROTO(sc_exit(long ret,) char * filename, struct utimbuf * times),
	TP_ARGS(sc_exit(ret,) filename, times),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(struct utimbuf *, times, times)))
)
#endif
#ifndef OVERRIDE_64_ustat
SC_LTTNG_TRACEPOINT_EVENT(ustat,
	TP_PROTO(sc_exit(long ret,) unsigned dev, struct ustat * ubuf),
	TP_ARGS(sc_exit(ret,) dev, ubuf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned, dev, dev)) sc_out(ctf_integer(struct ustat *, ubuf, ubuf)))
)
#endif
#ifndef OVERRIDE_64_statfs
SC_LTTNG_TRACEPOINT_EVENT(statfs,
	TP_PROTO(sc_exit(long ret,) const char * pathname, struct statfs * buf),
	TP_ARGS(sc_exit(ret,) pathname, buf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_out(ctf_integer(struct statfs *, buf, buf)))
)
#endif
#ifndef OVERRIDE_64_fstatfs
SC_LTTNG_TRACEPOINT_EVENT(fstatfs,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct statfs * buf),
	TP_ARGS(sc_exit(ret,) fd, buf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(struct statfs *, buf, buf)))
)
#endif
#ifndef OVERRIDE_64_sched_setparam
SC_LTTNG_TRACEPOINT_EVENT(sched_setparam,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct sched_param * param),
	TP_ARGS(sc_exit(ret,) pid, param),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(struct sched_param *, param, param)))
)
#endif
#ifndef OVERRIDE_64_sched_getparam
SC_LTTNG_TRACEPOINT_EVENT(sched_getparam,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct sched_param * param),
	TP_ARGS(sc_exit(ret,) pid, param),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_out(ctf_integer(struct sched_param *, param, param)))
)
#endif
#ifndef OVERRIDE_64_sched_rr_get_interval
SC_LTTNG_TRACEPOINT_EVENT(sched_rr_get_interval,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct timespec * interval),
	TP_ARGS(sc_exit(ret,) pid, interval),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_out(ctf_integer(struct timespec *, interval, interval)))
)
#endif
#ifndef OVERRIDE_64_pivot_root
SC_LTTNG_TRACEPOINT_EVENT(pivot_root,
	TP_PROTO(sc_exit(long ret,) const char * new_root, const char * put_old),
	TP_ARGS(sc_exit(ret,) new_root, put_old),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(new_root, new_root)) sc_in(ctf_user_string(put_old, put_old)))
)
#endif
#ifndef OVERRIDE_64_setrlimit
SC_LTTNG_TRACEPOINT_EVENT(setrlimit,
	TP_PROTO(sc_exit(long ret,) unsigned int resource, struct rlimit * rlim),
	TP_ARGS(sc_exit(ret,) resource, rlim),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, resource, resource)) sc_in(ctf_integer(struct rlimit *, rlim, rlim)))
)
#endif
#ifndef OVERRIDE_64_settimeofday
SC_LTTNG_TRACEPOINT_EVENT(settimeofday,
	TP_PROTO(sc_exit(long ret,) struct timeval * tv, struct timezone * tz),
	TP_ARGS(sc_exit(ret,) tv, tz),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(struct timeval *, tv, tv)) sc_in(ctf_integer(struct timezone *, tz, tz)))
)
#endif
#ifndef OVERRIDE_64_umount
SC_LTTNG_TRACEPOINT_EVENT(umount,
	TP_PROTO(sc_exit(long ret,) char * name, int flags),
	TP_ARGS(sc_exit(ret,) name, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(name, name)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_swapon
SC_LTTNG_TRACEPOINT_EVENT(swapon,
	TP_PROTO(sc_exit(long ret,) const char * specialfile, int swap_flags),
	TP_ARGS(sc_exit(ret,) specialfile, swap_flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(specialfile, specialfile)) sc_in(ctf_integer(int, swap_flags, swap_flags)))
)
#endif
#ifndef OVERRIDE_64_sethostname
SC_LTTNG_TRACEPOINT_EVENT(sethostname,
	TP_PROTO(sc_exit(long ret,) char * name, int len),
	TP_ARGS(sc_exit(ret,) name, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(name, name)) sc_in(ctf_integer(int, len, len)))
)
#endif
#ifndef OVERRIDE_64_setdomainname
SC_LTTNG_TRACEPOINT_EVENT(setdomainname,
	TP_PROTO(sc_exit(long ret,) char * name, int len),
	TP_ARGS(sc_exit(ret,) name, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(name, name)) sc_in(ctf_integer(int, len, len)))
)
#endif
#ifndef OVERRIDE_64_delete_module
SC_LTTNG_TRACEPOINT_EVENT(delete_module,
	TP_PROTO(sc_exit(long ret,) const char * name_user, unsigned int flags),
	TP_ARGS(sc_exit(ret,) name_user, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(name_user, name_user)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_removexattr
SC_LTTNG_TRACEPOINT_EVENT(removexattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name),
	TP_ARGS(sc_exit(ret,) pathname, name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_user_string(name, name)))
)
#endif
#ifndef OVERRIDE_64_lremovexattr
SC_LTTNG_TRACEPOINT_EVENT(lremovexattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name),
	TP_ARGS(sc_exit(ret,) pathname, name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_user_string(name, name)))
)
#endif
#ifndef OVERRIDE_64_fremovexattr
SC_LTTNG_TRACEPOINT_EVENT(fremovexattr,
	TP_PROTO(sc_exit(long ret,) int fd, const char * name),
	TP_ARGS(sc_exit(ret,) fd, name),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_user_string(name, name)))
)
#endif
#ifndef OVERRIDE_64_io_setup
SC_LTTNG_TRACEPOINT_EVENT(io_setup,
	TP_PROTO(sc_exit(long ret,) unsigned nr_events, aio_context_t * ctxp),
	TP_ARGS(sc_exit(ret,) nr_events, ctxp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned, nr_events, nr_events)) sc_in(ctf_integer(aio_context_t *, ctxp, ctxp)))
)
#endif
#ifndef OVERRIDE_64_timer_gettime
SC_LTTNG_TRACEPOINT_EVENT(timer_gettime,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id, struct itimerspec * setting),
	TP_ARGS(sc_exit(ret,) timer_id, setting),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(timer_t, timer_id, timer_id)) sc_out(ctf_integer(struct itimerspec *, setting, setting)))
)
#endif
#ifndef OVERRIDE_64_clock_settime
SC_LTTNG_TRACEPOINT_EVENT(clock_settime,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, const struct timespec * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_in(ctf_integer(const struct timespec *, tp, tp)))
)
#endif
#ifndef OVERRIDE_64_clock_gettime
SC_LTTNG_TRACEPOINT_EVENT(clock_gettime,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct timespec * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_out(ctf_integer(struct timespec *, tp, tp)))
)
#endif
#ifndef OVERRIDE_64_clock_getres
SC_LTTNG_TRACEPOINT_EVENT(clock_getres,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct timespec * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_out(ctf_integer(struct timespec *, tp, tp)))
)
#endif
#ifndef OVERRIDE_64_utimes
SC_LTTNG_TRACEPOINT_EVENT(utimes,
	TP_PROTO(sc_exit(long ret,) char * filename, struct timeval * utimes),
	TP_ARGS(sc_exit(ret,) filename, utimes),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(struct timeval *, utimes, utimes)))
)
#endif
#ifndef OVERRIDE_64_mq_notify
SC_LTTNG_TRACEPOINT_EVENT(mq_notify,
	TP_PROTO(sc_exit(long ret,) mqd_t mqdes, const struct sigevent * u_notification),
	TP_ARGS(sc_exit(ret,) mqdes, u_notification),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(mqd_t, mqdes, mqdes)) sc_in(ctf_integer(const struct sigevent *, u_notification, u_notification)))
)
#endif
#ifndef OVERRIDE_64_set_robust_list
SC_LTTNG_TRACEPOINT_EVENT(set_robust_list,
	TP_PROTO(sc_exit(long ret,) struct robust_list_head * head, size_t len),
	TP_ARGS(sc_exit(ret,) head, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(struct robust_list_head *, head, head)) sc_in(ctf_integer(size_t, len, len)))
)
#endif
#ifndef OVERRIDE_64_timerfd_gettime
SC_LTTNG_TRACEPOINT_EVENT(timerfd_gettime,
	TP_PROTO(sc_exit(long ret,) int ufd, struct itimerspec * otmr),
	TP_ARGS(sc_exit(ret,) ufd, otmr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, ufd, ufd)) sc_out(ctf_integer(struct itimerspec *, otmr, otmr)))
)
#endif
#ifndef OVERRIDE_64_pipe2
SC_LTTNG_TRACEPOINT_EVENT(pipe2,
	TP_PROTO(sc_exit(long ret,) int * fildes, int flags),
	TP_ARGS(sc_exit(ret,) fildes, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(int *, fildes, fildes)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_clock_adjtime
SC_LTTNG_TRACEPOINT_EVENT(clock_adjtime,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct timex * utx),
	TP_ARGS(sc_exit(ret,) which_clock, utx),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_inout(ctf_integer(struct timex *, utx, utx)))
)
#endif
#ifndef OVERRIDE_64_read
SC_LTTNG_TRACEPOINT_EVENT(read,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, char * buf, size_t count),
	TP_ARGS(sc_exit(ret,) fd, buf, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(size_t, count, count)))
)
#endif
#ifndef OVERRIDE_64_write
SC_LTTNG_TRACEPOINT_EVENT(write,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, const char * buf, size_t count),
	TP_ARGS(sc_exit(ret,) fd, buf, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(const char *, buf, buf)) sc_in(ctf_integer(size_t, count, count)))
)
#endif
#ifndef OVERRIDE_64_open
SC_LTTNG_TRACEPOINT_EVENT(open,
	TP_PROTO(sc_exit(long ret,) const char * filename, int flags, umode_t mode),
	TP_ARGS(sc_exit(ret,) filename, flags, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_64_poll
SC_LTTNG_TRACEPOINT_EVENT(poll,
	TP_PROTO(sc_exit(long ret,) struct pollfd * ufds, unsigned int nfds, int timeout_msecs),
	TP_ARGS(sc_exit(ret,) ufds, nfds, timeout_msecs),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct pollfd *, ufds, ufds)) sc_in(ctf_integer(unsigned int, nfds, nfds)) sc_in(ctf_integer(int, timeout_msecs, timeout_msecs)))
)
#endif
#ifndef OVERRIDE_64_readv
SC_LTTNG_TRACEPOINT_EVENT(readv,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, fd, fd)) sc_inout(ctf_integer(const struct iovec *, vec, vec)) sc_in(ctf_integer(unsigned long, vlen, vlen)))
)
#endif
#ifndef OVERRIDE_64_writev
SC_LTTNG_TRACEPOINT_EVENT(writev,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, fd, fd)) sc_inout(ctf_integer(const struct iovec *, vec, vec)) sc_in(ctf_integer(unsigned long, vlen, vlen)))
)
#endif
#ifndef OVERRIDE_64_mincore
SC_LTTNG_TRACEPOINT_EVENT(mincore,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, unsigned char * vec),
	TP_ARGS(sc_exit(ret,) start, len, vec),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(size_t, len, len)) sc_out(ctf_integer(unsigned char *, vec, vec)))
)
#endif
#ifndef OVERRIDE_64_shmat
SC_LTTNG_TRACEPOINT_EVENT(shmat,
	TP_PROTO(sc_exit(long ret,) int shmid, char * shmaddr, int shmflg),
	TP_ARGS(sc_exit(ret,) shmid, shmaddr, shmflg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, shmid, shmid)) sc_in(ctf_integer(char *, shmaddr, shmaddr)) sc_in(ctf_integer(int, shmflg, shmflg)))
)
#endif
#ifndef OVERRIDE_64_shmctl
SC_LTTNG_TRACEPOINT_EVENT(shmctl,
	TP_PROTO(sc_exit(long ret,) int shmid, int cmd, struct shmid_ds * buf),
	TP_ARGS(sc_exit(ret,) shmid, cmd, buf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, shmid, shmid)) sc_in(ctf_integer(int, cmd, cmd)) sc_inout(ctf_integer(struct shmid_ds *, buf, buf)))
)
#endif
#ifndef OVERRIDE_64_setitimer
SC_LTTNG_TRACEPOINT_EVENT(setitimer,
	TP_PROTO(sc_exit(long ret,) int which, struct itimerval * value, struct itimerval * ovalue),
	TP_ARGS(sc_exit(ret,) which, value, ovalue),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(struct itimerval *, value, value)) sc_out(ctf_integer(struct itimerval *, ovalue, ovalue)))
)
#endif
#ifndef OVERRIDE_64_connect
SC_LTTNG_TRACEPOINT_EVENT(connect,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * uservaddr, int addrlen),
	TP_ARGS(sc_exit(ret,) fd, uservaddr, addrlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(struct sockaddr *, uservaddr, uservaddr)) sc_in(ctf_integer(int, addrlen, addrlen)))
)
#endif
#ifndef OVERRIDE_64_accept
SC_LTTNG_TRACEPOINT_EVENT(accept,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * upeer_sockaddr, int * upeer_addrlen),
	TP_ARGS(sc_exit(ret,) fd, upeer_sockaddr, upeer_addrlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(struct sockaddr *, upeer_sockaddr, upeer_sockaddr)) sc_inout(ctf_integer(int *, upeer_addrlen, upeer_addrlen)))
)
#endif
#ifndef OVERRIDE_64_sendmsg
SC_LTTNG_TRACEPOINT_EVENT(sendmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct msghdr * msg, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, msg, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(struct msghdr *, msg, msg)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_recvmsg
SC_LTTNG_TRACEPOINT_EVENT(recvmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct msghdr * msg, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, msg, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_inout(ctf_integer(struct msghdr *, msg, msg)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_bind
SC_LTTNG_TRACEPOINT_EVENT(bind,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * umyaddr, int addrlen),
	TP_ARGS(sc_exit(ret,) fd, umyaddr, addrlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(struct sockaddr *, umyaddr, umyaddr)) sc_in(ctf_integer(int, addrlen, addrlen)))
)
#endif
#ifndef OVERRIDE_64_getsockname
SC_LTTNG_TRACEPOINT_EVENT(getsockname,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * usockaddr, int * usockaddr_len),
	TP_ARGS(sc_exit(ret,) fd, usockaddr, usockaddr_len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(struct sockaddr *, usockaddr, usockaddr)) sc_inout(ctf_integer(int *, usockaddr_len, usockaddr_len)))
)
#endif
#ifndef OVERRIDE_64_getpeername
SC_LTTNG_TRACEPOINT_EVENT(getpeername,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * usockaddr, int * usockaddr_len),
	TP_ARGS(sc_exit(ret,) fd, usockaddr, usockaddr_len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(struct sockaddr *, usockaddr, usockaddr)) sc_inout(ctf_integer(int *, usockaddr_len, usockaddr_len)))
)
#endif
#ifndef OVERRIDE_64_semop
SC_LTTNG_TRACEPOINT_EVENT(semop,
	TP_PROTO(sc_exit(long ret,) int semid, struct sembuf * tsops, unsigned nsops),
	TP_ARGS(sc_exit(ret,) semid, tsops, nsops),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, semid, semid)) sc_in(ctf_integer(struct sembuf *, tsops, tsops)) sc_in(ctf_integer(unsigned, nsops, nsops)))
)
#endif
#ifndef OVERRIDE_64_msgctl
SC_LTTNG_TRACEPOINT_EVENT(msgctl,
	TP_PROTO(sc_exit(long ret,) int msqid, int cmd, struct msqid_ds * buf),
	TP_ARGS(sc_exit(ret,) msqid, cmd, buf),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, msqid, msqid)) sc_in(ctf_integer(int, cmd, cmd)) sc_inout(ctf_integer(struct msqid_ds *, buf, buf)))
)
#endif
#ifndef OVERRIDE_64_getdents
SC_LTTNG_TRACEPOINT_EVENT(getdents,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct linux_dirent * dirent, unsigned int count),
	TP_ARGS(sc_exit(ret,) fd, dirent, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(struct linux_dirent *, dirent, dirent)) sc_in(ctf_integer(unsigned int, count, count)))
)
#endif
#ifndef OVERRIDE_64_readlink
SC_LTTNG_TRACEPOINT_EVENT(readlink,
	TP_PROTO(sc_exit(long ret,) const char * path, char * buf, int bufsiz),
	TP_ARGS(sc_exit(ret,) path, buf, bufsiz),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(path, path)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(int, bufsiz, bufsiz)))
)
#endif
#ifndef OVERRIDE_64_chown
SC_LTTNG_TRACEPOINT_EVENT(chown,
	TP_PROTO(sc_exit(long ret,) const char * filename, uid_t user, gid_t group),
	TP_ARGS(sc_exit(ret,) filename, user, group),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(uid_t, user, user)) sc_in(ctf_integer(gid_t, group, group)))
)
#endif
#ifndef OVERRIDE_64_lchown
SC_LTTNG_TRACEPOINT_EVENT(lchown,
	TP_PROTO(sc_exit(long ret,) const char * filename, uid_t user, gid_t group),
	TP_ARGS(sc_exit(ret,) filename, user, group),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(uid_t, user, user)) sc_in(ctf_integer(gid_t, group, group)))
)
#endif
#ifndef OVERRIDE_64_syslog
SC_LTTNG_TRACEPOINT_EVENT(syslog,
	TP_PROTO(sc_exit(long ret,) int type, char * buf, int len),
	TP_ARGS(sc_exit(ret,) type, buf, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, type, type)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(int, len, len)))
)
#endif
#ifndef OVERRIDE_64_getresuid
SC_LTTNG_TRACEPOINT_EVENT(getresuid,
	TP_PROTO(sc_exit(long ret,) uid_t * ruidp, uid_t * euidp, uid_t * suidp),
	TP_ARGS(sc_exit(ret,) ruidp, euidp, suidp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(uid_t *, ruidp, ruidp)) sc_out(ctf_integer(uid_t *, euidp, euidp)) sc_out(ctf_integer(uid_t *, suidp, suidp)))
)
#endif
#ifndef OVERRIDE_64_getresgid
SC_LTTNG_TRACEPOINT_EVENT(getresgid,
	TP_PROTO(sc_exit(long ret,) gid_t * rgidp, gid_t * egidp, gid_t * sgidp),
	TP_ARGS(sc_exit(ret,) rgidp, egidp, sgidp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(gid_t *, rgidp, rgidp)) sc_out(ctf_integer(gid_t *, egidp, egidp)) sc_out(ctf_integer(gid_t *, sgidp, sgidp)))
)
#endif
#ifndef OVERRIDE_64_rt_sigqueueinfo
SC_LTTNG_TRACEPOINT_EVENT(rt_sigqueueinfo,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int sig, siginfo_t * uinfo),
	TP_ARGS(sc_exit(ret,) pid, sig, uinfo),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, sig, sig)) sc_in(ctf_integer(siginfo_t *, uinfo, uinfo)))
)
#endif
#ifndef OVERRIDE_64_mknod
SC_LTTNG_TRACEPOINT_EVENT(mknod,
	TP_PROTO(sc_exit(long ret,) const char * filename, umode_t mode, unsigned dev),
	TP_ARGS(sc_exit(ret,) filename, mode, dev),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(umode_t, mode, mode)) sc_in(ctf_integer(unsigned, dev, dev)))
)
#endif
#ifndef OVERRIDE_64_sched_setscheduler
SC_LTTNG_TRACEPOINT_EVENT(sched_setscheduler,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int policy, struct sched_param * param),
	TP_ARGS(sc_exit(ret,) pid, policy, param),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, policy, policy)) sc_in(ctf_integer(struct sched_param *, param, param)))
)
#endif
#ifndef OVERRIDE_64_init_module
SC_LTTNG_TRACEPOINT_EVENT(init_module,
	TP_PROTO(sc_exit(long ret,) void * umod, unsigned long len, const char * uargs),
	TP_ARGS(sc_exit(ret,) umod, len, uargs),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(void *, umod, umod)) sc_in(ctf_integer(unsigned long, len, len)) sc_in(ctf_integer(const char *, uargs, uargs)))
)
#endif
#ifndef OVERRIDE_64_listxattr
SC_LTTNG_TRACEPOINT_EVENT(listxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, char * list, size_t size),
	TP_ARGS(sc_exit(ret,) pathname, list, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_out(ctf_integer(char *, list, list)) sc_in(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_64_llistxattr
SC_LTTNG_TRACEPOINT_EVENT(llistxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, char * list, size_t size),
	TP_ARGS(sc_exit(ret,) pathname, list, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_out(ctf_integer(char *, list, list)) sc_in(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_64_flistxattr
SC_LTTNG_TRACEPOINT_EVENT(flistxattr,
	TP_PROTO(sc_exit(long ret,) int fd, char * list, size_t size),
	TP_ARGS(sc_exit(ret,) fd, list, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(char *, list, list)) sc_in(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_64_sched_setaffinity
SC_LTTNG_TRACEPOINT_EVENT(sched_setaffinity,
	TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned int len, unsigned long * user_mask_ptr),
	TP_ARGS(sc_exit(ret,) pid, len, user_mask_ptr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(unsigned int, len, len)) sc_in(ctf_integer(unsigned long *, user_mask_ptr, user_mask_ptr)))
)
#endif
#ifndef OVERRIDE_64_sched_getaffinity
SC_LTTNG_TRACEPOINT_EVENT(sched_getaffinity,
	TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned int len, unsigned long * user_mask_ptr),
	TP_ARGS(sc_exit(ret,) pid, len, user_mask_ptr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(unsigned int, len, len)) sc_out(ctf_integer(unsigned long *, user_mask_ptr, user_mask_ptr)))
)
#endif
#ifndef OVERRIDE_64_io_submit
SC_LTTNG_TRACEPOINT_EVENT(io_submit,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx_id, long nr, struct iocb * * iocbpp),
	TP_ARGS(sc_exit(ret,) ctx_id, nr, iocbpp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(aio_context_t, ctx_id, ctx_id)) sc_in(ctf_integer(long, nr, nr)) sc_in(ctf_integer(struct iocb * *, iocbpp, iocbpp)))
)
#endif
#ifndef OVERRIDE_64_io_cancel
SC_LTTNG_TRACEPOINT_EVENT(io_cancel,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx_id, struct iocb * iocb, struct io_event * result),
	TP_ARGS(sc_exit(ret,) ctx_id, iocb, result),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(aio_context_t, ctx_id, ctx_id)) sc_in(ctf_integer(struct iocb *, iocb, iocb)) sc_out(ctf_integer(struct io_event *, result, result)))
)
#endif
#ifndef OVERRIDE_64_lookup_dcookie
SC_LTTNG_TRACEPOINT_EVENT(lookup_dcookie,
	TP_PROTO(sc_exit(long ret,) u64 cookie64, char * buf, size_t len),
	TP_ARGS(sc_exit(ret,) cookie64, buf, len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(u64, cookie64, cookie64)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(size_t, len, len)))
)
#endif
#ifndef OVERRIDE_64_getdents64
SC_LTTNG_TRACEPOINT_EVENT(getdents64,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct linux_dirent64 * dirent, unsigned int count),
	TP_ARGS(sc_exit(ret,) fd, dirent, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(struct linux_dirent64 *, dirent, dirent)) sc_in(ctf_integer(unsigned int, count, count)))
)
#endif
#ifndef OVERRIDE_64_timer_create
SC_LTTNG_TRACEPOINT_EVENT(timer_create,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct sigevent * timer_event_spec, timer_t * created_timer_id),
	TP_ARGS(sc_exit(ret,) which_clock, timer_event_spec, created_timer_id),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_in(ctf_integer(struct sigevent *, timer_event_spec, timer_event_spec)) sc_out(ctf_integer(timer_t *, created_timer_id, created_timer_id)))
)
#endif
#ifndef OVERRIDE_64_set_mempolicy
SC_LTTNG_TRACEPOINT_EVENT(set_mempolicy,
	TP_PROTO(sc_exit(long ret,) int mode, unsigned long * nmask, unsigned long maxnode),
	TP_ARGS(sc_exit(ret,) mode, nmask, maxnode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, mode, mode)) sc_in(ctf_integer(unsigned long *, nmask, nmask)) sc_in(ctf_integer(unsigned long, maxnode, maxnode)))
)
#endif
#ifndef OVERRIDE_64_mq_getsetattr
SC_LTTNG_TRACEPOINT_EVENT(mq_getsetattr,
	TP_PROTO(sc_exit(long ret,) mqd_t mqdes, const struct mq_attr * u_mqstat, struct mq_attr * u_omqstat),
	TP_ARGS(sc_exit(ret,) mqdes, u_mqstat, u_omqstat),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(mqd_t, mqdes, mqdes)) sc_in(ctf_integer(const struct mq_attr *, u_mqstat, u_mqstat)) sc_out(ctf_integer(struct mq_attr *, u_omqstat, u_omqstat)))
)
#endif
#ifndef OVERRIDE_64_inotify_add_watch
SC_LTTNG_TRACEPOINT_EVENT(inotify_add_watch,
	TP_PROTO(sc_exit(long ret,) int fd, const char * pathname, u32 mask),
	TP_ARGS(sc_exit(ret,) fd, pathname, mask),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(u32, mask, mask)))
)
#endif
#ifndef OVERRIDE_64_mkdirat
SC_LTTNG_TRACEPOINT_EVENT(mkdirat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * pathname, umode_t mode),
	TP_ARGS(sc_exit(ret,) dfd, pathname, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_64_futimesat
SC_LTTNG_TRACEPOINT_EVENT(futimesat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, struct timeval * utimes),
	TP_ARGS(sc_exit(ret,) dfd, filename, utimes),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(struct timeval *, utimes, utimes)))
)
#endif
#ifndef OVERRIDE_64_unlinkat
SC_LTTNG_TRACEPOINT_EVENT(unlinkat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * pathname, int flag),
	TP_ARGS(sc_exit(ret,) dfd, pathname, flag),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_integer(int, flag, flag)))
)
#endif
#ifndef OVERRIDE_64_symlinkat
SC_LTTNG_TRACEPOINT_EVENT(symlinkat,
	TP_PROTO(sc_exit(long ret,) const char * oldname, int newdfd, const char * newname),
	TP_ARGS(sc_exit(ret,) oldname, newdfd, newname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(oldname, oldname)) sc_in(ctf_integer(int, newdfd, newdfd)) sc_in(ctf_user_string(newname, newname)))
)
#endif
#ifndef OVERRIDE_64_fchmodat
SC_LTTNG_TRACEPOINT_EVENT(fchmodat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, umode_t mode),
	TP_ARGS(sc_exit(ret,) dfd, filename, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_64_faccessat
SC_LTTNG_TRACEPOINT_EVENT(faccessat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, int mode),
	TP_ARGS(sc_exit(ret,) dfd, filename, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(int, mode, mode)))
)
#endif
#ifndef OVERRIDE_64_get_robust_list
SC_LTTNG_TRACEPOINT_EVENT(get_robust_list,
	TP_PROTO(sc_exit(long ret,) int pid, struct robust_list_head * * head_ptr, size_t * len_ptr),
	TP_ARGS(sc_exit(ret,) pid, head_ptr, len_ptr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, pid, pid)) sc_out(ctf_integer(struct robust_list_head * *, head_ptr, head_ptr)) sc_out(ctf_integer(size_t *, len_ptr, len_ptr)))
)
#endif
#ifndef OVERRIDE_64_signalfd
SC_LTTNG_TRACEPOINT_EVENT(signalfd,
	TP_PROTO(sc_exit(long ret,) int ufd, sigset_t * user_mask, size_t sizemask),
	TP_ARGS(sc_exit(ret,) ufd, user_mask, sizemask),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, ufd, ufd)) sc_in(ctf_integer(sigset_t *, user_mask, user_mask)) sc_in(ctf_integer(size_t, sizemask, sizemask)))
)
#endif
#ifndef OVERRIDE_64_open_by_handle_at
SC_LTTNG_TRACEPOINT_EVENT(open_by_handle_at,
	TP_PROTO(sc_exit(long ret,) int mountdirfd, struct file_handle * handle, int flags),
	TP_ARGS(sc_exit(ret,) mountdirfd, handle, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, mountdirfd, mountdirfd)) sc_in(ctf_integer(struct file_handle *, handle, handle)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_getcpu
SC_LTTNG_TRACEPOINT_EVENT(getcpu,
	TP_PROTO(sc_exit(long ret,) unsigned * cpup, unsigned * nodep, struct getcpu_cache * unused),
	TP_ARGS(sc_exit(ret,) cpup, nodep, unused),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(unsigned *, cpup, cpup)) sc_out(ctf_integer(unsigned *, nodep, nodep)) sc_inout(ctf_integer(struct getcpu_cache *, unused, unused)))
)
#endif
#ifndef OVERRIDE_64_finit_module
SC_LTTNG_TRACEPOINT_EVENT(finit_module,
	TP_PROTO(sc_exit(long ret,) int fd, const char * uargs, int flags),
	TP_ARGS(sc_exit(ret,) fd, uargs, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(const char *, uargs, uargs)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_rt_sigaction
SC_LTTNG_TRACEPOINT_EVENT(rt_sigaction,
	TP_PROTO(sc_exit(long ret,) int sig, const struct sigaction * act, struct sigaction * oact, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) sig, act, oact, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, sig, sig)) sc_in(ctf_integer(const struct sigaction *, act, act)) sc_out(ctf_integer(struct sigaction *, oact, oact)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_64_rt_sigprocmask
SC_LTTNG_TRACEPOINT_EVENT(rt_sigprocmask,
	TP_PROTO(sc_exit(long ret,) int how, sigset_t * nset, sigset_t * oset, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) how, nset, oset, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, how, how)) sc_in(ctf_integer(sigset_t *, nset, nset)) sc_out(ctf_integer(sigset_t *, oset, oset)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_64_pread64
SC_LTTNG_TRACEPOINT_EVENT(pread64,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, char * buf, size_t count, loff_t pos),
	TP_ARGS(sc_exit(ret,) fd, buf, count, pos),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(size_t, count, count)) sc_in(ctf_integer(loff_t, pos, pos)))
)
#endif
#ifndef OVERRIDE_64_pwrite64
SC_LTTNG_TRACEPOINT_EVENT(pwrite64,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, const char * buf, size_t count, loff_t pos),
	TP_ARGS(sc_exit(ret,) fd, buf, count, pos),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, fd, fd)) sc_in(ctf_integer(const char *, buf, buf)) sc_in(ctf_integer(size_t, count, count)) sc_in(ctf_integer(loff_t, pos, pos)))
)
#endif
#ifndef OVERRIDE_64_sendfile64
SC_LTTNG_TRACEPOINT_EVENT(sendfile64,
	TP_PROTO(sc_exit(long ret,) int out_fd, int in_fd, loff_t * offset, size_t count),
	TP_ARGS(sc_exit(ret,) out_fd, in_fd, offset, count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, out_fd, out_fd)) sc_in(ctf_integer(int, in_fd, in_fd)) sc_inout(ctf_integer(loff_t *, offset, offset)) sc_in(ctf_integer(size_t, count, count)))
)
#endif
#ifndef OVERRIDE_64_socketpair
SC_LTTNG_TRACEPOINT_EVENT(socketpair,
	TP_PROTO(sc_exit(long ret,) int family, int type, int protocol, int * usockvec),
	TP_ARGS(sc_exit(ret,) family, type, protocol, usockvec),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, family, family)) sc_in(ctf_integer(int, type, type)) sc_in(ctf_integer(int, protocol, protocol)) sc_out(ctf_integer(int *, usockvec, usockvec)))
)
#endif
#ifndef OVERRIDE_64_wait4
SC_LTTNG_TRACEPOINT_EVENT(wait4,
	TP_PROTO(sc_exit(long ret,) pid_t upid, int * stat_addr, int options, struct rusage * ru),
	TP_ARGS(sc_exit(ret,) upid, stat_addr, options, ru),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, upid, upid)) sc_out(ctf_integer(int *, stat_addr, stat_addr)) sc_in(ctf_integer(int, options, options)) sc_out(ctf_integer(struct rusage *, ru, ru)))
)
#endif
#ifndef OVERRIDE_64_msgsnd
SC_LTTNG_TRACEPOINT_EVENT(msgsnd,
	TP_PROTO(sc_exit(long ret,) int msqid, struct msgbuf * msgp, size_t msgsz, int msgflg),
	TP_ARGS(sc_exit(ret,) msqid, msgp, msgsz, msgflg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, msqid, msqid)) sc_in(ctf_integer(struct msgbuf *, msgp, msgp)) sc_in(ctf_integer(size_t, msgsz, msgsz)) sc_in(ctf_integer(int, msgflg, msgflg)))
)
#endif
#ifndef OVERRIDE_64_rt_sigtimedwait
SC_LTTNG_TRACEPOINT_EVENT(rt_sigtimedwait,
	TP_PROTO(sc_exit(long ret,) const sigset_t * uthese, siginfo_t * uinfo, const struct timespec * uts, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) uthese, uinfo, uts, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(const sigset_t *, uthese, uthese)) sc_out(ctf_integer(siginfo_t *, uinfo, uinfo)) sc_in(ctf_integer(const struct timespec *, uts, uts)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_64_reboot
SC_LTTNG_TRACEPOINT_EVENT(reboot,
	TP_PROTO(sc_exit(long ret,) int magic1, int magic2, unsigned int cmd, void * arg),
	TP_ARGS(sc_exit(ret,) magic1, magic2, cmd, arg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, magic1, magic1)) sc_in(ctf_integer(int, magic2, magic2)) sc_in(ctf_integer(unsigned int, cmd, cmd)) sc_in(ctf_integer(void *, arg, arg)))
)
#endif
#ifndef OVERRIDE_64_quotactl
SC_LTTNG_TRACEPOINT_EVENT(quotactl,
	TP_PROTO(sc_exit(long ret,) unsigned int cmd, const char * special, qid_t id, void * addr),
	TP_ARGS(sc_exit(ret,) cmd, special, id, addr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned int, cmd, cmd)) sc_in(ctf_integer(const char *, special, special)) sc_in(ctf_integer(qid_t, id, id)) sc_inout(ctf_integer(void *, addr, addr)))
)
#endif
#ifndef OVERRIDE_64_getxattr
SC_LTTNG_TRACEPOINT_EVENT(getxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name, void * value, size_t size),
	TP_ARGS(sc_exit(ret,) pathname, name, value, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_user_string(name, name)) sc_out(ctf_integer(void *, value, value)) sc_in(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_64_lgetxattr
SC_LTTNG_TRACEPOINT_EVENT(lgetxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name, void * value, size_t size),
	TP_ARGS(sc_exit(ret,) pathname, name, value, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_user_string(name, name)) sc_out(ctf_integer(void *, value, value)) sc_in(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_64_fgetxattr
SC_LTTNG_TRACEPOINT_EVENT(fgetxattr,
	TP_PROTO(sc_exit(long ret,) int fd, const char * name, void * value, size_t size),
	TP_ARGS(sc_exit(ret,) fd, name, value, size),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_user_string(name, name)) sc_out(ctf_integer(void *, value, value)) sc_in(ctf_integer(size_t, size, size)))
)
#endif
#ifndef OVERRIDE_64_semtimedop
SC_LTTNG_TRACEPOINT_EVENT(semtimedop,
	TP_PROTO(sc_exit(long ret,) int semid, struct sembuf * tsops, unsigned nsops, const struct timespec * timeout),
	TP_ARGS(sc_exit(ret,) semid, tsops, nsops, timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, semid, semid)) sc_in(ctf_integer(struct sembuf *, tsops, tsops)) sc_in(ctf_integer(unsigned, nsops, nsops)) sc_inout(ctf_integer(const struct timespec *, timeout, timeout)))
)
#endif
#ifndef OVERRIDE_64_timer_settime
SC_LTTNG_TRACEPOINT_EVENT(timer_settime,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id, int flags, const struct itimerspec * new_setting, struct itimerspec * old_setting),
	TP_ARGS(sc_exit(ret,) timer_id, flags, new_setting, old_setting),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(timer_t, timer_id, timer_id)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(const struct itimerspec *, new_setting, new_setting)) sc_out(ctf_integer(struct itimerspec *, old_setting, old_setting)))
)
#endif
#ifndef OVERRIDE_64_clock_nanosleep
SC_LTTNG_TRACEPOINT_EVENT(clock_nanosleep,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, int flags, const struct timespec * rqtp, struct timespec * rmtp),
	TP_ARGS(sc_exit(ret,) which_clock, flags, rqtp, rmtp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(const clockid_t, which_clock, which_clock)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(const struct timespec *, rqtp, rqtp)) sc_out(ctf_integer(struct timespec *, rmtp, rmtp)))
)
#endif
#ifndef OVERRIDE_64_epoll_wait
SC_LTTNG_TRACEPOINT_EVENT(epoll_wait,
	TP_PROTO(sc_exit(long ret,) int epfd, struct epoll_event * events, int maxevents, int timeout),
	TP_ARGS(sc_exit(ret,) epfd, events, maxevents, timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, epfd, epfd)) sc_out(ctf_integer(struct epoll_event *, events, events)) sc_in(ctf_integer(int, maxevents, maxevents)) sc_in(ctf_integer(int, timeout, timeout)))
)
#endif
#ifndef OVERRIDE_64_epoll_ctl
SC_LTTNG_TRACEPOINT_EVENT(epoll_ctl,
	TP_PROTO(sc_exit(long ret,) int epfd, int op, int fd, struct epoll_event * event),
	TP_ARGS(sc_exit(ret,) epfd, op, fd, event),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, epfd, epfd)) sc_in(ctf_integer(int, op, op)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(struct epoll_event *, event, event)))
)
#endif
#ifndef OVERRIDE_64_mq_open
SC_LTTNG_TRACEPOINT_EVENT(mq_open,
	TP_PROTO(sc_exit(long ret,) const char * u_name, int oflag, umode_t mode, struct mq_attr * u_attr),
	TP_ARGS(sc_exit(ret,) u_name, oflag, mode, u_attr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(u_name, u_name)) sc_in(ctf_integer(int, oflag, oflag)) sc_in(ctf_integer(umode_t, mode, mode)) sc_in(ctf_integer(struct mq_attr *, u_attr, u_attr)))
)
#endif
#ifndef OVERRIDE_64_kexec_load
SC_LTTNG_TRACEPOINT_EVENT(kexec_load,
	TP_PROTO(sc_exit(long ret,) unsigned long entry, unsigned long nr_segments, struct kexec_segment * segments, unsigned long flags),
	TP_ARGS(sc_exit(ret,) entry, nr_segments, segments, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, entry, entry)) sc_in(ctf_integer(unsigned long, nr_segments, nr_segments)) sc_in(ctf_integer(struct kexec_segment *, segments, segments)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_request_key
SC_LTTNG_TRACEPOINT_EVENT(request_key,
	TP_PROTO(sc_exit(long ret,) const char * _type, const char * _description, const char * _callout_info, key_serial_t destringid),
	TP_ARGS(sc_exit(ret,) _type, _description, _callout_info, destringid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(_type, _type)) sc_in(ctf_integer(const char *, _description, _description)) sc_in(ctf_integer(const char *, _callout_info, _callout_info)) sc_in(ctf_integer(key_serial_t, destringid, destringid)))
)
#endif
#ifndef OVERRIDE_64_migrate_pages
SC_LTTNG_TRACEPOINT_EVENT(migrate_pages,
	TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned long maxnode, const unsigned long * old_nodes, const unsigned long * new_nodes),
	TP_ARGS(sc_exit(ret,) pid, maxnode, old_nodes, new_nodes),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(unsigned long, maxnode, maxnode)) sc_in(ctf_integer(const unsigned long *, old_nodes, old_nodes)) sc_in(ctf_integer(const unsigned long *, new_nodes, new_nodes)))
)
#endif
#ifndef OVERRIDE_64_openat
SC_LTTNG_TRACEPOINT_EVENT(openat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, int flags, umode_t mode),
	TP_ARGS(sc_exit(ret,) dfd, filename, flags, mode),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(umode_t, mode, mode)))
)
#endif
#ifndef OVERRIDE_64_mknodat
SC_LTTNG_TRACEPOINT_EVENT(mknodat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, umode_t mode, unsigned dev),
	TP_ARGS(sc_exit(ret,) dfd, filename, mode, dev),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(umode_t, mode, mode)) sc_in(ctf_integer(unsigned, dev, dev)))
)
#endif
#ifndef OVERRIDE_64_newfstatat
SC_LTTNG_TRACEPOINT_EVENT(newfstatat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, struct stat * statbuf, int flag),
	TP_ARGS(sc_exit(ret,) dfd, filename, statbuf, flag),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_out(ctf_integer(struct stat *, statbuf, statbuf)) sc_in(ctf_integer(int, flag, flag)))
)
#endif
#ifndef OVERRIDE_64_renameat
SC_LTTNG_TRACEPOINT_EVENT(renameat,
	TP_PROTO(sc_exit(long ret,) int olddfd, const char * oldname, int newdfd, const char * newname),
	TP_ARGS(sc_exit(ret,) olddfd, oldname, newdfd, newname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, olddfd, olddfd)) sc_in(ctf_user_string(oldname, oldname)) sc_in(ctf_integer(int, newdfd, newdfd)) sc_in(ctf_user_string(newname, newname)))
)
#endif
#ifndef OVERRIDE_64_readlinkat
SC_LTTNG_TRACEPOINT_EVENT(readlinkat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * pathname, char * buf, int bufsiz),
	TP_ARGS(sc_exit(ret,) dfd, pathname, buf, bufsiz),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(pathname, pathname)) sc_out(ctf_integer(char *, buf, buf)) sc_in(ctf_integer(int, bufsiz, bufsiz)))
)
#endif
#ifndef OVERRIDE_64_vmsplice
SC_LTTNG_TRACEPOINT_EVENT(vmsplice,
	TP_PROTO(sc_exit(long ret,) int fd, const struct iovec * iov, unsigned long nr_segs, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, iov, nr_segs, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(const struct iovec *, iov, iov)) sc_in(ctf_integer(unsigned long, nr_segs, nr_segs)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_utimensat
SC_LTTNG_TRACEPOINT_EVENT(utimensat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, struct timespec * utimes, int flags),
	TP_ARGS(sc_exit(ret,) dfd, filename, utimes, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(struct timespec *, utimes, utimes)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_timerfd_settime
SC_LTTNG_TRACEPOINT_EVENT(timerfd_settime,
	TP_PROTO(sc_exit(long ret,) int ufd, int flags, const struct itimerspec * utmr, struct itimerspec * otmr),
	TP_ARGS(sc_exit(ret,) ufd, flags, utmr, otmr),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, ufd, ufd)) sc_in(ctf_integer(int, flags, flags)) sc_in(ctf_integer(const struct itimerspec *, utmr, utmr)) sc_out(ctf_integer(struct itimerspec *, otmr, otmr)))
)
#endif
#ifndef OVERRIDE_64_accept4
SC_LTTNG_TRACEPOINT_EVENT(accept4,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * upeer_sockaddr, int * upeer_addrlen, int flags),
	TP_ARGS(sc_exit(ret,) fd, upeer_sockaddr, upeer_addrlen, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(struct sockaddr *, upeer_sockaddr, upeer_sockaddr)) sc_inout(ctf_integer(int *, upeer_addrlen, upeer_addrlen)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_signalfd4
SC_LTTNG_TRACEPOINT_EVENT(signalfd4,
	TP_PROTO(sc_exit(long ret,) int ufd, sigset_t * user_mask, size_t sizemask, int flags),
	TP_ARGS(sc_exit(ret,) ufd, user_mask, sizemask, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, ufd, ufd)) sc_in(ctf_integer(sigset_t *, user_mask, user_mask)) sc_in(ctf_integer(size_t, sizemask, sizemask)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_rt_tgsigqueueinfo
SC_LTTNG_TRACEPOINT_EVENT(rt_tgsigqueueinfo,
	TP_PROTO(sc_exit(long ret,) pid_t tgid, pid_t pid, int sig, siginfo_t * uinfo),
	TP_ARGS(sc_exit(ret,) tgid, pid, sig, uinfo),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, tgid, tgid)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, sig, sig)) sc_in(ctf_integer(siginfo_t *, uinfo, uinfo)))
)
#endif
#ifndef OVERRIDE_64_prlimit64
SC_LTTNG_TRACEPOINT_EVENT(prlimit64,
	TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned int resource, const struct rlimit64 * new_rlim, struct rlimit64 * old_rlim),
	TP_ARGS(sc_exit(ret,) pid, resource, new_rlim, old_rlim),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(unsigned int, resource, resource)) sc_in(ctf_integer(const struct rlimit64 *, new_rlim, new_rlim)) sc_out(ctf_integer(struct rlimit64 *, old_rlim, old_rlim)))
)
#endif
#ifndef OVERRIDE_64_sendmmsg
SC_LTTNG_TRACEPOINT_EVENT(sendmmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct mmsghdr * mmsg, unsigned int vlen, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, mmsg, vlen, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(struct mmsghdr *, mmsg, mmsg)) sc_in(ctf_integer(unsigned int, vlen, vlen)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_select
SC_LTTNG_TRACEPOINT_EVENT(select,
	TP_PROTO(sc_exit(long ret,) int n, fd_set * inp, fd_set * outp, fd_set * exp, struct timeval * tvp),
	TP_ARGS(sc_exit(ret,) n, inp, outp, exp, tvp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, n, n)) sc_inout(ctf_integer(fd_set *, inp, inp)) sc_inout(ctf_integer(fd_set *, outp, outp)) sc_inout(ctf_integer(fd_set *, exp, exp)) sc_inout(ctf_integer(struct timeval *, tvp, tvp)))
)
#endif
#ifndef OVERRIDE_64_setsockopt
SC_LTTNG_TRACEPOINT_EVENT(setsockopt,
	TP_PROTO(sc_exit(long ret,) int fd, int level, int optname, char * optval, int optlen),
	TP_ARGS(sc_exit(ret,) fd, level, optname, optval, optlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(int, level, level)) sc_in(ctf_integer(int, optname, optname)) sc_in(ctf_integer(char *, optval, optval)) sc_in(ctf_integer(int, optlen, optlen)))
)
#endif
#ifndef OVERRIDE_64_getsockopt
SC_LTTNG_TRACEPOINT_EVENT(getsockopt,
	TP_PROTO(sc_exit(long ret,) int fd, int level, int optname, char * optval, int * optlen),
	TP_ARGS(sc_exit(ret,) fd, level, optname, optval, optlen),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(int, level, level)) sc_in(ctf_integer(int, optname, optname)) sc_out(ctf_integer(char *, optval, optval)) sc_inout(ctf_integer(int *, optlen, optlen)))
)
#endif
#ifndef OVERRIDE_64_msgrcv
SC_LTTNG_TRACEPOINT_EVENT(msgrcv,
	TP_PROTO(sc_exit(long ret,) int msqid, struct msgbuf * msgp, size_t msgsz, long msgtyp, int msgflg),
	TP_ARGS(sc_exit(ret,) msqid, msgp, msgsz, msgtyp, msgflg),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, msqid, msqid)) sc_out(ctf_integer(struct msgbuf *, msgp, msgp)) sc_in(ctf_integer(size_t, msgsz, msgsz)) sc_in(ctf_integer(long, msgtyp, msgtyp)) sc_in(ctf_integer(int, msgflg, msgflg)))
)
#endif
#ifndef OVERRIDE_64_mount
SC_LTTNG_TRACEPOINT_EVENT(mount,
	TP_PROTO(sc_exit(long ret,) char * dev_name, char * dir_name, char * type, unsigned long flags, void * data),
	TP_ARGS(sc_exit(ret,) dev_name, dir_name, type, flags, data),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(dev_name, dev_name)) sc_in(ctf_user_string(dir_name, dir_name)) sc_in(ctf_user_string(type, type)) sc_in(ctf_integer(unsigned long, flags, flags)) sc_in(ctf_integer(void *, data, data)))
)
#endif
#ifndef OVERRIDE_64_setxattr
SC_LTTNG_TRACEPOINT_EVENT(setxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name, const void * value, size_t size, int flags),
	TP_ARGS(sc_exit(ret,) pathname, name, value, size, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_user_string(name, name)) sc_in(ctf_integer(const void *, value, value)) sc_in(ctf_integer(size_t, size, size)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_lsetxattr
SC_LTTNG_TRACEPOINT_EVENT(lsetxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name, const void * value, size_t size, int flags),
	TP_ARGS(sc_exit(ret,) pathname, name, value, size, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(pathname, pathname)) sc_in(ctf_user_string(name, name)) sc_in(ctf_integer(const void *, value, value)) sc_in(ctf_integer(size_t, size, size)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_fsetxattr
SC_LTTNG_TRACEPOINT_EVENT(fsetxattr,
	TP_PROTO(sc_exit(long ret,) int fd, const char * name, const void * value, size_t size, int flags),
	TP_ARGS(sc_exit(ret,) fd, name, value, size, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_user_string(name, name)) sc_in(ctf_integer(const void *, value, value)) sc_in(ctf_integer(size_t, size, size)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_io_getevents
SC_LTTNG_TRACEPOINT_EVENT(io_getevents,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx_id, long min_nr, long nr, struct io_event * events, struct timespec * timeout),
	TP_ARGS(sc_exit(ret,) ctx_id, min_nr, nr, events, timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(aio_context_t, ctx_id, ctx_id)) sc_in(ctf_integer(long, min_nr, min_nr)) sc_in(ctf_integer(long, nr, nr)) sc_out(ctf_integer(struct io_event *, events, events)) sc_inout(ctf_integer(struct timespec *, timeout, timeout)))
)
#endif
#ifndef OVERRIDE_64_get_mempolicy
SC_LTTNG_TRACEPOINT_EVENT(get_mempolicy,
	TP_PROTO(sc_exit(long ret,) int * policy, unsigned long * nmask, unsigned long maxnode, unsigned long addr, unsigned long flags),
	TP_ARGS(sc_exit(ret,) policy, nmask, maxnode, addr, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_out(ctf_integer(int *, policy, policy)) sc_out(ctf_integer(unsigned long *, nmask, nmask)) sc_in(ctf_integer(unsigned long, maxnode, maxnode)) sc_in(ctf_integer(unsigned long, addr, addr)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_mq_timedsend
SC_LTTNG_TRACEPOINT_EVENT(mq_timedsend,
	TP_PROTO(sc_exit(long ret,) mqd_t mqdes, const char * u_msg_ptr, size_t msg_len, unsigned int msg_prio, const struct timespec * u_abs_timeout),
	TP_ARGS(sc_exit(ret,) mqdes, u_msg_ptr, msg_len, msg_prio, u_abs_timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(mqd_t, mqdes, mqdes)) sc_in(ctf_integer(const char *, u_msg_ptr, u_msg_ptr)) sc_in(ctf_integer(size_t, msg_len, msg_len)) sc_in(ctf_integer(unsigned int, msg_prio, msg_prio)) sc_in(ctf_integer(const struct timespec *, u_abs_timeout, u_abs_timeout)))
)
#endif
#ifndef OVERRIDE_64_mq_timedreceive
SC_LTTNG_TRACEPOINT_EVENT(mq_timedreceive,
	TP_PROTO(sc_exit(long ret,) mqd_t mqdes, char * u_msg_ptr, size_t msg_len, unsigned int * u_msg_prio, const struct timespec * u_abs_timeout),
	TP_ARGS(sc_exit(ret,) mqdes, u_msg_ptr, msg_len, u_msg_prio, u_abs_timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(mqd_t, mqdes, mqdes)) sc_out(ctf_integer(char *, u_msg_ptr, u_msg_ptr)) sc_in(ctf_integer(size_t, msg_len, msg_len)) sc_out(ctf_integer(unsigned int *, u_msg_prio, u_msg_prio)) sc_in(ctf_integer(const struct timespec *, u_abs_timeout, u_abs_timeout)))
)
#endif
#ifndef OVERRIDE_64_waitid
SC_LTTNG_TRACEPOINT_EVENT(waitid,
	TP_PROTO(sc_exit(long ret,) int which, pid_t upid, struct siginfo * infop, int options, struct rusage * ru),
	TP_ARGS(sc_exit(ret,) which, upid, infop, options, ru),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, which, which)) sc_in(ctf_integer(pid_t, upid, upid)) sc_out(ctf_integer(struct siginfo *, infop, infop)) sc_in(ctf_integer(int, options, options)) sc_out(ctf_integer(struct rusage *, ru, ru)))
)
#endif
#ifndef OVERRIDE_64_add_key
SC_LTTNG_TRACEPOINT_EVENT(add_key,
	TP_PROTO(sc_exit(long ret,) const char * _type, const char * _description, const void * _payload, size_t plen, key_serial_t ringid),
	TP_ARGS(sc_exit(ret,) _type, _description, _payload, plen, ringid),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_user_string(_type, _type)) sc_in(ctf_integer(const char *, _description, _description)) sc_in(ctf_integer(const void *, _payload, _payload)) sc_in(ctf_integer(size_t, plen, plen)) sc_in(ctf_integer(key_serial_t, ringid, ringid)))
)
#endif
#ifndef OVERRIDE_64_fchownat
SC_LTTNG_TRACEPOINT_EVENT(fchownat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, uid_t user, gid_t group, int flag),
	TP_ARGS(sc_exit(ret,) dfd, filename, user, group, flag),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(filename, filename)) sc_in(ctf_integer(uid_t, user, user)) sc_in(ctf_integer(gid_t, group, group)) sc_in(ctf_integer(int, flag, flag)))
)
#endif
#ifndef OVERRIDE_64_linkat
SC_LTTNG_TRACEPOINT_EVENT(linkat,
	TP_PROTO(sc_exit(long ret,) int olddfd, const char * oldname, int newdfd, const char * newname, int flags),
	TP_ARGS(sc_exit(ret,) olddfd, oldname, newdfd, newname, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, olddfd, olddfd)) sc_in(ctf_user_string(oldname, oldname)) sc_in(ctf_integer(int, newdfd, newdfd)) sc_in(ctf_user_string(newname, newname)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_ppoll
SC_LTTNG_TRACEPOINT_EVENT(ppoll,
	TP_PROTO(sc_exit(long ret,) struct pollfd * ufds, unsigned int nfds, struct timespec * tsp, const sigset_t * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) ufds, nfds, tsp, sigmask, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(struct pollfd *, ufds, ufds)) sc_in(ctf_integer(unsigned int, nfds, nfds)) sc_in(ctf_integer(struct timespec *, tsp, tsp)) sc_in(ctf_integer(const sigset_t *, sigmask, sigmask)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_64_preadv
SC_LTTNG_TRACEPOINT_EVENT(preadv,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen, unsigned long pos_l, unsigned long pos_h),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen, pos_l, pos_h),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, fd, fd)) sc_out(ctf_integer(const struct iovec *, vec, vec)) sc_in(ctf_integer(unsigned long, vlen, vlen)) sc_in(ctf_integer(unsigned long, pos_l, pos_l)) sc_in(ctf_integer(unsigned long, pos_h, pos_h)))
)
#endif
#ifndef OVERRIDE_64_pwritev
SC_LTTNG_TRACEPOINT_EVENT(pwritev,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen, unsigned long pos_l, unsigned long pos_h),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen, pos_l, pos_h),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, fd, fd)) sc_in(ctf_integer(const struct iovec *, vec, vec)) sc_in(ctf_integer(unsigned long, vlen, vlen)) sc_in(ctf_integer(unsigned long, pos_l, pos_l)) sc_in(ctf_integer(unsigned long, pos_h, pos_h)))
)
#endif
#ifndef OVERRIDE_64_perf_event_open
SC_LTTNG_TRACEPOINT_EVENT(perf_event_open,
	TP_PROTO(sc_exit(long ret,) struct perf_event_attr * attr_uptr, pid_t pid, int cpu, int group_fd, unsigned long flags),
	TP_ARGS(sc_exit(ret,) attr_uptr, pid, cpu, group_fd, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(struct perf_event_attr *, attr_uptr, attr_uptr)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(int, cpu, cpu)) sc_in(ctf_integer(int, group_fd, group_fd)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_recvmmsg
SC_LTTNG_TRACEPOINT_EVENT(recvmmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct mmsghdr * mmsg, unsigned int vlen, unsigned int flags, struct timespec * timeout),
	TP_ARGS(sc_exit(ret,) fd, mmsg, vlen, flags, timeout),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(struct mmsghdr *, mmsg, mmsg)) sc_in(ctf_integer(unsigned int, vlen, vlen)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_inout(ctf_integer(struct timespec *, timeout, timeout)))
)
#endif
#ifndef OVERRIDE_64_fanotify_mark
SC_LTTNG_TRACEPOINT_EVENT(fanotify_mark,
	TP_PROTO(sc_exit(long ret,) int fanotify_fd, unsigned int flags, __u64 mask, int dfd, const char * pathname),
	TP_ARGS(sc_exit(ret,) fanotify_fd, flags, mask, dfd, pathname),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fanotify_fd, fanotify_fd)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_in(ctf_integer(__u64, mask, mask)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(pathname, pathname)))
)
#endif
#ifndef OVERRIDE_64_name_to_handle_at
SC_LTTNG_TRACEPOINT_EVENT(name_to_handle_at,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * name, struct file_handle * handle, int * mnt_id, int flag),
	TP_ARGS(sc_exit(ret,) dfd, name, handle, mnt_id, flag),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, dfd, dfd)) sc_in(ctf_user_string(name, name)) sc_inout(ctf_integer(struct file_handle *, handle, handle)) sc_out(ctf_integer(int *, mnt_id, mnt_id)) sc_in(ctf_integer(int, flag, flag)))
)
#endif
#ifndef OVERRIDE_64_sendto
SC_LTTNG_TRACEPOINT_EVENT(sendto,
	TP_PROTO(sc_exit(long ret,) int fd, void * buff, size_t len, unsigned int flags, struct sockaddr * addr, int addr_len),
	TP_ARGS(sc_exit(ret,) fd, buff, len, flags, addr, addr_len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_in(ctf_integer(void *, buff, buff)) sc_in(ctf_integer(size_t, len, len)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_in(ctf_integer(struct sockaddr *, addr, addr)) sc_in(ctf_integer(int, addr_len, addr_len)))
)
#endif
#ifndef OVERRIDE_64_recvfrom
SC_LTTNG_TRACEPOINT_EVENT(recvfrom,
	TP_PROTO(sc_exit(long ret,) int fd, void * ubuf, size_t size, unsigned int flags, struct sockaddr * addr, int * addr_len),
	TP_ARGS(sc_exit(ret,) fd, ubuf, size, flags, addr, addr_len),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd, fd)) sc_out(ctf_integer(void *, ubuf, ubuf)) sc_in(ctf_integer(size_t, size, size)) sc_in(ctf_integer(unsigned int, flags, flags)) sc_out(ctf_integer(struct sockaddr *, addr, addr)) sc_inout(ctf_integer(int *, addr_len, addr_len)))
)
#endif
#ifndef OVERRIDE_64_futex
SC_LTTNG_TRACEPOINT_EVENT(futex,
	TP_PROTO(sc_exit(long ret,) u32 * uaddr, int op, u32 val, struct timespec * utime, u32 * uaddr2, u32 val3),
	TP_ARGS(sc_exit(ret,) uaddr, op, val, utime, uaddr2, val3),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_inout(ctf_integer(u32 *, uaddr, uaddr)) sc_in(ctf_integer(int, op, op)) sc_in(ctf_integer(u32, val, val)) sc_in(ctf_integer(struct timespec *, utime, utime)) sc_inout(ctf_integer(u32 *, uaddr2, uaddr2)) sc_in(ctf_integer(u32, val3, val3)))
)
#endif
#ifndef OVERRIDE_64_mbind
SC_LTTNG_TRACEPOINT_EVENT(mbind,
	TP_PROTO(sc_exit(long ret,) unsigned long start, unsigned long len, unsigned long mode, unsigned long * nmask, unsigned long maxnode, unsigned flags),
	TP_ARGS(sc_exit(ret,) start, len, mode, nmask, maxnode, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(unsigned long, start, start)) sc_in(ctf_integer(unsigned long, len, len)) sc_in(ctf_integer(unsigned long, mode, mode)) sc_in(ctf_integer(unsigned long *, nmask, nmask)) sc_in(ctf_integer(unsigned long, maxnode, maxnode)) sc_in(ctf_integer(unsigned, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_pselect6
SC_LTTNG_TRACEPOINT_EVENT(pselect6,
	TP_PROTO(sc_exit(long ret,) int n, fd_set * inp, fd_set * outp, fd_set * exp, struct timespec * tsp, void * sig),
	TP_ARGS(sc_exit(ret,) n, inp, outp, exp, tsp, sig),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, n, n)) sc_inout(ctf_integer(fd_set *, inp, inp)) sc_inout(ctf_integer(fd_set *, outp, outp)) sc_inout(ctf_integer(fd_set *, exp, exp)) sc_inout(ctf_integer(struct timespec *, tsp, tsp)) sc_in(ctf_integer(void *, sig, sig)))
)
#endif
#ifndef OVERRIDE_64_splice
SC_LTTNG_TRACEPOINT_EVENT(splice,
	TP_PROTO(sc_exit(long ret,) int fd_in, loff_t * off_in, int fd_out, loff_t * off_out, size_t len, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd_in, off_in, fd_out, off_out, len, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, fd_in, fd_in)) sc_in(ctf_integer(loff_t *, off_in, off_in)) sc_in(ctf_integer(int, fd_out, fd_out)) sc_in(ctf_integer(loff_t *, off_out, off_out)) sc_in(ctf_integer(size_t, len, len)) sc_in(ctf_integer(unsigned int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_move_pages
SC_LTTNG_TRACEPOINT_EVENT(move_pages,
	TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned long nr_pages, const void * * pages, const int * nodes, int * status, int flags),
	TP_ARGS(sc_exit(ret,) pid, nr_pages, pages, nodes, status, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(unsigned long, nr_pages, nr_pages)) sc_in(ctf_integer(const void * *, pages, pages)) sc_in(ctf_integer(const int *, nodes, nodes)) sc_out(ctf_integer(int *, status, status)) sc_in(ctf_integer(int, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_epoll_pwait
SC_LTTNG_TRACEPOINT_EVENT(epoll_pwait,
	TP_PROTO(sc_exit(long ret,) int epfd, struct epoll_event * events, int maxevents, int timeout, const sigset_t * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) epfd, events, maxevents, timeout, sigmask, sigsetsize),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(int, epfd, epfd)) sc_out(ctf_integer(struct epoll_event *, events, events)) sc_in(ctf_integer(int, maxevents, maxevents)) sc_in(ctf_integer(int, timeout, timeout)) sc_in(ctf_integer(const sigset_t *, sigmask, sigmask)) sc_in(ctf_integer(size_t, sigsetsize, sigsetsize)))
)
#endif
#ifndef OVERRIDE_64_process_vm_readv
SC_LTTNG_TRACEPOINT_EVENT(process_vm_readv,
	TP_PROTO(sc_exit(long ret,) pid_t pid, const struct iovec * lvec, unsigned long liovcnt, const struct iovec * rvec, unsigned long riovcnt, unsigned long flags),
	TP_ARGS(sc_exit(ret,) pid, lvec, liovcnt, rvec, riovcnt, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_out(ctf_integer(const struct iovec *, lvec, lvec)) sc_in(ctf_integer(unsigned long, liovcnt, liovcnt)) sc_in(ctf_integer(const struct iovec *, rvec, rvec)) sc_in(ctf_integer(unsigned long, riovcnt, riovcnt)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif
#ifndef OVERRIDE_64_process_vm_writev
SC_LTTNG_TRACEPOINT_EVENT(process_vm_writev,
	TP_PROTO(sc_exit(long ret,) pid_t pid, const struct iovec * lvec, unsigned long liovcnt, const struct iovec * rvec, unsigned long riovcnt, unsigned long flags),
	TP_ARGS(sc_exit(ret,) pid, lvec, liovcnt, rvec, riovcnt, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(const struct iovec *, lvec, lvec)) sc_in(ctf_integer(unsigned long, liovcnt, liovcnt)) sc_in(ctf_integer(const struct iovec *, rvec, rvec)) sc_in(ctf_integer(unsigned long, riovcnt, riovcnt)) sc_in(ctf_integer(unsigned long, flags, flags)))
)
#endif

#endif /*  _TRACE_SYSCALLS_POINTERS_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"

#else /* CREATE_SYSCALL_TABLE */

#include "x86-64-syscalls-3.10.0-rc7_pointers_override.h"
#include "syscalls_pointers_override.h"

#ifndef OVERRIDE_TABLE_64_read
TRACE_SYSCALL_TABLE(read, read, 0, 3)
#endif
#ifndef OVERRIDE_TABLE_64_write
TRACE_SYSCALL_TABLE(write, write, 1, 3)
#endif
#ifndef OVERRIDE_TABLE_64_open
TRACE_SYSCALL_TABLE(open, open, 2, 3)
#endif
#ifndef OVERRIDE_TABLE_64_newstat
TRACE_SYSCALL_TABLE(newstat, newstat, 4, 2)
#endif
#ifndef OVERRIDE_TABLE_64_newfstat
TRACE_SYSCALL_TABLE(newfstat, newfstat, 5, 2)
#endif
#ifndef OVERRIDE_TABLE_64_newlstat
TRACE_SYSCALL_TABLE(newlstat, newlstat, 6, 2)
#endif
#ifndef OVERRIDE_TABLE_64_poll
TRACE_SYSCALL_TABLE(poll, poll, 7, 3)
#endif
#ifndef OVERRIDE_TABLE_64_rt_sigaction
TRACE_SYSCALL_TABLE(rt_sigaction, rt_sigaction, 13, 4)
#endif
#ifndef OVERRIDE_TABLE_64_rt_sigprocmask
TRACE_SYSCALL_TABLE(rt_sigprocmask, rt_sigprocmask, 14, 4)
#endif
#ifndef OVERRIDE_TABLE_64_pread64
TRACE_SYSCALL_TABLE(pread64, pread64, 17, 4)
#endif
#ifndef OVERRIDE_TABLE_64_pwrite64
TRACE_SYSCALL_TABLE(pwrite64, pwrite64, 18, 4)
#endif
#ifndef OVERRIDE_TABLE_64_readv
TRACE_SYSCALL_TABLE(readv, readv, 19, 3)
#endif
#ifndef OVERRIDE_TABLE_64_writev
TRACE_SYSCALL_TABLE(writev, writev, 20, 3)
#endif
#ifndef OVERRIDE_TABLE_64_access
TRACE_SYSCALL_TABLE(access, access, 21, 2)
#endif
#ifndef OVERRIDE_TABLE_64_pipe
TRACE_SYSCALL_TABLE(pipe, pipe, 22, 1)
#endif
#ifndef OVERRIDE_TABLE_64_select
TRACE_SYSCALL_TABLE(select, select, 23, 5)
#endif
#ifndef OVERRIDE_TABLE_64_mincore
TRACE_SYSCALL_TABLE(mincore, mincore, 27, 3)
#endif
#ifndef OVERRIDE_TABLE_64_shmat
TRACE_SYSCALL_TABLE(shmat, shmat, 30, 3)
#endif
#ifndef OVERRIDE_TABLE_64_shmctl
TRACE_SYSCALL_TABLE(shmctl, shmctl, 31, 3)
#endif
#ifndef OVERRIDE_TABLE_64_nanosleep
TRACE_SYSCALL_TABLE(nanosleep, nanosleep, 35, 2)
#endif
#ifndef OVERRIDE_TABLE_64_getitimer
TRACE_SYSCALL_TABLE(getitimer, getitimer, 36, 2)
#endif
#ifndef OVERRIDE_TABLE_64_setitimer
TRACE_SYSCALL_TABLE(setitimer, setitimer, 38, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sendfile64
TRACE_SYSCALL_TABLE(sendfile64, sendfile64, 40, 4)
#endif
#ifndef OVERRIDE_TABLE_64_connect
TRACE_SYSCALL_TABLE(connect, connect, 42, 3)
#endif
#ifndef OVERRIDE_TABLE_64_accept
TRACE_SYSCALL_TABLE(accept, accept, 43, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sendto
TRACE_SYSCALL_TABLE(sendto, sendto, 44, 6)
#endif
#ifndef OVERRIDE_TABLE_64_recvfrom
TRACE_SYSCALL_TABLE(recvfrom, recvfrom, 45, 6)
#endif
#ifndef OVERRIDE_TABLE_64_sendmsg
TRACE_SYSCALL_TABLE(sendmsg, sendmsg, 46, 3)
#endif
#ifndef OVERRIDE_TABLE_64_recvmsg
TRACE_SYSCALL_TABLE(recvmsg, recvmsg, 47, 3)
#endif
#ifndef OVERRIDE_TABLE_64_bind
TRACE_SYSCALL_TABLE(bind, bind, 49, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getsockname
TRACE_SYSCALL_TABLE(getsockname, getsockname, 51, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getpeername
TRACE_SYSCALL_TABLE(getpeername, getpeername, 52, 3)
#endif
#ifndef OVERRIDE_TABLE_64_socketpair
TRACE_SYSCALL_TABLE(socketpair, socketpair, 53, 4)
#endif
#ifndef OVERRIDE_TABLE_64_setsockopt
TRACE_SYSCALL_TABLE(setsockopt, setsockopt, 54, 5)
#endif
#ifndef OVERRIDE_TABLE_64_getsockopt
TRACE_SYSCALL_TABLE(getsockopt, getsockopt, 55, 5)
#endif
#ifndef OVERRIDE_TABLE_64_wait4
TRACE_SYSCALL_TABLE(wait4, wait4, 61, 4)
#endif
#ifndef OVERRIDE_TABLE_64_newuname
TRACE_SYSCALL_TABLE(newuname, newuname, 63, 1)
#endif
#ifndef OVERRIDE_TABLE_64_semop
TRACE_SYSCALL_TABLE(semop, semop, 65, 3)
#endif
#ifndef OVERRIDE_TABLE_64_shmdt
TRACE_SYSCALL_TABLE(shmdt, shmdt, 67, 1)
#endif
#ifndef OVERRIDE_TABLE_64_msgsnd
TRACE_SYSCALL_TABLE(msgsnd, msgsnd, 69, 4)
#endif
#ifndef OVERRIDE_TABLE_64_msgrcv
TRACE_SYSCALL_TABLE(msgrcv, msgrcv, 70, 5)
#endif
#ifndef OVERRIDE_TABLE_64_msgctl
TRACE_SYSCALL_TABLE(msgctl, msgctl, 71, 3)
#endif
#ifndef OVERRIDE_TABLE_64_truncate
TRACE_SYSCALL_TABLE(truncate, truncate, 76, 2)
#endif
#ifndef OVERRIDE_TABLE_64_getdents
TRACE_SYSCALL_TABLE(getdents, getdents, 78, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getcwd
TRACE_SYSCALL_TABLE(getcwd, getcwd, 79, 2)
#endif
#ifndef OVERRIDE_TABLE_64_chdir
TRACE_SYSCALL_TABLE(chdir, chdir, 80, 1)
#endif
#ifndef OVERRIDE_TABLE_64_rename
TRACE_SYSCALL_TABLE(rename, rename, 82, 2)
#endif
#ifndef OVERRIDE_TABLE_64_mkdir
TRACE_SYSCALL_TABLE(mkdir, mkdir, 83, 2)
#endif
#ifndef OVERRIDE_TABLE_64_rmdir
TRACE_SYSCALL_TABLE(rmdir, rmdir, 84, 1)
#endif
#ifndef OVERRIDE_TABLE_64_creat
TRACE_SYSCALL_TABLE(creat, creat, 85, 2)
#endif
#ifndef OVERRIDE_TABLE_64_link
TRACE_SYSCALL_TABLE(link, link, 86, 2)
#endif
#ifndef OVERRIDE_TABLE_64_unlink
TRACE_SYSCALL_TABLE(unlink, unlink, 87, 1)
#endif
#ifndef OVERRIDE_TABLE_64_symlink
TRACE_SYSCALL_TABLE(symlink, symlink, 88, 2)
#endif
#ifndef OVERRIDE_TABLE_64_readlink
TRACE_SYSCALL_TABLE(readlink, readlink, 89, 3)
#endif
#ifndef OVERRIDE_TABLE_64_chmod
TRACE_SYSCALL_TABLE(chmod, chmod, 90, 2)
#endif
#ifndef OVERRIDE_TABLE_64_chown
TRACE_SYSCALL_TABLE(chown, chown, 92, 3)
#endif
#ifndef OVERRIDE_TABLE_64_lchown
TRACE_SYSCALL_TABLE(lchown, lchown, 94, 3)
#endif
#ifndef OVERRIDE_TABLE_64_gettimeofday
TRACE_SYSCALL_TABLE(gettimeofday, gettimeofday, 96, 2)
#endif
#ifndef OVERRIDE_TABLE_64_getrlimit
TRACE_SYSCALL_TABLE(getrlimit, getrlimit, 97, 2)
#endif
#ifndef OVERRIDE_TABLE_64_getrusage
TRACE_SYSCALL_TABLE(getrusage, getrusage, 98, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sysinfo
TRACE_SYSCALL_TABLE(sysinfo, sysinfo, 99, 1)
#endif
#ifndef OVERRIDE_TABLE_64_times
TRACE_SYSCALL_TABLE(times, times, 100, 1)
#endif
#ifndef OVERRIDE_TABLE_64_syslog
TRACE_SYSCALL_TABLE(syslog, syslog, 103, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getgroups
TRACE_SYSCALL_TABLE(getgroups, getgroups, 115, 2)
#endif
#ifndef OVERRIDE_TABLE_64_setgroups
TRACE_SYSCALL_TABLE(setgroups, setgroups, 116, 2)
#endif
#ifndef OVERRIDE_TABLE_64_getresuid
TRACE_SYSCALL_TABLE(getresuid, getresuid, 118, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getresgid
TRACE_SYSCALL_TABLE(getresgid, getresgid, 120, 3)
#endif
#ifndef OVERRIDE_TABLE_64_rt_sigpending
TRACE_SYSCALL_TABLE(rt_sigpending, rt_sigpending, 127, 2)
#endif
#ifndef OVERRIDE_TABLE_64_rt_sigtimedwait
TRACE_SYSCALL_TABLE(rt_sigtimedwait, rt_sigtimedwait, 128, 4)
#endif
#ifndef OVERRIDE_TABLE_64_rt_sigqueueinfo
TRACE_SYSCALL_TABLE(rt_sigqueueinfo, rt_sigqueueinfo, 129, 3)
#endif
#ifndef OVERRIDE_TABLE_64_rt_sigsuspend
TRACE_SYSCALL_TABLE(rt_sigsuspend, rt_sigsuspend, 130, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sigaltstack
TRACE_SYSCALL_TABLE(sigaltstack, sigaltstack, 131, 2)
#endif
#ifndef OVERRIDE_TABLE_64_utime
TRACE_SYSCALL_TABLE(utime, utime, 132, 2)
#endif
#ifndef OVERRIDE_TABLE_64_mknod
TRACE_SYSCALL_TABLE(mknod, mknod, 133, 3)
#endif
#ifndef OVERRIDE_TABLE_64_ustat
TRACE_SYSCALL_TABLE(ustat, ustat, 136, 2)
#endif
#ifndef OVERRIDE_TABLE_64_statfs
TRACE_SYSCALL_TABLE(statfs, statfs, 137, 2)
#endif
#ifndef OVERRIDE_TABLE_64_fstatfs
TRACE_SYSCALL_TABLE(fstatfs, fstatfs, 138, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sched_setparam
TRACE_SYSCALL_TABLE(sched_setparam, sched_setparam, 142, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sched_getparam
TRACE_SYSCALL_TABLE(sched_getparam, sched_getparam, 143, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sched_setscheduler
TRACE_SYSCALL_TABLE(sched_setscheduler, sched_setscheduler, 144, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sched_rr_get_interval
TRACE_SYSCALL_TABLE(sched_rr_get_interval, sched_rr_get_interval, 148, 2)
#endif
#ifndef OVERRIDE_TABLE_64_pivot_root
TRACE_SYSCALL_TABLE(pivot_root, pivot_root, 155, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sysctl
TRACE_SYSCALL_TABLE(sysctl, sysctl, 156, 1)
#endif
#ifndef OVERRIDE_TABLE_64_adjtimex
TRACE_SYSCALL_TABLE(adjtimex, adjtimex, 159, 1)
#endif
#ifndef OVERRIDE_TABLE_64_setrlimit
TRACE_SYSCALL_TABLE(setrlimit, setrlimit, 160, 2)
#endif
#ifndef OVERRIDE_TABLE_64_chroot
TRACE_SYSCALL_TABLE(chroot, chroot, 161, 1)
#endif
#ifndef OVERRIDE_TABLE_64_acct
TRACE_SYSCALL_TABLE(acct, acct, 163, 1)
#endif
#ifndef OVERRIDE_TABLE_64_settimeofday
TRACE_SYSCALL_TABLE(settimeofday, settimeofday, 164, 2)
#endif
#ifndef OVERRIDE_TABLE_64_mount
TRACE_SYSCALL_TABLE(mount, mount, 165, 5)
#endif
#ifndef OVERRIDE_TABLE_64_umount
TRACE_SYSCALL_TABLE(umount, umount, 166, 2)
#endif
#ifndef OVERRIDE_TABLE_64_swapon
TRACE_SYSCALL_TABLE(swapon, swapon, 167, 2)
#endif
#ifndef OVERRIDE_TABLE_64_swapoff
TRACE_SYSCALL_TABLE(swapoff, swapoff, 168, 1)
#endif
#ifndef OVERRIDE_TABLE_64_reboot
TRACE_SYSCALL_TABLE(reboot, reboot, 169, 4)
#endif
#ifndef OVERRIDE_TABLE_64_sethostname
TRACE_SYSCALL_TABLE(sethostname, sethostname, 170, 2)
#endif
#ifndef OVERRIDE_TABLE_64_setdomainname
TRACE_SYSCALL_TABLE(setdomainname, setdomainname, 171, 2)
#endif
#ifndef OVERRIDE_TABLE_64_init_module
TRACE_SYSCALL_TABLE(init_module, init_module, 175, 3)
#endif
#ifndef OVERRIDE_TABLE_64_delete_module
TRACE_SYSCALL_TABLE(delete_module, delete_module, 176, 2)
#endif
#ifndef OVERRIDE_TABLE_64_quotactl
TRACE_SYSCALL_TABLE(quotactl, quotactl, 179, 4)
#endif
#ifndef OVERRIDE_TABLE_64_setxattr
TRACE_SYSCALL_TABLE(setxattr, setxattr, 188, 5)
#endif
#ifndef OVERRIDE_TABLE_64_lsetxattr
TRACE_SYSCALL_TABLE(lsetxattr, lsetxattr, 189, 5)
#endif
#ifndef OVERRIDE_TABLE_64_fsetxattr
TRACE_SYSCALL_TABLE(fsetxattr, fsetxattr, 190, 5)
#endif
#ifndef OVERRIDE_TABLE_64_getxattr
TRACE_SYSCALL_TABLE(getxattr, getxattr, 191, 4)
#endif
#ifndef OVERRIDE_TABLE_64_lgetxattr
TRACE_SYSCALL_TABLE(lgetxattr, lgetxattr, 192, 4)
#endif
#ifndef OVERRIDE_TABLE_64_fgetxattr
TRACE_SYSCALL_TABLE(fgetxattr, fgetxattr, 193, 4)
#endif
#ifndef OVERRIDE_TABLE_64_listxattr
TRACE_SYSCALL_TABLE(listxattr, listxattr, 194, 3)
#endif
#ifndef OVERRIDE_TABLE_64_llistxattr
TRACE_SYSCALL_TABLE(llistxattr, llistxattr, 195, 3)
#endif
#ifndef OVERRIDE_TABLE_64_flistxattr
TRACE_SYSCALL_TABLE(flistxattr, flistxattr, 196, 3)
#endif
#ifndef OVERRIDE_TABLE_64_removexattr
TRACE_SYSCALL_TABLE(removexattr, removexattr, 197, 2)
#endif
#ifndef OVERRIDE_TABLE_64_lremovexattr
TRACE_SYSCALL_TABLE(lremovexattr, lremovexattr, 198, 2)
#endif
#ifndef OVERRIDE_TABLE_64_fremovexattr
TRACE_SYSCALL_TABLE(fremovexattr, fremovexattr, 199, 2)
#endif
#ifndef OVERRIDE_TABLE_64_time
TRACE_SYSCALL_TABLE(time, time, 201, 1)
#endif
#ifndef OVERRIDE_TABLE_64_futex
TRACE_SYSCALL_TABLE(futex, futex, 202, 6)
#endif
#ifndef OVERRIDE_TABLE_64_sched_setaffinity
TRACE_SYSCALL_TABLE(sched_setaffinity, sched_setaffinity, 203, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sched_getaffinity
TRACE_SYSCALL_TABLE(sched_getaffinity, sched_getaffinity, 204, 3)
#endif
#ifndef OVERRIDE_TABLE_64_io_setup
TRACE_SYSCALL_TABLE(io_setup, io_setup, 206, 2)
#endif
#ifndef OVERRIDE_TABLE_64_io_getevents
TRACE_SYSCALL_TABLE(io_getevents, io_getevents, 208, 5)
#endif
#ifndef OVERRIDE_TABLE_64_io_submit
TRACE_SYSCALL_TABLE(io_submit, io_submit, 209, 3)
#endif
#ifndef OVERRIDE_TABLE_64_io_cancel
TRACE_SYSCALL_TABLE(io_cancel, io_cancel, 210, 3)
#endif
#ifndef OVERRIDE_TABLE_64_lookup_dcookie
TRACE_SYSCALL_TABLE(lookup_dcookie, lookup_dcookie, 212, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getdents64
TRACE_SYSCALL_TABLE(getdents64, getdents64, 217, 3)
#endif
#ifndef OVERRIDE_TABLE_64_set_tid_address
TRACE_SYSCALL_TABLE(set_tid_address, set_tid_address, 218, 1)
#endif
#ifndef OVERRIDE_TABLE_64_semtimedop
TRACE_SYSCALL_TABLE(semtimedop, semtimedop, 220, 4)
#endif
#ifndef OVERRIDE_TABLE_64_timer_create
TRACE_SYSCALL_TABLE(timer_create, timer_create, 222, 3)
#endif
#ifndef OVERRIDE_TABLE_64_timer_settime
TRACE_SYSCALL_TABLE(timer_settime, timer_settime, 223, 4)
#endif
#ifndef OVERRIDE_TABLE_64_timer_gettime
TRACE_SYSCALL_TABLE(timer_gettime, timer_gettime, 224, 2)
#endif
#ifndef OVERRIDE_TABLE_64_clock_settime
TRACE_SYSCALL_TABLE(clock_settime, clock_settime, 227, 2)
#endif
#ifndef OVERRIDE_TABLE_64_clock_gettime
TRACE_SYSCALL_TABLE(clock_gettime, clock_gettime, 228, 2)
#endif
#ifndef OVERRIDE_TABLE_64_clock_getres
TRACE_SYSCALL_TABLE(clock_getres, clock_getres, 229, 2)
#endif
#ifndef OVERRIDE_TABLE_64_clock_nanosleep
TRACE_SYSCALL_TABLE(clock_nanosleep, clock_nanosleep, 230, 4)
#endif
#ifndef OVERRIDE_TABLE_64_epoll_wait
TRACE_SYSCALL_TABLE(epoll_wait, epoll_wait, 232, 4)
#endif
#ifndef OVERRIDE_TABLE_64_epoll_ctl
TRACE_SYSCALL_TABLE(epoll_ctl, epoll_ctl, 233, 4)
#endif
#ifndef OVERRIDE_TABLE_64_utimes
TRACE_SYSCALL_TABLE(utimes, utimes, 235, 2)
#endif
#ifndef OVERRIDE_TABLE_64_mbind
TRACE_SYSCALL_TABLE(mbind, mbind, 237, 6)
#endif
#ifndef OVERRIDE_TABLE_64_set_mempolicy
TRACE_SYSCALL_TABLE(set_mempolicy, set_mempolicy, 238, 3)
#endif
#ifndef OVERRIDE_TABLE_64_get_mempolicy
TRACE_SYSCALL_TABLE(get_mempolicy, get_mempolicy, 239, 5)
#endif
#ifndef OVERRIDE_TABLE_64_mq_open
TRACE_SYSCALL_TABLE(mq_open, mq_open, 240, 4)
#endif
#ifndef OVERRIDE_TABLE_64_mq_unlink
TRACE_SYSCALL_TABLE(mq_unlink, mq_unlink, 241, 1)
#endif
#ifndef OVERRIDE_TABLE_64_mq_timedsend
TRACE_SYSCALL_TABLE(mq_timedsend, mq_timedsend, 242, 5)
#endif
#ifndef OVERRIDE_TABLE_64_mq_timedreceive
TRACE_SYSCALL_TABLE(mq_timedreceive, mq_timedreceive, 243, 5)
#endif
#ifndef OVERRIDE_TABLE_64_mq_notify
TRACE_SYSCALL_TABLE(mq_notify, mq_notify, 244, 2)
#endif
#ifndef OVERRIDE_TABLE_64_mq_getsetattr
TRACE_SYSCALL_TABLE(mq_getsetattr, mq_getsetattr, 245, 3)
#endif
#ifndef OVERRIDE_TABLE_64_kexec_load
TRACE_SYSCALL_TABLE(kexec_load, kexec_load, 246, 4)
#endif
#ifndef OVERRIDE_TABLE_64_waitid
TRACE_SYSCALL_TABLE(waitid, waitid, 247, 5)
#endif
#ifndef OVERRIDE_TABLE_64_add_key
TRACE_SYSCALL_TABLE(add_key, add_key, 248, 5)
#endif
#ifndef OVERRIDE_TABLE_64_request_key
TRACE_SYSCALL_TABLE(request_key, request_key, 249, 4)
#endif
#ifndef OVERRIDE_TABLE_64_inotify_add_watch
TRACE_SYSCALL_TABLE(inotify_add_watch, inotify_add_watch, 254, 3)
#endif
#ifndef OVERRIDE_TABLE_64_migrate_pages
TRACE_SYSCALL_TABLE(migrate_pages, migrate_pages, 256, 4)
#endif
#ifndef OVERRIDE_TABLE_64_openat
TRACE_SYSCALL_TABLE(openat, openat, 257, 4)
#endif
#ifndef OVERRIDE_TABLE_64_mkdirat
TRACE_SYSCALL_TABLE(mkdirat, mkdirat, 258, 3)
#endif
#ifndef OVERRIDE_TABLE_64_mknodat
TRACE_SYSCALL_TABLE(mknodat, mknodat, 259, 4)
#endif
#ifndef OVERRIDE_TABLE_64_fchownat
TRACE_SYSCALL_TABLE(fchownat, fchownat, 260, 5)
#endif
#ifndef OVERRIDE_TABLE_64_futimesat
TRACE_SYSCALL_TABLE(futimesat, futimesat, 261, 3)
#endif
#ifndef OVERRIDE_TABLE_64_newfstatat
TRACE_SYSCALL_TABLE(newfstatat, newfstatat, 262, 4)
#endif
#ifndef OVERRIDE_TABLE_64_unlinkat
TRACE_SYSCALL_TABLE(unlinkat, unlinkat, 263, 3)
#endif
#ifndef OVERRIDE_TABLE_64_renameat
TRACE_SYSCALL_TABLE(renameat, renameat, 264, 4)
#endif
#ifndef OVERRIDE_TABLE_64_linkat
TRACE_SYSCALL_TABLE(linkat, linkat, 265, 5)
#endif
#ifndef OVERRIDE_TABLE_64_symlinkat
TRACE_SYSCALL_TABLE(symlinkat, symlinkat, 266, 3)
#endif
#ifndef OVERRIDE_TABLE_64_readlinkat
TRACE_SYSCALL_TABLE(readlinkat, readlinkat, 267, 4)
#endif
#ifndef OVERRIDE_TABLE_64_fchmodat
TRACE_SYSCALL_TABLE(fchmodat, fchmodat, 268, 3)
#endif
#ifndef OVERRIDE_TABLE_64_faccessat
TRACE_SYSCALL_TABLE(faccessat, faccessat, 269, 3)
#endif
#ifndef OVERRIDE_TABLE_64_pselect6
TRACE_SYSCALL_TABLE(pselect6, pselect6, 270, 6)
#endif
#ifndef OVERRIDE_TABLE_64_ppoll
TRACE_SYSCALL_TABLE(ppoll, ppoll, 271, 5)
#endif
#ifndef OVERRIDE_TABLE_64_set_robust_list
TRACE_SYSCALL_TABLE(set_robust_list, set_robust_list, 273, 2)
#endif
#ifndef OVERRIDE_TABLE_64_get_robust_list
TRACE_SYSCALL_TABLE(get_robust_list, get_robust_list, 274, 3)
#endif
#ifndef OVERRIDE_TABLE_64_splice
TRACE_SYSCALL_TABLE(splice, splice, 275, 6)
#endif
#ifndef OVERRIDE_TABLE_64_vmsplice
TRACE_SYSCALL_TABLE(vmsplice, vmsplice, 278, 4)
#endif
#ifndef OVERRIDE_TABLE_64_move_pages
TRACE_SYSCALL_TABLE(move_pages, move_pages, 279, 6)
#endif
#ifndef OVERRIDE_TABLE_64_utimensat
TRACE_SYSCALL_TABLE(utimensat, utimensat, 280, 4)
#endif
#ifndef OVERRIDE_TABLE_64_epoll_pwait
TRACE_SYSCALL_TABLE(epoll_pwait, epoll_pwait, 281, 6)
#endif
#ifndef OVERRIDE_TABLE_64_signalfd
TRACE_SYSCALL_TABLE(signalfd, signalfd, 282, 3)
#endif
#ifndef OVERRIDE_TABLE_64_timerfd_settime
TRACE_SYSCALL_TABLE(timerfd_settime, timerfd_settime, 286, 4)
#endif
#ifndef OVERRIDE_TABLE_64_timerfd_gettime
TRACE_SYSCALL_TABLE(timerfd_gettime, timerfd_gettime, 287, 2)
#endif
#ifndef OVERRIDE_TABLE_64_accept4
TRACE_SYSCALL_TABLE(accept4, accept4, 288, 4)
#endif
#ifndef OVERRIDE_TABLE_64_signalfd4
TRACE_SYSCALL_TABLE(signalfd4, signalfd4, 289, 4)
#endif
#ifndef OVERRIDE_TABLE_64_pipe2
TRACE_SYSCALL_TABLE(pipe2, pipe2, 293, 2)
#endif
#ifndef OVERRIDE_TABLE_64_preadv
TRACE_SYSCALL_TABLE(preadv, preadv, 295, 5)
#endif
#ifndef OVERRIDE_TABLE_64_pwritev
TRACE_SYSCALL_TABLE(pwritev, pwritev, 296, 5)
#endif
#ifndef OVERRIDE_TABLE_64_rt_tgsigqueueinfo
TRACE_SYSCALL_TABLE(rt_tgsigqueueinfo, rt_tgsigqueueinfo, 297, 4)
#endif
#ifndef OVERRIDE_TABLE_64_perf_event_open
TRACE_SYSCALL_TABLE(perf_event_open, perf_event_open, 298, 5)
#endif
#ifndef OVERRIDE_TABLE_64_recvmmsg
TRACE_SYSCALL_TABLE(recvmmsg, recvmmsg, 299, 5)
#endif
#ifndef OVERRIDE_TABLE_64_fanotify_mark
TRACE_SYSCALL_TABLE(fanotify_mark, fanotify_mark, 301, 5)
#endif
#ifndef OVERRIDE_TABLE_64_prlimit64
TRACE_SYSCALL_TABLE(prlimit64, prlimit64, 302, 4)
#endif
#ifndef OVERRIDE_TABLE_64_name_to_handle_at
TRACE_SYSCALL_TABLE(name_to_handle_at, name_to_handle_at, 303, 5)
#endif
#ifndef OVERRIDE_TABLE_64_open_by_handle_at
TRACE_SYSCALL_TABLE(open_by_handle_at, open_by_handle_at, 304, 3)
#endif
#ifndef OVERRIDE_TABLE_64_clock_adjtime
TRACE_SYSCALL_TABLE(clock_adjtime, clock_adjtime, 305, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sendmmsg
TRACE_SYSCALL_TABLE(sendmmsg, sendmmsg, 307, 4)
#endif
#ifndef OVERRIDE_TABLE_64_getcpu
TRACE_SYSCALL_TABLE(getcpu, getcpu, 309, 3)
#endif
#ifndef OVERRIDE_TABLE_64_process_vm_readv
TRACE_SYSCALL_TABLE(process_vm_readv, process_vm_readv, 310, 6)
#endif
#ifndef OVERRIDE_TABLE_64_process_vm_writev
TRACE_SYSCALL_TABLE(process_vm_writev, process_vm_writev, 311, 6)
#endif
#ifndef OVERRIDE_TABLE_64_finit_module
TRACE_SYSCALL_TABLE(finit_module, finit_module, 313, 3)
#endif

#endif /* CREATE_SYSCALL_TABLE */
