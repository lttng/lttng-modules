#undef TRACE_SYSTEM
#define TRACE_SYSTEM napi

#if !defined(LTTNG_TRACE_NAPI_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_NAPI_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/netdevice.h>
#include <linux/ftrace.h>

#define NO_DEV "(no_device)"

LTTNG_TRACEPOINT_EVENT(napi_poll,

	TP_PROTO(struct napi_struct *napi),

	TP_ARGS(napi),

	TP_FIELDS(
		ctf_integer_hex(struct napi_struct *, napi, napi)
		ctf_string(dev_name, napi->dev ? napi->dev->name : NO_DEV)
	)
)

#undef NO_DEV

#endif /* LTTNG_TRACE_NAPI_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
