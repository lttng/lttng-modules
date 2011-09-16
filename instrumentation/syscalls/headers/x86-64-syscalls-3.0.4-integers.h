/* THIS FILE IS AUTO-GENERATED. DO NOT EDIT */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM syscalls

#if !defined(_TRACE_SYSCALLS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SYSCALLS_H

#include <linux/tracepoint.h>
#include <linux/syscalls.h>

TRACE_EVENT(sys_close,
	TP_PROTO(unsigned int fd),
	TP_ARGS(fd),
	TP_STRUCT__entry(__field(unsigned int, fd)),
	TP_fast_assign(tp_assign(fd, fd)),
	TP_printk()
)
TRACE_EVENT(sys_brk,
	TP_PROTO(unsigned long brk),
	TP_ARGS(brk),
	TP_STRUCT__entry(__field(unsigned long, brk)),
	TP_fast_assign(tp_assign(brk, brk)),
	TP_printk()
)
TRACE_EVENT(sys_dup,
	TP_PROTO(unsigned int fildes),
	TP_ARGS(fildes),
	TP_STRUCT__entry(__field(unsigned int, fildes)),
	TP_fast_assign(tp_assign(fildes, fildes)),
	TP_printk()
)
TRACE_EVENT(sys_alarm,
	TP_PROTO(unsigned int seconds),
	TP_ARGS(seconds),
	TP_STRUCT__entry(__field(unsigned int, seconds)),
	TP_fast_assign(tp_assign(seconds, seconds)),
	TP_printk()
)
TRACE_EVENT(sys_exit,
	TP_PROTO(int error_code),
	TP_ARGS(error_code),
	TP_STRUCT__entry(__field(int, error_code)),
	TP_fast_assign(tp_assign(error_code, error_code)),
	TP_printk()
)
TRACE_EVENT(sys_fsync,
	TP_PROTO(unsigned int fd),
	TP_ARGS(fd),
	TP_STRUCT__entry(__field(unsigned int, fd)),
	TP_fast_assign(tp_assign(fd, fd)),
	TP_printk()
)
TRACE_EVENT(sys_fdatasync,
	TP_PROTO(unsigned int fd),
	TP_ARGS(fd),
	TP_STRUCT__entry(__field(unsigned int, fd)),
	TP_fast_assign(tp_assign(fd, fd)),
	TP_printk()
)
TRACE_EVENT(sys_fchdir,
	TP_PROTO(unsigned int fd),
	TP_ARGS(fd),
	TP_STRUCT__entry(__field(unsigned int, fd)),
	TP_fast_assign(tp_assign(fd, fd)),
	TP_printk()
)
TRACE_EVENT(sys_umask,
	TP_PROTO(int mask),
	TP_ARGS(mask),
	TP_STRUCT__entry(__field(int, mask)),
	TP_fast_assign(tp_assign(mask, mask)),
	TP_printk()
)
TRACE_EVENT(sys_setuid,
	TP_PROTO(uid_t uid),
	TP_ARGS(uid),
	TP_STRUCT__entry(__field(uid_t, uid)),
	TP_fast_assign(tp_assign(uid, uid)),
	TP_printk()
)
TRACE_EVENT(sys_setgid,
	TP_PROTO(gid_t gid),
	TP_ARGS(gid),
	TP_STRUCT__entry(__field(gid_t, gid)),
	TP_fast_assign(tp_assign(gid, gid)),
	TP_printk()
)
TRACE_EVENT(sys_getpgid,
	TP_PROTO(pid_t pid),
	TP_ARGS(pid),
	TP_STRUCT__entry(__field(pid_t, pid)),
	TP_fast_assign(tp_assign(pid, pid)),
	TP_printk()
)
TRACE_EVENT(sys_setfsuid,
	TP_PROTO(uid_t uid),
	TP_ARGS(uid),
	TP_STRUCT__entry(__field(uid_t, uid)),
	TP_fast_assign(tp_assign(uid, uid)),
	TP_printk()
)
TRACE_EVENT(sys_setfsgid,
	TP_PROTO(gid_t gid),
	TP_ARGS(gid),
	TP_STRUCT__entry(__field(gid_t, gid)),
	TP_fast_assign(tp_assign(gid, gid)),
	TP_printk()
)
TRACE_EVENT(sys_getsid,
	TP_PROTO(pid_t pid),
	TP_ARGS(pid),
	TP_STRUCT__entry(__field(pid_t, pid)),
	TP_fast_assign(tp_assign(pid, pid)),
	TP_printk()
)
TRACE_EVENT(sys_personality,
	TP_PROTO(unsigned int personality),
	TP_ARGS(personality),
	TP_STRUCT__entry(__field(unsigned int, personality)),
	TP_fast_assign(tp_assign(personality, personality)),
	TP_printk()
)
TRACE_EVENT(sys_sched_getscheduler,
	TP_PROTO(pid_t pid),
	TP_ARGS(pid),
	TP_STRUCT__entry(__field(pid_t, pid)),
	TP_fast_assign(tp_assign(pid, pid)),
	TP_printk()
)
TRACE_EVENT(sys_sched_get_priority_max,
	TP_PROTO(int policy),
	TP_ARGS(policy),
	TP_STRUCT__entry(__field(int, policy)),
	TP_fast_assign(tp_assign(policy, policy)),
	TP_printk()
)
TRACE_EVENT(sys_sched_get_priority_min,
	TP_PROTO(int policy),
	TP_ARGS(policy),
	TP_STRUCT__entry(__field(int, policy)),
	TP_fast_assign(tp_assign(policy, policy)),
	TP_printk()
)
TRACE_EVENT(sys_mlockall,
	TP_PROTO(int flags),
	TP_ARGS(flags),
	TP_STRUCT__entry(__field(int, flags)),
	TP_fast_assign(tp_assign(flags, flags)),
	TP_printk()
)
TRACE_EVENT(sys_io_destroy,
	TP_PROTO(aio_context_t ctx),
	TP_ARGS(ctx),
	TP_STRUCT__entry(__field(aio_context_t, ctx)),
	TP_fast_assign(tp_assign(ctx, ctx)),
	TP_printk()
)
TRACE_EVENT(sys_epoll_create,
	TP_PROTO(int size),
	TP_ARGS(size),
	TP_STRUCT__entry(__field(int, size)),
	TP_fast_assign(tp_assign(size, size)),
	TP_printk()
)
TRACE_EVENT(sys_timer_getoverrun,
	TP_PROTO(timer_t timer_id),
	TP_ARGS(timer_id),
	TP_STRUCT__entry(__field(timer_t, timer_id)),
	TP_fast_assign(tp_assign(timer_id, timer_id)),
	TP_printk()
)
TRACE_EVENT(sys_timer_delete,
	TP_PROTO(timer_t timer_id),
	TP_ARGS(timer_id),
	TP_STRUCT__entry(__field(timer_t, timer_id)),
	TP_fast_assign(tp_assign(timer_id, timer_id)),
	TP_printk()
)
TRACE_EVENT(sys_exit_group,
	TP_PROTO(int error_code),
	TP_ARGS(error_code),
	TP_STRUCT__entry(__field(int, error_code)),
	TP_fast_assign(tp_assign(error_code, error_code)),
	TP_printk()
)
TRACE_EVENT(sys_unshare,
	TP_PROTO(unsigned long unshare_flags),
	TP_ARGS(unshare_flags),
	TP_STRUCT__entry(__field(unsigned long, unshare_flags)),
	TP_fast_assign(tp_assign(unshare_flags, unshare_flags)),
	TP_printk()
)
TRACE_EVENT(sys_eventfd,
	TP_PROTO(unsigned int count),
	TP_ARGS(count),
	TP_STRUCT__entry(__field(unsigned int, count)),
	TP_fast_assign(tp_assign(count, count)),
	TP_printk()
)
TRACE_EVENT(sys_epoll_create1,
	TP_PROTO(int flags),
	TP_ARGS(flags),
	TP_STRUCT__entry(__field(int, flags)),
	TP_fast_assign(tp_assign(flags, flags)),
	TP_printk()
)
TRACE_EVENT(sys_inotify_init1,
	TP_PROTO(int flags),
	TP_ARGS(flags),
	TP_STRUCT__entry(__field(int, flags)),
	TP_fast_assign(tp_assign(flags, flags)),
	TP_printk()
)
TRACE_EVENT(sys_syncfs,
	TP_PROTO(int fd),
	TP_ARGS(fd),
	TP_STRUCT__entry(__field(int, fd)),
	TP_fast_assign(tp_assign(fd, fd)),
	TP_printk()
)
TRACE_EVENT(sys_munmap,
	TP_PROTO(unsigned long addr, size_t len),
	TP_ARGS(addr, len),
	TP_STRUCT__entry(__field(unsigned long, addr) __field(size_t, len)),
	TP_fast_assign(tp_assign(addr, addr) tp_assign(len, len)),
	TP_printk()
)
TRACE_EVENT(sys_dup2,
	TP_PROTO(unsigned int oldfd, unsigned int newfd),
	TP_ARGS(oldfd, newfd),
	TP_STRUCT__entry(__field(unsigned int, oldfd) __field(unsigned int, newfd)),
	TP_fast_assign(tp_assign(oldfd, oldfd) tp_assign(newfd, newfd)),
	TP_printk()
)
TRACE_EVENT(sys_shutdown,
	TP_PROTO(int fd, int how),
	TP_ARGS(fd, how),
	TP_STRUCT__entry(__field(int, fd) __field(int, how)),
	TP_fast_assign(tp_assign(fd, fd) tp_assign(how, how)),
	TP_printk()
)
TRACE_EVENT(sys_listen,
	TP_PROTO(int fd, int backlog),
	TP_ARGS(fd, backlog),
	TP_STRUCT__entry(__field(int, fd) __field(int, backlog)),
	TP_fast_assign(tp_assign(fd, fd) tp_assign(backlog, backlog)),
	TP_printk()
)
TRACE_EVENT(sys_kill,
	TP_PROTO(pid_t pid, int sig),
	TP_ARGS(pid, sig),
	TP_STRUCT__entry(__field(pid_t, pid) __field(int, sig)),
	TP_fast_assign(tp_assign(pid, pid) tp_assign(sig, sig)),
	TP_printk()
)
TRACE_EVENT(sys_msgget,
	TP_PROTO(key_t key, int msgflg),
	TP_ARGS(key, msgflg),
	TP_STRUCT__entry(__field(key_t, key) __field(int, msgflg)),
	TP_fast_assign(tp_assign(key, key) tp_assign(msgflg, msgflg)),
	TP_printk()
)
TRACE_EVENT(sys_flock,
	TP_PROTO(unsigned int fd, unsigned int cmd),
	TP_ARGS(fd, cmd),
	TP_STRUCT__entry(__field(unsigned int, fd) __field(unsigned int, cmd)),
	TP_fast_assign(tp_assign(fd, fd) tp_assign(cmd, cmd)),
	TP_printk()
)
TRACE_EVENT(sys_ftruncate,
	TP_PROTO(unsigned int fd, unsigned long length),
	TP_ARGS(fd, length),
	TP_STRUCT__entry(__field(unsigned int, fd) __field(unsigned long, length)),
	TP_fast_assign(tp_assign(fd, fd) tp_assign(length, length)),
	TP_printk()
)
TRACE_EVENT(sys_fchmod,
	TP_PROTO(unsigned int fd, mode_t mode),
	TP_ARGS(fd, mode),
	TP_STRUCT__entry(__field(unsigned int, fd) __field(mode_t, mode)),
	TP_fast_assign(tp_assign(fd, fd) tp_assign(mode, mode)),
	TP_printk()
)
TRACE_EVENT(sys_setpgid,
	TP_PROTO(pid_t pid, pid_t pgid),
	TP_ARGS(pid, pgid),
	TP_STRUCT__entry(__field(pid_t, pid) __field(pid_t, pgid)),
	TP_fast_assign(tp_assign(pid, pid) tp_assign(pgid, pgid)),
	TP_printk()
)
TRACE_EVENT(sys_setreuid,
	TP_PROTO(uid_t ruid, uid_t euid),
	TP_ARGS(ruid, euid),
	TP_STRUCT__entry(__field(uid_t, ruid) __field(uid_t, euid)),
	TP_fast_assign(tp_assign(ruid, ruid) tp_assign(euid, euid)),
	TP_printk()
)
TRACE_EVENT(sys_setregid,
	TP_PROTO(gid_t rgid, gid_t egid),
	TP_ARGS(rgid, egid),
	TP_STRUCT__entry(__field(gid_t, rgid) __field(gid_t, egid)),
	TP_fast_assign(tp_assign(rgid, rgid) tp_assign(egid, egid)),
	TP_printk()
)
TRACE_EVENT(sys_getpriority,
	TP_PROTO(int which, int who),
	TP_ARGS(which, who),
	TP_STRUCT__entry(__field(int, which) __field(int, who)),
	TP_fast_assign(tp_assign(which, which) tp_assign(who, who)),
	TP_printk()
)
TRACE_EVENT(sys_mlock,
	TP_PROTO(unsigned long start, size_t len),
	TP_ARGS(start, len),
	TP_STRUCT__entry(__field(unsigned long, start) __field(size_t, len)),
	TP_fast_assign(tp_assign(start, start) tp_assign(len, len)),
	TP_printk()
)
TRACE_EVENT(sys_munlock,
	TP_PROTO(unsigned long start, size_t len),
	TP_ARGS(start, len),
	TP_STRUCT__entry(__field(unsigned long, start) __field(size_t, len)),
	TP_fast_assign(tp_assign(start, start) tp_assign(len, len)),
	TP_printk()
)
TRACE_EVENT(sys_tkill,
	TP_PROTO(pid_t pid, int sig),
	TP_ARGS(pid, sig),
	TP_STRUCT__entry(__field(pid_t, pid) __field(int, sig)),
	TP_fast_assign(tp_assign(pid, pid) tp_assign(sig, sig)),
	TP_printk()
)
TRACE_EVENT(sys_ioprio_get,
	TP_PROTO(int which, int who),
	TP_ARGS(which, who),
	TP_STRUCT__entry(__field(int, which) __field(int, who)),
	TP_fast_assign(tp_assign(which, which) tp_assign(who, who)),
	TP_printk()
)
TRACE_EVENT(sys_inotify_rm_watch,
	TP_PROTO(int fd, __s32 wd),
	TP_ARGS(fd, wd),
	TP_STRUCT__entry(__field(int, fd) __field(__s32, wd)),
	TP_fast_assign(tp_assign(fd, fd) tp_assign(wd, wd)),
	TP_printk()
)
TRACE_EVENT(sys_timerfd_create,
	TP_PROTO(int clockid, int flags),
	TP_ARGS(clockid, flags),
	TP_STRUCT__entry(__field(int, clockid) __field(int, flags)),
	TP_fast_assign(tp_assign(clockid, clockid) tp_assign(flags, flags)),
	TP_printk()
)
TRACE_EVENT(sys_eventfd2,
	TP_PROTO(unsigned int count, int flags),
	TP_ARGS(count, flags),
	TP_STRUCT__entry(__field(unsigned int, count) __field(int, flags)),
	TP_fast_assign(tp_assign(count, count) tp_assign(flags, flags)),
	TP_printk()
)
TRACE_EVENT(sys_setns,
	TP_PROTO(int fd, int nstype),
	TP_ARGS(fd, nstype),
	TP_STRUCT__entry(__field(int, fd) __field(int, nstype)),
	TP_fast_assign(tp_assign(fd, fd) tp_assign(nstype, nstype)),
	TP_printk()
)
TRACE_EVENT(sys_lseek,
	TP_PROTO(unsigned int fd, off_t offset, unsigned int origin),
	TP_ARGS(fd, offset, origin),
	TP_STRUCT__entry(__field(unsigned int, fd) __field(off_t, offset) __field(unsigned int, origin)),
	TP_fast_assign(tp_assign(fd, fd) tp_assign(offset, offset) tp_assign(origin, origin)),
	TP_printk()
)
TRACE_EVENT(sys_mprotect,
	TP_PROTO(unsigned long start, size_t len, unsigned long prot),
	TP_ARGS(start, len, prot),
	TP_STRUCT__entry(__field(unsigned long, start) __field(size_t, len) __field(unsigned long, prot)),
	TP_fast_assign(tp_assign(start, start) tp_assign(len, len) tp_assign(prot, prot)),
	TP_printk()
)
TRACE_EVENT(sys_ioctl,
	TP_PROTO(unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(fd, cmd, arg),
	TP_STRUCT__entry(__field(unsigned int, fd) __field(unsigned int, cmd) __field(unsigned long, arg)),
	TP_fast_assign(tp_assign(fd, fd) tp_assign(cmd, cmd) tp_assign(arg, arg)),
	TP_printk()
)
TRACE_EVENT(sys_msync,
	TP_PROTO(unsigned long start, size_t len, int flags),
	TP_ARGS(start, len, flags),
	TP_STRUCT__entry(__field(unsigned long, start) __field(size_t, len) __field(int, flags)),
	TP_fast_assign(tp_assign(start, start) tp_assign(len, len) tp_assign(flags, flags)),
	TP_printk()
)
TRACE_EVENT(sys_madvise,
	TP_PROTO(unsigned long start, size_t len_in, int behavior),
	TP_ARGS(start, len_in, behavior),
	TP_STRUCT__entry(__field(unsigned long, start) __field(size_t, len_in) __field(int, behavior)),
	TP_fast_assign(tp_assign(start, start) tp_assign(len_in, len_in) tp_assign(behavior, behavior)),
	TP_printk()
)
TRACE_EVENT(sys_shmget,
	TP_PROTO(key_t key, size_t size, int shmflg),
	TP_ARGS(key, size, shmflg),
	TP_STRUCT__entry(__field(key_t, key) __field(size_t, size) __field(int, shmflg)),
	TP_fast_assign(tp_assign(key, key) tp_assign(size, size) tp_assign(shmflg, shmflg)),
	TP_printk()
)
TRACE_EVENT(sys_socket,
	TP_PROTO(int family, int type, int protocol),
	TP_ARGS(family, type, protocol),
	TP_STRUCT__entry(__field(int, family) __field(int, type) __field(int, protocol)),
	TP_fast_assign(tp_assign(family, family) tp_assign(type, type) tp_assign(protocol, protocol)),
	TP_printk()
)
TRACE_EVENT(sys_semget,
	TP_PROTO(key_t key, int nsems, int semflg),
	TP_ARGS(key, nsems, semflg),
	TP_STRUCT__entry(__field(key_t, key) __field(int, nsems) __field(int, semflg)),
	TP_fast_assign(tp_assign(key, key) tp_assign(nsems, nsems) tp_assign(semflg, semflg)),
	TP_printk()
)
TRACE_EVENT(sys_fcntl,
	TP_PROTO(unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(fd, cmd, arg),
	TP_STRUCT__entry(__field(unsigned int, fd) __field(unsigned int, cmd) __field(unsigned long, arg)),
	TP_fast_assign(tp_assign(fd, fd) tp_assign(cmd, cmd) tp_assign(arg, arg)),
	TP_printk()
)
TRACE_EVENT(sys_fchown,
	TP_PROTO(unsigned int fd, uid_t user, gid_t group),
	TP_ARGS(fd, user, group),
	TP_STRUCT__entry(__field(unsigned int, fd) __field(uid_t, user) __field(gid_t, group)),
	TP_fast_assign(tp_assign(fd, fd) tp_assign(user, user) tp_assign(group, group)),
	TP_printk()
)
TRACE_EVENT(sys_setresuid,
	TP_PROTO(uid_t ruid, uid_t euid, uid_t suid),
	TP_ARGS(ruid, euid, suid),
	TP_STRUCT__entry(__field(uid_t, ruid) __field(uid_t, euid) __field(uid_t, suid)),
	TP_fast_assign(tp_assign(ruid, ruid) tp_assign(euid, euid) tp_assign(suid, suid)),
	TP_printk()
)
TRACE_EVENT(sys_setresgid,
	TP_PROTO(gid_t rgid, gid_t egid, gid_t sgid),
	TP_ARGS(rgid, egid, sgid),
	TP_STRUCT__entry(__field(gid_t, rgid) __field(gid_t, egid) __field(gid_t, sgid)),
	TP_fast_assign(tp_assign(rgid, rgid) tp_assign(egid, egid) tp_assign(sgid, sgid)),
	TP_printk()
)
TRACE_EVENT(sys_sysfs,
	TP_PROTO(int option, unsigned long arg1, unsigned long arg2),
	TP_ARGS(option, arg1, arg2),
	TP_STRUCT__entry(__field(int, option) __field(unsigned long, arg1) __field(unsigned long, arg2)),
	TP_fast_assign(tp_assign(option, option) tp_assign(arg1, arg1) tp_assign(arg2, arg2)),
	TP_printk()
)
TRACE_EVENT(sys_setpriority,
	TP_PROTO(int which, int who, int niceval),
	TP_ARGS(which, who, niceval),
	TP_STRUCT__entry(__field(int, which) __field(int, who) __field(int, niceval)),
	TP_fast_assign(tp_assign(which, which) tp_assign(who, who) tp_assign(niceval, niceval)),
	TP_printk()
)
TRACE_EVENT(sys_tgkill,
	TP_PROTO(pid_t tgid, pid_t pid, int sig),
	TP_ARGS(tgid, pid, sig),
	TP_STRUCT__entry(__field(pid_t, tgid) __field(pid_t, pid) __field(int, sig)),
	TP_fast_assign(tp_assign(tgid, tgid) tp_assign(pid, pid) tp_assign(sig, sig)),
	TP_printk()
)
TRACE_EVENT(sys_ioprio_set,
	TP_PROTO(int which, int who, int ioprio),
	TP_ARGS(which, who, ioprio),
	TP_STRUCT__entry(__field(int, which) __field(int, who) __field(int, ioprio)),
	TP_fast_assign(tp_assign(which, which) tp_assign(who, who) tp_assign(ioprio, ioprio)),
	TP_printk()
)
TRACE_EVENT(sys_dup3,
	TP_PROTO(unsigned int oldfd, unsigned int newfd, int flags),
	TP_ARGS(oldfd, newfd, flags),
	TP_STRUCT__entry(__field(unsigned int, oldfd) __field(unsigned int, newfd) __field(int, flags)),
	TP_fast_assign(tp_assign(oldfd, oldfd) tp_assign(newfd, newfd) tp_assign(flags, flags)),
	TP_printk()
)
TRACE_EVENT(sys_ptrace,
	TP_PROTO(long request, long pid, unsigned long ptrace0, unsigned long ptrace1),
	TP_ARGS(request, pid, ptrace0, ptrace1),
	TP_STRUCT__entry(__field(long, request) __field(long, pid) __field(unsigned long, ptrace0) __field(unsigned long, ptrace1)),
	TP_fast_assign(tp_assign(request, request) tp_assign(pid, pid) tp_assign(ptrace0, ptrace0) tp_assign(ptrace1, ptrace1)),
	TP_printk()
)
TRACE_EVENT(sys_tee,
	TP_PROTO(int fdin, int fdout, size_t tee0, unsigned int tee1),
	TP_ARGS(fdin, fdout, tee0, tee1),
	TP_STRUCT__entry(__field(int, fdin) __field(int, fdout) __field(size_t, tee0) __field(unsigned int, tee1)),
	TP_fast_assign(tp_assign(fdin, fdin) tp_assign(fdout, fdout) tp_assign(tee0, tee0) tp_assign(tee1, tee1)),
	TP_printk()
)
TRACE_EVENT(sys_mremap,
	TP_PROTO(unsigned long addr, unsigned long mremap0, unsigned long mremap1, unsigned long mremap2, unsigned long mremap3),
	TP_ARGS(addr, mremap0, mremap1, mremap2, mremap3),
	TP_STRUCT__entry(__field(unsigned long, addr) __field(unsigned long, mremap0) __field(unsigned long, mremap1) __field(unsigned long, mremap2) __field(unsigned long, mremap3)),
	TP_fast_assign(tp_assign(addr, addr) tp_assign(mremap0, mremap0) tp_assign(mremap1, mremap1) tp_assign(mremap2, mremap2) tp_assign(mremap3, mremap3)),
	TP_printk()
)
TRACE_EVENT(sys_prctl,
	TP_PROTO(int option, unsigned long prctl0, unsigned long prctl1, unsigned long prctl2, unsigned long prctl3),
	TP_ARGS(option, prctl0, prctl1, prctl2, prctl3),
	TP_STRUCT__entry(__field(int, option) __field(unsigned long, prctl0) __field(unsigned long, prctl1) __field(unsigned long, prctl2) __field(unsigned long, prctl3)),
	TP_fast_assign(tp_assign(option, option) tp_assign(prctl0, prctl0) tp_assign(prctl1, prctl1) tp_assign(prctl2, prctl2) tp_assign(prctl3, prctl3)),
	TP_printk()
)
TRACE_EVENT(sys_remap_file_pages,
	TP_PROTO(unsigned long start, unsigned long remap_file_pages0, unsigned long remap_file_pages1, unsigned long remap_file_pages2, unsigned long remap_file_pages3),
	TP_ARGS(start, remap_file_pages0, remap_file_pages1, remap_file_pages2, remap_file_pages3),
	TP_STRUCT__entry(__field(unsigned long, start) __field(unsigned long, remap_file_pages0) __field(unsigned long, remap_file_pages1) __field(unsigned long, remap_file_pages2) __field(unsigned long, remap_file_pages3)),
	TP_fast_assign(tp_assign(start, start) tp_assign(remap_file_pages0, remap_file_pages0) tp_assign(remap_file_pages1, remap_file_pages1) tp_assign(remap_file_pages2, remap_file_pages2) tp_assign(remap_file_pages3, remap_file_pages3)),
	TP_printk()
)
TRACE_EVENT(sys_mmap,
	TP_PROTO(unsigned long mmap0, unsigned long mmap1, unsigned long mmap2, unsigned long mmap3, unsigned long mmap4, unsigned long mmap5),
	TP_ARGS(mmap0, mmap1, mmap2, mmap3, mmap4, mmap5),
	TP_STRUCT__entry(__field(unsigned long, mmap0) __field(unsigned long, mmap1) __field(unsigned long, mmap2) __field(unsigned long, mmap3) __field(unsigned long, mmap4) __field(unsigned long, mmap5)),
	TP_fast_assign(tp_assign(mmap0, mmap0) tp_assign(mmap1, mmap1) tp_assign(mmap2, 12) tp_assign(mmap3, mmap3) tp_assign(mmap4, mmap4) tp_assign(mmap5, mmap5)),
	TP_printk()
)
TRACE_SYSCALL_TABLE(sys_close, 3)
TRACE_SYSCALL_TABLE(sys_lseek, 8)
TRACE_SYSCALL_TABLE(sys_mmap, 9)
TRACE_SYSCALL_TABLE(sys_mprotect, 10)
TRACE_SYSCALL_TABLE(sys_munmap, 11)
TRACE_SYSCALL_TABLE(sys_brk, 12)
TRACE_SYSCALL_TABLE(sys_ioctl, 16)
TRACE_SYSCALL_TABLE(sys_sched_yield, 24)
TRACE_SYSCALL_TABLE(sys_mremap, 25)
TRACE_SYSCALL_TABLE(sys_msync, 26)
TRACE_SYSCALL_TABLE(sys_madvise, 28)
TRACE_SYSCALL_TABLE(sys_shmget, 29)
TRACE_SYSCALL_TABLE(sys_dup, 32)
TRACE_SYSCALL_TABLE(sys_dup2, 33)
TRACE_SYSCALL_TABLE(sys_pause, 34)
TRACE_SYSCALL_TABLE(sys_alarm, 37)
TRACE_SYSCALL_TABLE(sys_getpid, 39)
TRACE_SYSCALL_TABLE(sys_socket, 41)
TRACE_SYSCALL_TABLE(sys_shutdown, 48)
TRACE_SYSCALL_TABLE(sys_listen, 50)
TRACE_SYSCALL_TABLE(sys_exit, 60)
TRACE_SYSCALL_TABLE(sys_kill, 62)
TRACE_SYSCALL_TABLE(sys_semget, 64)
TRACE_SYSCALL_TABLE(sys_msgget, 68)
TRACE_SYSCALL_TABLE(sys_fcntl, 72)
TRACE_SYSCALL_TABLE(sys_flock, 73)
TRACE_SYSCALL_TABLE(sys_fsync, 74)
TRACE_SYSCALL_TABLE(sys_fdatasync, 75)
TRACE_SYSCALL_TABLE(sys_ftruncate, 77)
TRACE_SYSCALL_TABLE(sys_fchdir, 81)
TRACE_SYSCALL_TABLE(sys_fchmod, 91)
TRACE_SYSCALL_TABLE(sys_fchown, 93)
TRACE_SYSCALL_TABLE(sys_umask, 95)
TRACE_SYSCALL_TABLE(sys_ptrace, 101)
TRACE_SYSCALL_TABLE(sys_getuid, 102)
TRACE_SYSCALL_TABLE(sys_getgid, 104)
TRACE_SYSCALL_TABLE(sys_setuid, 105)
TRACE_SYSCALL_TABLE(sys_setgid, 106)
TRACE_SYSCALL_TABLE(sys_geteuid, 107)
TRACE_SYSCALL_TABLE(sys_getegid, 108)
TRACE_SYSCALL_TABLE(sys_setpgid, 109)
TRACE_SYSCALL_TABLE(sys_getppid, 110)
TRACE_SYSCALL_TABLE(sys_getpgrp, 111)
TRACE_SYSCALL_TABLE(sys_setsid, 112)
TRACE_SYSCALL_TABLE(sys_setreuid, 113)
TRACE_SYSCALL_TABLE(sys_setregid, 114)
TRACE_SYSCALL_TABLE(sys_setresuid, 117)
TRACE_SYSCALL_TABLE(sys_setresgid, 119)
TRACE_SYSCALL_TABLE(sys_getpgid, 121)
TRACE_SYSCALL_TABLE(sys_setfsuid, 122)
TRACE_SYSCALL_TABLE(sys_setfsgid, 123)
TRACE_SYSCALL_TABLE(sys_getsid, 124)
TRACE_SYSCALL_TABLE(sys_personality, 135)
TRACE_SYSCALL_TABLE(sys_sysfs, 139)
TRACE_SYSCALL_TABLE(sys_getpriority, 140)
TRACE_SYSCALL_TABLE(sys_setpriority, 141)
TRACE_SYSCALL_TABLE(sys_sched_getscheduler, 145)
TRACE_SYSCALL_TABLE(sys_sched_get_priority_max, 146)
TRACE_SYSCALL_TABLE(sys_sched_get_priority_min, 147)
TRACE_SYSCALL_TABLE(sys_mlock, 149)
TRACE_SYSCALL_TABLE(sys_munlock, 150)
TRACE_SYSCALL_TABLE(sys_mlockall, 151)
TRACE_SYSCALL_TABLE(sys_munlockall, 152)
TRACE_SYSCALL_TABLE(sys_vhangup, 153)
TRACE_SYSCALL_TABLE(sys_prctl, 157)
TRACE_SYSCALL_TABLE(sys_sync, 162)
TRACE_SYSCALL_TABLE(sys_gettid, 186)
TRACE_SYSCALL_TABLE(sys_tkill, 200)
TRACE_SYSCALL_TABLE(sys_io_destroy, 207)
TRACE_SYSCALL_TABLE(sys_epoll_create, 213)
TRACE_SYSCALL_TABLE(sys_remap_file_pages, 216)
TRACE_SYSCALL_TABLE(sys_restart_syscall, 219)
TRACE_SYSCALL_TABLE(sys_timer_getoverrun, 225)
TRACE_SYSCALL_TABLE(sys_timer_delete, 226)
TRACE_SYSCALL_TABLE(sys_exit_group, 231)
TRACE_SYSCALL_TABLE(sys_tgkill, 234)
TRACE_SYSCALL_TABLE(sys_ioprio_set, 251)
TRACE_SYSCALL_TABLE(sys_ioprio_get, 252)
TRACE_SYSCALL_TABLE(sys_inotify_init, 253)
TRACE_SYSCALL_TABLE(sys_inotify_rm_watch, 255)
TRACE_SYSCALL_TABLE(sys_unshare, 272)
TRACE_SYSCALL_TABLE(sys_tee, 276)
TRACE_SYSCALL_TABLE(sys_timerfd_create, 283)
TRACE_SYSCALL_TABLE(sys_eventfd, 284)
TRACE_SYSCALL_TABLE(sys_eventfd2, 290)
TRACE_SYSCALL_TABLE(sys_epoll_create1, 291)
TRACE_SYSCALL_TABLE(sys_dup3, 292)
TRACE_SYSCALL_TABLE(sys_inotify_init1, 294)
TRACE_SYSCALL_TABLE(sys_syncfs, 306)
TRACE_SYSCALL_TABLE(sys_setns, 308)

#endif /*  _TRACE_SYSCALLS_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
