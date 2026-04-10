// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

#else	/* CREATE_SYSCALL_TABLE */

/*
 * Rename 'mmap_pgoff' to 'mmap2', while 'mmap_pgoff' is kernel internal,
 * 'mmap2' is exposed in unistd.h.
 */
#define OVERRIDE_TABLE_32_mmap_pgoff
#define OVERRIDE_TABLE_32_mmap2
TRACE_SYSCALL_TABLE(mmap2, mmap2, 192, 6)

/*
 * The 'length' argument is a 64bit loff_t split into 2 32bit registers and 1
 * unused register for alignment (args 2 -> 4).
 */
#define OVERRIDE_TABLE_32_ftruncate64
TRACE_SYSCALL_TABLE(ftruncate64, ftruncate64, 194, 4)

/*
 * The 'offset' argument is a 64bit loff_t split into 2 32bit registers and 1
 * unused register for alignment (args 3 -> 5).
 */
#define OVERRIDE_TABLE_32_readahead
TRACE_SYSCALL_TABLE(readahead, readahead, 225, 5)

/*
 * The 'offset' and 'len' arguments are 64bit loff_t split into 2 32bit
 * registers (args 4 -> 6).
 */
#define OVERRIDE_TABLE_32_fadvise64_64
TRACE_SYSCALL_TABLE(fadvise64_64, fadvise64_64, 270, 6)

/*
 * The 'offset' and 'nbytes' arguments are 64bit loff_t split into 2 32bit
 * registers (args 4 -> 6).
 */
#define OVERRIDE_TABLE_32_sync_file_range2
TRACE_SYSCALL_TABLE(sync_file_range2, sync_file_range2, 341, 6)

/*
 * The 'offset' and 'len' arguments are 64bit loff_t split into 2 32bit
 * registers (args 4 -> 6).
 */
#define OVERRIDE_TABLE_32_fallocate
TRACE_SYSCALL_TABLE(fallocate, fallocate, 352, 6)

#endif /* CREATE_SYSCALL_TABLE */
