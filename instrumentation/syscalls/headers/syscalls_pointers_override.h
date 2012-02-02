#define OVERRIDE_32_sys_execve
#define OVERRIDE_64_sys_execve

#ifndef CREATE_SYSCALL_TABLE

SC_TRACE_EVENT(sys_execve,
	TP_PROTO(const char *filename, char *const *argv, char *const *envp),
	TP_ARGS(filename, argv, envp),
	TP_STRUCT__entry(__string_from_user(filename, filename)
		__field_hex(char *const *, argv)
		__field_hex(char *const *, envp)),
	TP_fast_assign(tp_copy_string_from_user(filename, filename)
		tp_assign(argv, argv)
		tp_assign(envp, envp)),
	TP_printk()
)

#endif /* CREATE_SYSCALL_TABLE */
