// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

/*
 * Override 'pipe' to set the output field 'fildes' to an array of 2 integers
 * instead of the default integer pointer.
 */
#define OVERRIDE_32_pipe
SC_LTTNG_TRACEPOINT_EVENT(pipe,
	TP_PROTO(sc_exit(long ret,) int * fildes),
	TP_ARGS(sc_exit(ret,) fildes),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_out(ctf_user_array(int, fildes, fildes, 2))
	)
)

#else	/* CREATE_SYSCALL_TABLE */

/*
 * The 'offset' argument is a 64bit loff_t split into 2 32bit registers and 1
 * unused register for alignment (args 4 -> 6).
 */
#define OVERRIDE_TABLE_32_pread64
TRACE_SYSCALL_TABLE(pread64, pread64, 180, 6)

/*
 * The 'offset' argument is a 64bit loff_t split into 2 32bit registers and 1
 * unused register for alignment (args 4 -> 6).
 */
#define OVERRIDE_TABLE_32_pwrite64
TRACE_SYSCALL_TABLE(pwrite64, pwrite64, 181, 6)

/*
 * The 'length' argument is a 64bit loff_t split into 2 32bit registers and 1
 * unused register for alignment (args 2 -> 4).
 */
#define OVERRIDE_TABLE_32_truncate64
TRACE_SYSCALL_TABLE(truncate64, truncate64, 193, 4)

#endif /* CREATE_SYSCALL_TABLE */
