// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#include "common-syscall-mmap.h"

#ifndef CREATE_SYSCALL_TABLE

/*
 * PowerPC 32 is special, it's the only architecture to have both a 32-bit
 * 'mmap' and 'mmap2' with 6 arguments. The 'mmap2' override is handled in the
 * generic-32 header, add this architecture specific override for 'mmap',
 */
#define OVERRIDE_32_mmap
SC_LTTNG_TRACEPOINT_EVENT(mmap,
	TP_PROTO(sc_exit(unsigned long ret,)
		unsigned long addr, unsigned long len,
		unsigned long prot, unsigned long flags,
		unsigned long fd, unsigned long off),
	TP_ARGS(sc_exit(ret,) addr, len, prot, flags, fd, off),
	TP_FIELDS(sc_exit(ctf_integer_hex(unsigned long, ret, ret))
		sc_in(ctf_integer_hex(unsigned long, addr, addr))
		sc_in(ctf_integer(size_t, len, len))
#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM
		/*
		 * Use a custom field here so that tracer writes a single
		 * integer and the work of splitting it up in two fields is
		 * left to the trace reader.
		 */
		sc_in(ctf_enum(lttng_mmap_protection, int, prot, prot))
		sc_in(
			ctf_custom_field(
				ctf_custom_type(
					lttng_kernel_static_type_struct(2, LTTNG_MMAP_FLAGS, lttng_alignof(uint32_t) * CHAR_BIT)
				),
				flags,
				ctf_custom_code(
					ctf_integer_type(uint32_t, flags)
				)
			)
		)
#else
		sc_in(ctf_integer(int, prot, prot))
		sc_in(ctf_integer(int, flags, flags))
#endif /* CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM */
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(off_t, offset, off))
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

#define OVERRIDE_TABLE_32_sync_file_range2
TRACE_SYSCALL_TABLE(sync_file_range2, sync_file_range2, 308, 6)

#endif /* CREATE_SYSCALL_TABLE */

