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

static inline unsigned char __has_network_hdr(struct sk_buff *skb)
{
	/*
	 * If the header is not set yet,
	 * the network header will point to the head.
	 */
	return skb_network_header(skb) != skb->head;
}

static struct lttng_event_field emptyfields[] = {
};

/* Structures for network headers. */

static struct lttng_event_field ipv4fields[] = {
	[0] = {
		.name = "version",
		.type = __type_integer(uint8_t, 4, 4, 0, __BIG_ENDIAN, 10, none),
	},
	[1] = {
		.name = "ihl",
		.type = __type_integer(uint8_t, 4, 4, 0, __BIG_ENDIAN, 10, none),
	},
	[2] = {
		.name = "tos",
		.type = __type_integer(uint8_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
	},
	[3] = {
		.name = "tot_len",
		.type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
	},
	[4] = {
		.name = "id",
		.type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 16, none),
	},
	[5] = {
		.name = "frag_off",
		.type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
	},
	[6] = {
		.name = "ttl",
		.type = __type_integer(uint8_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
	},
	[7] = {
		.name = "protocol",
		.type = __type_integer(uint8_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
	},
	[8] = {
		.name = "checksum",
		.type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 16, none),
	},
	[9] = {
		.name = "saddr",
		.type = {
			.atype = atype_array,
			.u.array.elem_type = __type_integer(uint8_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
			.u.array.length = 4,
			.u.array.elem_alignment = lttng_alignof(uint8_t),
		},
	},
	[10] = {
		.name = "daddr",
		.type = {
			.atype = atype_array,
			.u.array.elem_type = __type_integer(uint8_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
			.u.array.length = 4,
			.u.array.elem_alignment = lttng_alignof(uint8_t),
		},
	},
};

static struct lttng_event_field ipv6fields[] = {
	[0] = {
		.name = "version",
		.type = __type_integer(uint8_t, 4, 4, 0, __BIG_ENDIAN, 10, none),
	},
	[1] = {
		.name = "prio",
		.type = __type_integer(uint8_t, 4, 4, 0, __BIG_ENDIAN, 10, none),
	},
	[2] = {
		.name = "flow_lbl",
		.type = {
			.atype = atype_array,
			.u.array.elem_type = __type_integer(uint8_t, 0, 0, 0, __BIG_ENDIAN, 16, none),
			.u.array.length = 3,
			.u.array.elem_alignment = lttng_alignof(uint8_t),
		},
	},
	[3] = {
		.name = "payload_len",
		.type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
	},
	[4] = {
		.name = "nexthdr",
		.type = __type_integer(uint8_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
	},
	[5] = {
		.name = "hop_limit",
		.type = __type_integer(uint8_t, 0, 0, 0, __BIG_ENDIAN, 10, none),
	},
	[6] = {
		.name = "saddr",
		.type = {
			.atype = atype_array,
			.u.array.elem_type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 16, none),
			.u.array.length = 8,
			.u.array.elem_alignment = lttng_alignof(uint16_t),
		},
	},
	[7] = {
		.name = "daddr",
		.type = {
			.atype = atype_array,
			.u.array.elem_type = __type_integer(uint16_t, 0, 0, 0, __BIG_ENDIAN, 16, none),
			.u.array.length = 8,
			.u.array.elem_alignment = lttng_alignof(uint16_t),
		},
	},
};

static struct lttng_event_field network_fields[] = {
	[0] = {
		.name = "unknown",
		.type = {
			.atype = atype_struct,
			.u._struct.nr_fields = 0,
			.u._struct.fields = emptyfields,
		},
	},
	[1] = {
		.name = "ipv4",
		.type = {
			.atype = atype_struct,
			.u._struct.nr_fields = ARRAY_SIZE(ipv4fields),
			.u._struct.fields = ipv4fields,
		},
	},
	[2] = {
		.name = "ipv6",
		.type = {
			.atype = atype_struct,
			.u._struct.nr_fields = ARRAY_SIZE(ipv6fields),
			.u._struct.fields = ipv6fields,
		},
	},
};

enum network_header_types {
	NH_NONE,
	NH_IPV4,
	NH_IPV6,
};

static inline unsigned char __get_network_header_type(struct sk_buff *skb)
{
	if (__has_network_hdr(skb)) {
		if (skb->protocol == htons(ETH_P_IPV6))
			return NH_IPV6;
		else if (skb->protocol == htons(ETH_P_IP))
			return NH_IPV4;
		/* Fallthrough for other header types. */
	}
	return NH_NONE;
}

#endif

LTTNG_TRACEPOINT_ENUM(net_network_header,
	TP_ENUM_VALUES(
		ctf_enum_value("_unknown", NH_NONE)
		ctf_enum_value("_ipv4", NH_IPV4)
		ctf_enum_value("_ipv6", NH_IPV6)
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
		ctf_enum(net_network_header, unsigned char,
			network_header_type, __get_network_header_type(skb))
		ctf_custom_field(
			ctf_custom_type(
				.atype = atype_variant,
				.u.variant.tag_name = "network_header_type",
				.u.variant.choices = network_fields,
				.u.variant.nr_choices = ARRAY_SIZE(network_fields),
			),
			network_header,
			ctf_custom_code(
				switch (__get_network_header_type(skb)) {
				case NH_IPV4: {
					ctf_align(uint16_t)
					ctf_array_type(uint8_t, ip_hdr(skb),
							sizeof(struct iphdr))
					break;
				}
				case NH_IPV6: {
					ctf_align(uint16_t)
					ctf_array_type(uint8_t, ipv6_hdr(skb),
							sizeof(struct ipv6hdr))
					break;
				}
				default:
					/* For any other header type, there is nothing to do. */
					break;
				}
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
