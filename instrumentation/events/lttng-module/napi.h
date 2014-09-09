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

	TP_STRUCT__entry(
		__field(	struct napi_struct *,	napi)
		__string(	dev_name, napi->dev ? napi->dev->name : NO_DEV)
	),

	TP_fast_assign(
		tp_assign(napi, napi)
		tp_strcpy(dev_name, napi->dev ? napi->dev->name : NO_DEV)
	),

	TP_printk("napi poll on napi struct %p for device %s",
		__entry->napi, __get_str(dev_name))
)

#undef NO_DEV

#endif /* LTTNG_TRACE_NAPI_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
