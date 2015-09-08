#ifndef CREATE_SYSCALL_TABLE

#define OVERRIDE_32_sched_setaffinity
SC_LTTNG_TRACEPOINT_EVENT(sched_setaffinity,
    TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned int len, unsigned long * user_mask_ptr),
    TP_ARGS(sc_exit(ret,) pid, len, user_mask_ptr),
    TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(unsigned int, len, len)) sc_in(ctf_integer(unsigned long *, user_mask_ptr, user_mask_ptr)))
)

#define OVERRIDE_32_sched_getaffinity
SC_LTTNG_TRACEPOINT_EVENT(sched_getaffinity,
    TP_PROTO(sc_exit(long ret,) pid_t pid, unsigned int len, unsigned long * user_mask_ptr),
    TP_ARGS(sc_exit(ret,) pid, len, user_mask_ptr),
    TP_FIELDS(sc_exit(ctf_integer(long, ret, ret)) sc_in(ctf_integer(pid_t, pid, pid)) sc_in(ctf_integer(unsigned int, len, len)) sc_out(ctf_integer(unsigned long *, user_mask_ptr, user_mask_ptr)))
)

#else	/* CREATE_SYSCALL_TABLE */

#define OVERRIDE_TABLE_32_clone
TRACE_SYSCALL_TABLE(clone, clone, 4120, 0)

#define OVERRIDE_TABLE_32_sys_sched_setaffinity
TRACE_SYSCALL_TABLE(sched_setaffinity, sched_setaffinity, 4239, 3)

#define OVERRIDE_TABLE_32_sys_sched_getaffinity
TRACE_SYSCALL_TABLE(sched_getaffinity, sched_getaffinity, 4240, 3)

#endif /* CREATE_SYSCALL_TABLE */
