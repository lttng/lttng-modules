#undef TRACE_SYSTEM
#define TRACE_SYSTEM udp

#if !defined(LTTNG_TRACE_UDP_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_UDP_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/udp.h>

LTTNG_TRACEPOINT_EVENT(udp_fail_queue_rcv_skb,

	TP_PROTO(int rc, struct sock *sk),

	TP_ARGS(rc, sk),

	TP_FIELDS(
		ctf_integer(int, rc, rc)
		ctf_integer(__u16, lport, inet_sk(sk)->inet_num)
	)
)

#endif /* LTTNG_TRACE_UDP_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
