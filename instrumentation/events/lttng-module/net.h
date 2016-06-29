#undef TRACE_SYSTEM
#define TRACE_SYSTEM net

#if !defined(LTTNG_TRACE_NET_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_NET_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <linux/version.h>
#include <lttng-endian.h>
#include <net/sock.h>

#ifndef ONCE_LTTNG_NET_H
#define ONCE_LTTNG_NET_H

static inline unsigned char __has_network_hdr(struct sk_buff *skb) {
	return skb_network_header(skb) != skb->head;
}

static struct lttng_event_field emptyfields[] = {
};

static struct lttng_event_field tcpfields[] = {
	[0] = {
		.name = "source",
		.type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
	},
	[1] = {
		.name = "dest",
		.type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
	},
	[2] = {
		.name = "seq",
		.type = __type_integer(uint32_t, 0, 0, 0, __BIG_ENDIAN, 16, none),
	},
	[3] = {
		.name = "ack_seq",
		.type = __type_integer(uint32_t, 0, 0, 0, __BIG_ENDIAN, 16, none),
	},
	[4] = {
		.name = "flags",
		.type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 16, none),
	},
	[5] = {
		.name = "window",
		.type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
	},
	[6] = {
		.name = "check",
		.type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 16, none),
	},
	[7] = {
		.name = "urg_ptr",
		.type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 16, none),
	},
};

static struct lttng_event_field transport_fields[] = {
	[0] = {
		.name = "none",
		.type = {
			.atype = atype_struct,
			.u._struct.nr_fields = 0,
			.u._struct.fields = emptyfields,
		}
	},
	[1] = {
		.name = "tcp",
		.type = {
			.atype = atype_struct,
			.u._struct.nr_fields = ARRAY_SIZE(tcpfields),
			.u._struct.fields = tcpfields,
		}
	},
};

enum transport_header_types {
	TH_NONE,
	TH_TCP,
};

static inline unsigned char __get_transport_header_type(struct sk_buff *skb)
{
	/* If the header is not set yet, the network header will point to the head */
	if (__has_network_hdr(skb)) {
		/* when both transport and network header are set, transport header
		 * is greater than network header, otherwise it points ot head */
		if (skb->transport_header > skb->network_header) {
			if ((skb->protocol == htons(ETH_P_IP) &&
					ip_hdr(skb)->protocol == IPPROTO_TCP) ||
				(skb->protocol == htons(ETH_P_IPV6) &&
					ipv6_hdr(skb)->nexthdr == IPPROTO_TCP))
				return TH_TCP;
		}
	}
	return TH_NONE;
}

#endif

LTTNG_TRACEPOINT_ENUM(net_transport_header,
	TP_ENUM_VALUES(
		ctf_enum_value("_none", TH_NONE)
		ctf_enum_value("_tcp", TH_TCP)
	)
)

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
		ctf_enum(net_transport_header, unsigned char, transport_header_type, __get_transport_header_type(skb))
		ctf_custom_field(
			ctf_custom_type(
				.atype = atype_variant,
				.u.variant.tag_name = "transport_header_type",
				.u.variant.choices = transport_fields,
				.u.variant.nr_choices = ARRAY_SIZE(transport_fields),
			),
			transport_header,
			ctf_custom_code(
				if (__get_transport_header_type(skb) == TH_TCP) {
					ctf_array_type(unsigned char, tcp_hdr(skb), sizeof(struct tcphdr))
				}
				/* For any other header type, there is nothing to do */
			)
		)
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
#include <probes/define_trace.h>
