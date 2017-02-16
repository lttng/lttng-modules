#ifndef CREATE_SYSCALL_TABLE

#define OVERRIDE_32_execve
#define OVERRIDE_64_execve
SC_LTTNG_TRACEPOINT_EVENT(execve,
	TP_PROTO(sc_exit(long ret,) const char *filename, char *const *argv, char *const *envp),
	TP_ARGS(sc_exit(ret,) filename, argv, envp),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_user_string(filename, filename))
		sc_in(ctf_integer_hex(char *const *, argv, argv))
		sc_in(ctf_integer_hex(char *const *, envp, envp))
	)
)

#define OVERRIDE_32_clone
#define OVERRIDE_64_clone
SC_LTTNG_TRACEPOINT_EVENT(clone,
	TP_PROTO(sc_exit(long ret,) unsigned long clone_flags, unsigned long newsp,
		void __user *parent_tid,
		void __user *child_tid),
	TP_ARGS(sc_exit(ret,) clone_flags, newsp, parent_tid, child_tid),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer_hex(unsigned long, clone_flags, clone_flags))
		sc_in(ctf_integer_hex(unsigned long, newsp, newsp))
		sc_in(ctf_integer_hex(void *, parent_tid, parent_tid))
		sc_in(ctf_integer_hex(void *, child_tid, child_tid))
	)
)

/* present in 32, missing in 64 due to old kernel headers */
#define OVERRIDE_32_getcpu
#define OVERRIDE_64_getcpu
SC_LTTNG_TRACEPOINT_EVENT(getcpu,
	TP_PROTO(sc_exit(long ret,) unsigned __user *cpup, unsigned __user *nodep, void *tcache),
	TP_ARGS(sc_exit(ret,) cpup, nodep, tcache),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_out(ctf_integer_hex(unsigned *, cpup, cpup))
		sc_out(ctf_integer_hex(unsigned *, nodep, nodep))
		sc_inout(ctf_integer_hex(void *, tcache, tcache))
	)
)

#define OVERRIDE_32_pipe2
#define OVERRIDE_64_pipe2
SC_LTTNG_TRACEPOINT_EVENT(pipe2,
	TP_PROTO(sc_exit(long ret,) int * fildes, int flags),
	TP_ARGS(sc_exit(ret,) fildes, flags),
	TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
		sc_out(ctf_user_array(int, fildes, fildes, 2))
		sc_in(ctf_integer(int, flags, flags))
	)
)

#define LTTNG_SYSCALL_SELECT_locvar			\
	unsigned long *fds_in, *fds_out, *fds_ex;	\
	unsigned long nr_bytes, nr_ulong; 		\
	uint8_t overflow;

#define LTTNG_SYSCALL_SELECT_code_pre							\
	sc_inout(									\
	{										\
		int err;								\
		unsigned int n_in_bytes;						\
											\
		tp_locvar->fds_in = NULL;						\
		tp_locvar->fds_out = NULL;						\
		tp_locvar->fds_ex = NULL;						\
		tp_locvar->overflow = 0;						\
											\
		sc_out(									\
			if (ret <= 0)							\
				goto error;						\
		)									\
											\
		if (n <= 0)								\
			goto error;							\
											\
		/* On error or bogus input, don't copy anything. */			\
		if (n >__FD_SETSIZE)							\
			goto error;							\
											\
		n_in_bytes = DIV_ROUND_UP((unsigned int) n, BITS_PER_BYTE);		\
											\
		/*									\
		 * Limit atomic memory allocation to one page, since n			\
		 * is limited to 1024 and the smallest page size on Linux		\
		 * is 4k, this should not happen, don't try to make it work.		\
		 */									\
		if (n_in_bytes > PAGE_SIZE) {						\
			WARN_ON_ONCE(1);						\
			/* Inform the user that we did not output everything. */	\
			tp_locvar->overflow = 1;					\
			goto error;							\
		} else {								\
			tp_locvar->nr_bytes = n_in_bytes;				\
			tp_locvar->nr_ulong = DIV_ROUND_UP(n_in_bytes,			\
					sizeof(unsigned long));				\
		}									\
											\
		if (inp) {								\
			tp_locvar->fds_in = kmalloc(					\
					tp_locvar->nr_ulong * sizeof(unsigned long),	\
					GFP_ATOMIC | GFP_NOWAIT);			\
			if (!tp_locvar->fds_in)						\
				goto error;						\
											\
			err = lib_ring_buffer_copy_from_user_check_nofault(		\
					tp_locvar->fds_in, inp,				\
					tp_locvar->nr_ulong * sizeof(unsigned long));	\
			if (err != 0)							\
				goto error;						\
		}									\
		if (outp) {								\
			tp_locvar->fds_out = kmalloc(					\
					tp_locvar->nr_ulong * sizeof(unsigned long),	\
					GFP_ATOMIC | GFP_NOWAIT);			\
			if (!tp_locvar->fds_out)					\
				goto error;						\
											\
			err = lib_ring_buffer_copy_from_user_check_nofault(		\
					tp_locvar->fds_out, outp,			\
					tp_locvar->nr_ulong * sizeof(unsigned long));	\
			if (err != 0)							\
				goto error;						\
		}									\
		if (exp) {								\
			tp_locvar->fds_ex = kmalloc(					\
					tp_locvar->nr_ulong * sizeof(unsigned long),	\
					GFP_ATOMIC | GFP_NOWAIT);			\
			if (!tp_locvar->fds_ex)						\
				goto error;						\
											\
			err = lib_ring_buffer_copy_from_user_check_nofault(		\
					tp_locvar->fds_ex, exp,				\
					tp_locvar->nr_ulong * sizeof(unsigned long));	\
			if (err != 0)							\
				goto error;						\
		}									\
		goto end;								\
											\
error:											\
	tp_locvar->nr_bytes = 0;							\
	tp_locvar->nr_ulong = 0;							\
end:	; /* Label at end of compound statement. */					\
	}										\
	)

#define LTTNG_SYSCALL_SELECT_fds_field_LE(name, input)							\
	ctf_custom_field(										\
		ctf_custom_type(									\
			.atype = atype_sequence,							\
			.u.sequence.length_type = __type_integer(					\
				uint8_t, 0, 0, 0, __BYTE_ORDER, 10, none),				\
			.u.sequence.elem_type = __type_integer(uint8_t, 0, 0, 0,			\
				__BYTE_ORDER, 16, none),						\
		),											\
		name,											\
		ctf_custom_code(									\
			unsigned int src;								\
			unsigned int nr_bytes_out = 0;							\
													\
			if (input) {									\
				ctf_integer_type(uint8_t, tp_locvar->nr_bytes)				\
				ctf_align(uint8_t)							\
			} else {									\
				ctf_integer_type(uint8_t, 0)						\
				ctf_align(uint8_t)							\
				goto skip_##name;							\
			}										\
													\
			for (src = 0; src < tp_locvar->nr_ulong; src++) {				\
				int dst;								\
				for (dst = 0; dst < sizeof(long); dst++) {				\
					if (nr_bytes_out++ >= tp_locvar->nr_bytes) {			\
						goto skip_##name;					\
					}								\
					ctf_user_integer_type(uint8_t,					\
						((uint8_t __user *) (input->fds_bits + src))[dst]);	\
				}									\
			}										\
		skip_##name:   ;									\
		)											\
	)

#define LTTNG_SYSCALL_SELECT_fds_field_BE(name, input)							\
	ctf_custom_field(										\
		ctf_custom_type(									\
			.atype = atype_sequence,							\
			.u.sequence.length_type = __type_integer(					\
				uint8_t, 0, 0, 0, __BYTE_ORDER, 10, none),				\
			.u.sequence.elem_type = __type_integer(uint8_t, 0, 0, 0,			\
				__BYTE_ORDER, 16, none),						\
		),											\
		name,											\
		ctf_custom_code(									\
			unsigned int src, nr_bytes_out = 0;						\
													\
			if (input) {									\
				ctf_integer_type(uint8_t, tp_locvar->nr_bytes)				\
				ctf_align(uint8_t)							\
			} else {									\
				ctf_integer_type(uint8_t, 0)						\
				ctf_align(uint8_t)							\
				goto skip_##name;							\
			}										\
													\
			for (src = 0; src < tp_locvar->nr_ulong; src++) {				\
				int dst;								\
				for (dst = sizeof(long); dst >= 0; dst--) {				\
					if (nr_bytes_out++ >= tp_locvar->nr_bytes) {			\
						goto skip_##name;					\
					}								\
					ctf_user_integer_type(uint8_t,					\
						((uint8_t __user *) (input->fds_bits + src))[dst]);	\
				}									\
			}										\
		skip_##name:   ;									\
		)											\
	)

#define LTTNG_SYSCALL_SELECT_code_post	\
	kfree(tp_locvar->fds_in);	\
	kfree(tp_locvar->fds_out);	\
	kfree(tp_locvar->fds_ex);

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM)
#define OVERRIDE_32_select
#define OVERRIDE_64_select
SC_LTTNG_TRACEPOINT_EVENT_CODE(select,
	TP_PROTO(sc_exit(long ret,) int n, fd_set __user *inp, fd_set __user *outp,
		fd_set __user *exp, struct timeval *tvp),
	TP_ARGS(sc_exit(ret,) n, inp, outp, exp, tvp),
	TP_locvar(
		LTTNG_SYSCALL_SELECT_locvar
	),
	TP_code_pre(
		LTTNG_SYSCALL_SELECT_code_pre
	),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, n, n))
		sc_inout(ctf_integer(uint8_t, overflow, tp_locvar->overflow))
		sc_inout(ctf_integer(struct timeval *, tvp, tvp))

		sc_inout(
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
			LTTNG_SYSCALL_SELECT_fds_field_LE(readfds, inp)
			LTTNG_SYSCALL_SELECT_fds_field_LE(writefds, outp)
			LTTNG_SYSCALL_SELECT_fds_field_LE(exceptfds, exp)
#else
			LTTNG_SYSCALL_SELECT_fds_field_BE(readfds, inp)
			LTTNG_SYSCALL_SELECT_fds_field_BE(writefds, outp)
			LTTNG_SYSCALL_SELECT_fds_field_BE(exceptfds, exp)
#endif
		)
	),
	TP_code_post(
		LTTNG_SYSCALL_SELECT_code_post
	)
)
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_X86_64) */

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM64) || defined(CONFIG_ARM)
#define OVERRIDE_32_pselect6
#define OVERRIDE_64_pselect6
SC_LTTNG_TRACEPOINT_EVENT_CODE(pselect6,
	TP_PROTO(sc_exit(long ret,) int n, fd_set __user * inp, fd_set __user * outp,
		fd_set __user * exp, struct timeval __user * tvp, void __user * sig),
	TP_ARGS(sc_exit(ret,) n, inp, outp, exp, tvp, sig),
	TP_locvar(
		LTTNG_SYSCALL_SELECT_locvar
	),
	TP_code_pre(
		LTTNG_SYSCALL_SELECT_code_pre
	),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, n, n))
		sc_inout(ctf_integer(uint8_t, overflow, tp_locvar->overflow))
		sc_inout(ctf_integer(struct timeval *, tvp, tvp))
		sc_in(ctf_integer_hex(void *, sig, sig))

		sc_inout(
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
			LTTNG_SYSCALL_SELECT_fds_field_LE(readfds, inp)
			LTTNG_SYSCALL_SELECT_fds_field_LE(writefds, outp)
			LTTNG_SYSCALL_SELECT_fds_field_LE(exceptfds, exp)
#else
			LTTNG_SYSCALL_SELECT_fds_field_BE(readfds, inp)
			LTTNG_SYSCALL_SELECT_fds_field_BE(writefds, outp)
			LTTNG_SYSCALL_SELECT_fds_field_BE(exceptfds, exp)
#endif
		)
	),
	TP_code_post(
		LTTNG_SYSCALL_SELECT_code_post
	)
)
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM64) || defined(CONFIG_ARM) */

#ifndef ONCE_LTTNG_TRACE_POLL_H
#define ONCE_LTTNG_TRACE_POLL_H

#define LTTNG_POLL_NRFLAGS (POLLNVAL + 1)
#define POLL_FLAGS_PADDING_SIZE (sizeof(uint8_t) * BITS_PER_BYTE) - \
	ilog2(LTTNG_POLL_NRFLAGS - 1)

/*
 * Only extract the values specified by iBCS2 for now.
 */
static struct lttng_event_field lttng_pollfd_flag_fields[] = {
	[ilog2(POLLIN)] = {
		.name = "POLLIN",
		.type = __type_integer(int, 1, 1, 0, __LITTLE_ENDIAN, 10, none),
	},
	[ilog2(POLLPRI)] = {
		.name = "POLLPRI",
		.type = __type_integer(int, 1, 1, 0, __LITTLE_ENDIAN, 10, none),
	},
	[ilog2(POLLOUT)] = {
		.name = "POLLOUT",
		.type = __type_integer(int, 1, 1, 0, __LITTLE_ENDIAN, 10, none),
	},
	[ilog2(POLLERR)] = {
		.name = "POLLERR",
		.type = __type_integer(int, 1, 1, 0, __LITTLE_ENDIAN, 10, none),
	},
	[ilog2(POLLHUP)] = {
		.name = "POLLHUP",
		.type = __type_integer(int, 1, 1, 0, __LITTLE_ENDIAN, 10, none),
	},
	[ilog2(POLLNVAL)] = {
		.name = "POLLNVAL",
		.type = __type_integer(int, 1, 1, 0, __LITTLE_ENDIAN, 10, none),
	},
	[ilog2(LTTNG_POLL_NRFLAGS)] = {
		.name = "padding",
		.type = __type_integer(int, POLL_FLAGS_PADDING_SIZE, 1, 0,
				__LITTLE_ENDIAN, 10, none),
	},
};

static struct lttng_event_field lttng_pollfd_fields[] = {
	[0] = {
		.name = "fd",
		.type = __type_integer(int, 0, 0, 0, __BYTE_ORDER, 10, none),
	},
	[1] = {
		.name = "raw_events",
		.type = __type_integer(short, 0, 0, 0, __BYTE_ORDER, 16, none),
	},
	[2] = {
		.name = "events",
		.type = {
			.atype = atype_struct,
			.u._struct.nr_fields = ARRAY_SIZE(lttng_pollfd_flag_fields),
			.u._struct.fields = lttng_pollfd_flag_fields,
		}
	},
};

static struct lttng_type lttng_pollfd_elem = {
	.atype = atype_struct,
	.u._struct.nr_fields = ARRAY_SIZE(lttng_pollfd_fields),
	.u._struct.fields = lttng_pollfd_fields,
};
#endif /* ONCE_LTTNG_TRACE_POLL_H */

#define LTTNG_SYSCALL_POLL_locvar				\
	unsigned int fds_length, fds_max_len, alloc_fds;	\
	struct pollfd *fds;					\
	uint8_t overflow;

#define LTTNG_SYSCALL_POLL_code_pre				\
	BUILD_BUG_ON(((ARRAY_SIZE(lttng_pollfd_flag_fields) - 1) +			\
			POLL_FLAGS_PADDING_SIZE) !=					\
				sizeof(uint8_t) * BITS_PER_BYTE);			\
	tp_locvar->fds = NULL;								\
	tp_locvar->overflow = 0;							\
											\
	sc_in(										\
		if (nfds > PAGE_SIZE / sizeof(struct pollfd)) {				\
			tp_locvar->fds_length = PAGE_SIZE / sizeof(struct pollfd);	\
			tp_locvar->fds_max_len = PAGE_SIZE / sizeof(struct pollfd);	\
			tp_locvar->overflow = 1;					\
		} else {								\
			tp_locvar->fds_length = nfds;					\
			tp_locvar->fds_max_len = nfds;					\
		}									\
		tp_locvar->alloc_fds = tp_locvar->fds_length * sizeof(struct pollfd);	\
	)										\
	/*										\
	 * On exit, the number of active FDs is determined by ret,			\
	 * nfds stays the same as the entry, but we only want to			\
	 * output the FDs that are relevant.						\
	 */										\
	sc_out(										\
		if (ret <= 0 || ret > nfds)						\
			goto error;							\
											\
		if (nfds > PAGE_SIZE / sizeof(struct pollfd)) {				\
			tp_locvar->fds_length = PAGE_SIZE / sizeof(struct pollfd);	\
			tp_locvar->fds_max_len = PAGE_SIZE / sizeof(struct pollfd);	\
			tp_locvar->overflow = 1;					\
		} else {								\
			tp_locvar->fds_length = ret;					\
			tp_locvar->fds_max_len = nfds;					\
		}									\
		tp_locvar->alloc_fds = tp_locvar->fds_max_len * sizeof(struct pollfd);	\
	)										\
	{										\
		int err;								\
											\
		tp_locvar->fds = kmalloc(tp_locvar->alloc_fds,				\
				GFP_ATOMIC | GFP_NOWAIT);				\
		if (!tp_locvar->fds)							\
			goto error;							\
		err = lib_ring_buffer_copy_from_user_check_nofault(			\
			tp_locvar->fds, ufds, tp_locvar->alloc_fds);			\
		if (err != 0)								\
			goto error;							\
	}										\
	goto end;									\
											\
error:											\
	tp_locvar->fds_length = 0;							\
	tp_locvar->fds_max_len = 0;							\
end:											\
	;

#define LTTNG_SYSCALL_POLL_fds_field								\
	sc_in(											\
		ctf_custom_field(								\
			ctf_custom_type(							\
				.atype = atype_sequence_compound,				\
				.u.sequence_compound.length_name = "fds_length",		\
				.u.sequence_compound.elem_type = &lttng_pollfd_elem,		\
			),									\
			fds,									\
			ctf_custom_code(							\
				uint32_t i;							\
												\
				ctf_align(int) /* Align on largest field in struct. */		\
				for (i = 0; i < tp_locvar->fds_length; i++) {			\
					ctf_integer_type(int, tp_locvar->fds[i].fd)		\
					ctf_integer_type(short, tp_locvar->fds[i].events)	\
					ctf_integer_bitfield_type(uint8_t,			\
						(uint8_t) tp_locvar->fds[i].events)		\
				}								\
			)									\
		)										\
	)											\
	sc_out(											\
		ctf_custom_field(								\
			ctf_custom_type(							\
				.atype = atype_sequence_compound,				\
				.u.sequence_compound.length_name = "fds_length",		\
				.u.sequence_compound.elem_type = &lttng_pollfd_elem,		\
			),									\
			fds,									\
			ctf_custom_code(							\
				unsigned int i, nr = 0;						\
												\
				ctf_align(int) /* Align on largest field in struct. */		\
				/*								\
				 * Iterate over the complete array, but only output		\
				 * "ret" active FDs.						\
				 */								\
				for (i = 0; i < tp_locvar->fds_max_len; i++) {			\
					if (!tp_locvar->fds[i].revents)				\
						continue;					\
					if (nr++ >= tp_locvar->fds_length)			\
						break;						\
					ctf_integer_type(int, tp_locvar->fds[i].fd)		\
					ctf_integer_type(short, tp_locvar->fds[i].revents)	\
					ctf_integer_bitfield_type(uint8_t,			\
						(uint8_t) tp_locvar->fds[i].revents)		\
				}								\
				/*								\
				 * If there is a discrepancy between ret and the		\
				 * content of revents (e.g. caused by userspace corrupting	\
				 * the array from a concurrent thread), we have to output	\
				 * zeros to keep the trace readable.				\
				 */								\
				for (i = nr; i < tp_locvar->fds_length; i++) {			\
					ctf_integer_type(int, 0)				\
					ctf_integer_type(short, 0)				\
					ctf_integer_bitfield_type(uint8_t, 0)			\
				}								\
			)									\
		)										\
	)

#define LTTNG_SYSCALL_POLL_code_post	\
	kfree(tp_locvar->fds);

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM)
#define OVERRIDE_32_poll
#define OVERRIDE_64_poll
SC_LTTNG_TRACEPOINT_EVENT_CODE(poll,
	TP_PROTO(sc_exit(long ret,) struct pollfd __user * ufds,
		unsigned int nfds, int timeout_msecs),
	TP_ARGS(sc_exit(ret,) ufds, nfds, timeout_msecs),
	TP_locvar(
		LTTNG_SYSCALL_POLL_locvar
	),
	TP_code_pre(
		LTTNG_SYSCALL_POLL_code_pre
	),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, timeout_msecs, timeout_msecs))
		sc_inout(ctf_integer(unsigned int, nfds, nfds))
		sc_inout(ctf_integer(unsigned int, fds_length, tp_locvar->fds_length))
		sc_in(ctf_integer(uint8_t, overflow, tp_locvar->overflow))
		LTTNG_SYSCALL_POLL_fds_field
	),
	TP_code_post(
		LTTNG_SYSCALL_POLL_code_post
	)
)
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_X86_64) */

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM64) || defined(CONFIG_ARM)
#define OVERRIDE_32_ppoll
#define OVERRIDE_64_ppoll
SC_LTTNG_TRACEPOINT_EVENT_CODE(ppoll,
	TP_PROTO(sc_exit(long ret,) struct pollfd __user * ufds,
		unsigned int nfds, struct timespec * tsp, const sigset_t * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) ufds, nfds, tsp, sigmask, sigsetsize),
	TP_locvar(
		LTTNG_SYSCALL_POLL_locvar
	),
	TP_code_pre(
		LTTNG_SYSCALL_POLL_code_pre
	),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(struct timespec *, tsp, tsp))
		sc_in(ctf_integer(const sigset_t *, sigmask, sigmask))
		sc_in(ctf_integer(size_t, sigsetsize, sigsetsize))
		sc_inout(ctf_integer(unsigned int, nfds, nfds))
		sc_inout(ctf_integer(unsigned int, fds_length, tp_locvar->fds_length))
		sc_inout(ctf_integer(uint8_t, overflow, tp_locvar->overflow))
		LTTNG_SYSCALL_POLL_fds_field
	),
	TP_code_post(
		LTTNG_SYSCALL_POLL_code_post
	)
)
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM64) || defined(CONFIG_ARM) */

#include <linux/eventpoll.h>

SC_LTTNG_TRACEPOINT_ENUM(lttng_epoll_op,
	TP_ENUM_VALUES(
		ctf_enum_value("EPOLL_CTL_ADD", EPOLL_CTL_ADD)
		ctf_enum_value("EPOLL_CTL_DEL", EPOLL_CTL_DEL)
		ctf_enum_value("EPOLL_CTL_MOD", EPOLL_CTL_MOD)
	)
)

#ifndef ONCE_LTTNG_TRACE_EPOLL_CTL_H
#define ONCE_LTTNG_TRACE_EPOLL_CTL_H

#define LTTNG_EPOLL_NRFLAGS (POLLHUP + 1)
#define EPOLL_FLAGS_PADDING_SIZE (sizeof(uint8_t) * BITS_PER_BYTE) - \
	ilog2(LTTNG_EPOLL_NRFLAGS - 1)

/*
 * Only extract the values specified by iBCS2 for now.
 */
static struct lttng_event_field lttng_epoll_ctl_events_fields[] = {
	/* 0x0001 */
	[ilog2(POLLIN)] = {
		.name = "EPOLLIN",
		.type = __type_integer(int, 1, 1, 0, __LITTLE_ENDIAN, 10, none),
	},
	/* 0x0002 */
	[ilog2(POLLPRI)] = {
		.name = "EPOLLPRI",
		.type = __type_integer(int, 1, 1, 0, __LITTLE_ENDIAN, 10, none),
	},
	/* 0x0004 */
	[ilog2(POLLOUT)] = {
		.name = "EPOLLOUT",
		.type = __type_integer(int, 1, 1, 0, __LITTLE_ENDIAN, 10, none),
	},
	/* 0x0008 */
	[ilog2(POLLERR)] = {
		.name = "EPOLLERR",
		.type = __type_integer(int, 1, 1, 0, __LITTLE_ENDIAN, 10, none),
	},
	/* 0x0010 */
	[ilog2(POLLHUP)] = {
		.name = "EPOLLHUP",
		.type = __type_integer(int, 1, 1, 0, __LITTLE_ENDIAN, 10, none),
	},
	[ilog2(LTTNG_EPOLL_NRFLAGS)] = {
		.name = "padding",
		.type = __type_integer(int, EPOLL_FLAGS_PADDING_SIZE, 1, 0,
				__LITTLE_ENDIAN, 10, none),
	},

};

static struct lttng_event_field lttng_epoll_data_fields[] = {
	[0] = {
		.name = "u64",
		.type = __type_integer(uint64_t, 0, 0, 0, __BYTE_ORDER, 16, none),
	},
	[1] = {
		.name = "fd",
		.type = __type_integer(int, 0, 0, 0, __BYTE_ORDER, 10, none),
	},
};

static struct lttng_event_field epoll_ctl_fields[] = {
	[0] = {
		.name = "data_union",
		.type = {
			.atype = atype_struct,
			.u._struct.nr_fields = ARRAY_SIZE(lttng_epoll_data_fields),
			.u._struct.fields = lttng_epoll_data_fields,
		}
	},
	[1] = {
		.name = "raw_events",
		.type = __type_integer(uint32_t, 0, 0, 0, __BYTE_ORDER, 16, none),
	},
	[2] = {
		.name = "events",
		.type = {
			.atype = atype_struct,
			.u._struct.nr_fields = ARRAY_SIZE(lttng_epoll_ctl_events_fields),
			.u._struct.fields = lttng_epoll_ctl_events_fields,
		}
	},
};
#endif /* ONCE_LTTNG_TRACE_EPOLL_CTL_H */

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM64) || defined(CONFIG_ARM)
#define OVERRIDE_32_epoll_ctl
#define OVERRIDE_64_epoll_ctl
SC_LTTNG_TRACEPOINT_EVENT_CODE(epoll_ctl,
	TP_PROTO(sc_exit(long ret,) int epfd, int op, int fd,
		struct epoll_event __user * uevent),
	TP_ARGS(sc_exit(ret,) epfd, op, fd, uevent),
	TP_locvar(
		struct epoll_event event;
		int err;
	),
	TP_code_pre(
		tp_locvar->err = lib_ring_buffer_copy_from_user_check_nofault(
			&tp_locvar->event, uevent, sizeof(struct epoll_event));
		),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, epfd, epfd))
		sc_in(ctf_enum(lttng_epoll_op, int, op_enum, op))
		sc_in(ctf_integer(int, fd, fd))
		sc_in(
			ctf_custom_field(
				ctf_custom_type(
					.atype = atype_struct,
					.u._struct.nr_fields = ARRAY_SIZE(epoll_ctl_fields),
					.u._struct.fields = epoll_ctl_fields,
				),
				event,
				ctf_custom_code(
					ctf_align(uint64_t)
					if (!tp_locvar->err) {
						ctf_integer_type(uint64_t, tp_locvar->event.data)
						ctf_integer_type(int, tp_locvar->event.data)
						ctf_integer_bitfield_type(uint32_t,
							tp_locvar->event.events)
						ctf_integer_bitfield_type(uint8_t,
							(uint8_t) tp_locvar->event.events)
					} else {
						ctf_integer_type(uint64_t, 0)
						ctf_integer_type(int, 0)
						ctf_integer_bitfield_type(uint32_t, 0)
						ctf_integer_bitfield_type(uint8_t, 0)
					}
				)
			)
		)
	),
	TP_code_post()
)
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM64) || defined(CONFIG_ARM) */

#ifndef ONCE_LTTNG_TRACE_EPOLL_H
#define ONCE_LTTNG_TRACE_EPOLL_H

static struct lttng_event_field lttng_epoll_wait_fields[] = {
	[0] = {
		.name = "data_union",
		.type = {
			.atype = atype_struct,
			.u._struct.nr_fields = ARRAY_SIZE(lttng_epoll_data_fields),
			.u._struct.fields = lttng_epoll_data_fields,
		}
	},
	[1] = {
		.name = "raw_events",
		.type = __type_integer(uint32_t, 0, 0, 0, __BYTE_ORDER, 16, none),
	},
	[2] = {
		.name = "events",
		.type = {
			.atype = atype_struct,
			.u._struct.nr_fields = ARRAY_SIZE(lttng_epoll_ctl_events_fields),
			.u._struct.fields = lttng_epoll_ctl_events_fields,
		}
	},
};

static struct lttng_type lttng_epoll_wait_elem = {
	.atype = atype_struct,
	.u._struct.nr_fields = ARRAY_SIZE(lttng_epoll_wait_fields),
	.u._struct.fields = lttng_epoll_wait_fields,
};

#endif /* ONCE_LTTNG_TRACE_EPOLL_H */

#define LTTNG_SYSCALL_EPOLL_WAIT_locvar		\
	sc_out(					\
		unsigned int fds_length;	\
		uint8_t overflow;		\
		struct epoll_event *events;	\
	)

#define LTTNG_SYSCALL_EPOLL_WAIT_code_pre					\
	BUILD_BUG_ON(((ARRAY_SIZE(lttng_epoll_ctl_events_fields) - 1) +		\
			EPOLL_FLAGS_PADDING_SIZE) !=				\
				sizeof(uint8_t) * BITS_PER_BYTE);		\
	sc_out({								\
		int err;							\
		unsigned long maxalloc;						\
										\
		tp_locvar->fds_length = 0;					\
		tp_locvar->events = NULL;					\
		tp_locvar->overflow = 0;					\
										\
		if (maxevents <= 0 || ret <= 0 || ret > maxevents)		\
			goto skip_code;						\
										\
		if (maxevents > PAGE_SIZE / sizeof(struct epoll_event)) {	\
			maxalloc = PAGE_SIZE / sizeof(struct epoll_event);	\
		} else {							\
			maxalloc = maxevents;					\
		}								\
										\
		if (ret > maxalloc) {						\
			tp_locvar->fds_length = maxalloc;			\
			tp_locvar->overflow = 1;				\
		} else {							\
			tp_locvar->fds_length = ret;				\
		}								\
										\
		tp_locvar->events = kmalloc(					\
			maxalloc * sizeof(struct epoll_event),			\
			GFP_ATOMIC | GFP_NOWAIT);				\
		if (!tp_locvar->events) {					\
			tp_locvar->fds_length = 0;				\
			goto skip_code;						\
		}								\
										\
		err = lib_ring_buffer_copy_from_user_check_nofault(		\
			tp_locvar->events, uevents,				\
			maxalloc * sizeof(struct epoll_event));			\
		if (err != 0)							\
			tp_locvar->fds_length = 0;				\
	}									\
	skip_code:								\
	)

#define LTTNG_SYSCALL_EPOLL_WAIT_fds_field						\
	ctf_custom_field(								\
		ctf_custom_type(							\
			.atype = atype_sequence_compound,				\
			.u.sequence_compound.length_name =				\
				"fds_length",						\
			.u.sequence_compound.elem_type =				\
				&lttng_epoll_wait_elem,					\
		),									\
		fds,									\
		ctf_custom_code(							\
			uint32_t i;							\
											\
			ctf_align(uint64_t)						\
			for (i = 0; i < tp_locvar->fds_length; i++) {			\
				ctf_integer_type(uint64_t, tp_locvar->events[i].data)	\
				ctf_integer_type(int, tp_locvar->events[i].data)	\
				ctf_integer_bitfield_type(uint32_t,			\
					tp_locvar->events[i].events)			\
				ctf_integer_bitfield_type(uint8_t,			\
					(uint8_t) tp_locvar->events[i].events)		\
			}								\
		)									\
	)

#define LTTNG_SYSCALL_EPOLL_WAIT_code_post	\
	sc_out(					\
		kfree(tp_locvar->events);	\
	)


#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM)
#define OVERRIDE_32_epoll_wait
#define OVERRIDE_64_epoll_wait
SC_LTTNG_TRACEPOINT_EVENT_CODE(epoll_wait,
	TP_PROTO(sc_exit(long ret,) int epfd, struct epoll_event __user * uevents,
		int maxevents, int timeout),
	TP_ARGS(sc_exit(ret,) epfd, uevents, maxevents, timeout),
	TP_locvar(
		LTTNG_SYSCALL_EPOLL_WAIT_locvar
	),
	TP_code_pre(
		LTTNG_SYSCALL_EPOLL_WAIT_code_pre
	),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, epfd, epfd))
		sc_in(ctf_integer(int, maxevents, maxevents))
		sc_in(ctf_integer(int, timeout, timeout))
		sc_out(ctf_integer(unsigned int, fds_length, tp_locvar->fds_length))
		sc_out(ctf_integer(uint8_t, overflow, tp_locvar->overflow))
		sc_out(
			LTTNG_SYSCALL_EPOLL_WAIT_fds_field
		)
	),
	TP_code_post(
		LTTNG_SYSCALL_EPOLL_WAIT_code_post
	)
)
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_X86_64) */

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM64) || defined(CONFIG_ARM)
#define OVERRIDE_32_epoll_pwait
#define OVERRIDE_64_epoll_pwait
SC_LTTNG_TRACEPOINT_EVENT_CODE(epoll_pwait,
	TP_PROTO(sc_exit(long ret,) int epfd, struct epoll_event __user * uevents,
		int maxevents, int timeout, const sigset_t __user * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) epfd, uevents, maxevents, timeout, sigmask, sigsetsize),
	TP_locvar(
		LTTNG_SYSCALL_EPOLL_WAIT_locvar
	),
	TP_code_pre(
		LTTNG_SYSCALL_EPOLL_WAIT_code_pre
	),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, epfd, epfd))
		sc_in(ctf_integer(int, maxevents, maxevents))
		sc_in(ctf_integer(int, timeout, timeout))
		sc_in(ctf_integer_hex(const sigset_t *, sigmask, sigmask))
		sc_in(ctf_integer(size_t, sigsetsize, sigsetsize))
		sc_out(ctf_integer(unsigned int, fds_length, tp_locvar->fds_length))
		sc_out(ctf_integer(uint8_t, overflow, tp_locvar->overflow))
		sc_out(
			LTTNG_SYSCALL_EPOLL_WAIT_fds_field
		)
	),
	TP_code_post(
		LTTNG_SYSCALL_EPOLL_WAIT_code_post
	)
)
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM64) || defined(CONFIG_ARM) */

#if (defined(CONFIG_X86_64) && !defined(LTTNG_SC_COMPAT)) || defined(CONFIG_ARM64) || defined(CONFIG_ARM)
#define OVERRIDE_32_socketpair
#define OVERRIDE_64_socketpair
SC_LTTNG_TRACEPOINT_EVENT(socketpair,
	TP_PROTO(sc_exit(long ret,) int family, int type, int protocol, int *usockvec),
	TP_ARGS(sc_exit(ret,) family, type, protocol, usockvec),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, family, family))
		sc_in(ctf_integer(int, type, type))
		sc_in(ctf_integer(int, protocol, protocol))
		sc_out(ctf_user_array(int, socket, usockvec, 2))
	)
)
#endif /* (defined(CONFIG_X86_64) && !defined(LTTNG_SC_COMPAT)) || defined(CONFIG_ARM64) || defined(CONFIG_ARM) */

#endif /* CREATE_SYSCALL_TABLE */
