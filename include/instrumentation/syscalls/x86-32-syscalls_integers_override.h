// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

#else /* #ifndef CREATE_SYSCALL_TABLE */

/*
 * Rename 'mmap_pgoff' to 'mmap2', while 'mmap_pgoff' is kernel internal,
 * 'mmap2' is exposed in unistd.h.
 */
#define OVERRIDE_TABLE_32_mmap_pgoff
#define OVERRIDE_TABLE_32_mmap2
TRACE_SYSCALL_TABLE(mmap2, mmap2, 192, 6)

#endif /* #else #ifndef CREATE_SYSCALL_TABLE */
