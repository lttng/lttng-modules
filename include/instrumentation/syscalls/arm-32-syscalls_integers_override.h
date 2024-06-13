// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

#if (__BYTE_ORDER == __LITTLE_ENDIAN)

#define OVERRIDE_32_arm_fadvise64_64
SC_LTTNG_TRACEPOINT_EVENT(arm_fadvise64_64,
	TP_PROTO(sc_exit(long ret,) int fd, int advice,
		int32_t offset_low, int32_t offset_high,
		int32_t len_low, int32_t len_high),
	TP_ARGS(sc_exit(ret,) fd, advice, offset_low, offset_high,
		len_low, len_high),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(int, advice, advice))
		sc_in(ctf_integer(loff_t, offset,
			((loff_t) offset_high << 32) | offset_low))
		sc_in(ctf_integer(loff_t, len,
			((loff_t) len_high << 32) | len_low))
	)
)

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

#define OVERRIDE_32_arm_fadvise64_64
SC_LTTNG_TRACEPOINT_EVENT(arm_fadvise64_64,
	TP_PROTO(sc_exit(long ret,) int fd, int advice,
		int32_t offset_high, int32_t offset_low,
		int32_t len_high, int32_t len_low),
	TP_ARGS(sc_exit(ret,) fd, advice, offset_high, offset_low,
		len_high, len_low),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(int, advice, advice))
		sc_in(ctf_integer(loff_t, offset,
			((loff_t) offset_high << 32) | offset_low))
		sc_in(ctf_integer(loff_t, len,
			((loff_t) len_high << 32) | len_low))
	)
)

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

/*
 * Rename 'mmap_pgoff' to 'mmap2', while 'mmap_pgoff' is kernel internal,
 * 'mmap2' is exposed in unistd.h.
 */
#define OVERRIDE_TABLE_32_mmap_pgoff
#define OVERRIDE_TABLE_32_mmap2
TRACE_SYSCALL_TABLE(mmap2, mmap2, 192, 6)
#define OVERRIDE_TABLE_32_arm_fadvise64_64
TRACE_SYSCALL_TABLE(arm_fadvise64_64, arm_fadvise64_64, 270, 6)
#define OVERRIDE_TABLE_32_sync_file_range2
TRACE_SYSCALL_TABLE(sync_file_range2, sync_file_range2, 341, 6)

#endif /* CREATE_SYSCALL_TABLE */


