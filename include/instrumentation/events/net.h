/* SPDX-License-Identifier: GPL-2.0-only */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM net

#if !defined(LTTNG_TRACE_NET_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_NET_H

#include <lttng/tracepoint-event.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <lttng/kernel-version.h>
#include <lttng/endian.h>
#include <net/sock.h>

#ifndef ONCE_LTTNG_NET_H
#define ONCE_LTTNG_NET_H

static inline unsigned char __has_network_hdr(struct sk_buff *skb)
{
	/*
	 * If the header is not set yet, the network header will point
	 * to the head.
	 */
	return skb_network_header(skb) != skb->head;
}

static struct lttng_event_field emptyfields[] = {
};

/* Structures for transport headers. */

static struct lttng_event_field tcpfields[] = {
	[0] = {
		.name = "source_port",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[1] = {
		.name = "dest_port",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[2] = {
		.name = "seq",
		.type = __type_integer(uint32_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[3] = {
		.name = "ack_seq",
		.type = __type_integer(uint32_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[4] = {
		.name = "data_offset",
		.type = __type_integer(uint8_t, 4, 4, 0,
				__BIG_ENDIAN, 10, none),
	},
	[5] = {
		.name = "reserved",
		.type = __type_integer(uint8_t, 3, 1, 0,
				__BIG_ENDIAN, 10, none),
	},
	[6] = {
		.name = "flags",
		.type = __type_integer(uint8_t, 9, 1, 0,
				__BIG_ENDIAN, 16, none),
	},
	[7] = {
		.name = "window_size",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[8] = {
		.name = "checksum",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 16, none),
	},
	[9] = {
		.name = "urg_ptr",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
};

static struct lttng_event_field udpfields[] = {
	[0] = {
		.name = "source_port",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[1] = {
		.name = "dest_port",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[2] = {
		.name = "len",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[3] = {
		.name = "check",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
};

static struct lttng_event_field icmpfields[] = {
	[0] = {
		.name = "type",
		.type = __type_integer(uint8_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[1] = {
		.name = "code",
		.type = __type_integer(uint8_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[2] = {
		.name = "checksum",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[3] = {
		.name = "gateway",
		.type = __type_integer(uint32_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
};


static struct lttng_event_field transport_fields[] = {
	[0] = {
		.name = "unknown",
		.type = {
			.type = lttng_kernel_type_struct_nestable,
			.u.struct_nestable.nr_fields = ARRAY_SIZE(emptyfields),
			.u.struct_nestable.fields = emptyfields,
			.u.struct_nestable.alignment = 0,
		},
		.nofilter = 1,
	},
	[1] = {
		.name = "tcp",
		.type = {
			.type = lttng_kernel_type_struct_nestable,
			.u.struct_nestable.nr_fields = ARRAY_SIZE(tcpfields),
			.u.struct_nestable.fields = tcpfields,
			.u.struct_nestable.alignment = 0,
		},
		.nofilter = 1,
	},
	[2] = {
		.name = "udp",
		.type = {
			.type = lttng_kernel_type_struct_nestable,
			.u.struct_nestable.nr_fields = ARRAY_SIZE(udpfields),
			.u.struct_nestable.fields = udpfields,
			.u.struct_nestable.alignment = 0,
		},
		.nofilter = 1,
	},
	[3] = {
		.name = "icmp",
		.type = {
			.type = lttng_kernel_type_struct_nestable,
			.u.struct_nestable.nr_fields = ARRAY_SIZE(icmpfields),
			.u.struct_nestable.fields = icmpfields,
			.u.struct_nestable.alignment = 0,
		},
		.nofilter = 1,
	},
};

enum transport_header_types {
	TH_NONE = 0,
	TH_TCP = 1,
	TH_UDP = 2,
	TH_ICMP = 3,
};

static inline enum transport_header_types __get_transport_header_type_ip(struct sk_buff *skb)
{
	switch (ip_hdr(skb)->protocol) {
	case IPPROTO_TCP:
		return TH_TCP;
	case IPPROTO_UDP:
		return TH_UDP;
	case IPPROTO_ICMP:
		return TH_ICMP;
	}
	return TH_NONE;
}

static inline enum transport_header_types __get_transport_header_type_ipv6(struct sk_buff *skb)
{
	switch (ipv6_hdr(skb)->nexthdr) {
	case IPPROTO_TCP:
		return TH_TCP;
	case IPPROTO_UDP:
		return TH_UDP;
	case IPPROTO_ICMP:
		return TH_ICMP;
	}
	return TH_NONE;
}

static inline enum transport_header_types __get_transport_header_type(struct sk_buff *skb)
{
	if (__has_network_hdr(skb)) {
		/*
		 * When both transport and network headers are set,
		 * transport header is greater than network header,
		 * otherwise it points to head.
		 */
		if (skb->transport_header > skb->network_header) {
			/*
			 * Get the transport protocol from the network
			 * header's data. This method works both for
			 * sent and received packets.
			 */
			if (skb->protocol == htons(ETH_P_IP)) {
				return __get_transport_header_type_ip(skb);
			} else if(skb->protocol == htons(ETH_P_IPV6)) {
				return __get_transport_header_type_ipv6(skb);
			}
		}
		/* Fallthrough for other cases where header is not recognized. */
	}
	return TH_NONE;
}

static struct lttng_kernel_enum_entry proto_transport_enum_entries[] = {
	[0] = {
		.start = { .value = 0, .signedness = 0, },
		.end = { .value = IPPROTO_ICMP - 1, .signedness = 0, },
		.string = "_unknown",
	},
	[1] = {
		.start = { .value = IPPROTO_ICMP, .signedness = 0, },
		.end = { .value = IPPROTO_ICMP, .signedness = 0, },
		.string = "_icmp",
	},
	[2] = {
		.start = { .value = IPPROTO_ICMP + 1, .signedness = 0, },
		.end = { .value = IPPROTO_TCP - 1, .signedness = 0, },
		.string = "_unknown",
	},
	[3] = {
		.start = { .value = IPPROTO_TCP, .signedness = 0, },
		.end = { .value = IPPROTO_TCP, .signedness = 0, },
		.string = "_tcp",
	},
	[4] = {
		.start = { .value = IPPROTO_TCP + 1, .signedness = 0, },
		.end = { .value = IPPROTO_UDP - 1, .signedness = 0, },
		.string = "_unknown",
	},
	[5] = {
		.start = { .value = IPPROTO_UDP, .signedness = 0, },
		.end = { .value = IPPROTO_UDP, .signedness = 0, },
		.string = "_udp",
	},
	[6] = {
		.start = { .value = IPPROTO_UDP + 1, .signedness = 0, },
		.end = { .value = 255, .signedness = 0, },
		.string = "_unknown",
	},
};

static const struct lttng_enum_desc proto_transport_header_type = {
	.name = "proto_transport_header_type",
	.entries = proto_transport_enum_entries,
	.nr_entries = ARRAY_SIZE(proto_transport_enum_entries),
};

static struct lttng_kernel_enum_entry transport_enum_entries[] = {
	[0] = {
		.start = { .value = TH_NONE, .signedness = 0, },
		.end = { .value = TH_NONE, .signedness = 0, },
		.string = "_unknown",
	},
	[1] = {
		.start = { .value = TH_TCP, .signedness = 0, },
		.end = { .value = TH_TCP, .signedness = 0, },
		.string = "_tcp",
	},
	[2] = {
		.start = { .value = TH_UDP, .signedness = 0, },
		.end = { .value = TH_UDP, .signedness = 0, },
		.string = "_udp",
	},
	[3] = {
		.start = { .value = TH_ICMP, .signedness = 0, },
		.end = { .value = TH_ICMP, .signedness = 0, },
		.string = "_icmp",
	},
};

static const struct lttng_enum_desc transport_header_type = {
	.name = "transport_header_type",
	.entries = transport_enum_entries,
	.nr_entries = ARRAY_SIZE(transport_enum_entries),
};

/* Structures for network headers. */

static struct lttng_event_field ipv4fields[] = {
	[0] = {
		.name = "version",
		.type = __type_integer(uint8_t, 4, 4, 0,
				__BIG_ENDIAN, 10, none),
	},
	[1] = {
		.name = "ihl",
		.type = __type_integer(uint8_t, 4, 4, 0,
				__BIG_ENDIAN, 10, none),
	},
	[2] = {
		.name = "tos",
		.type = __type_integer(uint8_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[3] = {
		.name = "tot_len",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[4] = {
		.name = "id",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 16, none),
	},
	[5] = {
		.name = "frag_off",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[6] = {
		.name = "ttl",
		.type = __type_integer(uint8_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[7] = {
		.name = "protocol",
		.type = {
			.type = lttng_kernel_type_enum_nestable,
			.u.enum_nestable.desc =
				&proto_transport_header_type,
			.u.enum_nestable.container_type =
				__LTTNG_COMPOUND_LITERAL(struct lttng_type,
					__type_integer(uint8_t, 0, 0, -1,
						__BIG_ENDIAN, 10, none)),
		},
	},
	[8] = {
		.name = "checksum",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 16, none),
	},
	[9] = {
		.name = "saddr",
		.type = {
			.type = lttng_kernel_type_array_nestable,
			.u.array_nestable.elem_type =
				__LTTNG_COMPOUND_LITERAL(struct lttng_type,
					__type_integer(uint8_t, 0, 0, 0,
						__BIG_ENDIAN, 10, none)),
			.u.array_nestable.length = 4,
			.u.array_nestable.alignment = lttng_alignof(uint8_t),
		},
	},
	[10] = {
		.name = "daddr",
		.type = {
			.type = lttng_kernel_type_array_nestable,
			.u.array_nestable.elem_type =
				__LTTNG_COMPOUND_LITERAL(struct lttng_type,
					__type_integer(uint8_t, 0, 0, 0,
						__BIG_ENDIAN, 10, none)),
			.u.array_nestable.length = 4,
			.u.array_nestable.alignment = lttng_alignof(uint8_t),
		},
	},
	[11] = {
		.name = "transport_header_type",
		.type = {
			.type = lttng_kernel_type_enum_nestable,
			.u.enum_nestable.desc = &transport_header_type,
			.u.enum_nestable.container_type =
				__LTTNG_COMPOUND_LITERAL(struct lttng_type,
					__type_integer(uint8_t, 0, 0, -1,
						__BYTE_ORDER, 10, none)),
		},
	},
	[12] = {
		.name = "transport_header",
		.type = {
			.type = lttng_kernel_type_variant_nestable,
			.u.variant_nestable.tag_name = "transport_header_type",
			.u.variant_nestable.choices = transport_fields,
			.u.variant_nestable.nr_choices = ARRAY_SIZE(transport_fields),
			.u.variant_nestable.alignment = 0,
		},
	},
};

static struct lttng_event_field ipv6fields[] = {
	[0] = {
		.name = "version",
		.type = __type_integer(uint8_t, 4, 4, 0,
				__BIG_ENDIAN, 10, none),
	},
	[1] = {
		.name = "prio",
		.type = __type_integer(uint8_t, 4, 4, 0,
				__BIG_ENDIAN, 10, none),
	},
	[2] = {
		.name = "flow_lbl",
		.type = {
			.type = lttng_kernel_type_array_nestable,
			.u.array_nestable.elem_type =
				__LTTNG_COMPOUND_LITERAL(struct lttng_type,
					__type_integer(uint8_t, 0, 0, 0,
						__BIG_ENDIAN, 16, none)),
			.u.array_nestable.length = 3,
			.u.array_nestable.alignment = lttng_alignof(uint8_t),
		},
	},
	[3] = {
		.name = "payload_len",
		.type = __type_integer(uint16_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[4] = {
		.name = "nexthdr",
		.type = {
			.type = lttng_kernel_type_enum_nestable,
			.u.enum_nestable.desc =
				&proto_transport_header_type,
			.u.enum_nestable.container_type =
				__LTTNG_COMPOUND_LITERAL(struct lttng_type,
					__type_integer(uint8_t, 0, 0, -1,
						__BIG_ENDIAN, 10, none)),
		},
	},
	[5] = {
		.name = "hop_limit",
		.type = __type_integer(uint8_t, 0, 0, 0,
				__BIG_ENDIAN, 10, none),
	},
	[6] = {
		.name = "saddr",
		.type = {
			.type = lttng_kernel_type_array_nestable,
			.u.array_nestable.elem_type =
				__LTTNG_COMPOUND_LITERAL(struct lttng_type,
					__type_integer(uint16_t, 0, 0, 0,
						__BIG_ENDIAN, 16, none)),
			.u.array_nestable.length = 8,
			.u.array_nestable.alignment = lttng_alignof(uint16_t),
		},
	},
	[7] = {
		.name = "daddr",
		.type = {
			.type = lttng_kernel_type_array_nestable,
			.u.array_nestable.elem_type =
				__LTTNG_COMPOUND_LITERAL(struct lttng_type,
					__type_integer(uint16_t, 0, 0, 0,
						__BIG_ENDIAN, 16, none)),
			.u.array_nestable.length = 8,
			.u.array_nestable.alignment = lttng_alignof(uint16_t),
		},
	},
	[8] = {
		.name = "transport_header_type",
		.type = {
			.type = lttng_kernel_type_enum_nestable,
			.u.enum_nestable.desc = &transport_header_type,
			.u.enum_nestable.container_type =
				__LTTNG_COMPOUND_LITERAL(struct lttng_type,
					__type_integer(uint8_t, 0, 0, -1,
						__BYTE_ORDER, 10, none)),
		},
	},
	[9] = {
		.name = "transport_header",
		.type = {
			.type = lttng_kernel_type_variant_nestable,
			.u.variant_nestable.tag_name = "transport_header_type",
			.u.variant_nestable.choices = transport_fields,
			.u.variant_nestable.nr_choices = ARRAY_SIZE(transport_fields),
			.u.variant_nestable.alignment = 0,
		},
	},
};

static struct lttng_event_field network_fields[] = {
	[0] = {
		.name = "unknown",
		.type = {
			.type = lttng_kernel_type_struct_nestable,
			.u.struct_nestable.nr_fields = 0,
			.u.struct_nestable.fields = emptyfields,
			.u.struct_nestable.alignment = 0,
		},
	},
	[1] = {
		.name = "ipv4",
		.type = {
			.type = lttng_kernel_type_struct_nestable,
			.u.struct_nestable.nr_fields = ARRAY_SIZE(ipv4fields),
			.u.struct_nestable.fields = ipv4fields,
			.u.struct_nestable.alignment = 0,
		},
	},
	[2] = {
		.name = "ipv6",
		.type = {
			.type = lttng_kernel_type_struct_nestable,
			.u.struct_nestable.nr_fields = ARRAY_SIZE(ipv6fields),
			.u.struct_nestable.fields = ipv6fields,
			.u.struct_nestable.alignment = 0,
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

	TP_PROTO(struct sk_buff *skb,
		 int rc,
		 struct net_device *dev,
		 unsigned int skb_len),

	TP_ARGS(skb, rc, dev, skb_len),

	TP_FIELDS(
		ctf_integer_hex(void *, skbaddr, skb)
		ctf_integer(int, rc, rc)
		ctf_integer(unsigned int, len, skb_len)
		ctf_string(name, dev->name)
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
				{
					.type = lttng_kernel_type_variant_nestable,
					.u.variant_nestable.tag_name = "network_header_type",
					.u.variant_nestable.choices = network_fields,
					.u.variant_nestable.nr_choices =
						ARRAY_SIZE(network_fields),
					.u.variant_nestable.alignment = 0,
				}
			),
			network_header,
			ctf_custom_code(
				bool has_network_header = false;

				/* Copy the network header. */
				switch (__get_network_header_type(skb)) {
				case NH_IPV4: {
					ctf_align(uint16_t)
					ctf_array_type(uint8_t, ip_hdr(skb),
							sizeof(struct iphdr))
					has_network_header = true;
					break;
				}
				case NH_IPV6: {
					ctf_align(uint16_t)
					ctf_array_type(uint8_t, ipv6_hdr(skb),
							sizeof(struct ipv6hdr))
					has_network_header = true;
					break;
				}
				default:
					/*
					 * For any other network header
					 * type, there is nothing to do.
					 */
					break;
				}

				if (has_network_header) {
					enum transport_header_types th_type =
						__get_transport_header_type(skb);

					/* Transport header type field. */
					ctf_integer_type(unsigned char, th_type)

					/* Copy the transport header. */
					switch (th_type) {
					case TH_TCP: {
						ctf_align(uint32_t)
						ctf_array_type(uint8_t, tcp_hdr(skb),
								sizeof(struct tcphdr))
						break;
					}
					case TH_UDP: {
						ctf_align(uint32_t)
						ctf_array_type(uint8_t, udp_hdr(skb),
								sizeof(struct udphdr))
						break;
					}
					case TH_ICMP: {
						ctf_align(uint32_t)
						ctf_array_type(uint8_t, icmp_hdr(skb),
								sizeof(struct icmphdr))
						break;
					}
					default:
						/*
						* For any other transport header type,
						* there is nothing to do.
						*/
						break;
					}
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

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,14,0))

/* Trace events for the receive entry points */
LTTNG_TRACEPOINT_EVENT_CLASS(net_dev_receive_entry_template,

	TP_PROTO(const struct sk_buff *skb),

	TP_ARGS(skb),

	TP_FIELDS(
		ctf_integer_hex(const void *, skbaddr, skb)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_receive_entry_template,

	napi_gro_frags_entry,

	net_napi_gro_frags_entry,

	TP_PROTO(const struct sk_buff *skb),

	TP_ARGS(skb)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_receive_entry_template,

	napi_gro_receive_entry,

	net_napi_gro_receive_entry,

	TP_PROTO(const struct sk_buff *skb),

	TP_ARGS(skb)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_receive_entry_template,

	netif_receive_skb_entry,

	net_if_receive_skb_entry,

	TP_PROTO(const struct sk_buff *skb),

	TP_ARGS(skb)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_receive_entry_template,

	netif_rx_entry,

	net_if_rx_entry,

	TP_PROTO(const struct sk_buff *skb),

	TP_ARGS(skb)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_receive_entry_template,

	netif_rx_ni_entry,

	net_if_rx_ni_entry,

	TP_PROTO(const struct sk_buff *skb),

	TP_ARGS(skb)
)

#endif /* kernel > 3.14 */

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,19,0))

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_receive_entry_template,

	netif_receive_skb_list_entry,

	net_if_receive_skb_list_entry,

	TP_PROTO(const struct sk_buff *skb),

	TP_ARGS(skb)
)

#endif /* kernel > 4.19 */

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,0,0))

/* Trace events for the receive exit points */
LTTNG_TRACEPOINT_EVENT_CLASS(net_dev_receive_exit_template,

	TP_PROTO(int ret),

	TP_ARGS(ret),

	TP_FIELDS(
		ctf_integer(int, ret, ret)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_receive_exit_template,

	napi_gro_frags_exit,

	net_napi_gro_frags_exit,

	TP_PROTO(int ret),

	TP_ARGS(ret)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_receive_exit_template,

	napi_gro_receive_exit,

	net_napi_gro_receive_exit,

	TP_PROTO(int ret),

	TP_ARGS(ret)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_receive_exit_template,

	netif_receive_skb_exit,

	net_if_receive_skb_exit,

	TP_PROTO(int ret),

	TP_ARGS(ret)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_receive_exit_template,

	netif_rx_exit,

	net_if_rx_exit,

	TP_PROTO(int ret),

	TP_ARGS(ret)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_receive_exit_template,

	netif_rx_ni_exit,

	net_if_rx_ni_exit,

	TP_PROTO(int ret),

	TP_ARGS(ret)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(net_dev_receive_exit_template,

	netif_receive_skb_list_exit,

	net_if_receive_skb_list_exit,

	TP_PROTO(int ret),

	TP_ARGS(ret)
)

#endif /* kernel > 5.0.0 */

#endif /* LTTNG_TRACE_NET_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
