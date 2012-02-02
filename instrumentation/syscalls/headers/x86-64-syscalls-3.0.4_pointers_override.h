#ifndef CREATE_SYSCALL_TABLE

#else	/* CREATE_SYSCALL_TABLE */

#define OVERRIDE_TABLE_64_sys_execve
TRACE_SYSCALL_TABLE(sys_execve, sys_execve, 59, 3)

#endif /* CREATE_SYSCALL_TABLE */
