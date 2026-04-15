// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM
/*
 * Enumeration of the open flags, as described in the 'open'
 * system call man page.
 */
SC_LTTNG_TRACEPOINT_ENUM(lttng_fcntl_cmd_flags,
	TP_ENUM_VALUES(
		ctf_enum_value("F_DUPFD", F_DUPFD)
		ctf_enum_value("F_GETFD", F_GETFD)
		ctf_enum_value("F_SETFD", F_SETFD)
		ctf_enum_value("F_GETFL", F_GETFL)
		ctf_enum_value("F_SETFL", F_SETFL)
		ctf_enum_value("F_GETLK", F_GETLK)
		ctf_enum_value("F_SETLK", F_SETLK)
		ctf_enum_value("F_SETLKW", F_SETLKW)
		ctf_enum_value("F_SETOWN", F_SETOWN)
		ctf_enum_value("F_GETOWN", F_GETOWN)
		ctf_enum_value("F_SETSIG", F_SETSIG)
		ctf_enum_value("F_GETSIG", F_GETSIG)
#if !defined(CONFIG_64BIT) || defined(CONFIG_COMPAT)
		ctf_enum_value("F_GETLK64", F_GETLK64)
		ctf_enum_value("F_SETLK64", F_SETLK64)
		ctf_enum_value("F_SETLKW64", F_SETLKW64)
#endif /* #if !defined(CONFIG_64BIT) || defined(CONFIG_COMPAT) */
		ctf_enum_value("F_SETOWN_EX", F_SETOWN_EX)
		ctf_enum_value("F_GETOWN_EX", F_GETOWN_EX)
		ctf_enum_value("F_GETOWNER_UIDS", F_GETOWNER_UIDS)
		ctf_enum_value("F_OFD_GETLK", F_OFD_GETLK)
		ctf_enum_value("F_OFD_SETLK", F_OFD_SETLK)
		ctf_enum_value("F_OFD_SETLKW", F_OFD_SETLKW)
		ctf_enum_value("F_SETLEASE", F_SETLEASE)
		ctf_enum_value("F_GETLEASE", F_GETLEASE)
		ctf_enum_value("F_NOTIFY", F_NOTIFY)
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,10,0))
		ctf_enum_value("F_DUPFD_QUERY", F_DUPFD_QUERY)
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,10,0)) */
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,12,0))
		ctf_enum_value("F_CREATED_QUERY", F_CREATED_QUERY)
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,12,0)) */
		ctf_enum_value("F_CANCELLK", F_CANCELLK)
		ctf_enum_value("F_DUPFD_CLOEXEC", F_DUPFD_CLOEXEC)
		ctf_enum_value("F_SETPIPE_SZ", F_SETPIPE_SZ)
		ctf_enum_value("F_GETPIPE_SZ", F_GETPIPE_SZ)
		ctf_enum_value("F_ADD_SEALS", F_ADD_SEALS)
		ctf_enum_value("F_GET_SEALS", F_GET_SEALS)
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,14,0))
		ctf_enum_value("F_GET_RW_HINT", F_GET_RW_HINT)
		ctf_enum_value("F_SET_RW_HINT", F_SET_RW_HINT)
		ctf_enum_value("F_GET_FILE_RW_HINT", F_GET_FILE_RW_HINT)
		ctf_enum_value("F_SET_FILE_RW_HINT", F_SET_FILE_RW_HINT)
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,14,0)) */
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,19,0))
		ctf_enum_value("F_GETDELEG", F_GETDELEG)
		ctf_enum_value("F_SETDELEG", F_SETDELEG)
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,19,0)) */
	)
)
#endif /* CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM */

#endif /* CREATE_SYSCALL_TABLE */
