/*
 * this is a place-holder for arm64 integer syscall definition override.
 */
#ifndef CREATE_SYSCALL_TABLE
#else	/* CREATE_SYSCALL_TABLE */
#define OVERRIDE_TABLE_64_mmap
TRACE_SYSCALL_TABLE(mmap, mmap, 222, 6)
#endif /* CREATE_SYSCALL_TABLE */
