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

#ifdef LTTNG_CREATE_FIELD_METADATA

static const struct lttng_kernel_event_field *emptyfields[] = {
};

/* Structures for transport headers. */

static const struct lttng_kernel_event_field *tcpfields[] = {
	[0] = lttng_kernel_static_event_field("source_port",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 10),
		false, false, false),
	[1] = lttng_kernel_static_event_field("dest_port",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 10),
		false, false, false),
	[2] = lttng_kernel_static_event_field("seq",
		lttng_kernel_static_type_integer_from_type(uint32_t, __BIG_ENDIAN, 10),
		false, false, false),
	[3] = lttng_kernel_static_event_field("ack_seq",
		lttng_kernel_static_type_integer_from_type(uint32_t, __BIG_ENDIAN, 10),
		false, false, false),
	[4] = lttng_kernel_static_event_field("data_offset",
		lttng_kernel_static_type_integer(4, 4, 0, __BIG_ENDIAN, 10),
		false, false, false),
	[5] = lttng_kernel_static_event_field("reserved",
		lttng_kernel_static_type_integer(3, 1, 0, __BIG_ENDIAN, 10),
		false, false, false),
	[6] = lttng_kernel_static_event_field("flags",
		lttng_kernel_static_type_integer(9, 1, 0, __BIG_ENDIAN, 16),
		false, false, false),
	[7] = lttng_kernel_static_event_field("window_size",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 10),
		false, false, false),
	[8] = lttng_kernel_static_event_field("checksum",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 16),
		false, false, false),
	[9] = lttng_kernel_static_event_field("urg_ptr",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 10),
		false, false, false),
};

static const struct lttng_kernel_event_field *udpfields[] = {
	[0] = lttng_kernel_static_event_field("source_port",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 10),
		false, false, false),
	[1] = lttng_kernel_static_event_field("dest_port",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 10),
		false, false, false),
	[2] = lttng_kernel_static_event_field("len",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 10),
		false, false, false),
	[3] = lttng_kernel_static_event_field("check",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 10),
		false, false, false),
};

static const struct lttng_kernel_event_field *icmpfields[] = {
	[0] = lttng_kernel_static_event_field("type",
		lttng_kernel_static_type_integer_from_type(uint8_t, __BIG_ENDIAN, 10),
		false, false, false),
	[1] = lttng_kernel_static_event_field("code",
		lttng_kernel_static_type_integer_from_type(uint8_t, __BIG_ENDIAN, 10),
		false, false, false),
	[2] = lttng_kernel_static_event_field("checksum",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 10),
		false, false, false),
	[3] = lttng_kernel_static_event_field("gateway",
		lttng_kernel_static_type_integer_from_type(uint32_t, __BIG_ENDIAN, 10),
		false, false, false),
};

static const struct lttng_kernel_event_field *transport_fields[] = {
	[0] = lttng_kernel_static_event_field("unknown",
		lttng_kernel_static_type_struct(ARRAY_SIZE(emptyfields), emptyfields, 0),
		false, false, true),
	[1] = lttng_kernel_static_event_field("tcp",
		lttng_kernel_static_type_struct(ARRAY_SIZE(tcpfields), tcpfields, 0),
		false, false, true),
	[2] = lttng_kernel_static_event_field("udp",
		lttng_kernel_static_type_struct(ARRAY_SIZE(udpfields), udpfields, 0),
		false, false, true),
	[3] = lttng_kernel_static_event_field("icmp",
		lttng_kernel_static_type_struct(ARRAY_SIZE(icmpfields), icmpfields, 0),
		false, false, true),
};

#endif /* LTTNG_CREATE_FIELD_METADATA */

LTTNG_TRACEPOINT_ENUM(proto_transport_header_type,
	TP_ENUM_VALUES(
		ctf_enum_range("_unknown", 0, IPPROTO_ICMP - 1)
		ctf_enum_value("_icmp", IPPROTO_ICMP)
		ctf_enum_range("_unknown", IPPROTO_ICMP + 1, IPPROTO_TCP - 1)
		ctf_enum_value("_tcp", IPPROTO_TCP)
		ctf_enum_range("_unknown", IPPROTO_TCP + 1, IPPROTO_UDP - 1)
		ctf_enum_value("_udp", IPPROTO_UDP)
		ctf_enum_range("_unknown", IPPROTO_UDP + 1, 255)
	)
)

LTTNG_TRACEPOINT_ENUM(transport_header_type,
	TP_ENUM_VALUES(
		ctf_enum_value("_unknown", TH_NONE)
		ctf_enum_value("_tcp", TH_TCP)
		ctf_enum_value("_udp", TH_UDP)
		ctf_enum_value("_icmp", TH_ICMP)
	)
)

#ifdef LTTNG_CREATE_FIELD_METADATA

/* Structures for network headers. */

static const struct lttng_kernel_event_field *ipv4fields[] = {
	[0] = lttng_kernel_static_event_field("version",
		lttng_kernel_static_type_integer(4, 4, 0, __BIG_ENDIAN, 10),
		false, false, false),
	[1] = lttng_kernel_static_event_field("ihl",
		lttng_kernel_static_type_integer(4, 4, 0, __BIG_ENDIAN, 10),
		false, false, false),
	[2] = lttng_kernel_static_event_field("tos",
		lttng_kernel_static_type_integer_from_type(uint8_t, __BIG_ENDIAN, 10),
		false, false, false),
	[3] = lttng_kernel_static_event_field("tot_len",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 10),
		false, false, false),
	[4] = lttng_kernel_static_event_field("id",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 16),
		false, false, false),
	[5] = lttng_kernel_static_event_field("frag_off",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 10),
		false, false, false),
	[6] = lttng_kernel_static_event_field("ttl",
		lttng_kernel_static_type_integer_from_type(uint8_t, __BIG_ENDIAN, 10),
		false, false, false),
	[7] = lttng_kernel_static_event_field("protocol",
		lttng_kernel_static_type_enum(&__enum_proto_transport_header_type,
			lttng_kernel_static_type_integer_from_type(uint8_t, __BIG_ENDIAN, 10)),
		false, false, false),
	[8] = lttng_kernel_static_event_field("checksum",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 16),
		false, false, false),
	[9] = lttng_kernel_static_event_field("saddr",
		lttng_kernel_static_type_array(4,
			lttng_kernel_static_type_integer_from_type(uint8_t, __BIG_ENDIAN, 10),
			lttng_alignof(uint8_t), none),
		false, false, false),
	[10] = lttng_kernel_static_event_field("daddr",
		lttng_kernel_static_type_array(4,
			lttng_kernel_static_type_integer_from_type(uint8_t, __BIG_ENDIAN, 10),
			lttng_alignof(uint8_t), none),
		false, false, false),
	[11] = lttng_kernel_static_event_field("transport_header_type",
		lttng_kernel_static_type_enum(&__enum_transport_header_type,
			lttng_kernel_static_type_integer_from_type(uint8_t, __BYTE_ORDER, 10)),
		false, false, false),
	[12] = lttng_kernel_static_event_field("transport_header",
		lttng_kernel_static_type_variant(ARRAY_SIZE(transport_fields), transport_fields,
			"transport_header_type", 0),
		false, false, false),
};

static const struct lttng_kernel_event_field *ipv6fields[] = {
	[0] = lttng_kernel_static_event_field("version",
		lttng_kernel_static_type_integer(4, 4, 0, __BIG_ENDIAN, 10),
		false, false, false),
	[1] = lttng_kernel_static_event_field("prio",
		lttng_kernel_static_type_integer(4, 4, 0, __BIG_ENDIAN, 10),
		false, false, false),
	[2] = lttng_kernel_static_event_field("flow_lbl",
		lttng_kernel_static_type_array(3, lttng_kernel_static_type_integer_from_type(uint8_t, __BIG_ENDIAN, 16), lttng_alignof(uint8_t), none),
		false, false, false),
	[3] = lttng_kernel_static_event_field("payload_len",
		lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 10),
		false, false, false),
	[4] = lttng_kernel_static_event_field("nexthdr",
		lttng_kernel_static_type_enum(&__enum_proto_transport_header_type,
			lttng_kernel_static_type_integer_from_type(uint8_t, __BIG_ENDIAN, 10)),
		false, false, false),
	[5] = lttng_kernel_static_event_field("hop_limit",
		lttng_kernel_static_type_integer_from_type(uint8_t, __BIG_ENDIAN, 10),
		false, false, false),
	[6] = lttng_kernel_static_event_field("saddr",
		lttng_kernel_static_type_array(8, lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 16), lttng_alignof(uint16_t), none),
		false, false, false),
	[7] = lttng_kernel_static_event_field("daddr",
		lttng_kernel_static_type_array(8, lttng_kernel_static_type_integer_from_type(uint16_t, __BIG_ENDIAN, 16), lttng_alignof(uint16_t), none),
		false, false, false),
	[8] = lttng_kernel_static_event_field("transport_header_type",
		lttng_kernel_static_type_enum(&__enum_transport_header_type,
			lttng_kernel_static_type_integer_from_type(uint8_t, __BYTE_ORDER, 10)),
		false, false, false),
	[9] = lttng_kernel_static_event_field("transport_header",
		lttng_kernel_static_type_variant(ARRAY_SIZE(transport_fields),
			transport_fields, "transport_header_type", 0),
		false, false, false),
};

static const struct lttng_kernel_event_field *network_fields[] = {
	[0] = lttng_kernel_static_event_field("unknown",
		lttng_kernel_static_type_struct(0, emptyfields, 0),
		false, false, false),
	[1] = lttng_kernel_static_event_field("ipv4",
		lttng_kernel_static_type_struct(ARRAY_SIZE(ipv4fields), ipv4fields, 0),
		false, false, false),
	[2] = lttng_kernel_static_event_field("ipv6",
		lttng_kernel_static_type_struct(ARRAY_SIZE(ipv6fields), ipv6fields, 0),
		false, false, false),
};

#endif /* LTTNG_CREATE_FIELD_METADATA */

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
				lttng_kernel_static_type_variant(ARRAY_SIZE(network_fields),
					network_fields, "network_header_type", 0)
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
