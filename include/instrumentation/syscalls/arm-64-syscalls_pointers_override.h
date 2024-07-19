// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

/*
 * this is a place-holder for arm64 integer syscall definition override.
 */
#ifndef CREATE_SYSCALL_TABLE

#else
#define OVERRIDE_TABLE_64_clone
TRACE_SYSCALL_TABLE(clone, clone, 220, 5)
#endif
