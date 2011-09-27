#ifndef CREATE_SYSCALL_TABLE

#define OVERRIDE_sys_open
TRACE_EVENT(sys_open,
	TP_PROTO(const char * filename, int flags, int mode),
	TP_ARGS(filename, flags, mode),
	TP_STRUCT__entry(__string(filename, filename) __field(int, flags) __field(int, mode)),
	TP_fast_assign(tp_copy_string_from_user(filename, filename) tp_assign(flags, flags) tp_assign(mode, mode)),
	TP_printk()
)

#else	/* CREATE_SYSCALL_TABLE */

TRACE_SYSCALL_TABLE(sys_open, sys_open, 2, 3)

#endif /* CREATE_SYSCALL_TABLE */
