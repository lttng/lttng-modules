// SPDX-FileCopyrightText: 2026 EfficiOS Inc.
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

#else /* CREATE_SYSCALL_TABLE */

/*
 * The 'length' argument is a 64bit loff_t split into 2 32bit registers (args 2 -> 3).
 */
#define OVERRIDE_TABLE_32_ftruncate64
TRACE_SYSCALL_TABLE(ftruncate64, ftruncate64, 46, 3)

/*
 * The 'offset' and 'nbytes' arguments are 64bit loff_t split into 2 32bit
 * registers (args 4 -> 6).
 */
#define OVERRIDE_TABLE_32_sync_file_range
TRACE_SYSCALL_TABLE(sync_file_range, sync_file_range, 84, 6)

/*
 * The 'offset' argument is a 64bit loff_t split into 2 32bit registers (args 3 -> 4).
 */
#define OVERRIDE_TABLE_32_readahead
TRACE_SYSCALL_TABLE(readahead, readahead, 213, 4)

/*
 * The 'offset' and 'len' arguments are 64bit loff_t split into 2 32bit
 * registers (args 4 -> 6).
 */
#define OVERRIDE_TABLE_32_fadvise64_64
TRACE_SYSCALL_TABLE(fadvise64_64, fadvise64_64, 223, 6)

/*
 * The 'offset' and 'len' arguments are 64bit loff_t split into 2 32bit
 * registers (args 4 -> 6).
 */
#define OVERRIDE_TABLE_32_fallocate
TRACE_SYSCALL_TABLE(fallocate, fallocate, 47, 6)

#endif /* CREATE_SYSCALL_TABLE */
