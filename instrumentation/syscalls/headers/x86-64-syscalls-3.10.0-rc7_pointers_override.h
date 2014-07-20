#ifndef CREATE_SYSCALL_TABLE

#else	/* CREATE_SYSCALL_TABLE */

#define OVERRIDE_TABLE_64_clone
TRACE_SYSCALL_TABLE(clone, clone, 56, 5)
#define OVERRIDE_TABLE_64_execve
TRACE_SYSCALL_TABLE(execve, execve, 59, 3)
#define OVERRIDE_TABLE_64_getcpu
TRACE_SYSCALL_TABLE(getcpu, getcpu, 309, 3)

#endif /* CREATE_SYSCALL_TABLE */
