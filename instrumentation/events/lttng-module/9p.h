/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM 9p

#if !defined(LTTNG_TRACE_9P_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_9P_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0))
LTTNG_TRACEPOINT_EVENT(9p_client_req,

	TP_PROTO(struct p9_client *clnt, int8_t type, int tag),

	TP_ARGS(clnt, type, tag),

	TP_FIELDS(
		ctf_integer_hex(void *, clnt, clnt)
		ctf_integer(u8, type, type)
		ctf_integer(u32, tag, tag)
	)
)

LTTNG_TRACEPOINT_EVENT(9p_client_res,

	TP_PROTO(struct p9_client *clnt, int8_t type, int tag, int err),

	TP_ARGS(clnt, type, tag, err),

	TP_FIELDS(
		ctf_integer_hex(void *, clnt, clnt)
		ctf_integer(u8, type, type)
		ctf_integer(u32, tag, tag)
		ctf_integer(u32, err, err)
	)
)

LTTNG_TRACEPOINT_EVENT(9p_protocol_dump,

	TP_PROTO(struct p9_client *clnt, struct p9_fcall *pdu),

	TP_ARGS(clnt, pdu),

	TP_FIELDS(
		ctf_integer_hex(void *, clnt, clnt)
		ctf_integer(u8, type, pdu->id)
		ctf_integer(u16, tag, pdu->tag)
		ctf_array(unsigned char, line, pdu->sdata, P9_PROTO_DUMP_SZ)
	)
)
#endif

#endif /* LTTNG_TRACE_9P_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
