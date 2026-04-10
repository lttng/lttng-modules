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

#else	/* CREATE_SYSCALL_TABLE */

#endif /* CREATE_SYSCALL_TABLE */
