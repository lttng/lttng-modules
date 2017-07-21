#ifndef CREATE_SYSCALL_TABLE

#define OVERRIDE_64_connect
SC_LTTNG_TRACEPOINT_EVENT_CODE(connect,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr __user * uservaddr, int addrlen),
	TP_ARGS(sc_exit(ret,) fd, uservaddr, addrlen),
	TP_locvar(
		__typeof__(uservaddr->sa_family) sa_family;
		uint16_t dport;
		uint32_t v4addr;
		uint16_t v6addr[8];
		unsigned int v4addr_len, v6addr_len;
	),
	TP_code_pre(
		sc_in(
			memset(tp_locvar, 0, sizeof(*tp_locvar));
			if (addrlen < sizeof(tp_locvar->sa_family))
				goto skip_code;
			(void) lib_ring_buffer_copy_from_user_check_nofault(&tp_locvar->sa_family,
					&uservaddr->sa_family, sizeof(tp_locvar->sa_family));
			switch (tp_locvar->sa_family) {
			case AF_INET:
				if (addrlen < sizeof(struct sockaddr_in))
					goto skip_code;
				(void) lib_ring_buffer_copy_from_user_check_nofault(&tp_locvar->dport,
						&((struct sockaddr_in __user *) uservaddr)->sin_port,
						sizeof(tp_locvar->dport));
				(void) lib_ring_buffer_copy_from_user_check_nofault(&tp_locvar->v4addr,
						&((struct sockaddr_in __user *) uservaddr)->sin_addr.s_addr,
						sizeof(tp_locvar->v4addr));
				tp_locvar->v4addr_len = 4;
				break;
			case AF_INET6:
				if (addrlen < sizeof(struct sockaddr_in6))
					goto skip_code;
				(void) lib_ring_buffer_copy_from_user_check_nofault(&tp_locvar->dport,
						&((struct sockaddr_in6 __user *) uservaddr)->sin6_port,
						sizeof(tp_locvar->dport));
				if (lib_ring_buffer_copy_from_user_check_nofault(tp_locvar->v6addr,
						&((struct sockaddr_in6 __user *) uservaddr)->sin6_addr.in6_u.u6_addr8,
						sizeof(tp_locvar->v6addr)))
					memset(tp_locvar->v6addr, 0, sizeof(tp_locvar->v6addr));
				tp_locvar->v6addr_len = 8;
				break;
			}
		skip_code:
		)
	),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer_hex(struct sockaddr *, uservaddr, uservaddr))
		sc_in(ctf_integer_hex(int, addrlen, addrlen))
		sc_in(ctf_integer(unsigned short, family, tp_locvar->sa_family))
		sc_in(ctf_integer_network(uint16_t, dport, tp_locvar->dport))
		sc_in(ctf_sequence_network(uint8_t, v4addr, &tp_locvar->v4addr, unsigned int, tp_locvar->v4addr_len))
		sc_in(ctf_sequence_network(uint16_t, v6addr, &tp_locvar->v6addr, unsigned int, tp_locvar->v6addr_len))
	),
	TP_code_post()
)

#define LTTNG_SYSCALL_ACCEPT_locvar			\
	__typeof__(upeer_sockaddr->sa_family) sa_family;\
	uint16_t sport;					\
	uint32_t v4addr;				\
	uint16_t v6addr[8];				\
	int v4addr_len, v6addr_len;			\
	int uaddr_len;

#define LTTNG_SYSCALL_ACCEPT_code_pre											\
	sc_inout(													\
		memset(tp_locvar, 0, sizeof(*tp_locvar));								\
		(void) lib_ring_buffer_copy_from_user_check_nofault(&tp_locvar->uaddr_len,				\
				upeer_addrlen, sizeof(tp_locvar->uaddr_len));						\
	)														\
	sc_out(														\
		if (tp_locvar->uaddr_len < sizeof(tp_locvar->sa_family))						\
			goto skip_code;											\
		(void) lib_ring_buffer_copy_from_user_check_nofault(&tp_locvar->sa_family,				\
				&upeer_sockaddr->sa_family, sizeof(tp_locvar->sa_family));				\
		switch (tp_locvar->sa_family) {										\
		case AF_INET:												\
			if (tp_locvar->uaddr_len < sizeof(struct sockaddr_in))						\
				goto skip_code;										\
			(void) lib_ring_buffer_copy_from_user_check_nofault(&tp_locvar->sport,				\
					&((struct sockaddr_in __user *) upeer_sockaddr)->sin_port,			\
					sizeof(tp_locvar->sport));							\
			(void) lib_ring_buffer_copy_from_user_check_nofault(&tp_locvar->v4addr,				\
					&((struct sockaddr_in __user *) upeer_sockaddr)->sin_addr.s_addr,		\
					sizeof(tp_locvar->v4addr));							\
			tp_locvar->v4addr_len = 4;									\
			break;												\
		case AF_INET6:												\
			if (tp_locvar->uaddr_len < sizeof(struct sockaddr_in6))						\
				goto skip_code;										\
			(void) lib_ring_buffer_copy_from_user_check_nofault(&tp_locvar->sport,				\
					&((struct sockaddr_in6 __user *) upeer_sockaddr)->sin6_port,			\
					sizeof(tp_locvar->sport));							\
			if (lib_ring_buffer_copy_from_user_check_nofault(tp_locvar->v6addr,				\
					&((struct sockaddr_in6 __user *) upeer_sockaddr)->sin6_addr.in6_u.u6_addr8,	\
					sizeof(tp_locvar->v6addr)))							\
				memset(tp_locvar->v6addr, 0, sizeof(tp_locvar->v6addr));				\
			tp_locvar->v6addr_len = 8;									\
			break;												\
		}													\
	skip_code:													\
	)

#define OVERRIDE_64_accept
SC_LTTNG_TRACEPOINT_EVENT_CODE(accept,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr __user * upeer_sockaddr, int __user * upeer_addrlen),
	TP_ARGS(sc_exit(ret,) fd, upeer_sockaddr, upeer_addrlen),
	TP_locvar(
		LTTNG_SYSCALL_ACCEPT_locvar
	),
	TP_code_pre(
		LTTNG_SYSCALL_ACCEPT_code_pre
	),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer_hex(struct sockaddr *, upeer_sockaddr, upeer_sockaddr))
		sc_inout(ctf_integer(int, upeer_addrlen, tp_locvar->uaddr_len))
		sc_out(ctf_integer(unsigned short, family, tp_locvar->sa_family))
		sc_out(ctf_integer_network(uint16_t, sport, tp_locvar->sport))
		sc_out(ctf_sequence_network(uint8_t, v4addr, &tp_locvar->v4addr, unsigned int, tp_locvar->v4addr_len))
		sc_out(ctf_sequence_network(uint16_t, v6addr, &tp_locvar->v6addr, unsigned int, tp_locvar->v6addr_len))
	),
	TP_code_post()
)

#define OVERRIDE_64_accept4
SC_LTTNG_TRACEPOINT_EVENT_CODE(accept4,
	TP_PROTO(sc_exit(long ret,) int fd, struct sockaddr __user * upeer_sockaddr, int __user * upeer_addrlen, int flags),
	TP_ARGS(sc_exit(ret,) fd, upeer_sockaddr, upeer_addrlen, flags),
	TP_locvar(
		LTTNG_SYSCALL_ACCEPT_locvar
	),
	TP_code_pre(
		LTTNG_SYSCALL_ACCEPT_code_pre
	),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(ctf_integer_hex(struct sockaddr *, upeer_sockaddr, upeer_sockaddr))
		sc_inout(ctf_integer(int, upeer_addrlen, tp_locvar->uaddr_len))
		sc_in(ctf_integer_hex(int, flags, flags))
		sc_out(ctf_integer(unsigned short, family, tp_locvar->sa_family))
		sc_out(ctf_integer_network(uint16_t, sport, tp_locvar->sport))
		sc_out(ctf_sequence_network(uint8_t, v4addr, &tp_locvar->v4addr, unsigned int, tp_locvar->v4addr_len))
		sc_out(ctf_sequence_network(uint16_t, v6addr, &tp_locvar->v6addr, unsigned int, tp_locvar->v6addr_len))
	),
	TP_code_post()
)

#define OVERRIDE_64_pipe
SC_LTTNG_TRACEPOINT_EVENT(pipe,
	TP_PROTO(sc_exit(long ret,) int * fildes),
	TP_ARGS(sc_exit(ret,) fildes),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_out(ctf_user_array(int, fildes, fildes, 2))
	)
)

#else	/* CREATE_SYSCALL_TABLE */

#define OVERRIDE_TABLE_64_clone
TRACE_SYSCALL_TABLE(clone, clone, 56, 5)
#define OVERRIDE_TABLE_64_execve
TRACE_SYSCALL_TABLE(execve, execve, 59, 3)
#define OVERRIDE_TABLE_64_getcpu
TRACE_SYSCALL_TABLE(getcpu, getcpu, 309, 3)

#endif /* CREATE_SYSCALL_TABLE */
