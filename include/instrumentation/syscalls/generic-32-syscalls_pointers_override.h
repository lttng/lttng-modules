// SPDX-FileCopyrightText: 2026 Michael Jeanson <mjeanson@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#include "common-syscall-32-loff_t.h"

#ifndef CREATE_SYSCALL_TABLE

/*
 * Disable 16bit ownership syscalls when CONFIG_UID16 is unset.
 */
# ifndef CONFIG_UID16
#  define OVERRIDE_32_getgroups16
#  define OVERRIDE_32_setgroups16
#  define OVERRIDE_32_lchown16
#  define OVERRIDE_32_getresuid16
#  define OVERRIDE_32_getresgid16
#  define OVERRIDE_32_chown16
# endif

/*
 * truncate64: Combine the low/high parts of 'length' into a 64-bit field.
 */
#define OVERRIDE_32_truncate64
SC_LTTNG_TRACEPOINT_EVENT(truncate64,
	TP_PROTO(sc_exit(long ret,) const char * path, tp_proto_loff_t_pad(length)),
	TP_ARGS(sc_exit(ret,) path, tp_arg_loff_t_pad(length)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_user_string(path, path))
		sc_in(ctf_integer(loff_t, length, tp_field_loff_t(length))))
)

/*
 * pread64: Combine the low/high parts of 'offset' into a 64-bit field.
 */
#define OVERRIDE_32_pread64
SC_LTTNG_TRACEPOINT_EVENT(pread64,
	TP_PROTO(sc_exit(long ret,) int fd, char * buf, size_t count, tp_proto_loff_t_pad(offset)),
	TP_ARGS(sc_exit(ret,) fd, buf, count, tp_arg_loff_t_pad(offset)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_out(ctf_integer(char *, buf, buf))
		sc_in(ctf_integer(size_t, count, count))
		sc_in(ctf_integer(loff_t, offset, tp_field_loff_t(offset))))
)

/*
 * pwrite64: Combine the low/high parts of 'offset' into a 64-bit field.
 */
#define OVERRIDE_32_pwrite64
SC_LTTNG_TRACEPOINT_EVENT(pwrite64,
	TP_PROTO(sc_exit(long ret,) int fd, const char * buf, size_t count, tp_proto_loff_t_pad(offset)),
	TP_ARGS(sc_exit(ret,) fd, buf, count, tp_arg_loff_t_pad(offset)),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(const char *, buf, buf))
		sc_in(ctf_integer(size_t, count, count))
		sc_in(ctf_integer(loff_t, offset, tp_field_loff_t(offset))))
)

#else /* CREATE_SYSCALL_TABLE */

/*
 * Disable 16bit ownership syscalls when CONFIG_UID16 is unset.
 */
# ifndef CONFIG_UID16
#  define OVERRIDE_TABLE_32_getgroups16
#  define OVERRIDE_TABLE_32_setgroups16
#  define OVERRIDE_TABLE_32_lchown16
#  define OVERRIDE_TABLE_32_getresuid16
#  define OVERRIDE_TABLE_32_getresgid16
#  define OVERRIDE_TABLE_32_chown16
# endif

#endif /* CREATE_SYSCALL_TABLE */
