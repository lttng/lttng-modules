// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM

/*
 * The 'fcntl' syscall is available on all architectures except riscv-32.
 */
#if !(defined(CONFIG_RISCV) && defined(CONFIG_32BIT))

/*
 * Override 'fcntl' to set the output field 'cmd' to a flag enumeration.
 */
#define OVERRIDE_32_fcntl
#define OVERRIDE_64_fcntl
SC_LTTNG_TRACEPOINT_EVENT(fcntl,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) fd, cmd, arg),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(unsigned int, fd, fd))
		sc_in(ctf_enum(lttng_fcntl_cmd_flags, unsigned int, cmd, cmd))
		sc_inout(ctf_integer(unsigned long, arg, arg)))
)
#endif

#endif /* CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM */

#endif /* CREATE_SYSCALL_TABLE */
