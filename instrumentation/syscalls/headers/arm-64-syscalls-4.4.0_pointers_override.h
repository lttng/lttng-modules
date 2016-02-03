/*
 * this is a place-holder for arm64 integer syscall definition override.
 */
#ifndef CREATE_SYSCALL_TABLE

#else
#define OVERRIDE_TABLE_64_clone
TRACE_SYSCALL_TABLE(clone, clone, 220, 5)
#endif
