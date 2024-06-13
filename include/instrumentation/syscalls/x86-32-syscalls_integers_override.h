// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

#define OVERRIDE_32_sync_file_range
SC_LTTNG_TRACEPOINT_EVENT(sync_file_range,
	TP_PROTO(sc_exit(long ret,) int fd,
		int32_t offset_low, int32_t offset_high,
		int32_t nbytes_low, int32_t nbytes_high,
		unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd, offset_low, offset_high,
		nbytes_low, nbytes_high, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(loff_t, offset,
			((loff_t) offset_high << 32) | offset_low))
		sc_in(ctf_integer(loff_t, nbytes,
			((loff_t) nbytes_high << 32) | nbytes_low))
		sc_in(ctf_integer(unsigned int, flags, flags))
	)
)

#else /* #ifndef CREATE_SYSCALL_TABLE */

/*
 * Rename 'mmap_pgoff' to 'mmap2', while 'mmap_pgoff' is kernel internal,
 * 'mmap2' is exposed in unistd.h.
 */
#define OVERRIDE_TABLE_32_mmap_pgoff
#define OVERRIDE_TABLE_32_mmap2
TRACE_SYSCALL_TABLE(mmap2, mmap2, 192, 6)

#define OVERRIDE_TABLE_32_sync_file_range
TRACE_SYSCALL_TABLE(sync_file_range, sync_file_range, 314, 6)

#endif /* #else #ifndef CREATE_SYSCALL_TABLE */
