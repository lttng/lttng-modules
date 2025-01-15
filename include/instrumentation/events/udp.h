/* SPDX-License-Identifier: GPL-2.0-only */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM udp

#if !defined(LTTNG_TRACE_UDP_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_UDP_H

#include <lttng/tracepoint-event.h>
#include <linux/udp.h>
#include <lttng/kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,10,0) || \
    LTTNG_RHEL_KERNEL_RANGE(5,14,0,503,21,1,  5,15,0,0,0,0))

LTTNG_TRACEPOINT_ENUM(lttng_sk_family,
	TP_ENUM_VALUES(
		ctf_enum_value("AF_INET", AF_INET)
		ctf_enum_value("AF_INET6", AF_INET6)
	)
)

LTTNG_TRACEPOINT_EVENT(udp_fail_queue_rcv_skb,

	TP_PROTO(int rc, struct sock *sk, struct sk_buff *skb),

	TP_ARGS(rc, sk, skb),

	TP_FIELDS(
		ctf_integer(int, rc, rc)
		ctf_integer(__u16, sport, ntohs(udp_hdr(skb)->source))
		ctf_integer(__u16, dport, ntohs(udp_hdr(skb)->dest))
		ctf_enum(lttng_sk_family, __u16, family, sk->sk_family)
		/*
		 * The 'saddr' and 'daddr' fields from the upstream tracepoint
		 * are currently not extracted. It is recommended to use a
		 * tracepoint from the 'net' probe instead which includes all
		 * fields from the IP header.
		 */
	)
)
#else
LTTNG_TRACEPOINT_EVENT(udp_fail_queue_rcv_skb,

	TP_PROTO(int rc, struct sock *sk),

	TP_ARGS(rc, sk),

	TP_FIELDS(
		ctf_integer(int, rc, rc)
		ctf_integer(__u16, lport, inet_sk(sk)->inet_num)
	)
)
#endif

#endif /* LTTNG_TRACE_UDP_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
