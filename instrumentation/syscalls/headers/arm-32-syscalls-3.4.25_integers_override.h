

#define OVERRIDE_TABLE_32_arm_fadvise64_64
#define OVERRIDE_TABLE_32_sync_file_range2

#ifndef CREATE_SYSCALL_TABLE

SC_LTTNG_TRACEPOINT_EVENT(arm_fadvise64_64,
	TP_PROTO(int fd, int advice, loff_t offset, loff_t len),
	TP_ARGS(fd, advice, offset, len),
	TP_STRUCT__entry(
		__field_hex(int, fd)
		__field_hex(int, advice)
		__field_hex(loff_t, offset)
		__field_hex(loff_t, len)),
	TP_fast_assign(
		tp_assign(fd, fd)
		tp_assign(advice, advice)
		tp_assign(offset, offset)
		tp_assign(len, len)),
	TP_printk()
)

SC_LTTNG_TRACEPOINT_EVENT(sync_file_range2,
	TP_PROTO(int fd, loff_t offset, loff_t nbytes, unsigned int flags),
	TP_ARGS(fd, offset, nbytes, flags),
	TP_STRUCT__entry(
		__field_hex(int, fd)
		__field_hex(loff_t, offset)
		__field_hex(loff_t, nbytes)
		__field_hex(unsigned int, flags)),
	TP_fast_assign(
		tp_assign(fd, fd)
		tp_assign(offset, offset)
		tp_assign(nbytes, nbytes)
		tp_assign(flags, flags)),
	TP_printk()
)

#else	/* CREATE_SYSCALL_TABLE */

#define OVERRIDE_TABLE_32_mmap
TRACE_SYSCALL_TABLE(mmap, mmap, 90, 6)

#define OVERRIDE_TABLE_32_arm_fadvise64_64
TRACE_SYSCALL_TABLE(arm_fadvise64_64, arm_fadvise64_64, 270, 4)
#define OVERRIDE_TABLE_32_sync_file_range2
TRACE_SYSCALL_TABLE(sync_file_range2, sync_file_range2, 341, 4)

#endif /* CREATE_SYSCALL_TABLE */


