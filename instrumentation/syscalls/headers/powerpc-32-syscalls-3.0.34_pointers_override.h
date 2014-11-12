#define OVERRIDE_TABLE_32_mmap2

#ifndef CREATE_SYSCALL_TABLE

SC_LTTNG_TRACEPOINT_EVENT(mmap2,
	TP_PROTO(void *addr, size_t len, int prot,
                 int flags, int fd, off_t pgoff),
	TP_ARGS(addr, len, prot, flags, fd, pgoff),
	TP_FIELDS(
		ctf_integer_hex(void *, addr, addr)
		ctf_integer(size_t, len, len)
		ctf_integer(int, prot, prot)
		ctf_integer(int, flags, flags)
		ctf_integer(int, fd, fd)
		ctf_integer(off_t, pgoff, pgoff)
	)
)

#else	/* CREATE_SYSCALL_TABLE */

#define OVERRIDE_TABLE_32_execve
TRACE_SYSCALL_TABLE(execve, execve, 11, 3)
#define OVERRIDE_TABLE_32_clone
TRACE_SYSCALL_TABLE(clone, clone, 120, 5)
#define OVERRIDE_TABLE_32_mmap2
TRACE_SYSCALL_TABLE(mmap2, mmap2, 192, 6)

#endif /* CREATE_SYSCALL_TABLE */

