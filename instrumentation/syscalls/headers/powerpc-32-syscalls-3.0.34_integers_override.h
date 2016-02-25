#ifndef CREATE_SYSCALL_TABLE

#define OVERRIDE_32_mmap2
SC_LTTNG_TRACEPOINT_EVENT(mmap2,
	TP_PROTO(sc_exit(unsigned long ret,)
		unsigned long addr, unsigned long len,
		unsigned long prot, unsigned long flags,
		unsigned long fd, unsigned long pgoff),
	TP_ARGS(sc_exit(ret,) addr, len, prot, flags, fd, pgoff),
	TP_FIELDS(sc_exit(ctf_integer_hex(unsigned long, ret, ret))
		sc_in(ctf_integer_hex(unsigned long, addr, addr))
		sc_in(ctf_integer(size_t, len, len))
		sc_in(ctf_integer(int, prot, prot))
		sc_in(ctf_integer(int, flags, flags))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(off_t, pgoff, pgoff))
	)
)

#if (__BYTE_ORDER == __LITTLE_ENDIAN)

#define OVERRIDE_32_sync_file_range2
SC_LTTNG_TRACEPOINT_EVENT(sync_file_range2,
	TP_PROTO(sc_exit(long ret,) int fd,
		unsigned int flags,
		int32_t offset_low, int32_t offset_high,
		int32_t nbytes_low, int32_t nbytes_high),
	TP_ARGS(sc_exit(ret,) fd, flags, offset_low, offset_high,
		nbytes_low, nbytes_high),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(unsigned int, flags, flags))
		sc_in(ctf_integer(loff_t, offset,
			((loff_t) offset_high << 32) | offset_low))
		sc_in(ctf_integer(loff_t, nbytes,
			((loff_t) nbytes_high << 32) | nbytes_low))
	)
)

#else	/* __BIG_ENDIAN */

#define OVERRIDE_32_sync_file_range2
SC_LTTNG_TRACEPOINT_EVENT(sync_file_range2,
	TP_PROTO(sc_exit(long ret,) int fd,
		unsigned int flags,
		int32_t offset_high, int32_t offset_low,
		int32_t nbytes_high, int32_t nbytes_low),
	TP_ARGS(sc_exit(ret,) fd, flags, offset_high, offset_low,
		nbytes_high, nbytes_low),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(unsigned int, flags, flags))
		sc_in(ctf_integer(loff_t, offset,
			((loff_t) offset_high << 32) | offset_low))
		sc_in(ctf_integer(loff_t, nbytes,
			((loff_t) nbytes_high << 32) | nbytes_low))
	)
)

#endif

#else	/* CREATE_SYSCALL_TABLE */

#define OVERRIDE_TABLE_32_mmap
TRACE_SYSCALL_TABLE(mmap, mmap, 90, 6)
#define OVERRIDE_TABLE_32_mmap2
TRACE_SYSCALL_TABLE(mmap2, mmap2, 192, 6)
#define OVERRIDE_TABLE_32_sync_file_range2
TRACE_SYSCALL_TABLE(sync_file_range2, sync_file_range2, 308, 6)

#endif /* CREATE_SYSCALL_TABLE */

