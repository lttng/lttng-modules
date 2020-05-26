/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only) */

#ifndef CREATE_SYSCALL_TABLE

#define OVERRIDE_32_mmap
#define OVERRIDE_64_mmap
SC_LTTNG_TRACEPOINT_EVENT(mmap,
	TP_PROTO(sc_exit(unsigned long ret,)
		unsigned long addr, unsigned long len,
		unsigned long prot, unsigned long flags,
		unsigned long fd, unsigned long off),
	TP_ARGS(sc_exit(ret,) addr, len, prot, flags, fd, off),
	TP_FIELDS(sc_exit(ctf_integer_hex(unsigned long, ret, ret))
		sc_in(ctf_integer_hex(unsigned long, addr, addr))
		sc_in(ctf_integer(size_t, len, len))
		sc_in(ctf_integer(int, prot, prot))
		sc_in(ctf_integer(int, flags, flags))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer(off_t, offset, off))
	)
)

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
#endif	/* #if !defined(CONFIG_64BIT) || defined(CONFIG_COMPAT) */
		ctf_enum_value("F_SETOWN_EX", F_SETOWN_EX)
		ctf_enum_value("F_GETOWN_EX", F_GETOWN_EX)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
		ctf_enum_value("F_GETOWNER_UIDS", F_GETOWNER_UIDS)
#endif /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0)) */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0))
		ctf_enum_value("F_OFD_GETLK", F_OFD_GETLK)
		ctf_enum_value("F_OFD_SETLK", F_OFD_SETLK)
		ctf_enum_value("F_OFD_SETLKW", F_OFD_SETLKW)
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)) */
		ctf_enum_value("F_SETLEASE", F_SETLEASE)
		ctf_enum_value("F_GETLEASE", F_GETLEASE)
		ctf_enum_value("F_NOTIFY", F_NOTIFY)
		ctf_enum_value("F_CANCELLK", F_CANCELLK)
		ctf_enum_value("F_DUPFD_CLOEXEC", F_DUPFD_CLOEXEC)
		ctf_enum_value("F_SETPIPE_SZ", F_SETPIPE_SZ)
		ctf_enum_value("F_GETPIPE_SZ", F_GETPIPE_SZ)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0))
		ctf_enum_value("F_ADD_SEALS", F_ADD_SEALS)
		ctf_enum_value("F_GET_SEALS", F_GET_SEALS)
#endif /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0)) */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0))
		ctf_enum_value("F_GET_RW_HINT", F_GET_RW_HINT)
		ctf_enum_value("F_SET_RW_HINT", F_SET_RW_HINT)
		ctf_enum_value("F_GET_FILE_RW_HINT", F_GET_FILE_RW_HINT)
		ctf_enum_value("F_SET_FILE_RW_HINT", F_SET_FILE_RW_HINT)
#endif /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)) */
	)
)

#define OVERRIDE_32_fcntl
#define OVERRIDE_64_fcntl
SC_LTTNG_TRACEPOINT_EVENT(fcntl,
	TP_PROTO(sc_exit(long ret,) unsigned int fd, unsigned int cmd, unsigned long arg),
	TP_ARGS(sc_exit(ret,) fd, cmd, arg),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(unsigned int, fd, fd))
		sc_in(ctf_enum(lttng_fcntl_cmd_flags, unsigned int, cmd, cmd))
		sc_inout(ctf_integer(unsigned long, arg, arg)))
)

#endif /* CREATE_SYSCALL_TABLE */
