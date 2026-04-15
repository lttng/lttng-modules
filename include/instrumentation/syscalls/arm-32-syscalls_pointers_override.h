// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

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
