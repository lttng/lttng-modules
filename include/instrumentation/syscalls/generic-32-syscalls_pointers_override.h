// SPDX-FileCopyrightText: 2026 Michael Jeanson <mjeanson@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

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
