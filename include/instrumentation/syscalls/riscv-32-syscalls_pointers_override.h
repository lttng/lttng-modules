// SPDX-FileCopyrightText: 2026 EfficiOS Inc.
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

#else /* CREATE_SYSCALL_TABLE */

/*
 * The 'length' argument is a 64bit loff_t split into 2 32bit registers (args 2 -> 3).
 */
#define OVERRIDE_TABLE_32_truncate64
TRACE_SYSCALL_TABLE(truncate64, truncate64, 45, 3)

/*
 * The 'offset' argument is a 64bit loff_t split into 2 32bit registers (args 4 -> 5).
 */
#define OVERRIDE_TABLE_32_pread64
TRACE_SYSCALL_TABLE(pread64, pread64, 67, 5)

/*
 * The 'offset' argument is a 64bit loff_t split into 2 32bit registers (args 4 -> 5).
 */
#define OVERRIDE_TABLE_32_pwrite64
TRACE_SYSCALL_TABLE(pwrite64, pwrite64, 68, 5)

#endif /* CREATE_SYSCALL_TABLE */
