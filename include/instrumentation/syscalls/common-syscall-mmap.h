// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM

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
		ctf_enum_value("PROT_NONE",	PROT_NONE)
		ctf_enum_value("PROT_READ",	PROT_READ)
		ctf_enum_value("PROT_WRITE",	PROT_WRITE)
		ctf_enum_value("PROT_EXEC",	PROT_EXEC)
	)
)

SC_LTTNG_TRACEPOINT_ENUM(lttng_mmap_flags_mapping_type,
	TP_ENUM_VALUES(
		ctf_enum_value("MAP_SHARED",		MAP_SHARED)
		ctf_enum_value("MAP_PRIVATE",		MAP_PRIVATE)
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0))
		ctf_enum_value("MAP_SHARED_VALIDATE",	MAP_SHARED_VALIDATE)
#endif /* (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0)) */
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,11,0))
		ctf_enum_value("MAP_DROPPABLE",	MAP_DROPPABLE)
#endif /* (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,11,0)) */
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

#if defined (MAP_HUGE_2MB) && MAP_HUGE_2MB != 0
		ctf_enum_value("MAP_HUGE_2MB",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_HUGE_2MB))
#endif /* defined (MAP_HUGE_2MB) && MAP_HUGE_2MB != 0 */

#if defined (MAP_HUGE_1GB) && MAP_HUGE_1GB != 0
		ctf_enum_value("MAP_HUGE_1GB",		LTTNG_MMAP_FLAGS_TO_CTF(MAP_HUGE_1GB))
#endif /* defined (MAP_HUGE_1GB) && MAP_HUGE_1GB != 0 */

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
		false, false)

#define LTTNG_MMAP_FLAGS_OPTIONS					\
	lttng_kernel_static_event_field("options",			\
		lttng_kernel_static_type_enum(&__enum_lttng_mmap_flags_options, \
				lttng_kernel_static_type_integer(28, 1, 0, __BYTE_ORDER, 16)), \
		false, false)

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

#endif /* CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM */

#endif /* CREATE_SYSCALL_TABLE */
