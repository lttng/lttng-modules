#define OVERRIDE_32_execve
#define OVERRIDE_64_execve

#ifndef CREATE_SYSCALL_TABLE

SC_LTTNG_TRACEPOINT_EVENT(execve,
	TP_PROTO(sc_exit(long ret,) const char *filename, char *const *argv, char *const *envp),
	TP_ARGS(sc_exit(ret,) filename, argv, envp),
	TP_STRUCT__entry(sc_exit(__field(long, ret))
		sc_in(__string_from_user(filename, filename))
		sc_in(__field_hex(char *const *, argv))
		sc_in(__field_hex(char *const *, envp))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))
		sc_in(tp_copy_string_from_user(filename, filename))
		sc_in(tp_assign(argv, argv))
		sc_in(tp_assign(envp, envp))),
	TP_printk()
)

SC_LTTNG_TRACEPOINT_EVENT(clone,
	TP_PROTO(sc_exit(long ret,) unsigned long clone_flags, unsigned long newsp,
		void __user *parent_tid,
		void __user *child_tid),
	TP_ARGS(sc_exit(ret,) clone_flags, newsp, parent_tid, child_tid),
	TP_STRUCT__entry(
		sc_exit(__field(long, ret))
		sc_in(__field_hex(unsigned long, clone_flags))
		sc_in(__field_hex(unsigned long, newsp))
		sc_in(__field_hex(void *, parent_tid))
		sc_in(__field_hex(void *, child_tid))),
	TP_fast_assign(
		sc_exit(tp_assign(ret, ret))
		sc_in(tp_assign(clone_flags, clone_flags))
		sc_in(tp_assign(newsp, newsp))
		sc_in(tp_assign(parent_tid, parent_tid))
		sc_in(tp_assign(child_tid, child_tid))),
	TP_printk()
)

/* present in 32, missing in 64 due to old kernel headers */
#define OVERRIDE_32_getcpu
#define OVERRIDE_64_getcpu
SC_LTTNG_TRACEPOINT_EVENT(getcpu,
	TP_PROTO(sc_exit(long ret,) unsigned __user *cpup, unsigned __user *nodep, void *tcache),
	TP_ARGS(sc_exit(ret,) cpup, nodep, tcache),
	TP_STRUCT__entry(
		sc_exit(__field(long, ret))
		sc_out(__field_hex(unsigned *, cpup))
		sc_out(__field_hex(unsigned *, nodep))
		sc_inout(__field_hex(void *, tcache))),
	TP_fast_assign(
		sc_exit(tp_assign(ret, ret))
		sc_out(tp_assign(cpup, cpup))
		sc_out(tp_assign(nodep, nodep))
		sc_inout(tp_assign(tcache, tcache))),
	TP_printk()
)

#define OVERRIDE_32_pipe
#define OVERRIDE_64_pipe
SC_LTTNG_TRACEPOINT_EVENT(pipe,
	TP_PROTO(sc_exit(long ret,) int * fildes),
	TP_ARGS(sc_exit(ret,) fildes),
	TP_STRUCT__entry(sc_exit(__field(long, ret))
		sc_out(__array(int, fildes, 2))
	),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))
		sc_out(tp_memcpy_from_user(fildes, fildes, 2))
	),
	TP_printk()
)

#define OVERRIDE_32_pipe2
#define OVERRIDE_64_pipe2
SC_LTTNG_TRACEPOINT_EVENT(pipe2,
	TP_PROTO(sc_exit(long ret,) int * fildes, int flags),
	TP_ARGS(sc_exit(ret,) fildes, flags),
	TP_STRUCT__entry(sc_exit(__field(long, ret))
		sc_out(__array(int, fildes, 2))
		sc_in(__field(int, flags))
	),
	TP_fast_assign(sc_exit(tp_assign(ret, ret))
		sc_out(tp_memcpy_from_user(fildes, fildes, 2))
		sc_in(tp_assign(flags, flags))
	),
	TP_printk()
)

#endif /* CREATE_SYSCALL_TABLE */
