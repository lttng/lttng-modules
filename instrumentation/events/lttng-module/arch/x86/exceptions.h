#if !defined(LTTNG_TRACE_EXCEPTIONS_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_EXCEPTIONS_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/version.h>

#undef TRACE_SYSTEM
#define TRACE_SYSTEM x86_exceptions

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
		ctf_integer_hex(unsigned char, error_code, error_code)
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
#define TRACE_INCLUDE_PATH ../instrumentation/events/lttng-module/arch/x86
#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_FILE exceptions

/* This part must be outside protection */
#include <probes/define_trace.h>
