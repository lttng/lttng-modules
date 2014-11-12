

#define OVERRIDE_TABLE_32_arm_fadvise64_64
#define OVERRIDE_TABLE_32_sync_file_range2

#ifndef CREATE_SYSCALL_TABLE

SC_LTTNG_TRACEPOINT_EVENT(arm_fadvise64_64,
	TP_PROTO(int fd, int advice, loff_t offset, loff_t len),
	TP_ARGS(fd, advice, offset, len),
	TP_FIELDS(
		ctf_integer_hex(int, fd, fd)
		ctf_integer_hex(int, advice, advice)
		ctf_integer_hex(loff_t, offset, offset)
		ctf_integer_hex(loff_t, len, len)
	)
)

SC_LTTNG_TRACEPOINT_EVENT(sync_file_range2,
	TP_PROTO(int fd, loff_t offset, loff_t nbytes, unsigned int flags),
	TP_ARGS(fd, offset, nbytes, flags),
	TP_FIELDS(
		ctf_integer_hex(int, fd, fd)
		ctf_integer_hex(loff_t, offset, offset)
		ctf_integer_hex(loff_t, nbytes, nbytes)
		ctf_integer_hex(unsigned int, flags), flags)
	)
)

#else	/* CREATE_SYSCALL_TABLE */

#define OVERRIDE_TABLE_32_mmap
TRACE_SYSCALL_TABLE(mmap, mmap, 90, 6)

#define OVERRIDE_TABLE_32_arm_fadvise64_64
TRACE_SYSCALL_TABLE(arm_fadvise64_64, arm_fadvise64_64, 270, 4)
#define OVERRIDE_TABLE_32_sync_file_range2
TRACE_SYSCALL_TABLE(sync_file_range2, sync_file_range2, 341, 4)

#endif /* CREATE_SYSCALL_TABLE */


