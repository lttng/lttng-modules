#define OVERRIDE_TABLE_32_mmap2

#ifndef CREATE_SYSCALL_TABLE

SC_LTTNG_TRACEPOINT_EVENT(mmap2,
	TP_PROTO(void *addr, size_t len, int prot,
                 int flags, int fd, off_t pgoff),
	TP_ARGS(addr, len, prot, flags, fd, pgoff),
	TP_STRUCT__entry(
		__field_hex(void *, addr)
		__field(size_t, len)
		__field(int, prot)
		__field(int, flags)
		__field(int, fd)
		__field(off_t, pgoff)),
	TP_fast_assign(
		tp_assign(addr, addr)
		tp_assign(len, len)
		tp_assign(prot, prot)
		tp_assign(flags, flags)
		tp_assign(fd, fd)
		tp_assign(pgoff, pgoff)),
	TP_printk()
)

#else	/* CREATE_SYSCALL_TABLE */

#define OVERRIDE_TABLE_32_execve
TRACE_SYSCALL_TABLE(execve, execve, 11, 3)
#define OVERRIDE_TABLE_32_clone
TRACE_SYSCALL_TABLE(clone, clone, 120, 5)
#define OVERRIDE_TABLE_32_mmap2
TRACE_SYSCALL_TABLE(mmap2, mmap2, 192, 6)

#endif /* CREATE_SYSCALL_TABLE */

