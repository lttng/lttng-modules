/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM lttng_kallsyms

#if !defined(LTTNG_TRACE_LTTNG_KALLSYMS_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_LTTNG_KALLSYMS_H

#include <probes/lttng-tracepoint-event.h>

LTTNG_TRACEPOINT_EVENT(lttng_kallsyms_kernel_symbol,

	TP_PROTO(struct lttng_session *session,
		unsigned long addr, const char *symbol, const char *module),

	TP_ARGS(session, addr, symbol, module),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, addr, addr)
		ctf_string(symbol, symbol)
		ctf_string(module, module)
	)
)

#endif /*  LTTNG_TRACE_LTTNG_KALLSYMS_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
