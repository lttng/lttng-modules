// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

#else	/* CREATE_SYSCALL_TABLE */

#define OVERRIDE_TABLE_64_sys_clone
TRACE_SYSCALL_TABLE(clone, clone, 5055, 0)

#endif /* CREATE_SYSCALL_TABLE */
