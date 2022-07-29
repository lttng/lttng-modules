/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM skb

#if !defined(LTTNG_TRACE_SKB_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_SKB_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <lttng-kernel-version.h>

/*
 * Tracepoint for free an sk_buff:
 */
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,17,0) \
	|| LTTNG_KERNEL_RANGE(5,15,58, 5,16,0))

LTTNG_TRACEPOINT_ENUM(skb_drop_reason,
	TP_ENUM_VALUES(
		ctf_enum_value("NOT_SPECIFIED",	SKB_DROP_REASON_NOT_SPECIFIED)
		ctf_enum_value("NO_SOCKET",	SKB_DROP_REASON_NO_SOCKET)
		ctf_enum_value("PKT_TOO_SMALL",	SKB_DROP_REASON_PKT_TOO_SMALL)
		ctf_enum_value("TCP_CSUM",	SKB_DROP_REASON_TCP_CSUM)
		ctf_enum_value("SOCKET_FILTER",	SKB_DROP_REASON_SOCKET_FILTER)
		ctf_enum_value("UDP_CSUM",	SKB_DROP_REASON_UDP_CSUM)
		ctf_enum_value("MAX",		SKB_DROP_REASON_MAX)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(kfree_skb,

	skb_kfree,

	TP_PROTO(struct sk_buff *skb, void *location,
		enum skb_drop_reason reason),

	TP_ARGS(skb, location, reason),

	TP_FIELDS(
		ctf_integer_hex(void *, skbaddr, skb)
		ctf_integer_hex(void *, location, location)
		ctf_integer_network(unsigned short, protocol, skb->protocol)
		ctf_enum(skb_drop_reason, uint8_t, reason, reason)
	)
)
#else
LTTNG_TRACEPOINT_EVENT_MAP(kfree_skb,

	skb_kfree,

	TP_PROTO(struct sk_buff *skb, void *location),

	TP_ARGS(skb, location),

	TP_FIELDS(
		ctf_integer_hex(void *, skbaddr, skb)
		ctf_integer_hex(void *, location, location)
		ctf_integer_network(unsigned short, protocol, skb->protocol)
	)
)
#endif

LTTNG_TRACEPOINT_EVENT_MAP(consume_skb,

	skb_consume,

	TP_PROTO(struct sk_buff *skb),

	TP_ARGS(skb),

	TP_FIELDS(
		ctf_integer_hex(void *, skbaddr, skb)
	)
)

LTTNG_TRACEPOINT_EVENT(skb_copy_datagram_iovec,

	TP_PROTO(const struct sk_buff *skb, int len),

	TP_ARGS(skb, len),

	TP_FIELDS(
		ctf_integer_hex(const void *, skbaddr, skb)
		ctf_integer(int, len, len)
	)
)

#endif /* LTTNG_TRACE_SKB_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
