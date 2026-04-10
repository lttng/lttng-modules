// SPDX-FileCopyrightText: 2026 Michael Jeanson <mjeanson@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#include "common-syscall-32-loff_t.h"
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

/*
 * ftruncate64: Combine the low/high parts of 'length' into a 64-bit field.
 */
#define OVERRIDE_32_ftruncate64
SC_LTTNG_TRACEPOINT_EVENT(ftruncate64,
	TP_PROTO(sc_exit(long ret,) int fd, tp_proto_loff_t_pad(length)),
	TP_ARGS(sc_exit(ret,) fd, tp_arg_loff_t_pad(length)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(loff_t, length, tp_field_loff_t(length))))
)

/*
 * readahead: Combine the low/high parts of 'offset' into a 64-bit field.
 */
#define OVERRIDE_32_readahead
SC_LTTNG_TRACEPOINT_EVENT(readahead,
	TP_PROTO(sc_exit(long ret,) int fd, tp_proto_loff_t_pad(offset), size_t count),
	TP_ARGS(sc_exit(ret,) fd, tp_arg_loff_t_pad(offset), count),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(loff_t, offset, tp_field_loff_t(offset)))
		sc_in(ctf_integer(size_t, count, count)))
)

#if defined(CONFIG_PPC) || defined(CONFIG_ARM) || defined(CONFIG_ARM64)
/*
 * fadvise64_64: Combine the low/high parts of 'offset' and 'len' into 64-bit fields.
 *   This variant moves the 'advice' arg before 'offset' to align the 32bit register pairs.
 */
#define OVERRIDE_32_fadvise64_64
SC_LTTNG_TRACEPOINT_EVENT(fadvise64_64,
	TP_PROTO(sc_exit(long ret,) int fd,
		int advice,
		tp_proto_loff_t(offset),
		tp_proto_loff_t(len)),
	TP_ARGS(sc_exit(ret,) fd,
		advice,
		tp_arg_loff_t(offset),
		tp_arg_loff_t(len)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(int, advice, advice))
		sc_in(ctf_integer(loff_t, offset, tp_field_loff_t(offset)))
		sc_in(ctf_integer(loff_t, len, tp_field_loff_t(len))))
)
#else
/*
 * fadvise64_64: Combine the low/high parts of 'offset' and 'len' into 64-bit fields.
 */
#define OVERRIDE_32_fadvise64_64
SC_LTTNG_TRACEPOINT_EVENT(fadvise64_64,
	TP_PROTO(sc_exit(long ret,) int fd,
		tp_proto_loff_t(offset),
		tp_proto_loff_t(len),
		int advice),
	TP_ARGS(sc_exit(ret,) fd,
		tp_arg_loff_t(offset),
		tp_arg_loff_t(len),
		advice),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(int, advice, advice))
		sc_in(ctf_integer(loff_t, offset, tp_field_loff_t(offset)))
		sc_in(ctf_integer(loff_t, len, tp_field_loff_t(len))))
)
#endif

#if defined(CONFIG_PPC) || defined(CONFIG_ARM) || defined(CONFIG_ARM64)
/*
 * sync_file_range2: Combine the low/high parts of 'offset' and 'nbytes' into 64-bit fields.
 *   This variant moves the 'flags' arg before 'offset' to align the 32bit register pairs.
 */
#define OVERRIDE_32_sync_file_range2
SC_LTTNG_TRACEPOINT_EVENT(sync_file_range2,
	TP_PROTO(sc_exit(long ret,) int fd,
		unsigned int flags,
		tp_proto_loff_t(offset),
		tp_proto_loff_t(nbytes)),
	TP_ARGS(sc_exit(ret,) fd, flags,
		tp_arg_loff_t(offset),
		tp_arg_loff_t(nbytes)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(unsigned int, flags, flags))
		sc_in(ctf_integer(loff_t, offset, tp_field_loff_t(offset)))
		sc_in(ctf_integer(loff_t, nbytes, tp_field_loff_t(nbytes))))
)
#else
/*
 * sync_file_range: Combine the low/high parts of 'offset' and 'nbytes' into 64-bit fields.
 */
#define OVERRIDE_32_sync_file_range
SC_LTTNG_TRACEPOINT_EVENT(sync_file_range,
	TP_PROTO(sc_exit(long ret,) int fd,
		tp_proto_loff_t(offset),
		tp_proto_loff_t(nbytes),
		unsigned int flags),
	TP_ARGS(sc_exit(ret,) fd,
		tp_arg_loff_t(offset),
		tp_arg_loff_t(nbytes),
		flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(loff_t, offset, tp_field_loff_t(offset)))
		sc_in(ctf_integer(loff_t, nbytes, tp_field_loff_t(nbytes)))
		sc_in(ctf_integer(unsigned int, flags, flags))
	)
)
#endif

/*
 * Combine the low/high parts of 'offset' and 'len' into 64-bit fields.
 */
#define OVERRIDE_32_fallocate
SC_LTTNG_TRACEPOINT_EVENT(fallocate,
	TP_PROTO(sc_exit(long ret,) int fd, int mode,
		tp_proto_loff_t(offset), tp_proto_loff_t(len)),
	TP_ARGS(sc_exit(ret,) fd, mode,
		tp_arg_loff_t(offset),
		tp_arg_loff_t(len)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(int, mode, mode))
		sc_in(ctf_integer(loff_t, offset, tp_field_loff_t(offset)))
		sc_in(ctf_integer(loff_t, len, tp_field_loff_t(len))))
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
