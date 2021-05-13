/* SPDX-License-Identifier: GPL-2.0-only */
#if !defined(LTTNG_TRACE_EXCEPTIONS_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_EXCEPTIONS_H

#include <lttng/tracepoint-event.h>
#include <lttng/kernel-version.h>

#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0))
#include <../arch/x86/include/asm/traps.h>
#else /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0)) */

/* Only define this enum once. */

#ifndef ONCE_LTTNG_EXCEPTIONS_H
#define ONCE_LTTNG_EXCEPTIONS_H

enum {
	X86_PF_PROT =	1 << 0,
	X86_PF_WRITE =	1 << 1,
	X86_PF_USER =	1 << 2,
	X86_PF_RSVD =	1 << 3,
	X86_PF_INSTR =	1 << 4,
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,6,0))
	X86_PF_PK =	1 << 5,
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,6,0)) */
};

#endif /* ONCE_LTTNG_EXCEPTIONS_H */

#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0)) */
#endif /* CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM */

#undef TRACE_SYSTEM
#define TRACE_SYSTEM x86_exceptions

#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM
LTTNG_TRACEPOINT_ENUM(lttng_x86_pf_error_code,
	TP_ENUM_VALUES(
		ctf_enum_value("PROTECTION_FAULT",	X86_PF_PROT)
		ctf_enum_value("WRITE_ACCESS",		X86_PF_WRITE)
		ctf_enum_value("USER_MODE",		X86_PF_USER)
		ctf_enum_value("RESERVED_BIT",		X86_PF_RSVD)
		ctf_enum_value("INSTRUCTION_FETCH",	X86_PF_INSTR)
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,6,0))
		ctf_enum_value("PROTECTION_KEYS_BLOCK", X86_PF_PK)
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,6,0)) */
	)
)
#endif /* CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM */

LTTNG_TRACEPOINT_EVENT_CLASS(x86_exceptions_class,

	TP_PROTO(unsigned long address, struct pt_regs *regs,
		unsigned long error_code),

	TP_ARGS(address, regs, error_code),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, address, address)
		ctf_integer_hex(unsigned long, ip, regs->ip)
		/*
		 * Note: we cast error_code from unsigned long
		 * to unsigned char to save space in the trace.
		 * Currently, only 5 low bits are used. Should be made
		 * larger if error codes are added to the kernel.
		 */
#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM
		ctf_enum(lttng_x86_pf_error_code, unsigned char, error_code, error_code)
#else
		ctf_integer_hex(unsigned char, error_code, error_code)
#endif
	)
)

#define LTTNG_EXCEPTIONS_TRACEPOINT_EVENT_INSTANCE_MAP(name, map)	\
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(x86_exceptions_class,		\
	name,								\
	map,								\
	TP_PROTO(unsigned long address, struct pt_regs *regs,		\
		unsigned long error_code),				\
	TP_ARGS(address, regs, error_code)				\
)

LTTNG_EXCEPTIONS_TRACEPOINT_EVENT_INSTANCE_MAP(page_fault_user,
		x86_exceptions_page_fault_user)
LTTNG_EXCEPTIONS_TRACEPOINT_EVENT_INSTANCE_MAP(page_fault_kernel,
		x86_exceptions_page_fault_kernel)

#endif /* LTTNG_TRACE_EXCEPTIONS_H */

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH instrumentation/events/arch/x86
#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_FILE exceptions

/* This part must be outside protection */
#include <lttng/define_trace.h>
