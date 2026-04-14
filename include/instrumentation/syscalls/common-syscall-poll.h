// SPDX-FileCopyrightText: 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
//
// SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.1-only

#ifndef CREATE_SYSCALL_TABLE

#ifdef LTTNG_CREATE_FIELD_METADATA
#ifndef ONCE_LTTNG_TRACE_POLL_H
#define ONCE_LTTNG_TRACE_POLL_H

#define LTTNG_POLL_NRFLAGS (POLLNVAL + 1)
#define POLL_FLAGS_PADDING_SIZE (sizeof(uint8_t) * BITS_PER_BYTE) - \
	ilog2(LTTNG_POLL_NRFLAGS - 1)

#ifndef LTTNG_TRACEPOINT_TYPE_EXTERN
/*
 * Only extract the values specified by iBCS2 for now.
 */
static const struct lttng_kernel_event_field *lttng_pollfd_flag_fields[] = {
	[ilog2(POLLIN)] = lttng_kernel_static_event_field("POLLIN",
				lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, 10),
				false, false),
	[ilog2(POLLPRI)] = lttng_kernel_static_event_field("POLLPRI",
				lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, 10),
				false, false),
	[ilog2(POLLOUT)] = lttng_kernel_static_event_field("POLLOUT",
				lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, 10),
				false, false),
	[ilog2(POLLERR)] = lttng_kernel_static_event_field("POLLERR",
				lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, 10),
				false, false),
	[ilog2(POLLHUP)] = lttng_kernel_static_event_field("POLLHUP",
				lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, 10),
				false, false),
	[ilog2(POLLNVAL)] = lttng_kernel_static_event_field("POLLNVAL",
				lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, 10),
				false, false),
	[ilog2(LTTNG_POLL_NRFLAGS)] = lttng_kernel_static_event_field("padding",
				lttng_kernel_static_type_integer(POLL_FLAGS_PADDING_SIZE, 1, 0, __LITTLE_ENDIAN, 10),
				false, false),
};

static_assert(((ARRAY_SIZE(lttng_pollfd_flag_fields) - 1) + POLL_FLAGS_PADDING_SIZE) == sizeof(uint8_t) * BITS_PER_BYTE);

#endif /* LTTNG_TRACEPOINT_TYPE_EXTERN */

LTTNG_TRACEPOINT_TYPE(PARAMS(const struct lttng_kernel_type_struct lttng_pollfd_flag_fields_struct),
	PARAMS(lttng_kernel_static_type_struct_init(ARRAY_SIZE(lttng_pollfd_flag_fields), lttng_pollfd_flag_fields, 0)))

#ifndef LTTNG_TRACEPOINT_TYPE_EXTERN
static const struct lttng_kernel_event_field *lttng_pollfd_fields[] = {
	[0] = lttng_kernel_static_event_field("fd", lttng_kernel_static_type_integer_from_type(int, __BYTE_ORDER, 10),
				false, false),
	[1] = lttng_kernel_static_event_field("raw_events", lttng_kernel_static_type_integer_from_type(uint16_t, __BYTE_ORDER, 16),
				false, false),
	[2] = lttng_kernel_static_event_field("events",
		(const struct lttng_kernel_type_common *) &lttng_pollfd_flag_fields_struct,
		false, false),
};
#endif /* LTTNG_TRACEPOINT_TYPE_EXTERN */

LTTNG_TRACEPOINT_TYPE(PARAMS(const struct lttng_kernel_type_struct lttng_pollfd_fields_struct),
	PARAMS(lttng_kernel_static_type_struct_init(ARRAY_SIZE(lttng_pollfd_fields), lttng_pollfd_fields, 0)))

#endif /* ONCE_LTTNG_TRACE_POLL_H */
#endif /* LTTNG_CREATE_FIELD_METADATA */

#define LTTNG_SYSCALL_POLL_locvar				\
	unsigned int fds_length, fds_max_len, alloc_fds;	\
	struct pollfd *fds;					\
	uint8_t overflow;

#define LTTNG_SYSCALL_POLL_code_pre				\
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
		tp_locvar->fds = lttng_tp_mempool_alloc(tp_locvar->alloc_fds);		\
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
				lttng_kernel_static_type_sequence("fds_length",			\
					(const struct lttng_kernel_type_common *) &lttng_pollfd_fields_struct, \
					0,							\
					none)							\
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
				lttng_kernel_static_type_sequence("fds_length",			\
					(const struct lttng_kernel_type_common *) &lttng_pollfd_fields_struct, \
					0,							\
					none)							\
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
	lttng_tp_mempool_free(tp_locvar->fds);

#endif
