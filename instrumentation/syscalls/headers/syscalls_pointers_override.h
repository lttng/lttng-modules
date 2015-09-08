#define OVERRIDE_32_execve
#define OVERRIDE_64_execve

#ifndef CREATE_SYSCALL_TABLE

SC_LTTNG_TRACEPOINT_EVENT(execve,
	TP_PROTO(sc_exit(long ret,) const char *filename, char *const *argv, char *const *envp),
	TP_ARGS(sc_exit(ret,) filename, argv, envp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_user_string(filename, filename))
		sc_in(ctf_integer_hex(char *const *, argv, argv))
		sc_in(ctf_integer_hex(char *const *, envp, envp))
	)
)

SC_LTTNG_TRACEPOINT_EVENT(clone,
	TP_PROTO(sc_exit(long ret,) unsigned long clone_flags, unsigned long newsp,
		void __user *parent_tid,
		void __user *child_tid),
	TP_ARGS(sc_exit(ret,) clone_flags, newsp, parent_tid, child_tid),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer_hex(unsigned long, clone_flags, clone_flags))
		sc_in(ctf_integer_hex(unsigned long, newsp, newsp))
		sc_in(ctf_integer_hex(void *, parent_tid, parent_tid))
		sc_in(ctf_integer_hex(void *, child_tid, child_tid))
	)
)

/* present in 32, missing in 64 due to old kernel headers */
#define OVERRIDE_32_getcpu
#define OVERRIDE_64_getcpu
SC_LTTNG_TRACEPOINT_EVENT(getcpu,
	TP_PROTO(sc_exit(long ret,) unsigned __user *cpup, unsigned __user *nodep, void *tcache),
	TP_ARGS(sc_exit(ret,) cpup, nodep, tcache),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_out(ctf_integer_hex(unsigned *, cpup, cpup))
		sc_out(ctf_integer_hex(unsigned *, nodep, nodep))
		sc_inout(ctf_integer_hex(void *, tcache, tcache))
	)
)

#define OVERRIDE_32_pipe2
#define OVERRIDE_64_pipe2
SC_LTTNG_TRACEPOINT_EVENT(pipe2,
	TP_PROTO(sc_exit(long ret,) int * fildes, int flags),
	TP_ARGS(sc_exit(ret,) fildes, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_out(ctf_user_array(int, fildes, fildes, 2))
		sc_in(ctf_integer(int, flags, flags))
	)
)

#endif /* CREATE_SYSCALL_TABLE */
