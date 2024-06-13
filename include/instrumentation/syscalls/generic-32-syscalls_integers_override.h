// SPDX-FileCopyrightText: 2026 Michael Jeanson <mjeanson@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#include "common-syscall-mmap.h"

#ifndef CREATE_SYSCALL_TABLE

/*
 * Disable 16bit ownership syscalls when CONFIG_UID16 is unset.
 */
# ifndef CONFIG_UID16
#  define OVERRIDE_32_getuid16
#  define OVERRIDE_32_getgid16
#  define OVERRIDE_32_geteuid16
#  define OVERRIDE_32_getegid16
#  define OVERRIDE_32_setuid16
#  define OVERRIDE_32_setgid16
#  define OVERRIDE_32_setfsuid16
#  define OVERRIDE_32_setfsgid16
#  define OVERRIDE_32_setreuid16
#  define OVERRIDE_32_setregid16
#  define OVERRIDE_32_fchown16
#  define OVERRIDE_32_setresuid16
#  define OVERRIDE_32_setresgid16
# endif

/*
 * Most 32-bit architectures have 'mmap2', some have 'mmap_pgoff'. Only 'mmap2'
 * is exposed in unistd.h, normalize the event name to 'mmap2' across
 * architectures.
 */
#define OVERRIDE_32_mmap_pgoff
#define OVERRIDE_32_mmap2
SC_LTTNG_TRACEPOINT_EVENT(mmap2,
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

#else /* CREATE_SYSCALL_TABLE */

/*
 * Disable 16bit ownership syscalls when CONFIG_UID16 is unset.
 */
# ifndef CONFIG_UID16
#  define OVERRIDE_TABLE_32_getuid16
#  define OVERRIDE_TABLE_32_getgid16
#  define OVERRIDE_TABLE_32_geteuid16
#  define OVERRIDE_TABLE_32_getegid16
#  define OVERRIDE_TABLE_32_setuid16
#  define OVERRIDE_TABLE_32_setgid16
#  define OVERRIDE_TABLE_32_setreuid16
#  define OVERRIDE_TABLE_32_setregid16
#  define OVERRIDE_TABLE_32_fchown16
#  define OVERRIDE_TABLE_32_setfsuid16
#  define OVERRIDE_TABLE_32_setfsgid16
#  define OVERRIDE_TABLE_32_setresuid16
#  define OVERRIDE_TABLE_32_setresgid16
# endif

#endif /* CREATE_SYSCALL_TABLE */
