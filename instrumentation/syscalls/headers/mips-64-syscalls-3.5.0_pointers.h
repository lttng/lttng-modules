/* THIS FILE IS AUTO-GENERATED. DO NOT EDIT */
#ifndef CREATE_SYSCALL_TABLE

#if !defined(_TRACE_SYSCALLS_POINTERS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SYSCALLS_POINTERS_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/syscalls.h>
#include "mips-64-syscalls-3.5.0_pointers_override.h"
#include "syscalls_pointers_override.h"

#ifndef OVERRIDE_64_oldumount
SC_LTTNG_TRACEPOINT_EVENT(oldumount,
	TP_PROTO(sc_exit(long ret,) char * name),
	TP_ARGS(sc_exit(ret,) name),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__string_from_user(name, name))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_copy_string_from_user(name, name))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_olduname
SC_LTTNG_TRACEPOINT_EVENT(olduname,
	TP_PROTO(sc_exit(long ret,) struct oldold_utsname * name),
	TP_ARGS(sc_exit(ret,) name),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field_hex(struct oldold_utsname *, name))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(name, name))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_uselib
SC_LTTNG_TRACEPOINT_EVENT(uselib,
	TP_PROTO(sc_exit(long ret,) const char * library),
	TP_ARGS(sc_exit(ret,) library),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field_hex(const char *, library))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(library, library))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_uname
SC_LTTNG_TRACEPOINT_EVENT(uname,
	TP_PROTO(sc_exit(long ret,) struct old_utsname * name),
	TP_ARGS(sc_exit(ret,) name),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field_hex(struct old_utsname *, name))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(name, name))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sysinfo
SC_LTTNG_TRACEPOINT_EVENT(sysinfo,
	TP_PROTO(sc_exit(long ret,) struct sysinfo * info),
	TP_ARGS(sc_exit(ret,) info),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_out(__field_hex(struct sysinfo *, info))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_out(tp_assign(info, info))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_times
SC_LTTNG_TRACEPOINT_EVENT(times,
	TP_PROTO(sc_exit(long ret,) struct tms * tbuf),
	TP_ARGS(sc_exit(ret,) tbuf),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_out(__field_hex(struct tms *, tbuf))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_out(tp_assign(tbuf, tbuf))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sysctl
SC_LTTNG_TRACEPOINT_EVENT(sysctl,
	TP_PROTO(sc_exit(long ret,) struct __sysctl_args * args),
	TP_ARGS(sc_exit(ret,) args),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field_hex(struct __sysctl_args *, args))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(args, args))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_adjtimex
SC_LTTNG_TRACEPOINT_EVENT(adjtimex,
	TP_PROTO(sc_exit(long ret,) struct timex * txc_p),
	TP_ARGS(sc_exit(ret,) txc_p),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field_hex(struct timex *, txc_p))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(txc_p, txc_p))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_newuname
SC_LTTNG_TRACEPOINT_EVENT(newuname,
	TP_PROTO(sc_exit(long ret,) struct new_utsname * name),
	TP_ARGS(sc_exit(ret,) name),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_out(__field_hex(struct new_utsname *, name))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_out(tp_assign(name, name))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_shmdt
SC_LTTNG_TRACEPOINT_EVENT(shmdt,
	TP_PROTO(sc_exit(long ret,) char * shmaddr),
	TP_ARGS(sc_exit(ret,) shmaddr),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field_hex(char *, shmaddr))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(shmaddr, shmaddr))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_chdir
SC_LTTNG_TRACEPOINT_EVENT(chdir,
	TP_PROTO(sc_exit(long ret,) const char * filename),
	TP_ARGS(sc_exit(ret,) filename),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(filename, filename))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(filename, filename))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_rmdir
SC_LTTNG_TRACEPOINT_EVENT(rmdir,
	TP_PROTO(sc_exit(long ret,) const char * pathname),
	TP_ARGS(sc_exit(ret,) pathname),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_unlink
SC_LTTNG_TRACEPOINT_EVENT(unlink,
	TP_PROTO(sc_exit(long ret,) const char * pathname),
	TP_ARGS(sc_exit(ret,) pathname),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_chroot
SC_LTTNG_TRACEPOINT_EVENT(chroot,
	TP_PROTO(sc_exit(long ret,) const char * filename),
	TP_ARGS(sc_exit(ret,) filename),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(filename, filename))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(filename, filename))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_swapoff
SC_LTTNG_TRACEPOINT_EVENT(swapoff,
	TP_PROTO(sc_exit(long ret,) const char * specialfile),
	TP_ARGS(sc_exit(ret,) specialfile),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(specialfile, specialfile))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(specialfile, specialfile))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_set_tid_address
SC_LTTNG_TRACEPOINT_EVENT(set_tid_address,
	TP_PROTO(sc_exit(long ret,) int * tidptr),
	TP_ARGS(sc_exit(ret,) tidptr),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field_hex(int *, tidptr))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(tidptr, tidptr))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_nanosleep
SC_LTTNG_TRACEPOINT_EVENT(nanosleep,
	TP_PROTO(sc_exit(long ret,) struct timespec * rqtp, struct timespec * rmtp),
	TP_ARGS(sc_exit(ret,) rqtp, rmtp),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field_hex(struct timespec *, rqtp)) sc_out(__field_hex(struct timespec *, rmtp))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(rqtp, rqtp)) sc_out(tp_assign(rmtp, rmtp))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getitimer
SC_LTTNG_TRACEPOINT_EVENT(getitimer,
	TP_PROTO(sc_exit(long ret,) int which, struct itimerval * value),
	TP_ARGS(sc_exit(ret,) which, value),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, which)) sc_out(__field_hex(struct itimerval *, value))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which, which)) sc_out(tp_assign(value, value))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_gettimeofday
SC_LTTNG_TRACEPOINT_EVENT(gettimeofday,
	TP_PROTO(sc_exit(long ret,) struct timeval * tv, struct timezone * tz),
	TP_ARGS(sc_exit(ret,) tv, tz),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_out(__field_hex(struct timeval *, tv)) sc_out(__field_hex(struct timezone *, tz))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_out(tp_assign(tv, tv)) sc_out(tp_assign(tz, tz))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getrlimit
SC_LTTNG_TRACEPOINT_EVENT(getrlimit,
	TP_PROTO(sc_exit(long ret,) unsigned int resource, struct rlimit * rlim),
	TP_ARGS(sc_exit(ret,) resource, rlim),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, resource)) sc_out(__field_hex(struct rlimit *, rlim))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(resource, resource)) sc_out(tp_assign(rlim, rlim))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getrusage
SC_LTTNG_TRACEPOINT_EVENT(getrusage,
	TP_PROTO(sc_exit(long ret,) int who, struct rusage * ru),
	TP_ARGS(sc_exit(ret,) who, ru),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, who)) sc_out(__field_hex(struct rusage *, ru))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(who, who)) sc_out(tp_assign(ru, ru))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_rt_sigpending
SC_LTTNG_TRACEPOINT_EVENT(rt_sigpending,
	TP_PROTO(sc_exit(long ret,) sigset_t * set, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) set, sigsetsize),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_out(__field_hex(sigset_t *, set)) sc_in(__field(size_t, sigsetsize))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_out(tp_assign(set, set)) sc_in(tp_assign(sigsetsize, sigsetsize))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_utime
SC_LTTNG_TRACEPOINT_EVENT(utime,
	TP_PROTO(sc_exit(long ret,) char * filename, struct utimbuf * times),
	TP_ARGS(sc_exit(ret,) filename, times),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(filename, filename)) sc_in(__field_hex(struct utimbuf *, times))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(times, times))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_ustat
SC_LTTNG_TRACEPOINT_EVENT(ustat,
	TP_PROTO(sc_exit(long ret,) unsigned dev, struct ustat * ubuf),
	TP_ARGS(sc_exit(ret,) dev, ubuf),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned, dev)) sc_out(__field_hex(struct ustat *, ubuf))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(dev, dev)) sc_out(tp_assign(ubuf, ubuf))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_statfs
SC_LTTNG_TRACEPOINT_EVENT(statfs,
	TP_PROTO(sc_exit(long ret,) const char * pathname, struct statfs * buf),
	TP_ARGS(sc_exit(ret,) pathname, buf),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname)) sc_out(__field_hex(struct statfs *, buf))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_out(tp_assign(buf, buf))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fstatfs
SC_LTTNG_TRACEPOINT_EVENT(fstatfs,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct statfs * buf),
	TP_ARGS(sc_exit(ret,) fd, buf),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_out(__field_hex(struct statfs *, buf))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(buf, buf))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sched_rr_get_interval
SC_LTTNG_TRACEPOINT_EVENT(sched_rr_get_interval,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct timespec * interval),
	TP_ARGS(sc_exit(ret,) pid, interval),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_out(__field_hex(struct timespec *, interval))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_out(tp_assign(interval, interval))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setrlimit
SC_LTTNG_TRACEPOINT_EVENT(setrlimit,
	TP_PROTO(sc_exit(long ret,) unsigned int resource, struct rlimit * rlim),
	TP_ARGS(sc_exit(ret,) resource, rlim),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, resource)) sc_in(__field_hex(struct rlimit *, rlim))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(resource, resource)) sc_in(tp_assign(rlim, rlim))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_settimeofday
SC_LTTNG_TRACEPOINT_EVENT(settimeofday,
	TP_PROTO(sc_exit(long ret,) struct timeval * tv, struct timezone * tz),
	TP_ARGS(sc_exit(ret,) tv, tz),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field_hex(struct timeval *, tv)) sc_in(__field_hex(struct timezone *, tz))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(tv, tv)) sc_in(tp_assign(tz, tz))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_io_setup
SC_LTTNG_TRACEPOINT_EVENT(io_setup,
	TP_PROTO(sc_exit(long ret,) unsigned nr_events, aio_context_t * ctxp),
	TP_ARGS(sc_exit(ret,) nr_events, ctxp),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned, nr_events)) sc_in(__field_hex(aio_context_t *, ctxp))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(nr_events, nr_events)) sc_in(tp_assign(ctxp, ctxp))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_timer_gettime
SC_LTTNG_TRACEPOINT_EVENT(timer_gettime,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id, struct itimerspec * setting),
	TP_ARGS(sc_exit(ret,) timer_id, setting),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(timer_t, timer_id)) sc_out(__field_hex(struct itimerspec *, setting))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(timer_id, timer_id)) sc_out(tp_assign(setting, setting))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_clock_settime
SC_LTTNG_TRACEPOINT_EVENT(clock_settime,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, const struct timespec * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(const clockid_t, which_clock)) sc_in(__field_hex(const struct timespec *, tp))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which_clock, which_clock)) sc_in(tp_assign(tp, tp))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_clock_gettime
SC_LTTNG_TRACEPOINT_EVENT(clock_gettime,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct timespec * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(const clockid_t, which_clock)) sc_out(__field_hex(struct timespec *, tp))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which_clock, which_clock)) sc_out(tp_assign(tp, tp))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_clock_getres
SC_LTTNG_TRACEPOINT_EVENT(clock_getres,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct timespec * tp),
	TP_ARGS(sc_exit(ret,) which_clock, tp),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(const clockid_t, which_clock)) sc_out(__field_hex(struct timespec *, tp))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which_clock, which_clock)) sc_out(tp_assign(tp, tp))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_utimes
SC_LTTNG_TRACEPOINT_EVENT(utimes,
	TP_PROTO(sc_exit(long ret,) char * filename, struct timeval * utimes),
	TP_ARGS(sc_exit(ret,) filename, utimes),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(filename, filename)) sc_in(__field_hex(struct timeval *, utimes))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(utimes, utimes))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_set_robust_list
SC_LTTNG_TRACEPOINT_EVENT(set_robust_list,
	TP_PROTO(sc_exit(long ret,) struct robust_list_head * head, size_t len),
	TP_ARGS(sc_exit(ret,) head, len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field_hex(struct robust_list_head *, head)) sc_in(__field(size_t, len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(head, head)) sc_in(tp_assign(len, len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_timerfd_gettime
SC_LTTNG_TRACEPOINT_EVENT(timerfd_gettime,
	TP_PROTO(sc_exit(long ret,) int ufd, struct itimerspec * otmr),
	TP_ARGS(sc_exit(ret,) ufd, otmr),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, ufd)) sc_out(__field_hex(struct itimerspec *, otmr))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ufd, ufd)) sc_out(tp_assign(otmr, otmr))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_clock_adjtime
SC_LTTNG_TRACEPOINT_EVENT(clock_adjtime,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct timex * utx),
	TP_ARGS(sc_exit(ret,) which_clock, utx),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(const clockid_t, which_clock)) sc_inout(__field_hex(struct timex *, utx))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which_clock, which_clock)) sc_inout(tp_assign(utx, utx))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_newstat
SC_LTTNG_TRACEPOINT_EVENT(newstat,
	TP_PROTO(sc_exit(long ret,) const char * filename, struct stat * statbuf),
	TP_ARGS(sc_exit(ret,) filename, statbuf),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(filename, filename)) sc_out(__field_hex(struct stat *, statbuf))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(filename, filename)) sc_out(tp_assign(statbuf, statbuf))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_newfstat
SC_LTTNG_TRACEPOINT_EVENT(newfstat,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct stat * statbuf),
	TP_ARGS(sc_exit(ret,) fd, statbuf),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_out(__field_hex(struct stat *, statbuf))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(statbuf, statbuf))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_newlstat
SC_LTTNG_TRACEPOINT_EVENT(newlstat,
	TP_PROTO(sc_exit(long ret,) const char * filename, struct stat * statbuf),
	TP_ARGS(sc_exit(ret,) filename, statbuf),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(filename, filename)) sc_out(__field_hex(struct stat *, statbuf))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(filename, filename)) sc_out(tp_assign(statbuf, statbuf))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_access
SC_LTTNG_TRACEPOINT_EVENT(access,
	TP_PROTO(sc_exit(long ret,) const char * filename, int mode),
	TP_ARGS(sc_exit(ret,) filename, mode),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(filename, filename)) sc_in(__field(int, mode))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(mode, mode))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_truncate
SC_LTTNG_TRACEPOINT_EVENT(truncate,
	TP_PROTO(sc_exit(long ret,) const char * path, long length),
	TP_ARGS(sc_exit(ret,) path, length),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(path, path)) sc_in(__field(long, length))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(path, path)) sc_in(tp_assign(length, length))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getcwd
SC_LTTNG_TRACEPOINT_EVENT(getcwd,
	TP_PROTO(sc_exit(long ret,) char * buf, unsigned long size),
	TP_ARGS(sc_exit(ret,) buf, size),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_out(__field_hex(char *, buf)) sc_in(__field(unsigned long, size))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_out(tp_assign(buf, buf)) sc_in(tp_assign(size, size))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_rename
SC_LTTNG_TRACEPOINT_EVENT(rename,
	TP_PROTO(sc_exit(long ret,) const char * oldname, const char * newname),
	TP_ARGS(sc_exit(ret,) oldname, newname),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(oldname, oldname)) sc_in(__string_from_user(newname, newname))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(oldname, oldname)) sc_in(tp_copy_string_from_user(newname, newname))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_mkdir
SC_LTTNG_TRACEPOINT_EVENT(mkdir,
	TP_PROTO(sc_exit(long ret,) const char * pathname, umode_t mode),
	TP_ARGS(sc_exit(ret,) pathname, mode),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname)) sc_in(__field(umode_t, mode))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_in(tp_assign(mode, mode))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_creat
SC_LTTNG_TRACEPOINT_EVENT(creat,
	TP_PROTO(sc_exit(long ret,) const char * pathname, umode_t mode),
	TP_ARGS(sc_exit(ret,) pathname, mode),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname)) sc_in(__field(umode_t, mode))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_in(tp_assign(mode, mode))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_link
SC_LTTNG_TRACEPOINT_EVENT(link,
	TP_PROTO(sc_exit(long ret,) const char * oldname, const char * newname),
	TP_ARGS(sc_exit(ret,) oldname, newname),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(oldname, oldname)) sc_in(__string_from_user(newname, newname))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(oldname, oldname)) sc_in(tp_copy_string_from_user(newname, newname))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_symlink
SC_LTTNG_TRACEPOINT_EVENT(symlink,
	TP_PROTO(sc_exit(long ret,) const char * oldname, const char * newname),
	TP_ARGS(sc_exit(ret,) oldname, newname),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(oldname, oldname)) sc_in(__string_from_user(newname, newname))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(oldname, oldname)) sc_in(tp_copy_string_from_user(newname, newname))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_chmod
SC_LTTNG_TRACEPOINT_EVENT(chmod,
	TP_PROTO(sc_exit(long ret,) const char * filename, umode_t mode),
	TP_ARGS(sc_exit(ret,) filename, mode),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(filename, filename)) sc_in(__field(umode_t, mode))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(mode, mode))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getgroups
SC_LTTNG_TRACEPOINT_EVENT(getgroups,
	TP_PROTO(sc_exit(long ret,) int gidsetsize, gid_t * grouplist),
	TP_ARGS(sc_exit(ret,) gidsetsize, grouplist),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, gidsetsize)) sc_out(__field_hex(gid_t *, grouplist))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(gidsetsize, gidsetsize)) sc_out(tp_assign(grouplist, grouplist))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setgroups
SC_LTTNG_TRACEPOINT_EVENT(setgroups,
	TP_PROTO(sc_exit(long ret,) int gidsetsize, gid_t * grouplist),
	TP_ARGS(sc_exit(ret,) gidsetsize, grouplist),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, gidsetsize)) sc_in(__field_hex(gid_t *, grouplist))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(gidsetsize, gidsetsize)) sc_in(tp_assign(grouplist, grouplist))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_rt_sigpending
SC_LTTNG_TRACEPOINT_EVENT(32_rt_sigpending,
	TP_PROTO(sc_exit(long ret,) compat_sigset_t * uset, unsigned int sigsetsize),
	TP_ARGS(sc_exit(ret,) uset, sigsetsize),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field_hex(compat_sigset_t *, uset)) sc_inout(__field(unsigned int, sigsetsize))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(uset, uset)) sc_inout(tp_assign(sigsetsize, sigsetsize))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sched_setparam
SC_LTTNG_TRACEPOINT_EVENT(sched_setparam,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct sched_param * param),
	TP_ARGS(sc_exit(ret,) pid, param),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field_hex(struct sched_param *, param))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(param, param))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sched_getparam
SC_LTTNG_TRACEPOINT_EVENT(sched_getparam,
	TP_PROTO(sc_exit(long ret,) pid_t pid, struct sched_param * param),
	TP_ARGS(sc_exit(ret,) pid, param),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_out(__field_hex(struct sched_param *, param))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_out(tp_assign(param, param))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_sched_rr_get_interval
SC_LTTNG_TRACEPOINT_EVENT(32_sched_rr_get_interval,
	TP_PROTO(sc_exit(long ret,) compat_pid_t pid, struct compat_timespec * interval),
	TP_ARGS(sc_exit(ret,) pid, interval),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(compat_pid_t, pid)) sc_inout(__field_hex(struct compat_timespec *, interval))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(pid, pid)) sc_inout(tp_assign(interval, interval))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_pivot_root
SC_LTTNG_TRACEPOINT_EVENT(pivot_root,
	TP_PROTO(sc_exit(long ret,) const char * new_root, const char * put_old),
	TP_ARGS(sc_exit(ret,) new_root, put_old),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(new_root, new_root)) sc_in(__string_from_user(put_old, put_old))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(new_root, new_root)) sc_in(tp_copy_string_from_user(put_old, put_old))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_umount
SC_LTTNG_TRACEPOINT_EVENT(umount,
	TP_PROTO(sc_exit(long ret,) char * name, int flags),
	TP_ARGS(sc_exit(ret,) name, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(name, name)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(name, name)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_swapon
SC_LTTNG_TRACEPOINT_EVENT(swapon,
	TP_PROTO(sc_exit(long ret,) const char * specialfile, int swap_flags),
	TP_ARGS(sc_exit(ret,) specialfile, swap_flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(specialfile, specialfile)) sc_in(__field(int, swap_flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(specialfile, specialfile)) sc_in(tp_assign(swap_flags, swap_flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sethostname
SC_LTTNG_TRACEPOINT_EVENT(sethostname,
	TP_PROTO(sc_exit(long ret,) char * name, int len),
	TP_ARGS(sc_exit(ret,) name, len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(name, name)) sc_in(__field(int, len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(name, name)) sc_in(tp_assign(len, len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setdomainname
SC_LTTNG_TRACEPOINT_EVENT(setdomainname,
	TP_PROTO(sc_exit(long ret,) char * name, int len),
	TP_ARGS(sc_exit(ret,) name, len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(name, name)) sc_in(__field(int, len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(name, name)) sc_in(tp_assign(len, len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_delete_module
SC_LTTNG_TRACEPOINT_EVENT(delete_module,
	TP_PROTO(sc_exit(long ret,) const char * name_user, unsigned int flags),
	TP_ARGS(sc_exit(ret,) name_user, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(name_user, name_user)) sc_in(__field(unsigned int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(name_user, name_user)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_removexattr
SC_LTTNG_TRACEPOINT_EVENT(removexattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name),
	TP_ARGS(sc_exit(ret,) pathname, name),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname)) sc_in(__string_from_user(name, name))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_in(tp_copy_string_from_user(name, name))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_lremovexattr
SC_LTTNG_TRACEPOINT_EVENT(lremovexattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name),
	TP_ARGS(sc_exit(ret,) pathname, name),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname)) sc_in(__string_from_user(name, name))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_in(tp_copy_string_from_user(name, name))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fremovexattr
SC_LTTNG_TRACEPOINT_EVENT(fremovexattr,
	TP_PROTO(sc_exit(long ret,) int fd, const char * name),
	TP_ARGS(sc_exit(ret,) fd, name),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__string_from_user(name, name))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_copy_string_from_user(name, name))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_pipe2
SC_LTTNG_TRACEPOINT_EVENT(pipe2,
	TP_PROTO(sc_exit(long ret,) int * fildes, int flags),
	TP_ARGS(sc_exit(ret,) fildes, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_out(__field_hex(int *, fildes)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_out(tp_assign(fildes, fildes)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_waitpid
SC_LTTNG_TRACEPOINT_EVENT(waitpid,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int * stat_addr, int options),
	TP_ARGS(sc_exit(ret,) pid, stat_addr, options),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(pid_t, pid)) sc_inout(__field_hex(int *, stat_addr)) sc_inout(__field(int, options))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(pid, pid)) sc_inout(tp_assign(stat_addr, stat_addr)) sc_inout(tp_assign(options, options))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_sigaction
SC_LTTNG_TRACEPOINT_EVENT(32_sigaction,
	TP_PROTO(sc_exit(long ret,) long sig, const struct sigaction32 * act, struct sigaction32 * oact),
	TP_ARGS(sc_exit(ret,) sig, act, oact),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(long, sig)) sc_inout(__field_hex(const struct sigaction32 *, act)) sc_inout(__field_hex(struct sigaction32 *, oact))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(sig, sig)) sc_inout(tp_assign(act, act)) sc_inout(tp_assign(oact, oact))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_readv
SC_LTTNG_TRACEPOINT_EVENT(readv,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, fd)) sc_inout(__field_hex(const struct iovec *, vec)) sc_in(__field(unsigned long, vlen))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_inout(tp_assign(vec, vec)) sc_in(tp_assign(vlen, vlen))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_writev
SC_LTTNG_TRACEPOINT_EVENT(writev,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, fd)) sc_inout(__field_hex(const struct iovec *, vec)) sc_in(__field(unsigned long, vlen))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_inout(tp_assign(vec, vec)) sc_in(tp_assign(vlen, vlen))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_shmctl
SC_LTTNG_TRACEPOINT_EVENT(shmctl,
	TP_PROTO(sc_exit(long ret,) int shmid, int cmd, struct shmid_ds * buf),
	TP_ARGS(sc_exit(ret,) shmid, cmd, buf),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, shmid)) sc_in(__field(int, cmd)) sc_inout(__field_hex(struct shmid_ds *, buf))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(shmid, shmid)) sc_in(tp_assign(cmd, cmd)) sc_inout(tp_assign(buf, buf))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setitimer
SC_LTTNG_TRACEPOINT_EVENT(setitimer,
	TP_PROTO(sc_exit(long ret,) int which, struct itimerval * value, struct itimerval * ovalue),
	TP_ARGS(sc_exit(ret,) which, value, ovalue),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, which)) sc_in(__field_hex(struct itimerval *, value)) sc_out(__field_hex(struct itimerval *, ovalue))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which, which)) sc_in(tp_assign(value, value)) sc_out(tp_assign(ovalue, ovalue))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sendmsg
SC_LTTNG_TRACEPOINT_EVENT(sendmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct msghdr * msg, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, msg, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field_hex(struct msghdr *, msg)) sc_in(__field(unsigned int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(msg, msg)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_recvmsg
SC_LTTNG_TRACEPOINT_EVENT(recvmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct msghdr * msg, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, msg, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_inout(__field_hex(struct msghdr *, msg)) sc_in(__field(unsigned int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_inout(tp_assign(msg, msg)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_msgctl
SC_LTTNG_TRACEPOINT_EVENT(msgctl,
	TP_PROTO(sc_exit(long ret,) int msqid, int cmd, struct msqid_ds * buf),
	TP_ARGS(sc_exit(ret,) msqid, cmd, buf),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, msqid)) sc_in(__field(int, cmd)) sc_inout(__field_hex(struct msqid_ds *, buf))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(msqid, msqid)) sc_in(tp_assign(cmd, cmd)) sc_inout(tp_assign(buf, buf))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getdents
SC_LTTNG_TRACEPOINT_EVENT(getdents,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct linux_dirent * dirent, unsigned int count),
	TP_ARGS(sc_exit(ret,) fd, dirent, count),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_out(__field_hex(struct linux_dirent *, dirent)) sc_in(__field(unsigned int, count))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(dirent, dirent)) sc_in(tp_assign(count, count))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_rt_sigqueueinfo
SC_LTTNG_TRACEPOINT_EVENT(rt_sigqueueinfo,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int sig, siginfo_t * uinfo),
	TP_ARGS(sc_exit(ret,) pid, sig, uinfo),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field(int, sig)) sc_in(__field_hex(siginfo_t *, uinfo))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(sig, sig)) sc_in(tp_assign(uinfo, uinfo))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sched_setaffinity
SC_LTTNG_TRACEPOINT_EVENT(sched_setaffinity,
	TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned int len, unsigned long * user_mask_ptr),
	TP_ARGS(sc_exit(ret,) pid, len, user_mask_ptr),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field(unsigned int, len)) sc_in(__field_hex(unsigned long *, user_mask_ptr))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(len, len)) sc_in(tp_assign(user_mask_ptr, user_mask_ptr))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sched_getaffinity
SC_LTTNG_TRACEPOINT_EVENT(sched_getaffinity,
	TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned int len, unsigned long * user_mask_ptr),
	TP_ARGS(sc_exit(ret,) pid, len, user_mask_ptr),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field(unsigned int, len)) sc_out(__field_hex(unsigned long *, user_mask_ptr))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(len, len)) sc_out(tp_assign(user_mask_ptr, user_mask_ptr))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_io_submit
SC_LTTNG_TRACEPOINT_EVENT(io_submit,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx_id, long nr, struct iocb * * iocbpp),
	TP_ARGS(sc_exit(ret,) ctx_id, nr, iocbpp),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(aio_context_t, ctx_id)) sc_in(__field(long, nr)) sc_in(__field_hex(struct iocb * *, iocbpp))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ctx_id, ctx_id)) sc_in(tp_assign(nr, nr)) sc_in(tp_assign(iocbpp, iocbpp))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_timer_create
SC_LTTNG_TRACEPOINT_EVENT(timer_create,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, struct sigevent * timer_event_spec, timer_t * created_timer_id),
	TP_ARGS(sc_exit(ret,) which_clock, timer_event_spec, created_timer_id),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(const clockid_t, which_clock)) sc_in(__field_hex(struct sigevent *, timer_event_spec)) sc_out(__field_hex(timer_t *, created_timer_id))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which_clock, which_clock)) sc_in(tp_assign(timer_event_spec, timer_event_spec)) sc_out(tp_assign(created_timer_id, created_timer_id))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_futimesat
SC_LTTNG_TRACEPOINT_EVENT(futimesat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, struct timeval * utimes),
	TP_ARGS(sc_exit(ret,) dfd, filename, utimes),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, dfd)) sc_in(__string_from_user(filename, filename)) sc_in(__field_hex(struct timeval *, utimes))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(dfd, dfd)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(utimes, utimes))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_get_robust_list
SC_LTTNG_TRACEPOINT_EVENT(get_robust_list,
	TP_PROTO(sc_exit(long ret,) int pid, struct robust_list_head * * head_ptr, size_t * len_ptr),
	TP_ARGS(sc_exit(ret,) pid, head_ptr, len_ptr),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, pid)) sc_out(__field_hex(struct robust_list_head * *, head_ptr)) sc_out(__field_hex(size_t *, len_ptr))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_out(tp_assign(head_ptr, head_ptr)) sc_out(tp_assign(len_ptr, len_ptr))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_signalfd
SC_LTTNG_TRACEPOINT_EVENT(signalfd,
	TP_PROTO(sc_exit(long ret,) int ufd, sigset_t * user_mask, size_t sizemask),
	TP_ARGS(sc_exit(ret,) ufd, user_mask, sizemask),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, ufd)) sc_in(__field_hex(sigset_t *, user_mask)) sc_in(__field(size_t, sizemask))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ufd, ufd)) sc_in(tp_assign(user_mask, user_mask)) sc_in(tp_assign(sizemask, sizemask))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_read
SC_LTTNG_TRACEPOINT_EVENT(read,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, char * buf, size_t count),
	TP_ARGS(sc_exit(ret,) fd, buf, count),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_out(__field_hex(char *, buf)) sc_in(__field(size_t, count))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(buf, buf)) sc_in(tp_assign(count, count))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_write
SC_LTTNG_TRACEPOINT_EVENT(write,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, const char * buf, size_t count),
	TP_ARGS(sc_exit(ret,) fd, buf, count),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_in(__field_hex(const char *, buf)) sc_in(__field(size_t, count))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(buf, buf)) sc_in(tp_assign(count, count))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_open
SC_LTTNG_TRACEPOINT_EVENT(open,
	TP_PROTO(sc_exit(long ret,) const char * filename, int flags, umode_t mode),
	TP_ARGS(sc_exit(ret,) filename, flags, mode),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(filename, filename)) sc_in(__field(int, flags)) sc_in(__field(umode_t, mode))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(mode, mode))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_poll
SC_LTTNG_TRACEPOINT_EVENT(poll,
	TP_PROTO(sc_exit(long ret,) struct pollfd * ufds, unsigned int nfds, int timeout_msecs),
	TP_ARGS(sc_exit(ret,) ufds, nfds, timeout_msecs),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field_hex(struct pollfd *, ufds)) sc_in(__field(unsigned int, nfds)) sc_in(__field(int, timeout_msecs))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(ufds, ufds)) sc_in(tp_assign(nfds, nfds)) sc_in(tp_assign(timeout_msecs, timeout_msecs))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_mincore
SC_LTTNG_TRACEPOINT_EVENT(mincore,
	TP_PROTO(sc_exit(long ret,) unsigned long start, size_t len, unsigned char * vec),
	TP_ARGS(sc_exit(ret,) start, len, vec),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, start)) sc_in(__field(size_t, len)) sc_out(__field_hex(unsigned char *, vec))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(start, start)) sc_in(tp_assign(len, len)) sc_out(tp_assign(vec, vec))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_shmat
SC_LTTNG_TRACEPOINT_EVENT(shmat,
	TP_PROTO(sc_exit(long ret,) int shmid, char * shmaddr, int shmflg),
	TP_ARGS(sc_exit(ret,) shmid, shmaddr, shmflg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, shmid)) sc_in(__field_hex(char *, shmaddr)) sc_in(__field(int, shmflg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(shmid, shmid)) sc_in(tp_assign(shmaddr, shmaddr)) sc_in(tp_assign(shmflg, shmflg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_connect
SC_LTTNG_TRACEPOINT_EVENT(connect,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * uservaddr, int addrlen),
	TP_ARGS(sc_exit(ret,) fd, uservaddr, addrlen),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field_hex(struct sockaddr *, uservaddr)) sc_in(__field_hex(int, addrlen))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(uservaddr, uservaddr)) sc_in(tp_assign(addrlen, addrlen))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_accept
SC_LTTNG_TRACEPOINT_EVENT(accept,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * upeer_sockaddr, int * upeer_addrlen),
	TP_ARGS(sc_exit(ret,) fd, upeer_sockaddr, upeer_addrlen),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_out(__field_hex(struct sockaddr *, upeer_sockaddr)) sc_inout(__field_hex(int *, upeer_addrlen))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(upeer_sockaddr, upeer_sockaddr)) sc_inout(tp_assign(upeer_addrlen, upeer_addrlen))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_bind
SC_LTTNG_TRACEPOINT_EVENT(bind,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * umyaddr, int addrlen),
	TP_ARGS(sc_exit(ret,) fd, umyaddr, addrlen),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field_hex(struct sockaddr *, umyaddr)) sc_in(__field_hex(int, addrlen))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(umyaddr, umyaddr)) sc_in(tp_assign(addrlen, addrlen))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getsockname
SC_LTTNG_TRACEPOINT_EVENT(getsockname,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * usockaddr, int * usockaddr_len),
	TP_ARGS(sc_exit(ret,) fd, usockaddr, usockaddr_len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_out(__field_hex(struct sockaddr *, usockaddr)) sc_inout(__field_hex(int *, usockaddr_len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(usockaddr, usockaddr)) sc_inout(tp_assign(usockaddr_len, usockaddr_len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getpeername
SC_LTTNG_TRACEPOINT_EVENT(getpeername,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * usockaddr, int * usockaddr_len),
	TP_ARGS(sc_exit(ret,) fd, usockaddr, usockaddr_len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_out(__field_hex(struct sockaddr *, usockaddr)) sc_inout(__field_hex(int *, usockaddr_len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(usockaddr, usockaddr)) sc_inout(tp_assign(usockaddr_len, usockaddr_len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_semop
SC_LTTNG_TRACEPOINT_EVENT(semop,
	TP_PROTO(sc_exit(long ret,) int semid, struct sembuf * tsops, unsigned nsops),
	TP_ARGS(sc_exit(ret,) semid, tsops, nsops),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, semid)) sc_in(__field_hex(struct sembuf *, tsops)) sc_in(__field(unsigned, nsops))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(semid, semid)) sc_in(tp_assign(tsops, tsops)) sc_in(tp_assign(nsops, nsops))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_readlink
SC_LTTNG_TRACEPOINT_EVENT(readlink,
	TP_PROTO(sc_exit(long ret,) const char * path, char * buf, int bufsiz),
	TP_ARGS(sc_exit(ret,) path, buf, bufsiz),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(path, path)) sc_out(__field_hex(char *, buf)) sc_in(__field(int, bufsiz))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(path, path)) sc_out(tp_assign(buf, buf)) sc_in(tp_assign(bufsiz, bufsiz))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_chown
SC_LTTNG_TRACEPOINT_EVENT(chown,
	TP_PROTO(sc_exit(long ret,) const char * filename, uid_t user, gid_t group),
	TP_ARGS(sc_exit(ret,) filename, user, group),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(filename, filename)) sc_in(__field(uid_t, user)) sc_in(__field(gid_t, group))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(user, user)) sc_in(tp_assign(group, group))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_lchown
SC_LTTNG_TRACEPOINT_EVENT(lchown,
	TP_PROTO(sc_exit(long ret,) const char * filename, uid_t user, gid_t group),
	TP_ARGS(sc_exit(ret,) filename, user, group),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(filename, filename)) sc_in(__field(uid_t, user)) sc_in(__field(gid_t, group))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(user, user)) sc_in(tp_assign(group, group))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_syslog
SC_LTTNG_TRACEPOINT_EVENT(syslog,
	TP_PROTO(sc_exit(long ret,) int type, char * buf, int len),
	TP_ARGS(sc_exit(ret,) type, buf, len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, type)) sc_out(__field_hex(char *, buf)) sc_in(__field(int, len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(type, type)) sc_out(tp_assign(buf, buf)) sc_in(tp_assign(len, len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getresuid
SC_LTTNG_TRACEPOINT_EVENT(getresuid,
	TP_PROTO(sc_exit(long ret,) uid_t * ruidp, uid_t * euidp, uid_t * suidp),
	TP_ARGS(sc_exit(ret,) ruidp, euidp, suidp),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_out(__field_hex(uid_t *, ruidp)) sc_out(__field_hex(uid_t *, euidp)) sc_out(__field_hex(uid_t *, suidp))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_out(tp_assign(ruidp, ruidp)) sc_out(tp_assign(euidp, euidp)) sc_out(tp_assign(suidp, suidp))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getresgid
SC_LTTNG_TRACEPOINT_EVENT(getresgid,
	TP_PROTO(sc_exit(long ret,) gid_t * rgidp, gid_t * egidp, gid_t * sgidp),
	TP_ARGS(sc_exit(ret,) rgidp, egidp, sgidp),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_out(__field_hex(gid_t *, rgidp)) sc_out(__field_hex(gid_t *, egidp)) sc_out(__field_hex(gid_t *, sgidp))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_out(tp_assign(rgidp, rgidp)) sc_out(tp_assign(egidp, egidp)) sc_out(tp_assign(sgidp, sgidp))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_rt_sigqueueinfo
SC_LTTNG_TRACEPOINT_EVENT(32_rt_sigqueueinfo,
	TP_PROTO(sc_exit(long ret,) int pid, int sig, compat_siginfo_t * uinfo),
	TP_ARGS(sc_exit(ret,) pid, sig, uinfo),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(int, pid)) sc_inout(__field(int, sig)) sc_inout(__field_hex(compat_siginfo_t *, uinfo))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(pid, pid)) sc_inout(tp_assign(sig, sig)) sc_inout(tp_assign(uinfo, uinfo))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_mknod
SC_LTTNG_TRACEPOINT_EVENT(mknod,
	TP_PROTO(sc_exit(long ret,) const char * filename, umode_t mode, unsigned dev),
	TP_ARGS(sc_exit(ret,) filename, mode, dev),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(filename, filename)) sc_in(__field(umode_t, mode)) sc_in(__field(unsigned, dev))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(mode, mode)) sc_in(tp_assign(dev, dev))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sched_setscheduler
SC_LTTNG_TRACEPOINT_EVENT(sched_setscheduler,
	TP_PROTO(sc_exit(long ret,) pid_t pid, int policy, struct sched_param * param),
	TP_ARGS(sc_exit(ret,) pid, policy, param),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field(int, policy)) sc_in(__field_hex(struct sched_param *, param))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(policy, policy)) sc_in(tp_assign(param, param))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_init_module
SC_LTTNG_TRACEPOINT_EVENT(init_module,
	TP_PROTO(sc_exit(long ret,) void * umod, unsigned long len, const char * uargs),
	TP_ARGS(sc_exit(ret,) umod, len, uargs),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field_hex(void *, umod)) sc_in(__field(unsigned long, len)) sc_in(__field_hex(const char *, uargs))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(umod, umod)) sc_in(tp_assign(len, len)) sc_in(tp_assign(uargs, uargs))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_listxattr
SC_LTTNG_TRACEPOINT_EVENT(listxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, char * list, size_t size),
	TP_ARGS(sc_exit(ret,) pathname, list, size),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname)) sc_out(__field_hex(char *, list)) sc_in(__field(size_t, size))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_out(tp_assign(list, list)) sc_in(tp_assign(size, size))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_llistxattr
SC_LTTNG_TRACEPOINT_EVENT(llistxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, char * list, size_t size),
	TP_ARGS(sc_exit(ret,) pathname, list, size),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname)) sc_out(__field_hex(char *, list)) sc_in(__field(size_t, size))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_out(tp_assign(list, list)) sc_in(tp_assign(size, size))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_flistxattr
SC_LTTNG_TRACEPOINT_EVENT(flistxattr,
	TP_PROTO(sc_exit(long ret,) int fd, char * list, size_t size),
	TP_ARGS(sc_exit(ret,) fd, list, size),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_out(__field_hex(char *, list)) sc_in(__field(size_t, size))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(list, list)) sc_in(tp_assign(size, size))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_cachectl
SC_LTTNG_TRACEPOINT_EVENT(cachectl,
	TP_PROTO(sc_exit(long ret,) char * addr, int nbytes, int op),
	TP_ARGS(sc_exit(ret,) addr, nbytes, op),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field_hex(char *, addr)) sc_inout(__field(int, nbytes)) sc_inout(__field(int, op))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(addr, addr)) sc_inout(tp_assign(nbytes, nbytes)) sc_inout(tp_assign(op, op))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_io_cancel
SC_LTTNG_TRACEPOINT_EVENT(io_cancel,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx_id, struct iocb * iocb, struct io_event * result),
	TP_ARGS(sc_exit(ret,) ctx_id, iocb, result),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(aio_context_t, ctx_id)) sc_in(__field_hex(struct iocb *, iocb)) sc_out(__field_hex(struct io_event *, result))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ctx_id, ctx_id)) sc_in(tp_assign(iocb, iocb)) sc_out(tp_assign(result, result))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_inotify_add_watch
SC_LTTNG_TRACEPOINT_EVENT(inotify_add_watch,
	TP_PROTO(sc_exit(long ret,) int fd, const char * pathname, u32 mask),
	TP_ARGS(sc_exit(ret,) fd, pathname, mask),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__string_from_user(pathname, pathname)) sc_in(__field(u32, mask))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_in(tp_assign(mask, mask))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_mkdirat
SC_LTTNG_TRACEPOINT_EVENT(mkdirat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * pathname, umode_t mode),
	TP_ARGS(sc_exit(ret,) dfd, pathname, mode),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, dfd)) sc_in(__string_from_user(pathname, pathname)) sc_in(__field(umode_t, mode))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(dfd, dfd)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_in(tp_assign(mode, mode))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_unlinkat
SC_LTTNG_TRACEPOINT_EVENT(unlinkat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * pathname, int flag),
	TP_ARGS(sc_exit(ret,) dfd, pathname, flag),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, dfd)) sc_in(__string_from_user(pathname, pathname)) sc_in(__field(int, flag))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(dfd, dfd)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_in(tp_assign(flag, flag))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_symlinkat
SC_LTTNG_TRACEPOINT_EVENT(symlinkat,
	TP_PROTO(sc_exit(long ret,) const char * oldname, int newdfd, const char * newname),
	TP_ARGS(sc_exit(ret,) oldname, newdfd, newname),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(oldname, oldname)) sc_in(__field(int, newdfd)) sc_in(__string_from_user(newname, newname))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(oldname, oldname)) sc_in(tp_assign(newdfd, newdfd)) sc_in(tp_copy_string_from_user(newname, newname))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fchmodat
SC_LTTNG_TRACEPOINT_EVENT(fchmodat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, umode_t mode),
	TP_ARGS(sc_exit(ret,) dfd, filename, mode),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, dfd)) sc_in(__string_from_user(filename, filename)) sc_in(__field(umode_t, mode))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(dfd, dfd)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(mode, mode))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_faccessat
SC_LTTNG_TRACEPOINT_EVENT(faccessat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, int mode),
	TP_ARGS(sc_exit(ret,) dfd, filename, mode),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, dfd)) sc_in(__string_from_user(filename, filename)) sc_in(__field(int, mode))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(dfd, dfd)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(mode, mode))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getcpu
SC_LTTNG_TRACEPOINT_EVENT(getcpu,
	TP_PROTO(sc_exit(long ret,) unsigned * cpup, unsigned * nodep, struct getcpu_cache * unused),
	TP_ARGS(sc_exit(ret,) cpup, nodep, unused),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_out(__field_hex(unsigned *, cpup)) sc_out(__field_hex(unsigned *, nodep)) sc_inout(__field_hex(struct getcpu_cache *, unused))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_out(tp_assign(cpup, cpup)) sc_out(tp_assign(nodep, nodep)) sc_inout(tp_assign(unused, unused))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getdents64
SC_LTTNG_TRACEPOINT_EVENT(getdents64,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, struct linux_dirent64 * dirent, unsigned int count),
	TP_ARGS(sc_exit(ret,) fd, dirent, count),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, fd)) sc_out(__field_hex(struct linux_dirent64 *, dirent)) sc_in(__field(unsigned int, count))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(dirent, dirent)) sc_in(tp_assign(count, count))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_send
SC_LTTNG_TRACEPOINT_EVENT(send,
	TP_PROTO(sc_exit(long ret,) int fd, void * buff, size_t len, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, buff, len, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(int, fd)) sc_inout(__field_hex(void *, buff)) sc_inout(__field(size_t, len)) sc_inout(__field(unsigned int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(fd, fd)) sc_inout(tp_assign(buff, buff)) sc_inout(tp_assign(len, len)) sc_inout(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_truncate64
SC_LTTNG_TRACEPOINT_EVENT(32_truncate64,
	TP_PROTO(sc_exit(long ret,) const char * path, unsigned long __dummy, unsigned long a2, unsigned long a3),
	TP_ARGS(sc_exit(ret,) path, __dummy, a2, a3),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__string_from_user(path, path)) sc_inout(__field(unsigned long, __dummy)) sc_inout(__field(unsigned long, a2)) sc_inout(__field(unsigned long, a3))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_copy_string_from_user(path, path)) sc_inout(tp_assign(__dummy, __dummy)) sc_inout(tp_assign(a2, a2)) sc_inout(tp_assign(a3, a3))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_rt_sigaction
SC_LTTNG_TRACEPOINT_EVENT(rt_sigaction,
	TP_PROTO(sc_exit(long ret,) int sig, const struct sigaction * act, struct sigaction * oact, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) sig, act, oact, sigsetsize),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, sig)) sc_in(__field_hex(const struct sigaction *, act)) sc_out(__field_hex(struct sigaction *, oact)) sc_in(__field(size_t, sigsetsize))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(sig, sig)) sc_in(tp_assign(act, act)) sc_out(tp_assign(oact, oact)) sc_in(tp_assign(sigsetsize, sigsetsize))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_rt_sigprocmask
SC_LTTNG_TRACEPOINT_EVENT(rt_sigprocmask,
	TP_PROTO(sc_exit(long ret,) int how, sigset_t * nset, sigset_t * oset, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) how, nset, oset, sigsetsize),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, how)) sc_in(__field_hex(sigset_t *, nset)) sc_out(__field_hex(sigset_t *, oset)) sc_in(__field(size_t, sigsetsize))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(how, how)) sc_in(tp_assign(nset, nset)) sc_out(tp_assign(oset, oset)) sc_in(tp_assign(sigsetsize, sigsetsize))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_wait4
SC_LTTNG_TRACEPOINT_EVENT(wait4,
	TP_PROTO(sc_exit(long ret,) pid_t upid, int * stat_addr, int options, struct rusage * ru),
	TP_ARGS(sc_exit(ret,) upid, stat_addr, options, ru),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, upid)) sc_out(__field_hex(int *, stat_addr)) sc_in(__field(int, options)) sc_out(__field_hex(struct rusage *, ru))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(upid, upid)) sc_out(tp_assign(stat_addr, stat_addr)) sc_in(tp_assign(options, options)) sc_out(tp_assign(ru, ru))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_msgsnd
SC_LTTNG_TRACEPOINT_EVENT(msgsnd,
	TP_PROTO(sc_exit(long ret,) int msqid, struct msgbuf * msgp, size_t msgsz, int msgflg),
	TP_ARGS(sc_exit(ret,) msqid, msgp, msgsz, msgflg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, msqid)) sc_in(__field_hex(struct msgbuf *, msgp)) sc_in(__field(size_t, msgsz)) sc_in(__field(int, msgflg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(msqid, msqid)) sc_in(tp_assign(msgp, msgp)) sc_in(tp_assign(msgsz, msgsz)) sc_in(tp_assign(msgflg, msgflg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_rt_sigtimedwait
SC_LTTNG_TRACEPOINT_EVENT(rt_sigtimedwait,
	TP_PROTO(sc_exit(long ret,) const sigset_t * uthese, siginfo_t * uinfo, const struct timespec * uts, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) uthese, uinfo, uts, sigsetsize),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_out(__field_hex(const sigset_t *, uthese)) sc_out(__field_hex(siginfo_t *, uinfo)) sc_in(__field_hex(const struct timespec *, uts)) sc_in(__field(size_t, sigsetsize))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_out(tp_assign(uthese, uthese)) sc_out(tp_assign(uinfo, uinfo)) sc_in(tp_assign(uts, uts)) sc_in(tp_assign(sigsetsize, sigsetsize))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_semtimedop
SC_LTTNG_TRACEPOINT_EVENT(semtimedop,
	TP_PROTO(sc_exit(long ret,) int semid, struct sembuf * tsops, unsigned nsops, const struct timespec * timeout),
	TP_ARGS(sc_exit(ret,) semid, tsops, nsops, timeout),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, semid)) sc_in(__field_hex(struct sembuf *, tsops)) sc_in(__field(unsigned, nsops)) sc_inout(__field_hex(const struct timespec *, timeout))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(semid, semid)) sc_in(tp_assign(tsops, tsops)) sc_in(tp_assign(nsops, nsops)) sc_inout(tp_assign(timeout, timeout))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_timer_settime
SC_LTTNG_TRACEPOINT_EVENT(timer_settime,
	TP_PROTO(sc_exit(long ret,) timer_t timer_id, int flags, const struct itimerspec * new_setting, struct itimerspec * old_setting),
	TP_ARGS(sc_exit(ret,) timer_id, flags, new_setting, old_setting),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(timer_t, timer_id)) sc_in(__field(int, flags)) sc_in(__field_hex(const struct itimerspec *, new_setting)) sc_out(__field_hex(struct itimerspec *, old_setting))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(timer_id, timer_id)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(new_setting, new_setting)) sc_out(tp_assign(old_setting, old_setting))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_clock_nanosleep
SC_LTTNG_TRACEPOINT_EVENT(clock_nanosleep,
	TP_PROTO(sc_exit(long ret,) const clockid_t which_clock, int flags, const struct timespec * rqtp, struct timespec * rmtp),
	TP_ARGS(sc_exit(ret,) which_clock, flags, rqtp, rmtp),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(const clockid_t, which_clock)) sc_in(__field(int, flags)) sc_in(__field_hex(const struct timespec *, rqtp)) sc_out(__field_hex(struct timespec *, rmtp))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which_clock, which_clock)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(rqtp, rqtp)) sc_out(tp_assign(rmtp, rmtp))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_vmsplice
SC_LTTNG_TRACEPOINT_EVENT(vmsplice,
	TP_PROTO(sc_exit(long ret,) int fd, const struct iovec * iov, unsigned long nr_segs, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, iov, nr_segs, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field_hex(const struct iovec *, iov)) sc_in(__field(unsigned long, nr_segs)) sc_in(__field(unsigned int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(iov, iov)) sc_in(tp_assign(nr_segs, nr_segs)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_utimensat
SC_LTTNG_TRACEPOINT_EVENT(utimensat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, struct timespec * utimes, int flags),
	TP_ARGS(sc_exit(ret,) dfd, filename, utimes, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, dfd)) sc_in(__string_from_user(filename, filename)) sc_in(__field_hex(struct timespec *, utimes)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(dfd, dfd)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(utimes, utimes)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_timerfd_settime
SC_LTTNG_TRACEPOINT_EVENT(timerfd_settime,
	TP_PROTO(sc_exit(long ret,) int ufd, int flags, const struct itimerspec * utmr, struct itimerspec * otmr),
	TP_ARGS(sc_exit(ret,) ufd, flags, utmr, otmr),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, ufd)) sc_in(__field(int, flags)) sc_in(__field_hex(const struct itimerspec *, utmr)) sc_out(__field_hex(struct itimerspec *, otmr))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ufd, ufd)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(utmr, utmr)) sc_out(tp_assign(otmr, otmr))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_rt_tgsigqueueinfo
SC_LTTNG_TRACEPOINT_EVENT(rt_tgsigqueueinfo,
	TP_PROTO(sc_exit(long ret,) pid_t tgid, pid_t pid, int sig, siginfo_t * uinfo),
	TP_ARGS(sc_exit(ret,) tgid, pid, sig, uinfo),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, tgid)) sc_in(__field(pid_t, pid)) sc_in(__field(int, sig)) sc_in(__field_hex(siginfo_t *, uinfo))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(tgid, tgid)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(sig, sig)) sc_in(tp_assign(uinfo, uinfo))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sendmmsg
SC_LTTNG_TRACEPOINT_EVENT(sendmmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct mmsghdr * mmsg, unsigned int vlen, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, mmsg, vlen, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field_hex(struct mmsghdr *, mmsg)) sc_in(__field(unsigned int, vlen)) sc_in(__field(unsigned int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(mmsg, mmsg)) sc_in(tp_assign(vlen, vlen)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_rt_sigaction
SC_LTTNG_TRACEPOINT_EVENT(32_rt_sigaction,
	TP_PROTO(sc_exit(long ret,) int sig, const struct sigaction32 * act, struct sigaction32 * oact, unsigned int sigsetsize),
	TP_ARGS(sc_exit(ret,) sig, act, oact, sigsetsize),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(int, sig)) sc_inout(__field_hex(const struct sigaction32 *, act)) sc_inout(__field_hex(struct sigaction32 *, oact)) sc_inout(__field(unsigned int, sigsetsize))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(sig, sig)) sc_inout(tp_assign(act, act)) sc_inout(tp_assign(oact, oact)) sc_inout(tp_assign(sigsetsize, sigsetsize))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_rt_sigprocmask
SC_LTTNG_TRACEPOINT_EVENT(32_rt_sigprocmask,
	TP_PROTO(sc_exit(long ret,) int how, compat_sigset_t * set, compat_sigset_t * oset, unsigned int sigsetsize),
	TP_ARGS(sc_exit(ret,) how, set, oset, sigsetsize),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(int, how)) sc_inout(__field_hex(compat_sigset_t *, set)) sc_inout(__field_hex(compat_sigset_t *, oset)) sc_inout(__field(unsigned int, sigsetsize))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(how, how)) sc_inout(tp_assign(set, set)) sc_inout(tp_assign(oset, oset)) sc_inout(tp_assign(sigsetsize, sigsetsize))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_sendfile
SC_LTTNG_TRACEPOINT_EVENT(32_sendfile,
	TP_PROTO(sc_exit(long ret,) long out_fd, long in_fd, compat_off_t * offset, s32 count),
	TP_ARGS(sc_exit(ret,) out_fd, in_fd, offset, count),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(long, out_fd)) sc_inout(__field(long, in_fd)) sc_inout(__field_hex(compat_off_t *, offset)) sc_inout(__field(s32, count))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(out_fd, out_fd)) sc_inout(tp_assign(in_fd, in_fd)) sc_inout(tp_assign(offset, offset)) sc_inout(tp_assign(count, count))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_socketpair
SC_LTTNG_TRACEPOINT_EVENT(socketpair,
	TP_PROTO(sc_exit(long ret,) int family, int type, int protocol, int * usockvec),
	TP_ARGS(sc_exit(ret,) family, type, protocol, usockvec),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, family)) sc_in(__field(int, type)) sc_in(__field(int, protocol)) sc_out(__field_hex(int *, usockvec))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(family, family)) sc_in(tp_assign(type, type)) sc_in(tp_assign(protocol, protocol)) sc_out(tp_assign(usockvec, usockvec))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_reboot
SC_LTTNG_TRACEPOINT_EVENT(reboot,
	TP_PROTO(sc_exit(long ret,) int magic1, int magic2, unsigned int cmd, void * arg),
	TP_ARGS(sc_exit(ret,) magic1, magic2, cmd, arg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, magic1)) sc_in(__field(int, magic2)) sc_in(__field(unsigned int, cmd)) sc_in(__field_hex(void *, arg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(magic1, magic1)) sc_in(tp_assign(magic2, magic2)) sc_in(tp_assign(cmd, cmd)) sc_in(tp_assign(arg, arg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_quotactl
SC_LTTNG_TRACEPOINT_EVENT(quotactl,
	TP_PROTO(sc_exit(long ret,) unsigned int cmd, const char * special, qid_t id, void * addr),
	TP_ARGS(sc_exit(ret,) cmd, special, id, addr),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned int, cmd)) sc_in(__field_hex(const char *, special)) sc_in(__field(qid_t, id)) sc_inout(__field_hex(void *, addr))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(cmd, cmd)) sc_in(tp_assign(special, special)) sc_in(tp_assign(id, id)) sc_inout(tp_assign(addr, addr))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getxattr
SC_LTTNG_TRACEPOINT_EVENT(getxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name, void * value, size_t size),
	TP_ARGS(sc_exit(ret,) pathname, name, value, size),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname)) sc_in(__string_from_user(name, name)) sc_out(__field_hex(void *, value)) sc_in(__field(size_t, size))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_in(tp_copy_string_from_user(name, name)) sc_out(tp_assign(value, value)) sc_in(tp_assign(size, size))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_lgetxattr
SC_LTTNG_TRACEPOINT_EVENT(lgetxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name, void * value, size_t size),
	TP_ARGS(sc_exit(ret,) pathname, name, value, size),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname)) sc_in(__string_from_user(name, name)) sc_out(__field_hex(void *, value)) sc_in(__field(size_t, size))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_in(tp_copy_string_from_user(name, name)) sc_out(tp_assign(value, value)) sc_in(tp_assign(size, size))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fgetxattr
SC_LTTNG_TRACEPOINT_EVENT(fgetxattr,
	TP_PROTO(sc_exit(long ret,) int fd, const char * name, void * value, size_t size),
	TP_ARGS(sc_exit(ret,) fd, name, value, size),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__string_from_user(name, name)) sc_out(__field_hex(void *, value)) sc_in(__field(size_t, size))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_copy_string_from_user(name, name)) sc_out(tp_assign(value, value)) sc_in(tp_assign(size, size))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_epoll_ctl
SC_LTTNG_TRACEPOINT_EVENT(epoll_ctl,
	TP_PROTO(sc_exit(long ret,) int epfd, int op, int fd, struct epoll_event * event),
	TP_ARGS(sc_exit(ret,) epfd, op, fd, event),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, epfd)) sc_in(__field(int, op)) sc_in(__field(int, fd)) sc_in(__field_hex(struct epoll_event *, event))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(epfd, epfd)) sc_in(tp_assign(op, op)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(event, event))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_epoll_wait
SC_LTTNG_TRACEPOINT_EVENT(epoll_wait,
	TP_PROTO(sc_exit(long ret,) int epfd, struct epoll_event * events, int maxevents, int timeout),
	TP_ARGS(sc_exit(ret,) epfd, events, maxevents, timeout),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, epfd)) sc_out(__field_hex(struct epoll_event *, events)) sc_in(__field(int, maxevents)) sc_in(__field(int, timeout))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(epfd, epfd)) sc_out(tp_assign(events, events)) sc_in(tp_assign(maxevents, maxevents)) sc_in(tp_assign(timeout, timeout))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sendfile64
SC_LTTNG_TRACEPOINT_EVENT(sendfile64,
	TP_PROTO(sc_exit(long ret,) int out_fd, int in_fd, loff_t * offset, size_t count),
	TP_ARGS(sc_exit(ret,) out_fd, in_fd, offset, count),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, out_fd)) sc_in(__field(int, in_fd)) sc_inout(__field_hex(loff_t *, offset)) sc_in(__field(size_t, count))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(out_fd, out_fd)) sc_in(tp_assign(in_fd, in_fd)) sc_inout(tp_assign(offset, offset)) sc_in(tp_assign(count, count))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_openat
SC_LTTNG_TRACEPOINT_EVENT(openat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, int flags, umode_t mode),
	TP_ARGS(sc_exit(ret,) dfd, filename, flags, mode),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, dfd)) sc_in(__string_from_user(filename, filename)) sc_in(__field(int, flags)) sc_in(__field(umode_t, mode))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(dfd, dfd)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(mode, mode))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_mknodat
SC_LTTNG_TRACEPOINT_EVENT(mknodat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, umode_t mode, unsigned dev),
	TP_ARGS(sc_exit(ret,) dfd, filename, mode, dev),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, dfd)) sc_in(__string_from_user(filename, filename)) sc_in(__field(umode_t, mode)) sc_in(__field(unsigned, dev))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(dfd, dfd)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(mode, mode)) sc_in(tp_assign(dev, dev))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_newfstatat
SC_LTTNG_TRACEPOINT_EVENT(newfstatat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, struct stat * statbuf, int flag),
	TP_ARGS(sc_exit(ret,) dfd, filename, statbuf, flag),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, dfd)) sc_in(__string_from_user(filename, filename)) sc_out(__field_hex(struct stat *, statbuf)) sc_in(__field(int, flag))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(dfd, dfd)) sc_in(tp_copy_string_from_user(filename, filename)) sc_out(tp_assign(statbuf, statbuf)) sc_in(tp_assign(flag, flag))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_renameat
SC_LTTNG_TRACEPOINT_EVENT(renameat,
	TP_PROTO(sc_exit(long ret,) int olddfd, const char * oldname, int newdfd, const char * newname),
	TP_ARGS(sc_exit(ret,) olddfd, oldname, newdfd, newname),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, olddfd)) sc_in(__string_from_user(oldname, oldname)) sc_in(__field(int, newdfd)) sc_in(__string_from_user(newname, newname))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(olddfd, olddfd)) sc_in(tp_copy_string_from_user(oldname, oldname)) sc_in(tp_assign(newdfd, newdfd)) sc_in(tp_copy_string_from_user(newname, newname))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_readlinkat
SC_LTTNG_TRACEPOINT_EVENT(readlinkat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * pathname, char * buf, int bufsiz),
	TP_ARGS(sc_exit(ret,) dfd, pathname, buf, bufsiz),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, dfd)) sc_in(__string_from_user(pathname, pathname)) sc_out(__field_hex(char *, buf)) sc_in(__field(int, bufsiz))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(dfd, dfd)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_out(tp_assign(buf, buf)) sc_in(tp_assign(bufsiz, bufsiz))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_signalfd4
SC_LTTNG_TRACEPOINT_EVENT(signalfd4,
	TP_PROTO(sc_exit(long ret,) int ufd, sigset_t * user_mask, size_t sizemask, int flags),
	TP_ARGS(sc_exit(ret,) ufd, user_mask, sizemask, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, ufd)) sc_in(__field_hex(sigset_t *, user_mask)) sc_in(__field(size_t, sizemask)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ufd, ufd)) sc_in(tp_assign(user_mask, user_mask)) sc_in(tp_assign(sizemask, sizemask)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_accept4
SC_LTTNG_TRACEPOINT_EVENT(accept4,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * upeer_sockaddr, int * upeer_addrlen, int flags),
	TP_ARGS(sc_exit(ret,) fd, upeer_sockaddr, upeer_addrlen, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_out(__field_hex(struct sockaddr *, upeer_sockaddr)) sc_inout(__field_hex(int *, upeer_addrlen)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(upeer_sockaddr, upeer_sockaddr)) sc_inout(tp_assign(upeer_addrlen, upeer_addrlen)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_prlimit64
SC_LTTNG_TRACEPOINT_EVENT(prlimit64,
	TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned int resource, const struct rlimit64 * new_rlim, struct rlimit64 * old_rlim),
	TP_ARGS(sc_exit(ret,) pid, resource, new_rlim, old_rlim),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field(unsigned int, resource)) sc_in(__field_hex(const struct rlimit64 *, new_rlim)) sc_out(__field_hex(struct rlimit64 *, old_rlim))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(resource, resource)) sc_in(tp_assign(new_rlim, new_rlim)) sc_out(tp_assign(old_rlim, old_rlim))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_llseek
SC_LTTNG_TRACEPOINT_EVENT(32_llseek,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int offset_high, unsigned int offset_low, loff_t * result, unsigned int origin),
	TP_ARGS(sc_exit(ret,) fd, offset_high, offset_low, result, origin),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(unsigned int, fd)) sc_inout(__field(unsigned int, offset_high)) sc_inout(__field(unsigned int, offset_low)) sc_inout(__field_hex(loff_t *, result)) sc_inout(__field(unsigned int, origin))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(fd, fd)) sc_inout(tp_assign(offset_high, offset_high)) sc_inout(tp_assign(offset_low, offset_low)) sc_inout(tp_assign(result, result)) sc_inout(tp_assign(origin, origin))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_waitid
SC_LTTNG_TRACEPOINT_EVENT(32_waitid,
	TP_PROTO(sc_exit(long ret,) int which, compat_pid_t pid, compat_siginfo_t * uinfo, int options, struct compat_rusage * uru),
	TP_ARGS(sc_exit(ret,) which, pid, uinfo, options, uru),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(int, which)) sc_inout(__field(compat_pid_t, pid)) sc_inout(__field_hex(compat_siginfo_t *, uinfo)) sc_inout(__field(int, options)) sc_inout(__field_hex(struct compat_rusage *, uru))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(which, which)) sc_inout(tp_assign(pid, pid)) sc_inout(tp_assign(uinfo, uinfo)) sc_inout(tp_assign(options, options)) sc_inout(tp_assign(uru, uru))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_select
SC_LTTNG_TRACEPOINT_EVENT(select,
	TP_PROTO(sc_exit(long ret,) int n, fd_set * inp, fd_set * outp, fd_set * exp, struct timeval * tvp),
	TP_ARGS(sc_exit(ret,) n, inp, outp, exp, tvp),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, n)) sc_inout(__field_hex(fd_set *, inp)) sc_inout(__field_hex(fd_set *, outp)) sc_inout(__field_hex(fd_set *, exp)) sc_inout(__field_hex(struct timeval *, tvp))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(n, n)) sc_inout(tp_assign(inp, inp)) sc_inout(tp_assign(outp, outp)) sc_inout(tp_assign(exp, exp)) sc_inout(tp_assign(tvp, tvp))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setsockopt
SC_LTTNG_TRACEPOINT_EVENT(setsockopt,
	TP_PROTO(sc_exit(long ret,) int fd, int level, int optname, char * optval, int optlen),
	TP_ARGS(sc_exit(ret,) fd, level, optname, optval, optlen),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(int, level)) sc_in(__field(int, optname)) sc_in(__field_hex(char *, optval)) sc_in(__field(int, optlen))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(level, level)) sc_in(tp_assign(optname, optname)) sc_in(tp_assign(optval, optval)) sc_in(tp_assign(optlen, optlen))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_msgrcv
SC_LTTNG_TRACEPOINT_EVENT(msgrcv,
	TP_PROTO(sc_exit(long ret,) int msqid, struct msgbuf * msgp, size_t msgsz, long msgtyp, int msgflg),
	TP_ARGS(sc_exit(ret,) msqid, msgp, msgsz, msgtyp, msgflg),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, msqid)) sc_out(__field_hex(struct msgbuf *, msgp)) sc_in(__field(size_t, msgsz)) sc_in(__field(long, msgtyp)) sc_in(__field(int, msgflg))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(msqid, msqid)) sc_out(tp_assign(msgp, msgp)) sc_in(tp_assign(msgsz, msgsz)) sc_in(tp_assign(msgtyp, msgtyp)) sc_in(tp_assign(msgflg, msgflg))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_mount
SC_LTTNG_TRACEPOINT_EVENT(mount,
	TP_PROTO(sc_exit(long ret,) char * dev_name, char * dir_name, char * type, unsigned long flags, void * data),
	TP_ARGS(sc_exit(ret,) dev_name, dir_name, type, flags, data),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(dev_name, dev_name)) sc_in(__string_from_user(dir_name, dir_name)) sc_in(__string_from_user(type, type)) sc_in(__field(unsigned long, flags)) sc_in(__field_hex(void *, data))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(dev_name, dev_name)) sc_in(tp_copy_string_from_user(dir_name, dir_name)) sc_in(tp_copy_string_from_user(type, type)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(data, data))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_io_getevents
SC_LTTNG_TRACEPOINT_EVENT(io_getevents,
	TP_PROTO(sc_exit(long ret,) aio_context_t ctx_id, long min_nr, long nr, struct io_event * events, struct timespec * timeout),
	TP_ARGS(sc_exit(ret,) ctx_id, min_nr, nr, events, timeout),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(aio_context_t, ctx_id)) sc_in(__field(long, min_nr)) sc_in(__field(long, nr)) sc_out(__field_hex(struct io_event *, events)) sc_inout(__field_hex(struct timespec *, timeout))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(ctx_id, ctx_id)) sc_in(tp_assign(min_nr, min_nr)) sc_in(tp_assign(nr, nr)) sc_out(tp_assign(events, events)) sc_inout(tp_assign(timeout, timeout))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_waitid
SC_LTTNG_TRACEPOINT_EVENT(waitid,
	TP_PROTO(sc_exit(long ret,) int which, pid_t upid, struct siginfo * infop, int options, struct rusage * ru),
	TP_ARGS(sc_exit(ret,) which, upid, infop, options, ru),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, which)) sc_in(__field(pid_t, upid)) sc_out(__field_hex(struct siginfo *, infop)) sc_in(__field(int, options)) sc_out(__field_hex(struct rusage *, ru))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(which, which)) sc_in(tp_assign(upid, upid)) sc_out(tp_assign(infop, infop)) sc_in(tp_assign(options, options)) sc_out(tp_assign(ru, ru))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_ppoll
SC_LTTNG_TRACEPOINT_EVENT(ppoll,
	TP_PROTO(sc_exit(long ret,) struct pollfd * ufds, unsigned int nfds, struct timespec * tsp, const sigset_t * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) ufds, nfds, tsp, sigmask, sigsetsize),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field_hex(struct pollfd *, ufds)) sc_in(__field(unsigned int, nfds)) sc_in(__field_hex(struct timespec *, tsp)) sc_in(__field_hex(const sigset_t *, sigmask)) sc_in(__field(size_t, sigsetsize))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(ufds, ufds)) sc_in(tp_assign(nfds, nfds)) sc_in(tp_assign(tsp, tsp)) sc_in(tp_assign(sigmask, sigmask)) sc_in(tp_assign(sigsetsize, sigsetsize))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_recvmmsg
SC_LTTNG_TRACEPOINT_EVENT(recvmmsg,
	TP_PROTO(sc_exit(long ret,) int fd, struct mmsghdr * mmsg, unsigned int vlen, unsigned int flags, struct timespec * timeout),
	TP_ARGS(sc_exit(ret,) fd, mmsg, vlen, flags, timeout),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_out(__field_hex(struct mmsghdr *, mmsg)) sc_in(__field(unsigned int, vlen)) sc_in(__field(unsigned int, flags)) sc_inout(__field_hex(struct timespec *, timeout))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(mmsg, mmsg)) sc_in(tp_assign(vlen, vlen)) sc_in(tp_assign(flags, flags)) sc_inout(tp_assign(timeout, timeout))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_getsockopt
SC_LTTNG_TRACEPOINT_EVENT(getsockopt,
	TP_PROTO(sc_exit(long ret,) int fd, int level, int optname, char * optval, int * optlen),
	TP_ARGS(sc_exit(ret,) fd, level, optname, optval, optlen),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field(int, level)) sc_in(__field(int, optname)) sc_out(__field_hex(char *, optval)) sc_inout(__field_hex(int *, optlen))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(level, level)) sc_in(tp_assign(optname, optname)) sc_out(tp_assign(optval, optval)) sc_inout(tp_assign(optlen, optlen))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_setxattr
SC_LTTNG_TRACEPOINT_EVENT(setxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name, const void * value, size_t size, int flags),
	TP_ARGS(sc_exit(ret,) pathname, name, value, size, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname)) sc_in(__string_from_user(name, name)) sc_in(__field_hex(const void *, value)) sc_in(__field(size_t, size)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_in(tp_copy_string_from_user(name, name)) sc_in(tp_assign(value, value)) sc_in(tp_assign(size, size)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_lsetxattr
SC_LTTNG_TRACEPOINT_EVENT(lsetxattr,
	TP_PROTO(sc_exit(long ret,) const char * pathname, const char * name, const void * value, size_t size, int flags),
	TP_ARGS(sc_exit(ret,) pathname, name, value, size, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__string_from_user(pathname, pathname)) sc_in(__string_from_user(name, name)) sc_in(__field_hex(const void *, value)) sc_in(__field(size_t, size)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_copy_string_from_user(pathname, pathname)) sc_in(tp_copy_string_from_user(name, name)) sc_in(tp_assign(value, value)) sc_in(tp_assign(size, size)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fsetxattr
SC_LTTNG_TRACEPOINT_EVENT(fsetxattr,
	TP_PROTO(sc_exit(long ret,) int fd, const char * name, const void * value, size_t size, int flags),
	TP_ARGS(sc_exit(ret,) fd, name, value, size, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__string_from_user(name, name)) sc_in(__field_hex(const void *, value)) sc_in(__field(size_t, size)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_copy_string_from_user(name, name)) sc_in(tp_assign(value, value)) sc_in(tp_assign(size, size)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_fchownat
SC_LTTNG_TRACEPOINT_EVENT(fchownat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, uid_t user, gid_t group, int flag),
	TP_ARGS(sc_exit(ret,) dfd, filename, user, group, flag),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, dfd)) sc_in(__string_from_user(filename, filename)) sc_in(__field(uid_t, user)) sc_in(__field(gid_t, group)) sc_in(__field(int, flag))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(dfd, dfd)) sc_in(tp_copy_string_from_user(filename, filename)) sc_in(tp_assign(user, user)) sc_in(tp_assign(group, group)) sc_in(tp_assign(flag, flag))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_linkat
SC_LTTNG_TRACEPOINT_EVENT(linkat,
	TP_PROTO(sc_exit(long ret,) int olddfd, const char * oldname, int newdfd, const char * newname, int flags),
	TP_ARGS(sc_exit(ret,) olddfd, oldname, newdfd, newname, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, olddfd)) sc_in(__string_from_user(oldname, oldname)) sc_in(__field(int, newdfd)) sc_in(__string_from_user(newname, newname)) sc_in(__field(int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(olddfd, olddfd)) sc_in(tp_copy_string_from_user(oldname, oldname)) sc_in(tp_assign(newdfd, newdfd)) sc_in(tp_copy_string_from_user(newname, newname)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_preadv
SC_LTTNG_TRACEPOINT_EVENT(preadv,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen, unsigned long pos_l, unsigned long pos_h),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen, pos_l, pos_h),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, fd)) sc_out(__field_hex(const struct iovec *, vec)) sc_in(__field(unsigned long, vlen)) sc_in(__field(unsigned long, pos_l)) sc_in(__field(unsigned long, pos_h))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(vec, vec)) sc_in(tp_assign(vlen, vlen)) sc_in(tp_assign(pos_l, pos_l)) sc_in(tp_assign(pos_h, pos_h))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_pwritev
SC_LTTNG_TRACEPOINT_EVENT(pwritev,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, const struct iovec * vec, unsigned long vlen, unsigned long pos_l, unsigned long pos_h),
	TP_ARGS(sc_exit(ret,) fd, vec, vlen, pos_l, pos_h),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(unsigned long, fd)) sc_in(__field_hex(const struct iovec *, vec)) sc_in(__field(unsigned long, vlen)) sc_in(__field(unsigned long, pos_l)) sc_in(__field(unsigned long, pos_h))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(vec, vec)) sc_in(tp_assign(vlen, vlen)) sc_in(tp_assign(pos_l, pos_l)) sc_in(tp_assign(pos_h, pos_h))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_pread
SC_LTTNG_TRACEPOINT_EVENT(32_pread,
	TP_PROTO(sc_exit(long ret,) unsigned long fd, char * buf, size_t count, unsigned long unused, unsigned long a4, unsigned long a5),
	TP_ARGS(sc_exit(ret,) fd, buf, count, unused, a4, a5),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(unsigned long, fd)) sc_inout(__field_hex(char *, buf)) sc_inout(__field(size_t, count)) sc_inout(__field(unsigned long, unused)) sc_inout(__field(unsigned long, a4)) sc_inout(__field(unsigned long, a5))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(fd, fd)) sc_inout(tp_assign(buf, buf)) sc_inout(tp_assign(count, count)) sc_inout(tp_assign(unused, unused)) sc_inout(tp_assign(a4, a4)) sc_inout(tp_assign(a5, a5))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_pwrite
SC_LTTNG_TRACEPOINT_EVENT(32_pwrite,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, const char * buf, size_t count, u32 unused, u64 a4, u64 a5),
	TP_ARGS(sc_exit(ret,) fd, buf, count, unused, a4, a5),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(unsigned int, fd)) sc_inout(__field_hex(const char *, buf)) sc_inout(__field(size_t, count)) sc_inout(__field(u32, unused)) sc_inout(__field(u64, a4)) sc_inout(__field(u64, a5))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(fd, fd)) sc_inout(tp_assign(buf, buf)) sc_inout(tp_assign(count, count)) sc_inout(tp_assign(unused, unused)) sc_inout(tp_assign(a4, a4)) sc_inout(tp_assign(a5, a5))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_fanotify_mark
SC_LTTNG_TRACEPOINT_EVENT(32_fanotify_mark,
	TP_PROTO(sc_exit(long ret,) int fanotify_fd, unsigned int flags, u64 a3, u64 a4, int dfd, const char * pathname),
	TP_ARGS(sc_exit(ret,) fanotify_fd, flags, a3, a4, dfd, pathname),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field(int, fanotify_fd)) sc_inout(__field(unsigned int, flags)) sc_inout(__field(u64, a3)) sc_inout(__field(u64, a4)) sc_inout(__field(int, dfd)) sc_inout(__string_from_user(pathname, pathname))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(fanotify_fd, fanotify_fd)) sc_inout(tp_assign(flags, flags)) sc_inout(tp_assign(a3, a3)) sc_inout(tp_assign(a4, a4)) sc_inout(tp_assign(dfd, dfd)) sc_inout(tp_copy_string_from_user(pathname, pathname))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_recvfrom
SC_LTTNG_TRACEPOINT_EVENT(recvfrom,
	TP_PROTO(sc_exit(long ret,) int fd, void * ubuf, size_t size, unsigned int flags, struct sockaddr * addr, int * addr_len),
	TP_ARGS(sc_exit(ret,) fd, ubuf, size, flags, addr, addr_len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_out(__field_hex(void *, ubuf)) sc_in(__field(size_t, size)) sc_in(__field(unsigned int, flags)) sc_out(__field_hex(struct sockaddr *, addr)) sc_inout(__field_hex(int *, addr_len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_out(tp_assign(ubuf, ubuf)) sc_in(tp_assign(size, size)) sc_in(tp_assign(flags, flags)) sc_out(tp_assign(addr, addr)) sc_inout(tp_assign(addr_len, addr_len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_futex
SC_LTTNG_TRACEPOINT_EVENT(futex,
	TP_PROTO(sc_exit(long ret,) u32 * uaddr, int op, u32 val, struct timespec * utime, u32 * uaddr2, u32 val3),
	TP_ARGS(sc_exit(ret,) uaddr, op, val, utime, uaddr2, val3),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field_hex(u32 *, uaddr)) sc_in(__field(int, op)) sc_in(__field(u32, val)) sc_in(__field_hex(struct timespec *, utime)) sc_inout(__field_hex(u32 *, uaddr2)) sc_in(__field(u32, val3))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(uaddr, uaddr)) sc_in(tp_assign(op, op)) sc_in(tp_assign(val, val)) sc_in(tp_assign(utime, utime)) sc_inout(tp_assign(uaddr2, uaddr2)) sc_in(tp_assign(val3, val3))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_pselect6
SC_LTTNG_TRACEPOINT_EVENT(pselect6,
	TP_PROTO(sc_exit(long ret,) int n, fd_set * inp, fd_set * outp, fd_set * exp, struct timespec * tsp, void * sig),
	TP_ARGS(sc_exit(ret,) n, inp, outp, exp, tsp, sig),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, n)) sc_inout(__field_hex(fd_set *, inp)) sc_inout(__field_hex(fd_set *, outp)) sc_inout(__field_hex(fd_set *, exp)) sc_inout(__field_hex(struct timespec *, tsp)) sc_in(__field_hex(void *, sig))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(n, n)) sc_inout(tp_assign(inp, inp)) sc_inout(tp_assign(outp, outp)) sc_inout(tp_assign(exp, exp)) sc_inout(tp_assign(tsp, tsp)) sc_in(tp_assign(sig, sig))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_epoll_pwait
SC_LTTNG_TRACEPOINT_EVENT(epoll_pwait,
	TP_PROTO(sc_exit(long ret,) int epfd, struct epoll_event * events, int maxevents, int timeout, const sigset_t * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) epfd, events, maxevents, timeout, sigmask, sigsetsize),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, epfd)) sc_out(__field_hex(struct epoll_event *, events)) sc_in(__field(int, maxevents)) sc_in(__field(int, timeout)) sc_in(__field_hex(const sigset_t *, sigmask)) sc_in(__field(size_t, sigsetsize))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(epfd, epfd)) sc_out(tp_assign(events, events)) sc_in(tp_assign(maxevents, maxevents)) sc_in(tp_assign(timeout, timeout)) sc_in(tp_assign(sigmask, sigmask)) sc_in(tp_assign(sigsetsize, sigsetsize))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_process_vm_readv
SC_LTTNG_TRACEPOINT_EVENT(process_vm_readv,
	TP_PROTO(sc_exit(long ret,) pid_t pid, const struct iovec * lvec, unsigned long liovcnt, const struct iovec * rvec, unsigned long riovcnt, unsigned long flags),
	TP_ARGS(sc_exit(ret,) pid, lvec, liovcnt, rvec, riovcnt, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_out(__field_hex(const struct iovec *, lvec)) sc_in(__field(unsigned long, liovcnt)) sc_in(__field_hex(const struct iovec *, rvec)) sc_in(__field(unsigned long, riovcnt)) sc_in(__field(unsigned long, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_out(tp_assign(lvec, lvec)) sc_in(tp_assign(liovcnt, liovcnt)) sc_in(tp_assign(rvec, rvec)) sc_in(tp_assign(riovcnt, riovcnt)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_process_vm_writev
SC_LTTNG_TRACEPOINT_EVENT(process_vm_writev,
	TP_PROTO(sc_exit(long ret,) pid_t pid, const struct iovec * lvec, unsigned long liovcnt, const struct iovec * rvec, unsigned long riovcnt, unsigned long flags),
	TP_ARGS(sc_exit(ret,) pid, lvec, liovcnt, rvec, riovcnt, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(pid_t, pid)) sc_in(__field_hex(const struct iovec *, lvec)) sc_in(__field(unsigned long, liovcnt)) sc_in(__field_hex(const struct iovec *, rvec)) sc_in(__field(unsigned long, riovcnt)) sc_in(__field(unsigned long, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(pid, pid)) sc_in(tp_assign(lvec, lvec)) sc_in(tp_assign(liovcnt, liovcnt)) sc_in(tp_assign(rvec, rvec)) sc_in(tp_assign(riovcnt, riovcnt)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_sendto
SC_LTTNG_TRACEPOINT_EVENT(sendto,
	TP_PROTO(sc_exit(long ret,) int fd, void * buff, size_t len, unsigned int flags, struct sockaddr * addr, int addr_len),
	TP_ARGS(sc_exit(ret,) fd, buff, len, flags, addr, addr_len),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd)) sc_in(__field_hex(void *, buff)) sc_in(__field(size_t, len)) sc_in(__field(unsigned int, flags)) sc_in(__field_hex(struct sockaddr *, addr)) sc_in(__field_hex(int, addr_len))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(buff, buff)) sc_in(tp_assign(len, len)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(addr, addr)) sc_in(tp_assign(addr_len, addr_len))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_32_futex
SC_LTTNG_TRACEPOINT_EVENT(32_futex,
	TP_PROTO(sc_exit(long ret,) u32 * uaddr, int op, u32 val, struct compat_timespec * utime, u32 * uaddr2, u32 val3),
	TP_ARGS(sc_exit(ret,) uaddr, op, val, utime, uaddr2, val3),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_inout(__field_hex(u32 *, uaddr)) sc_inout(__field(int, op)) sc_inout(__field(u32, val)) sc_inout(__field_hex(struct compat_timespec *, utime)) sc_inout(__field_hex(u32 *, uaddr2)) sc_inout(__field(u32, val3))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_inout(tp_assign(uaddr, uaddr)) sc_inout(tp_assign(op, op)) sc_inout(tp_assign(val, val)) sc_inout(tp_assign(utime, utime)) sc_inout(tp_assign(uaddr2, uaddr2)) sc_inout(tp_assign(val3, val3))),
	TP_printk()
)
#endif
#ifndef OVERRIDE_64_splice
SC_LTTNG_TRACEPOINT_EVENT(splice,
	TP_PROTO(sc_exit(long ret,) int fd_in, loff_t * off_in, int fd_out, loff_t * off_out, size_t len, unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd_in, off_in, fd_out, off_out, len, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field(int, fd_in)) sc_in(__field_hex(loff_t *, off_in)) sc_in(__field(int, fd_out)) sc_in(__field_hex(loff_t *, off_out)) sc_in(__field(size_t, len)) sc_in(__field(unsigned int, flags))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(fd_in, fd_in)) sc_in(tp_assign(off_in, off_in)) sc_in(tp_assign(fd_out, fd_out)) sc_in(tp_assign(off_out, off_out)) sc_in(tp_assign(len, len)) sc_in(tp_assign(flags, flags))),
	TP_printk()
)
#endif

#endif /*  _TRACE_SYSCALLS_POINTERS_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"

#else /* CREATE_SYSCALL_TABLE */

#include "mips-64-syscalls-3.5.0_pointers_override.h"
#include "syscalls_pointers_override.h"

#ifndef OVERRIDE_TABLE_64_waitpid
TRACE_SYSCALL_TABLE(waitpid, waitpid, 4007, 3)
#endif
#ifndef OVERRIDE_TABLE_64_oldumount
TRACE_SYSCALL_TABLE(oldumount, oldumount, 4022, 1)
#endif
#ifndef OVERRIDE_TABLE_64_olduname
TRACE_SYSCALL_TABLE(olduname, olduname, 4059, 1)
#endif
#ifndef OVERRIDE_TABLE_64_32_sigaction
TRACE_SYSCALL_TABLE(32_sigaction, 32_sigaction, 4067, 3)
#endif
#ifndef OVERRIDE_TABLE_64_uselib
TRACE_SYSCALL_TABLE(uselib, uselib, 4086, 1)
#endif
#ifndef OVERRIDE_TABLE_64_uname
TRACE_SYSCALL_TABLE(uname, uname, 4109, 1)
#endif
#ifndef OVERRIDE_TABLE_64_32_llseek
TRACE_SYSCALL_TABLE(32_llseek, 32_llseek, 4140, 5)
#endif
#ifndef OVERRIDE_TABLE_64_send
TRACE_SYSCALL_TABLE(send, send, 4178, 4)
#endif
#ifndef OVERRIDE_TABLE_64_32_pread
TRACE_SYSCALL_TABLE(32_pread, 32_pread, 4200, 6)
#endif
#ifndef OVERRIDE_TABLE_64_32_pwrite
TRACE_SYSCALL_TABLE(32_pwrite, 32_pwrite, 4201, 6)
#endif
#ifndef OVERRIDE_TABLE_64_32_truncate64
TRACE_SYSCALL_TABLE(32_truncate64, 32_truncate64, 4211, 4)
#endif
#ifndef OVERRIDE_TABLE_64_32_waitid
TRACE_SYSCALL_TABLE(32_waitid, 32_waitid, 4278, 5)
#endif
#ifndef OVERRIDE_TABLE_64_32_fanotify_mark
TRACE_SYSCALL_TABLE(32_fanotify_mark, 32_fanotify_mark, 4337, 6)
#endif
#ifndef OVERRIDE_TABLE_64_rt_sigaction
TRACE_SYSCALL_TABLE(rt_sigaction, rt_sigaction, 5013, 4)
#endif
#ifndef OVERRIDE_TABLE_64_rt_sigprocmask
TRACE_SYSCALL_TABLE(rt_sigprocmask, rt_sigprocmask, 5014, 4)
#endif
#ifndef OVERRIDE_TABLE_64_readv
TRACE_SYSCALL_TABLE(readv, readv, 5018, 3)
#endif
#ifndef OVERRIDE_TABLE_64_writev
TRACE_SYSCALL_TABLE(writev, writev, 5019, 3)
#endif
#ifndef OVERRIDE_TABLE_64_select
TRACE_SYSCALL_TABLE(select, select, 5022, 5)
#endif
#ifndef OVERRIDE_TABLE_64_shmctl
TRACE_SYSCALL_TABLE(shmctl, shmctl, 5030, 3)
#endif
#ifndef OVERRIDE_TABLE_64_nanosleep
TRACE_SYSCALL_TABLE(nanosleep, nanosleep, 5034, 2)
#endif
#ifndef OVERRIDE_TABLE_64_getitimer
TRACE_SYSCALL_TABLE(getitimer, getitimer, 5035, 2)
#endif
#ifndef OVERRIDE_TABLE_64_setitimer
TRACE_SYSCALL_TABLE(setitimer, setitimer, 5036, 3)
#endif
#ifndef OVERRIDE_TABLE_64_recvfrom
TRACE_SYSCALL_TABLE(recvfrom, recvfrom, 5044, 6)
#endif
#ifndef OVERRIDE_TABLE_64_sendmsg
TRACE_SYSCALL_TABLE(sendmsg, sendmsg, 5045, 3)
#endif
#ifndef OVERRIDE_TABLE_64_recvmsg
TRACE_SYSCALL_TABLE(recvmsg, recvmsg, 5046, 3)
#endif
#ifndef OVERRIDE_TABLE_64_setsockopt
TRACE_SYSCALL_TABLE(setsockopt, setsockopt, 5053, 5)
#endif
#ifndef OVERRIDE_TABLE_64_wait4
TRACE_SYSCALL_TABLE(wait4, wait4, 5059, 4)
#endif
#ifndef OVERRIDE_TABLE_64_msgsnd
TRACE_SYSCALL_TABLE(msgsnd, msgsnd, 5067, 4)
#endif
#ifndef OVERRIDE_TABLE_64_msgrcv
TRACE_SYSCALL_TABLE(msgrcv, msgrcv, 5068, 5)
#endif
#ifndef OVERRIDE_TABLE_64_msgctl
TRACE_SYSCALL_TABLE(msgctl, msgctl, 5069, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getdents
TRACE_SYSCALL_TABLE(getdents, getdents, 5076, 3)
#endif
#ifndef OVERRIDE_TABLE_64_gettimeofday
TRACE_SYSCALL_TABLE(gettimeofday, gettimeofday, 5094, 2)
#endif
#ifndef OVERRIDE_TABLE_64_getrlimit
TRACE_SYSCALL_TABLE(getrlimit, getrlimit, 5095, 2)
#endif
#ifndef OVERRIDE_TABLE_64_getrusage
TRACE_SYSCALL_TABLE(getrusage, getrusage, 5096, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sysinfo
TRACE_SYSCALL_TABLE(sysinfo, sysinfo, 5097, 1)
#endif
#ifndef OVERRIDE_TABLE_64_times
TRACE_SYSCALL_TABLE(times, times, 5098, 1)
#endif
#ifndef OVERRIDE_TABLE_64_rt_sigpending
TRACE_SYSCALL_TABLE(rt_sigpending, rt_sigpending, 5125, 2)
#endif
#ifndef OVERRIDE_TABLE_64_rt_sigtimedwait
TRACE_SYSCALL_TABLE(rt_sigtimedwait, rt_sigtimedwait, 5126, 4)
#endif
#ifndef OVERRIDE_TABLE_64_rt_sigqueueinfo
TRACE_SYSCALL_TABLE(rt_sigqueueinfo, rt_sigqueueinfo, 5127, 3)
#endif
#ifndef OVERRIDE_TABLE_64_utime
TRACE_SYSCALL_TABLE(utime, utime, 5130, 2)
#endif
#ifndef OVERRIDE_TABLE_64_ustat
TRACE_SYSCALL_TABLE(ustat, ustat, 5133, 2)
#endif
#ifndef OVERRIDE_TABLE_64_statfs
TRACE_SYSCALL_TABLE(statfs, statfs, 5134, 2)
#endif
#ifndef OVERRIDE_TABLE_64_fstatfs
TRACE_SYSCALL_TABLE(fstatfs, fstatfs, 5135, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sched_rr_get_interval
TRACE_SYSCALL_TABLE(sched_rr_get_interval, sched_rr_get_interval, 5145, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sysctl
TRACE_SYSCALL_TABLE(sysctl, sysctl, 5152, 1)
#endif
#ifndef OVERRIDE_TABLE_64_adjtimex
TRACE_SYSCALL_TABLE(adjtimex, adjtimex, 5154, 1)
#endif
#ifndef OVERRIDE_TABLE_64_setrlimit
TRACE_SYSCALL_TABLE(setrlimit, setrlimit, 5155, 2)
#endif
#ifndef OVERRIDE_TABLE_64_settimeofday
TRACE_SYSCALL_TABLE(settimeofday, settimeofday, 5159, 2)
#endif
#ifndef OVERRIDE_TABLE_64_mount
TRACE_SYSCALL_TABLE(mount, mount, 5160, 5)
#endif
#ifndef OVERRIDE_TABLE_64_futex
TRACE_SYSCALL_TABLE(futex, futex, 5194, 6)
#endif
#ifndef OVERRIDE_TABLE_64_sched_setaffinity
TRACE_SYSCALL_TABLE(sched_setaffinity, sched_setaffinity, 5195, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sched_getaffinity
TRACE_SYSCALL_TABLE(sched_getaffinity, sched_getaffinity, 5196, 3)
#endif
#ifndef OVERRIDE_TABLE_64_io_setup
TRACE_SYSCALL_TABLE(io_setup, io_setup, 5200, 2)
#endif
#ifndef OVERRIDE_TABLE_64_io_getevents
TRACE_SYSCALL_TABLE(io_getevents, io_getevents, 5202, 5)
#endif
#ifndef OVERRIDE_TABLE_64_io_submit
TRACE_SYSCALL_TABLE(io_submit, io_submit, 5203, 3)
#endif
#ifndef OVERRIDE_TABLE_64_semtimedop
TRACE_SYSCALL_TABLE(semtimedop, semtimedop, 5214, 4)
#endif
#ifndef OVERRIDE_TABLE_64_timer_create
TRACE_SYSCALL_TABLE(timer_create, timer_create, 5216, 3)
#endif
#ifndef OVERRIDE_TABLE_64_timer_settime
TRACE_SYSCALL_TABLE(timer_settime, timer_settime, 5217, 4)
#endif
#ifndef OVERRIDE_TABLE_64_timer_gettime
TRACE_SYSCALL_TABLE(timer_gettime, timer_gettime, 5218, 2)
#endif
#ifndef OVERRIDE_TABLE_64_clock_settime
TRACE_SYSCALL_TABLE(clock_settime, clock_settime, 5221, 2)
#endif
#ifndef OVERRIDE_TABLE_64_clock_gettime
TRACE_SYSCALL_TABLE(clock_gettime, clock_gettime, 5222, 2)
#endif
#ifndef OVERRIDE_TABLE_64_clock_getres
TRACE_SYSCALL_TABLE(clock_getres, clock_getres, 5223, 2)
#endif
#ifndef OVERRIDE_TABLE_64_clock_nanosleep
TRACE_SYSCALL_TABLE(clock_nanosleep, clock_nanosleep, 5224, 4)
#endif
#ifndef OVERRIDE_TABLE_64_utimes
TRACE_SYSCALL_TABLE(utimes, utimes, 5226, 2)
#endif
#ifndef OVERRIDE_TABLE_64_waitid
TRACE_SYSCALL_TABLE(waitid, waitid, 5237, 5)
#endif
#ifndef OVERRIDE_TABLE_64_futimesat
TRACE_SYSCALL_TABLE(futimesat, futimesat, 5251, 3)
#endif
#ifndef OVERRIDE_TABLE_64_pselect6
TRACE_SYSCALL_TABLE(pselect6, pselect6, 5260, 6)
#endif
#ifndef OVERRIDE_TABLE_64_ppoll
TRACE_SYSCALL_TABLE(ppoll, ppoll, 5261, 5)
#endif
#ifndef OVERRIDE_TABLE_64_vmsplice
TRACE_SYSCALL_TABLE(vmsplice, vmsplice, 5266, 4)
#endif
#ifndef OVERRIDE_TABLE_64_set_robust_list
TRACE_SYSCALL_TABLE(set_robust_list, set_robust_list, 5268, 2)
#endif
#ifndef OVERRIDE_TABLE_64_get_robust_list
TRACE_SYSCALL_TABLE(get_robust_list, get_robust_list, 5269, 3)
#endif
#ifndef OVERRIDE_TABLE_64_epoll_pwait
TRACE_SYSCALL_TABLE(epoll_pwait, epoll_pwait, 5272, 6)
#endif
#ifndef OVERRIDE_TABLE_64_utimensat
TRACE_SYSCALL_TABLE(utimensat, utimensat, 5275, 4)
#endif
#ifndef OVERRIDE_TABLE_64_signalfd
TRACE_SYSCALL_TABLE(signalfd, signalfd, 5276, 3)
#endif
#ifndef OVERRIDE_TABLE_64_timerfd_gettime
TRACE_SYSCALL_TABLE(timerfd_gettime, timerfd_gettime, 5281, 2)
#endif
#ifndef OVERRIDE_TABLE_64_timerfd_settime
TRACE_SYSCALL_TABLE(timerfd_settime, timerfd_settime, 5282, 4)
#endif
#ifndef OVERRIDE_TABLE_64_rt_tgsigqueueinfo
TRACE_SYSCALL_TABLE(rt_tgsigqueueinfo, rt_tgsigqueueinfo, 5291, 4)
#endif
#ifndef OVERRIDE_TABLE_64_recvmmsg
TRACE_SYSCALL_TABLE(recvmmsg, recvmmsg, 5294, 5)
#endif
#ifndef OVERRIDE_TABLE_64_clock_adjtime
TRACE_SYSCALL_TABLE(clock_adjtime, clock_adjtime, 5300, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sendmmsg
TRACE_SYSCALL_TABLE(sendmmsg, sendmmsg, 5302, 4)
#endif
#ifndef OVERRIDE_TABLE_64_process_vm_readv
TRACE_SYSCALL_TABLE(process_vm_readv, process_vm_readv, 5304, 6)
#endif
#ifndef OVERRIDE_TABLE_64_process_vm_writev
TRACE_SYSCALL_TABLE(process_vm_writev, process_vm_writev, 5305, 6)
#endif
#ifndef OVERRIDE_TABLE_64_read
TRACE_SYSCALL_TABLE(read, read, 6000, 3)
#endif
#ifndef OVERRIDE_TABLE_64_write
TRACE_SYSCALL_TABLE(write, write, 6001, 3)
#endif
#ifndef OVERRIDE_TABLE_64_open
TRACE_SYSCALL_TABLE(open, open, 6002, 3)
#endif
#ifndef OVERRIDE_TABLE_64_newstat
TRACE_SYSCALL_TABLE(newstat, newstat, 6004, 2)
#endif
#ifndef OVERRIDE_TABLE_64_newfstat
TRACE_SYSCALL_TABLE(newfstat, newfstat, 6005, 2)
#endif
#ifndef OVERRIDE_TABLE_64_newlstat
TRACE_SYSCALL_TABLE(newlstat, newlstat, 6006, 2)
#endif
#ifndef OVERRIDE_TABLE_64_poll
TRACE_SYSCALL_TABLE(poll, poll, 6007, 3)
#endif
#ifndef OVERRIDE_TABLE_64_32_rt_sigaction
TRACE_SYSCALL_TABLE(32_rt_sigaction, 32_rt_sigaction, 6013, 4)
#endif
#ifndef OVERRIDE_TABLE_64_32_rt_sigprocmask
TRACE_SYSCALL_TABLE(32_rt_sigprocmask, 32_rt_sigprocmask, 6014, 4)
#endif
#ifndef OVERRIDE_TABLE_64_access
TRACE_SYSCALL_TABLE(access, access, 6020, 2)
#endif
#ifndef OVERRIDE_TABLE_64_mincore
TRACE_SYSCALL_TABLE(mincore, mincore, 6026, 3)
#endif
#ifndef OVERRIDE_TABLE_64_shmat
TRACE_SYSCALL_TABLE(shmat, shmat, 6029, 3)
#endif
#ifndef OVERRIDE_TABLE_64_32_sendfile
TRACE_SYSCALL_TABLE(32_sendfile, 32_sendfile, 6039, 4)
#endif
#ifndef OVERRIDE_TABLE_64_connect
TRACE_SYSCALL_TABLE(connect, connect, 6041, 3)
#endif
#ifndef OVERRIDE_TABLE_64_accept
TRACE_SYSCALL_TABLE(accept, accept, 6042, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sendto
TRACE_SYSCALL_TABLE(sendto, sendto, 6043, 6)
#endif
#ifndef OVERRIDE_TABLE_64_bind
TRACE_SYSCALL_TABLE(bind, bind, 6048, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getsockname
TRACE_SYSCALL_TABLE(getsockname, getsockname, 6050, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getpeername
TRACE_SYSCALL_TABLE(getpeername, getpeername, 6051, 3)
#endif
#ifndef OVERRIDE_TABLE_64_socketpair
TRACE_SYSCALL_TABLE(socketpair, socketpair, 6052, 4)
#endif
#ifndef OVERRIDE_TABLE_64_getsockopt
TRACE_SYSCALL_TABLE(getsockopt, getsockopt, 6054, 5)
#endif
#ifndef OVERRIDE_TABLE_64_newuname
TRACE_SYSCALL_TABLE(newuname, newuname, 6061, 1)
#endif
#ifndef OVERRIDE_TABLE_64_semop
TRACE_SYSCALL_TABLE(semop, semop, 6063, 3)
#endif
#ifndef OVERRIDE_TABLE_64_shmdt
TRACE_SYSCALL_TABLE(shmdt, shmdt, 6065, 1)
#endif
#ifndef OVERRIDE_TABLE_64_truncate
TRACE_SYSCALL_TABLE(truncate, truncate, 6074, 2)
#endif
#ifndef OVERRIDE_TABLE_64_getcwd
TRACE_SYSCALL_TABLE(getcwd, getcwd, 6077, 2)
#endif
#ifndef OVERRIDE_TABLE_64_chdir
TRACE_SYSCALL_TABLE(chdir, chdir, 6078, 1)
#endif
#ifndef OVERRIDE_TABLE_64_rename
TRACE_SYSCALL_TABLE(rename, rename, 6080, 2)
#endif
#ifndef OVERRIDE_TABLE_64_mkdir
TRACE_SYSCALL_TABLE(mkdir, mkdir, 6081, 2)
#endif
#ifndef OVERRIDE_TABLE_64_rmdir
TRACE_SYSCALL_TABLE(rmdir, rmdir, 6082, 1)
#endif
#ifndef OVERRIDE_TABLE_64_creat
TRACE_SYSCALL_TABLE(creat, creat, 6083, 2)
#endif
#ifndef OVERRIDE_TABLE_64_link
TRACE_SYSCALL_TABLE(link, link, 6084, 2)
#endif
#ifndef OVERRIDE_TABLE_64_unlink
TRACE_SYSCALL_TABLE(unlink, unlink, 6085, 1)
#endif
#ifndef OVERRIDE_TABLE_64_symlink
TRACE_SYSCALL_TABLE(symlink, symlink, 6086, 2)
#endif
#ifndef OVERRIDE_TABLE_64_readlink
TRACE_SYSCALL_TABLE(readlink, readlink, 6087, 3)
#endif
#ifndef OVERRIDE_TABLE_64_chmod
TRACE_SYSCALL_TABLE(chmod, chmod, 6088, 2)
#endif
#ifndef OVERRIDE_TABLE_64_chown
TRACE_SYSCALL_TABLE(chown, chown, 6090, 3)
#endif
#ifndef OVERRIDE_TABLE_64_lchown
TRACE_SYSCALL_TABLE(lchown, lchown, 6092, 3)
#endif
#ifndef OVERRIDE_TABLE_64_syslog
TRACE_SYSCALL_TABLE(syslog, syslog, 6101, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getgroups
TRACE_SYSCALL_TABLE(getgroups, getgroups, 6113, 2)
#endif
#ifndef OVERRIDE_TABLE_64_setgroups
TRACE_SYSCALL_TABLE(setgroups, setgroups, 6114, 2)
#endif
#ifndef OVERRIDE_TABLE_64_getresuid
TRACE_SYSCALL_TABLE(getresuid, getresuid, 6116, 3)
#endif
#ifndef OVERRIDE_TABLE_64_getresgid
TRACE_SYSCALL_TABLE(getresgid, getresgid, 6118, 3)
#endif
#ifndef OVERRIDE_TABLE_64_32_rt_sigpending
TRACE_SYSCALL_TABLE(32_rt_sigpending, 32_rt_sigpending, 6125, 2)
#endif
#ifndef OVERRIDE_TABLE_64_32_rt_sigqueueinfo
TRACE_SYSCALL_TABLE(32_rt_sigqueueinfo, 32_rt_sigqueueinfo, 6127, 3)
#endif
#ifndef OVERRIDE_TABLE_64_mknod
TRACE_SYSCALL_TABLE(mknod, mknod, 6131, 3)
#endif
#ifndef OVERRIDE_TABLE_64_sched_setparam
TRACE_SYSCALL_TABLE(sched_setparam, sched_setparam, 6139, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sched_getparam
TRACE_SYSCALL_TABLE(sched_getparam, sched_getparam, 6140, 2)
#endif
#ifndef OVERRIDE_TABLE_64_sched_setscheduler
TRACE_SYSCALL_TABLE(sched_setscheduler, sched_setscheduler, 6141, 3)
#endif
#ifndef OVERRIDE_TABLE_64_32_sched_rr_get_interval
TRACE_SYSCALL_TABLE(32_sched_rr_get_interval, 32_sched_rr_get_interval, 6145, 2)
#endif
#ifndef OVERRIDE_TABLE_64_pivot_root
TRACE_SYSCALL_TABLE(pivot_root, pivot_root, 6151, 2)
#endif
#ifndef OVERRIDE_TABLE_64_chroot
TRACE_SYSCALL_TABLE(chroot, chroot, 6156, 1)
#endif
#ifndef OVERRIDE_TABLE_64_umount
TRACE_SYSCALL_TABLE(umount, umount, 6161, 2)
#endif
#ifndef OVERRIDE_TABLE_64_swapon
TRACE_SYSCALL_TABLE(swapon, swapon, 6162, 2)
#endif
#ifndef OVERRIDE_TABLE_64_swapoff
TRACE_SYSCALL_TABLE(swapoff, swapoff, 6163, 1)
#endif
#ifndef OVERRIDE_TABLE_64_reboot
TRACE_SYSCALL_TABLE(reboot, reboot, 6164, 4)
#endif
#ifndef OVERRIDE_TABLE_64_sethostname
TRACE_SYSCALL_TABLE(sethostname, sethostname, 6165, 2)
#endif
#ifndef OVERRIDE_TABLE_64_setdomainname
TRACE_SYSCALL_TABLE(setdomainname, setdomainname, 6166, 2)
#endif
#ifndef OVERRIDE_TABLE_64_init_module
TRACE_SYSCALL_TABLE(init_module, init_module, 6168, 3)
#endif
#ifndef OVERRIDE_TABLE_64_delete_module
TRACE_SYSCALL_TABLE(delete_module, delete_module, 6169, 2)
#endif
#ifndef OVERRIDE_TABLE_64_quotactl
TRACE_SYSCALL_TABLE(quotactl, quotactl, 6172, 4)
#endif
#ifndef OVERRIDE_TABLE_64_setxattr
TRACE_SYSCALL_TABLE(setxattr, setxattr, 6180, 5)
#endif
#ifndef OVERRIDE_TABLE_64_lsetxattr
TRACE_SYSCALL_TABLE(lsetxattr, lsetxattr, 6181, 5)
#endif
#ifndef OVERRIDE_TABLE_64_fsetxattr
TRACE_SYSCALL_TABLE(fsetxattr, fsetxattr, 6182, 5)
#endif
#ifndef OVERRIDE_TABLE_64_getxattr
TRACE_SYSCALL_TABLE(getxattr, getxattr, 6183, 4)
#endif
#ifndef OVERRIDE_TABLE_64_lgetxattr
TRACE_SYSCALL_TABLE(lgetxattr, lgetxattr, 6184, 4)
#endif
#ifndef OVERRIDE_TABLE_64_fgetxattr
TRACE_SYSCALL_TABLE(fgetxattr, fgetxattr, 6185, 4)
#endif
#ifndef OVERRIDE_TABLE_64_listxattr
TRACE_SYSCALL_TABLE(listxattr, listxattr, 6186, 3)
#endif
#ifndef OVERRIDE_TABLE_64_llistxattr
TRACE_SYSCALL_TABLE(llistxattr, llistxattr, 6187, 3)
#endif
#ifndef OVERRIDE_TABLE_64_flistxattr
TRACE_SYSCALL_TABLE(flistxattr, flistxattr, 6188, 3)
#endif
#ifndef OVERRIDE_TABLE_64_removexattr
TRACE_SYSCALL_TABLE(removexattr, removexattr, 6189, 2)
#endif
#ifndef OVERRIDE_TABLE_64_lremovexattr
TRACE_SYSCALL_TABLE(lremovexattr, lremovexattr, 6190, 2)
#endif
#ifndef OVERRIDE_TABLE_64_fremovexattr
TRACE_SYSCALL_TABLE(fremovexattr, fremovexattr, 6191, 2)
#endif
#ifndef OVERRIDE_TABLE_64_32_futex
TRACE_SYSCALL_TABLE(32_futex, 32_futex, 6194, 6)
#endif
#ifndef OVERRIDE_TABLE_64_cachectl
TRACE_SYSCALL_TABLE(cachectl, cachectl, 6198, 3)
#endif
#ifndef OVERRIDE_TABLE_64_io_cancel
TRACE_SYSCALL_TABLE(io_cancel, io_cancel, 6204, 3)
#endif
#ifndef OVERRIDE_TABLE_64_epoll_ctl
TRACE_SYSCALL_TABLE(epoll_ctl, epoll_ctl, 6208, 4)
#endif
#ifndef OVERRIDE_TABLE_64_epoll_wait
TRACE_SYSCALL_TABLE(epoll_wait, epoll_wait, 6209, 4)
#endif
#ifndef OVERRIDE_TABLE_64_set_tid_address
TRACE_SYSCALL_TABLE(set_tid_address, set_tid_address, 6213, 1)
#endif
#ifndef OVERRIDE_TABLE_64_sendfile64
TRACE_SYSCALL_TABLE(sendfile64, sendfile64, 6219, 4)
#endif
#ifndef OVERRIDE_TABLE_64_inotify_add_watch
TRACE_SYSCALL_TABLE(inotify_add_watch, inotify_add_watch, 6248, 3)
#endif
#ifndef OVERRIDE_TABLE_64_openat
TRACE_SYSCALL_TABLE(openat, openat, 6251, 4)
#endif
#ifndef OVERRIDE_TABLE_64_mkdirat
TRACE_SYSCALL_TABLE(mkdirat, mkdirat, 6252, 3)
#endif
#ifndef OVERRIDE_TABLE_64_mknodat
TRACE_SYSCALL_TABLE(mknodat, mknodat, 6253, 4)
#endif
#ifndef OVERRIDE_TABLE_64_fchownat
TRACE_SYSCALL_TABLE(fchownat, fchownat, 6254, 5)
#endif
#ifndef OVERRIDE_TABLE_64_newfstatat
TRACE_SYSCALL_TABLE(newfstatat, newfstatat, 6256, 4)
#endif
#ifndef OVERRIDE_TABLE_64_unlinkat
TRACE_SYSCALL_TABLE(unlinkat, unlinkat, 6257, 3)
#endif
#ifndef OVERRIDE_TABLE_64_renameat
TRACE_SYSCALL_TABLE(renameat, renameat, 6258, 4)
#endif
#ifndef OVERRIDE_TABLE_64_linkat
TRACE_SYSCALL_TABLE(linkat, linkat, 6259, 5)
#endif
#ifndef OVERRIDE_TABLE_64_symlinkat
TRACE_SYSCALL_TABLE(symlinkat, symlinkat, 6260, 3)
#endif
#ifndef OVERRIDE_TABLE_64_readlinkat
TRACE_SYSCALL_TABLE(readlinkat, readlinkat, 6261, 4)
#endif
#ifndef OVERRIDE_TABLE_64_fchmodat
TRACE_SYSCALL_TABLE(fchmodat, fchmodat, 6262, 3)
#endif
#ifndef OVERRIDE_TABLE_64_faccessat
TRACE_SYSCALL_TABLE(faccessat, faccessat, 6263, 3)
#endif
#ifndef OVERRIDE_TABLE_64_splice
TRACE_SYSCALL_TABLE(splice, splice, 6267, 6)
#endif
#ifndef OVERRIDE_TABLE_64_getcpu
TRACE_SYSCALL_TABLE(getcpu, getcpu, 6275, 3)
#endif
#ifndef OVERRIDE_TABLE_64_signalfd4
TRACE_SYSCALL_TABLE(signalfd4, signalfd4, 6287, 4)
#endif
#ifndef OVERRIDE_TABLE_64_pipe2
TRACE_SYSCALL_TABLE(pipe2, pipe2, 6291, 2)
#endif
#ifndef OVERRIDE_TABLE_64_preadv
TRACE_SYSCALL_TABLE(preadv, preadv, 6293, 5)
#endif
#ifndef OVERRIDE_TABLE_64_pwritev
TRACE_SYSCALL_TABLE(pwritev, pwritev, 6294, 5)
#endif
#ifndef OVERRIDE_TABLE_64_accept4
TRACE_SYSCALL_TABLE(accept4, accept4, 6297, 4)
#endif
#ifndef OVERRIDE_TABLE_64_getdents64
TRACE_SYSCALL_TABLE(getdents64, getdents64, 6299, 3)
#endif
#ifndef OVERRIDE_TABLE_64_prlimit64
TRACE_SYSCALL_TABLE(prlimit64, prlimit64, 6302, 4)
#endif

#endif /* CREATE_SYSCALL_TABLE */
