#undef TRACE_SYSTEM
#define TRACE_SYSTEM printk

#if !defined(LTTNG_TRACE_PRINTK_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_PRINTK_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))

LTTNG_TRACEPOINT_EVENT_MAP(console,

	printk_console,

	TP_PROTO(const char *text, size_t len),

	TP_ARGS(text, len),

	TP_FIELDS(
		ctf_sequence_text(char, msg, text, size_t, len)
	)
)

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,5,0))

LTTNG_TRACEPOINT_EVENT_CONDITION_MAP(console,

	printk_console,

	TP_PROTO(const char *log_buf, unsigned start, unsigned end,
		 unsigned log_buf_len),

	TP_ARGS(log_buf, start, end, log_buf_len),

	TP_CONDITION(start != end),

	TP_FIELDS(
		ctf_sequence_text(char, msg, log_buf + start,
			size_t, end - start)
	)
)

#else /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0)) */

LTTNG_TRACEPOINT_EVENT_CONDITION_MAP(console,

	printk_console,

	TP_PROTO(const char *log_buf, unsigned start, unsigned end,
		 unsigned log_buf_len),

	TP_ARGS(log_buf, start, end, log_buf_len),

	TP_CONDITION(start != end),

	TP_FIELDS(
		/*
		 * printk buffer is gathered from two segments on older kernels.
		 */
		ctf_sequence_text(char, msg1,
			log_buf + (start & (log_buf_len - 1)),
			size_t, (start & (log_buf_len - 1)) > (end & (log_buf_len - 1))
				? log_buf_len - (start & (log_buf_len - 1))
				: end - start)
		ctf_sequence_text(char, msg2,
			log_buf,
			size_t, (start & (log_buf_len - 1)) > (end & (log_buf_len - 1))
				? end & (log_buf_len - 1)
				: 0)
	)
)

#endif

#endif /* LTTNG_TRACE_PRINTK_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
