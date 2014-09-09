#ifndef CREATE_SYSCALL_TABLE

#define OVERRIDE_64_connect
SC_LTTNG_TRACEPOINT_EVENT_CODE(connect,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * uservaddr, int addrlen),
	TP_ARGS(sc_exit(ret,) fd, uservaddr, addrlen),
	TP_locvar(
		__typeof__(uservaddr->sa_family) sa_family;
		uint16_t dport;
		uint32_t v4addr;
		uint16_t v6addr[8];
		int v4addr_len, v6addr_len;
	),
	TP_code(
		sc_in(
			memset(tp_locvar, 0, sizeof(*tp_locvar));
			if (addrlen < sizeof(struct sockaddr))
				goto skip_code;
			(void) get_user(tp_locvar->sa_family, &uservaddr->sa_family);
			switch (tp_locvar->sa_family) {
			case AF_INET:
				if (addrlen < sizeof(struct sockaddr_in))
					goto skip_code;
				(void) get_user(tp_locvar->dport, &((struct sockaddr_in *) uservaddr)->sin_port);
				(void) get_user(tp_locvar->v4addr, &((struct sockaddr_in *) uservaddr)->sin_addr.s_addr);
				tp_locvar->v4addr_len = 4;
				break;
			case AF_INET6:
				if (addrlen < sizeof(struct sockaddr_in6))
					goto skip_code;
				(void) get_user(tp_locvar->dport, &((struct sockaddr_in6 *) uservaddr)->sin6_port);
				(void) copy_from_user(tp_locvar->v6addr,
						&((struct sockaddr_in6 *) uservaddr)->sin6_addr.in6_u.u6_addr8,
						sizeof(tp_locvar->v6addr));
				tp_locvar->v6addr_len = 8;
				break;
			}
		skip_code:
		)
	),
	TP_STRUCT__entry(
		sc_exit(__field(long, ret))
		sc_in(__field(int, fd))
		sc_in(__field_hex(struct sockaddr *, uservaddr))
		sc_in(__field_hex(int, addrlen))
		sc_in(__field(int, family))
		sc_in(__field_network(uint16_t, dport))
		sc_in(__dynamic_array_network(uint8_t, v4addr, tp_locvar->v4addr_len))
		sc_in(__dynamic_array_network_hex(uint16_t, v6addr, tp_locvar->v6addr_len))
	),
	TP_fast_assign(
		sc_exit(tp_assign(ret, ret))
		sc_in(tp_assign(fd, fd))
		sc_in(tp_assign(uservaddr, uservaddr))
		sc_in(tp_assign(addrlen, addrlen))
		sc_in(tp_assign(family, tp_locvar->sa_family))
		sc_in(tp_assign(dport, tp_locvar->dport))
		sc_in(tp_memcpy_dyn(v4addr, &tp_locvar->v4addr))
		sc_in(tp_memcpy_dyn(v6addr, &tp_locvar->v6addr))
	),
	TP_printk()
)

#define OVERRIDE_64_accept
SC_LTTNG_TRACEPOINT_EVENT_CODE(accept,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr * upeer_sockaddr, int * upeer_addrlen),
	TP_ARGS(sc_exit(ret,) fd, upeer_sockaddr, upeer_addrlen),
	TP_locvar(
		__typeof__(upeer_sockaddr->sa_family) sa_family;
		uint16_t sport;
		uint32_t v4addr;
		uint16_t v6addr[8];
		int v4addr_len, v6addr_len;
		int uaddr_len;
	),
	TP_code(
		sc_inout(
			memset(tp_locvar, 0, sizeof(*tp_locvar));
			(void) get_user(tp_locvar->uaddr_len, upeer_addrlen);
		)
		sc_out(
			if (tp_locvar->uaddr_len < sizeof(struct sockaddr))
				goto skip_code;
			(void) get_user(tp_locvar->sa_family, &upeer_sockaddr->sa_family);
			switch (tp_locvar->sa_family) {
			case AF_INET:
				if (tp_locvar->uaddr_len < sizeof(struct sockaddr_in))
					goto skip_code;
				(void) get_user(tp_locvar->sport, &((struct sockaddr_in *) upeer_sockaddr)->sin_port);
				(void) get_user(tp_locvar->v4addr, &((struct sockaddr_in *) upeer_sockaddr)->sin_addr.s_addr);
				tp_locvar->v4addr_len = 4;
				break;
			case AF_INET6:
				if (tp_locvar->uaddr_len < sizeof(struct sockaddr_in6))
					goto skip_code;
				(void) get_user(tp_locvar->sport, &((struct sockaddr_in6 *) upeer_sockaddr)->sin6_port);
				(void) copy_from_user(tp_locvar->v6addr,
						&((struct sockaddr_in6 *) upeer_sockaddr)->sin6_addr.in6_u.u6_addr8,
						sizeof(tp_locvar->v6addr));
				tp_locvar->v6addr_len = 8;
				break;
			}
		skip_code:
		)
	),
	TP_STRUCT__entry(
		sc_exit(__field(long, ret))
		sc_in(__field(int, fd))
		sc_in(__field_hex(struct sockaddr *, upeer_sockaddr))
		sc_inout(__field(int, upeer_addrlen))
		sc_out(__field(int, family))
		sc_out(__field_network(uint16_t, sport))
		sc_out(__dynamic_array_network(uint8_t, v4addr, tp_locvar->v4addr_len))
		sc_out(__dynamic_array_network_hex(uint16_t, v6addr, tp_locvar->v6addr_len))
	),
	TP_fast_assign(
		sc_exit(tp_assign(ret, ret))
		sc_in(tp_assign(fd, fd))
		sc_in(tp_assign(upeer_sockaddr, upeer_sockaddr))
		sc_inout(tp_assign(upeer_addrlen, tp_locvar->uaddr_len))
		sc_out(tp_assign(family, tp_locvar->sa_family))
		sc_out(tp_assign(sport, tp_locvar->sport))
		sc_out(tp_memcpy_dyn(v4addr, &tp_locvar->v4addr))
		sc_out(tp_memcpy_dyn(v6addr, &tp_locvar->v6addr))
	),
	TP_printk()
)

#else	/* CREATE_SYSCALL_TABLE */

#define OVERRIDE_TABLE_64_clone
TRACE_SYSCALL_TABLE(clone, clone, 56, 5)
#define OVERRIDE_TABLE_64_execve
TRACE_SYSCALL_TABLE(execve, execve, 59, 3)
#define OVERRIDE_TABLE_64_getcpu
TRACE_SYSCALL_TABLE(getcpu, getcpu, 309, 3)

#endif /* CREATE_SYSCALL_TABLE */
