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

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64)
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

#endif /* CREATE_SYSCALL_TABLE */
