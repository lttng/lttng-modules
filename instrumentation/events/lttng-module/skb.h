#undef TRACE_SYSTEM
#define TRACE_SYSTEM skb

#if !defined(LTTNG_TRACE_SKB_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_SKB_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/version.h>

/*
 * Tracepoint for free an sk_buff:
 */
LTTNG_TRACEPOINT_EVENT_MAP(kfree_skb,

	skb_kfree,

	TP_PROTO(struct sk_buff *skb, void *location),

	TP_ARGS(skb, location),

	TP_FIELDS(
		ctf_integer(void *, skbaddr, skb)
		ctf_integer(void *, location, location)
		ctf_integer_network(unsigned short, protocol, skb->protocol)
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
LTTNG_TRACEPOINT_EVENT_MAP(consume_skb,

	skb_consume,

	TP_PROTO(struct sk_buff *skb),

	TP_ARGS(skb),

	TP_FIELDS(
		ctf_integer(void *, skbaddr, skb)
	)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
LTTNG_TRACEPOINT_EVENT(skb_copy_datagram_iovec,

	TP_PROTO(const struct sk_buff *skb, int len),

	TP_ARGS(skb, len),

	TP_FIELDS(
		ctf_integer(const void *, skbaddr, skb)
		ctf_integer(int, len, len)
	)
)
#endif

#endif /* LTTNG_TRACE_SKB_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
