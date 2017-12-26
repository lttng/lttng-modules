#undef TRACE_SYSTEM
#define TRACE_SYSTEM sock

#if !defined(LTTNG_TRACE_SOCK_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_SOCK_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/version.h>
#include <net/sock.h>

LTTNG_TRACEPOINT_EVENT(sock_rcvqueue_full,

	TP_PROTO(struct sock *sk, struct sk_buff *skb),

	TP_ARGS(sk, skb),

	TP_FIELDS(
		ctf_integer(int, rmem_alloc, atomic_read(&sk->sk_rmem_alloc))
		ctf_integer(unsigned int, truesize, skb->truesize)
		ctf_integer(int, sk_rcvbuf, sk->sk_rcvbuf)
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0))

LTTNG_TRACEPOINT_EVENT(sock_exceed_buf_limit,

	TP_PROTO(struct sock *sk, struct proto *prot, long allocated),

	TP_ARGS(sk, prot, allocated),

	TP_FIELDS(
		ctf_string(name, prot->name)
		ctf_array(long, sysctl_mem, prot->sysctl_mem, 3)
		ctf_integer(long, allocated, allocated)
		ctf_integer(int, sysctl_rmem, sk_get_rmem0(sk, prot))
		ctf_integer(int, rmem_alloc, atomic_read(&sk->sk_rmem_alloc))
	)
)

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)) */

LTTNG_TRACEPOINT_EVENT(sock_exceed_buf_limit,

	TP_PROTO(struct sock *sk, struct proto *prot, long allocated),

	TP_ARGS(sk, prot, allocated),

	TP_FIELDS(
		ctf_string(name, prot->name)
		ctf_array(long, sysctl_mem, prot->sysctl_mem, 3)
		ctf_integer(long, allocated, allocated)
		ctf_integer(int, sysctl_rmem, prot->sysctl_rmem[0])
		ctf_integer(int, rmem_alloc, atomic_read(&sk->sk_rmem_alloc))
	)
)

#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)) */

#endif /* LTTNG_TRACE_SOCK_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
