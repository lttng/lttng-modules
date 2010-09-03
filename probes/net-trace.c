/*
 * ltt/probes/net-trace.c
 *
 * Net tracepoint probes.
 *
 * (C) Copyright 2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/string.h>
#include <trace/net.h>
#include <trace/ipv4.h>
#include <trace/ipv6.h>
#include <trace/socket.h>

#include "../ltt-type-serializer.h"

void probe_net_dev_xmit(void *_data, struct sk_buff *skb);

DEFINE_MARKER_TP(net, dev_xmit, net_dev_xmit, probe_net_dev_xmit,
	"skb %p protocol #n2u%hu");

notrace void probe_net_dev_xmit(void *_data, struct sk_buff *skb)
{
	struct marker *marker;
	struct serialize_long_short data;

	data.f1 = (unsigned long)skb;
	data.f2 = skb->protocol;

	marker = &GET_MARKER(net, dev_xmit);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(long));
}

void probe_net_dev_receive(void *_data, struct sk_buff *skb);

DEFINE_MARKER_TP(net, dev_receive, net_dev_receive, probe_net_dev_receive,
	"skb %p protocol #n2u%hu");

notrace void probe_net_dev_receive(void *_data, struct sk_buff *skb)
{
	struct marker *marker;
	struct serialize_long_short data;

	data.f1 = (unsigned long)skb;
	data.f2 = skb->protocol;

	marker = &GET_MARKER(net, dev_receive);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(long));
}

void probe_ipv4_addr_add(void *_data, struct in_ifaddr *ifa)
{
	trace_mark_tp(netif_state, insert_ifa_ipv4, ipv4_addr_add,
		probe_ipv4_addr_add, "label %s address #4u%u",
		ifa->ifa_label, (unsigned int)ifa->ifa_address);
}

void probe_ipv4_addr_del(void *_data, struct in_ifaddr *ifa)
{
	trace_mark_tp(netif_state, del_ifa_ipv4, ipv4_addr_del,
		probe_ipv4_addr_del, "label %s address #4u%u",
		ifa->ifa_label, (unsigned int)ifa->ifa_address);
}

void probe_ipv6_addr_add(void *_data, struct inet6_ifaddr *ifa)
{
	__u8 *addr = ifa->addr.s6_addr;

	trace_mark_tp(netif_state, insert_ifa_ipv6, ipv6_addr_add,
		probe_ipv6_addr_add,
		"label %s "
		"a15 #1x%c a14 #1x%c a13 #1x%c a12 #1x%c "
		"a11 #1x%c a10 #1x%c a9 #1x%c a8 #1x%c "
		"a7 #1x%c a6 #1x%c a5 #1x%c a4 #1x%c "
		"a3 #1x%c a2 #1x%c a1 #1x%c a0 #1x%c",
		ifa->idev->dev->name,
		addr[15], addr[14], addr[13], addr[12],
		addr[11], addr[10], addr[9], addr[8],
		addr[7], addr[6], addr[5], addr[4],
		addr[3], addr[2], addr[1], addr[0]);
}

void probe_ipv6_addr_del(void *_data, struct inet6_ifaddr *ifa)
{
	__u8 *addr = ifa->addr.s6_addr;

	trace_mark_tp(netif_state, insert_ifa_ipv6, ipv6_addr_del,
		probe_ipv6_addr_del,
		"label %s "
		"a15 #1x%c a14 #1x%c a13 #1x%c a12 #1x%c "
		"a11 #1x%c a10 #1x%c a9 #1x%c a8 #1x%c "
		"a7 #1x%c a6 #1x%c a5 #1x%c a4 #1x%c "
		"a3 #1x%c a2 #1x%c a1 #1x%c a0 #1x%c",
		ifa->idev->dev->name,
		addr[15], addr[14], addr[13], addr[12],
		addr[11], addr[10], addr[9], addr[8],
		addr[7], addr[6], addr[5], addr[4],
		addr[3], addr[2], addr[1], addr[0]);
}

void probe_socket_create(void *_data, int family, int type, int protocol,
	struct socket *sock, int ret)
{
	trace_mark_tp(net, socket_create, socket_create, probe_socket_create,
		"family %d type %d protocol %d sock %p ret %d",
		family, type, protocol, sock, ret);
}

void probe_socket_bind(void *_data, int fd, struct sockaddr __user *umyaddr, int addrlen,
	int ret)
{
	trace_mark_tp(net, socket_bind, socket_bind, probe_socket_bind,
		"fd %d umyaddr %p addrlen %d ret %d",
		fd, umyaddr, addrlen, ret);
}

void probe_socket_connect(void *_data, int fd, struct sockaddr __user *uservaddr,
	int addrlen, int ret)
{
	trace_mark_tp(net, socket_connect, socket_connect, probe_socket_connect,
		"fd %d uservaddr %p addrlen %d ret %d",
		fd, uservaddr, addrlen, ret);
}

void probe_socket_listen(void *_data, int fd, int backlog, int ret)
{
	trace_mark_tp(net, socket_listen, socket_listen, probe_socket_listen,
		"fd %d backlog %d ret %d",
		fd, backlog, ret);
}

void probe_socket_accept(void *_data, int fd, struct sockaddr __user *upeer_sockaddr,
	int __user *upeer_addrlen, int flags, int ret)
{
	trace_mark_tp(net, socket_accept, socket_accept, probe_socket_accept,
		"fd %d upeer_sockaddr %p upeer_addrlen %p flags %d ret %d",
		fd, upeer_sockaddr, upeer_addrlen, flags, ret);
}

void probe_socket_getsockname(void *_data, int fd, struct sockaddr __user *usockaddr,
	int __user *usockaddr_len, int ret)
{
	trace_mark_tp(net, socket_getsockname, socket_getsockname,
		probe_socket_getsockname,
		"fd %d usockaddr %p usockaddr_len %p ret %d",
		fd, usockaddr, usockaddr_len, ret);
}

void probe_socket_getpeername(void *_data, int fd, struct sockaddr __user *usockaddr,
	int __user *usockaddr_len, int ret)
{
	trace_mark_tp(net, socket_getpeername, socket_getpeername,
		probe_socket_getpeername,
		"fd %d usockaddr %p usockaddr_len %p ret %d",
		fd, usockaddr, usockaddr_len, ret);
}

void probe_socket_socketpair(void *_data, int family, int type, int protocol,
	int __user *usockvec, int ret)
{
	trace_mark_tp(net, socket_socketpair, socket_socketpair,
		probe_socket_socketpair,
		"family %d type %d protocol %d usockvec %p ret %d",
		family, type, protocol, usockvec, ret);
}

void probe_socket_sendmsg(void *_data, struct socket *sock, struct msghdr *msg, size_t size,
	int ret);

DEFINE_MARKER_TP(net, socket_sendmsg, net_socket_sendmsg,
	probe_socket_sendmsg,
	"sock %p msg %p size %zu ret %d");

notrace void probe_socket_sendmsg(void *_data, struct socket *sock, struct msghdr *msg,
	size_t size, int ret)
{
	struct marker *marker;
	struct serialize_long_long_sizet_int data;

	data.f1 = (unsigned long)sock;
	data.f2 = (unsigned long)msg;
	data.f3 = size;
	data.f4 = ret;

	marker = &GET_MARKER(net, socket_sendmsg);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(size_t));
}

void probe_socket_recvmsg(void *_data, struct socket *sock, struct msghdr *msg, size_t size,
	int flags, int ret);

DEFINE_MARKER_TP(net, socket_recvmsg, net_socket_recvmsg,
	probe_socket_recvmsg,
	"sock %p msg %p size %zu flags %d ret %d");

notrace void probe_socket_recvmsg(void *_data, struct socket *sock, struct msghdr *msg,
	size_t size, int flags, int ret)
{
	struct marker *marker;
	struct serialize_long_long_sizet_int_int data;

	data.f1 = (unsigned long)sock;
	data.f2 = (unsigned long)msg;
	data.f3 = size;
	data.f4 = flags;
	data.f5 = ret;

	marker = &GET_MARKER(net, socket_recvmsg);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(size_t));
}

void probe_socket_setsockopt(void *_data, int fd, int level, int optname,
	char __user *optval, int optlen, int ret)
{
	trace_mark_tp(net, socket_setsockopt, socket_setsockopt,
		probe_socket_setsockopt,
		"fd %d level %d optname %d optval %p optlen %d ret %d",
		fd, level, optname, optval, optlen, ret);
}

void probe_socket_getsockopt(void *_data, int fd, int level, int optname,
	char __user *optval, int __user *optlen, int ret)
{
	trace_mark_tp(net, socket_getsockopt, socket_getsockopt,
		probe_socket_getsockopt,
		"fd %d level %d optname %d optval %p optlen %p ret %d",
		fd, level, optname, optval, optlen, ret);
}

void probe_socket_shutdown(void *_data, int fd, int how, int ret)
{
	trace_mark_tp(net, socket_shutdown, socket_shutdown,
		probe_socket_shutdown,
		"fd %d how %d ret %d",
		fd, how, ret);
}

void probe_socket_call(void *_data, int call, unsigned long a0)
{
	trace_mark_tp(net, socket_call, socket_call, probe_socket_call,
		"call %d a0 %lu", call, a0);
}

void probe_tcpv4_rcv(void *_data, struct sk_buff *skb);

DEFINE_MARKER_TP(net, tcpv4_rcv, net_tcpv4_rcv, probe_tcpv4_rcv,
	"skb %p");

notrace void probe_tcpv4_rcv(void *_data, struct sk_buff *skb)
{
	struct marker *marker;

	marker = &GET_MARKER(net, tcpv4_rcv);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&skb, sizeof(skb), sizeof(skb));
}

void probe_udpv4_rcv(void *_data, struct sk_buff *skb);

DEFINE_MARKER_TP(net, udpv4_rcv, net_udpv4_rcv, probe_udpv4_rcv,
	"skb %p");

notrace void probe_udpv4_rcv(void *_data, struct sk_buff *skb)
{
	struct marker *marker;

	marker = &GET_MARKER(net, udpv4_rcv);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&skb, sizeof(skb), sizeof(skb));
}

#ifdef CONFIG_NETPOLL
void probe_net_napi_schedule(void *_data, struct napi_struct *n);

DEFINE_MARKER_TP(net, napi_schedule, net_napi_schedule,
	probe_net_napi_schedule,
	"napi_struct %p name %s");

notrace void probe_net_napi_schedule(void *_data, struct napi_struct *n)
{
	struct marker *marker;
	struct serialize_long_ifname data;
	size_t data_len = 0;

	data.f1 = (unsigned long)n;
	data_len += sizeof(data.f1);
	/* No need to align for strings */
	strcpy(data.f2, n->dev ? n->dev->name : "<unk>");
	data_len += strlen(data.f2) + 1;

	marker = &GET_MARKER(net, napi_schedule);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, data_len, sizeof(long));
}

void probe_net_napi_poll(void *_data, struct napi_struct *n);

DEFINE_MARKER_TP(net, napi_poll, net_napi_poll,
	probe_net_napi_poll,
	"napi_struct %p name %s");

notrace void probe_net_napi_poll(void *_data, struct napi_struct *n)
{
	struct marker *marker;
	struct serialize_long_ifname data;
	size_t data_len = 0;

	data.f1 = (unsigned long)n;
	data_len += sizeof(data.f1);
	/* No need to align for strings */
	strcpy(data.f2, n->dev ? n->dev->name : "<unk>");
	data_len += strlen(data.f2) + 1;

	marker = &GET_MARKER(net, napi_poll);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, data_len, sizeof(long));
}

void probe_net_napi_complete(void *_data, struct napi_struct *n);

DEFINE_MARKER_TP(net, napi_complete, net_napi_complete,
	probe_net_napi_complete,
	"napi_struct %p name %s");

notrace void probe_net_napi_complete(void *_data, struct napi_struct *n)
{
	struct marker *marker;
	struct serialize_long_ifname data;
	size_t data_len = 0;

	data.f1 = (unsigned long)n;
	data_len += sizeof(data.f1);
	/* No need to align for strings */
	strcpy(data.f2, n->dev ? n->dev->name : "<unk>");
	data_len += strlen(data.f2) + 1;

	marker = &GET_MARKER(net, napi_complete);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, data_len, sizeof(long));
}
#else /* !CONFIG_NETPOLL */
void probe_net_napi_schedule(void *_data, struct napi_struct *n);

DEFINE_MARKER_TP(net, napi_schedule, net_napi_schedule,
	probe_net_napi_schedule,
	"napi_struct %p");

notrace void probe_net_napi_schedule(void *_data, struct napi_struct *n)
{
	struct marker *marker;
	unsigned long data;

	data = (unsigned long)n;

	marker = &GET_MARKER(net, napi_schedule);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, sizeof(data), sizeof(data));
}

void probe_net_napi_poll(void *_data, struct napi_struct *n);

DEFINE_MARKER_TP(net, napi_poll, net_napi_poll,
	probe_net_napi_poll,
	"napi_struct %p");

notrace void probe_net_napi_poll(void *_data, struct napi_struct *n)
{
	struct marker *marker;
	unsigned long data;

	data = (unsigned long)n;

	marker = &GET_MARKER(net, napi_poll);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, sizeof(data), sizeof(data));
}

void probe_net_napi_complete(void *_data, struct napi_struct *n);

DEFINE_MARKER_TP(net, napi_complete, net_napi_complete,
	probe_net_napi_complete,
	"napi_struct %p");

notrace void probe_net_napi_complete(void *_data, struct napi_struct *n)
{
	struct marker *marker;
	unsigned long data;

	data = (unsigned long)n;

	marker = &GET_MARKER(net, napi_complete);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, sizeof(data), sizeof(data));
}
#endif

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Net Tracepoint Probes");
