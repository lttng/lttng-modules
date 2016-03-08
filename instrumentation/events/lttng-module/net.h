#undef TRACE_SYSTEM
#define TRACE_SYSTEM net

#if !defined(LTTNG_TRACE_NET_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_NET_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/version.h>

LTTNG_TRACEPOINT_EVENT(net_dev_xmit,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,40))
	TP_PROTO(struct sk_buff *skb,
		 int rc,
		 struct net_device *dev,
		 unsigned int skb_len),

	TP_ARGS(skb, rc, dev, skb_len),
#else
	TP_PROTO(struct sk_buff *skb,
		 int rc),
	
	TP_ARGS(skb, rc),
#endif

	TP_FIELDS(
		ctf_integer_hex(void *, skbaddr, skb)
		ctf_integer(int, rc, rc)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,40))
		ctf_integer(unsigned int, len, skb_len)
		ctf_string(name, dev->name)
#else
		ctf_integer(unsigned int, len, skb->len)
		ctf_string(name, skb->dev->name)
#endif
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(net_dev_template,

	TP_PROTO(struct sk_buff *skb),

	TP_ARGS(skb),

	TP_FIELDS(
		ctf_integer_hex(void *, skbaddr, skb)
		ctf_integer(unsigned int, len, skb->len)
		ctf_string(name, skb->dev->name)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(net_dev_template, net_dev_queue,

	TP_PROTO(struct sk_buff *skb),

	TP_ARGS(skb)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_template,

	netif_receive_skb,

	net_if_receive_skb,

	TP_PROTO(struct sk_buff *skb),

	TP_ARGS(skb)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_template,

	netif_rx,

	net_if_rx,

	TP_PROTO(struct sk_buff *skb),

	TP_ARGS(skb)
)
#endif /* LTTNG_TRACE_NET_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
