/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only) */

#ifndef CREATE_SYSCALL_TABLE

/*
 * The `flags` argument of the mmap syscall is split in two parts:
 * - The type of mapping is described by the four least significant bits of the 4
 *   bytes integer,
 * - The options on the mapping are described by the remaining 28 most
 *   significant bits.
 */
#define MAPPING_TYPE_RESERVED_BITS 4
#define LTTNG_MMAP_FLAGS_TO_CTF(x) ((x) >> MAPPING_TYPE_RESERVED_BITS)

/*
 * Enumeration of the mmap flags, as described in the 'mmap'
 * system call man page.
 */
SC_LTTNG_TRACEPOINT_ENUM(lttng_mmap_protection,
	TP_ENUM_VALUES(
		ctf_enum_value("PROT_EXEC",	PROT_EXEC)
		ctf_enum_value("PROT_READ",	PROT_READ)
		ctf_enum_value("PROT_WRITE",	PROT_WRITE)
		ctf_enum_value("PROT_NONE",	PROT_NONE)
	)
)

SC_LTTNG_TRACEPOINT_ENUM(lttng_mmap_flags_mapping_type,
	TP_ENUM_VALUES(
		ctf_enum_value("MAP_SHARED",		MAP_SHARED)
		ctf_enum_value("MAP_PRIVATE",		MAP_PRIVATE)
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0))
		ctf_enum_value("MAP_SHARED_VALIDATE",	MAP_SHARED_VALIDATE)
#endif /* (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0)) */
	)
)

/*
 * Shift the values of the options so we can read them from the `flags` integer
 * directly.
 */
SC_LTTNG_TRACEPOINT_ENUM(lttng_mmap_flags_options,
	TP_ENUM_VALUES(
		ctf_enum_value("<none>",		0)

#if defined (MAP_32BIT) && MAP_32BIT != 0
		ctf_enum_value("MAP_32BIT",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_32BIT))
#endif /* defined (MAP_32BIT) && MAP_32BIT != 0 */

		ctf_enum_value("MAP_ANONYMOUS",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_ANONYMOUS))
		ctf_enum_value("MAP_DENYWRITE",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_DENYWRITE))
		ctf_enum_value("MAP_EXECUTABLE",	LTTNG_MMAP_FLAGS_TO_CTF(MAP_EXECUTABLE))
		ctf_enum_value("MAP_FIXED",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_FIXED))

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,17,0))
		ctf_enum_value("MAP_FIXED_NOREPLACE",	LTTNG_MMAP_FLAGS_TO_CTF(MAP_FIXED_NOREPLACE))
#endif /* (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,17,0)) */

		ctf_enum_value("MAP_GROWSDOWN",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_GROWSDOWN))
		ctf_enum_value("MAP_HUGETLB",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_HUGETLB))

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0))

#if defined (MAP_HUGE_2MB) && MAP_HUGE_2MB != 0
		ctf_enum_value("MAP_HUGE_2MB",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_HUGE_2MB))
#endif /* defined (MAP_HUGE_2MB) && MAP_HUGE_2MB != 0 */

#if defined (MAP_HUGE_1GB) && MAP_HUGE_1GB != 0
		ctf_enum_value("MAP_HUGE_1GB",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_HUGE_1GB))
#endif /* defined (MAP_HUGE_1GB) && MAP_HUGE_1GB != 0 */

#endif /* (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,8,0)) */

		ctf_enum_value("MAP_LOCKED",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_LOCKED))
		ctf_enum_value("MAP_NONBLOCK",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_NONBLOCK))
		ctf_enum_value("MAP_NORESERVE",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_NORESERVE))
		ctf_enum_value("MAP_POPULATE",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_POPULATE))
		ctf_enum_value("MAP_STACK",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_STACK))

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0))
		ctf_enum_value("MAP_SYNC",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_SYNC))
#endif /* (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0)) */

#if defined (MAP_UNINITIALIZED) && MAP_UNINITIALIZED != 0
		ctf_enum_value("MAP_UNINITIALIZED",	LTTNG_MMAP_FLAGS_TO_CTF(MAP_UNINITIALIZED))
#endif /* defined (MAP_UNINITIALIZED) && MAP_UNINITIALIZED != 0 */
	)
)

#define LTTNG_MMAP_FLAGS_TYPE						\
	lttng_kernel_static_event_field("type",				\
		lttng_kernel_static_type_enum(&__enum_lttng_mmap_flags_mapping_type, \
				lttng_kernel_static_type_integer(4, 1, 0, __BYTE_ORDER, 16)), \
		false, false, false)

#define LTTNG_MMAP_FLAGS_OPTIONS					\
	lttng_kernel_static_event_field("options",			\
		lttng_kernel_static_type_enum(&__enum_lttng_mmap_flags_options, \
				lttng_kernel_static_type_integer(28, 1, 0, __BYTE_ORDER, 16)), \
		false, false, false)

#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#define LTTNG_MMAP_FLAGS			\
lttng_kernel_static_event_field_array(		\
	[0] = LTTNG_MMAP_FLAGS_TYPE,		\
	[1] = LTTNG_MMAP_FLAGS_OPTIONS,		\
)
#else
#define LTTNG_MMAP_FLAGS			\
lttng_kernel_static_event_field_array(		\
	[0] = LTTNG_MMAP_FLAGS_OPTIONS,		\
	[1] = LTTNG_MMAP_FLAGS_TYPE,		\
)
#endif

/*
 * Use a custom field here so that tracer writes a single integer and the
 * work of splitting it up in two fields is left to the trace reader.
 */
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
		sc_in(ctf_enum(lttng_mmap_protection, int, prot, prot))
		sc_in(
			ctf_custom_field(
				ctf_custom_type(
					lttng_kernel_static_type_struct(2, LTTNG_MMAP_FLAGS, lttng_alignof(uint32_t) * CHAR_BIT)
				),
				flags,
				ctf_custom_code(
					ctf_integer_type(uint32_t, flags)
				)
			)
		)
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
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,6,0))
		ctf_enum_value("F_GETOWNER_UIDS", F_GETOWNER_UIDS)
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,6,0)) */
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,15,0))
		ctf_enum_value("F_OFD_GETLK", F_OFD_GETLK)
		ctf_enum_value("F_OFD_SETLK", F_OFD_SETLK)
		ctf_enum_value("F_OFD_SETLKW", F_OFD_SETLKW)
#endif /* (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,15,0)) */
		ctf_enum_value("F_SETLEASE", F_SETLEASE)
		ctf_enum_value("F_GETLEASE", F_GETLEASE)
		ctf_enum_value("F_NOTIFY", F_NOTIFY)
		ctf_enum_value("F_CANCELLK", F_CANCELLK)
		ctf_enum_value("F_DUPFD_CLOEXEC", F_DUPFD_CLOEXEC)
		ctf_enum_value("F_SETPIPE_SZ", F_SETPIPE_SZ)
		ctf_enum_value("F_GETPIPE_SZ", F_GETPIPE_SZ)
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,17,0))
		ctf_enum_value("F_ADD_SEALS", F_ADD_SEALS)
		ctf_enum_value("F_GET_SEALS", F_GET_SEALS)
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,17,0)) */
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,14,0))
		ctf_enum_value("F_GET_RW_HINT", F_GET_RW_HINT)
		ctf_enum_value("F_SET_RW_HINT", F_SET_RW_HINT)
		ctf_enum_value("F_GET_FILE_RW_HINT", F_GET_FILE_RW_HINT)
		ctf_enum_value("F_SET_FILE_RW_HINT", F_SET_FILE_RW_HINT)
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,14,0)) */
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
