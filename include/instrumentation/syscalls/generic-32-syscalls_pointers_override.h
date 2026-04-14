// SPDX-FileCopyrightText: 2026 Michael Jeanson <mjeanson@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#include "common-syscall-32-loff_t.h"
#include "common-syscall-poll.h"

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

/*
 * The 'pselect6_time32' syscall is only available on x86-32, ppc-32 and arm-32.
 */
#if defined(CONFIG_X86) || defined(CONFIG_PPC) || defined(CONFIG_ARM) || defined(CONFIG_ARM64)
/*
 * Instead of extracting the user-space pointers of the 3 fd_set,
 * extract the bitmask of the FDs in the sets (in, out, ex) in the form
 * of an array of uint8_t (1024 FDs is the limit in the kernel).
 */
#define OVERRIDE_32_pselect6_time32
SC_LTTNG_TRACEPOINT_EVENT_CODE(pselect6_time32,
	TP_PROTO(sc_exit(long ret,) int n, fd_set __user * inp, fd_set __user * outp,
		fd_set __user * exp, struct old_timespec32 __user * tvp, void __user * sig),
	TP_ARGS(sc_exit(ret,) n, inp, outp, exp, tvp, sig),
	TP_locvar(
		LTTNG_SYSCALL_SELECT_locvar
	),
	TP_code_pre(
		LTTNG_SYSCALL_SELECT_code_pre
	),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, n, n))
		sc_inout(ctf_integer(uint8_t, overflow, tp_locvar->overflow))
		sc_inout(ctf_integer(struct old_timespec32 *, tvp, tvp))
		sc_in(ctf_integer_hex(void *, sig, sig))

		sc_inout(
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
			LTTNG_SYSCALL_SELECT_fds_field_LE(readfds, inp)
			LTTNG_SYSCALL_SELECT_fds_field_LE(writefds, outp)
			LTTNG_SYSCALL_SELECT_fds_field_LE(exceptfds, exp)
#else
			LTTNG_SYSCALL_SELECT_fds_field_BE(readfds, inp)
			LTTNG_SYSCALL_SELECT_fds_field_BE(writefds, outp)
			LTTNG_SYSCALL_SELECT_fds_field_BE(exceptfds, exp)
#endif
		)
	),
	TP_code_post(
		LTTNG_SYSCALL_SELECT_code_post
	)
)
#endif /* defined(CONFIG_X86) || defined(CONFIG_PPC) || defined(CONFIG_ARM) || defined(CONFIG_ARM64) */

/*
 * The 'ppoll_time32' syscall is only available on x86-32, ppc-32 and arm-32.
 */
#if defined(CONFIG_X86) || defined(CONFIG_PPC) || defined(CONFIG_ARM) || defined(CONFIG_ARM64)
/*
 * Instead of printing the pointer address of the poll set, extract all the FDs
 * and flags from the poll set. For now, only output the standardized
 * set of events to limit the verbosity of the output, and also extract
 * the raw value. In the future, moving to CTF2 will allow hiding unset
 * fields and then allow extracting all the fields.
 */
#define OVERRIDE_32_ppoll_time32
SC_LTTNG_TRACEPOINT_EVENT_CODE(ppoll_time32,
	TP_PROTO(sc_exit(long ret,) struct pollfd __user * ufds,
		unsigned int nfds, struct old_timespec32 * tsp, const sigset_t * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) ufds, nfds, tsp, sigmask, sigsetsize),
	TP_locvar(
		LTTNG_SYSCALL_POLL_locvar
	),
	TP_code_pre(
		LTTNG_SYSCALL_POLL_code_pre
	),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(struct old_timespec32 *, tsp, tsp))
		sc_in(ctf_integer(const sigset_t *, sigmask, sigmask))
		sc_in(ctf_integer(size_t, sigsetsize, sigsetsize))
		sc_inout(ctf_integer(unsigned int, nfds, nfds))
		sc_inout(ctf_integer(unsigned int, fds_length, tp_locvar->fds_length))
		sc_inout(ctf_integer(uint8_t, overflow, tp_locvar->overflow))
		LTTNG_SYSCALL_POLL_fds_field
	),
	TP_code_post(
		LTTNG_SYSCALL_POLL_code_post
	)
)
#endif /* defined(CONFIG_X86) || defined(CONFIG_PPC) || defined(CONFIG_ARM) || defined(CONFIG_ARM64) */

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
