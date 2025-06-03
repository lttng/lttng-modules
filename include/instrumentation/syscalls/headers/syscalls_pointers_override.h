/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only) */

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

#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM

/*
 * Clone()'s `flags` field has two parts:
 *     1. exit signal: the least significant byte of the `unsigned long` is
 *         the signal the kernel must send to the parent process on child
 *         exit,
 *     2. clone options: the remaining bytes of the `unsigned long` is used a
 *         bitwise flag for the clone options.
 */
#define CLONE_EXIT_SIGNAL_FLAG_RESERVED_BITS 8
#define LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(x) ((x) >> CLONE_EXIT_SIGNAL_FLAG_RESERVED_BITS)

SC_LTTNG_TRACEPOINT_ENUM(lttng_clone_exit_signal_flags,
	TP_ENUM_VALUES(
		ctf_enum_value("SIGHUP", SIGHUP)
		ctf_enum_value("SIGINT", SIGINT)
		ctf_enum_value("SIGQUIT", SIGQUIT)
		ctf_enum_value("SIGILL", SIGILL)
		ctf_enum_value("SIGTRAP", SIGTRAP)
		ctf_enum_value("SIGABRT", SIGABRT)
		ctf_enum_value("SIGIOT", SIGIOT)
		ctf_enum_value("SIGBUS", SIGBUS)
#ifdef SIGEMT
		ctf_enum_value("SIGEMT", SIGEMT)
#endif /* #ifdef SIGEMT */
		ctf_enum_value("SIGFPE", SIGFPE)
		ctf_enum_value("SIGKILL", SIGKILL)
		ctf_enum_value("SIGUSR1", SIGUSR1)
		ctf_enum_value("SIGSEGV", SIGSEGV)
		ctf_enum_value("SIGUSR2", SIGUSR2)
		ctf_enum_value("SIGPIPE", SIGPIPE)
		ctf_enum_value("SIGALRM", SIGALRM)
		ctf_enum_value("SIGTERM", SIGTERM)
#ifdef SIGSTKFLT
		ctf_enum_value("SIGSTKFLT", SIGSTKFLT)
#endif /* #ifdef SIGSTKFLT */
		ctf_enum_value("SIGCHLD", SIGCHLD)
#ifdef SIGCLD
		ctf_enum_value("SIGCLD", SIGCLD)
#endif /* #ifdef SIGCLD */
		ctf_enum_value("SIGCONT", SIGCONT)
		ctf_enum_value("SIGSTOP", SIGSTOP)
		ctf_enum_value("SIGTSTP", SIGTSTP)
		ctf_enum_value("SIGTTIN", SIGTTIN)
		ctf_enum_value("SIGTTOU", SIGTTOU)
		ctf_enum_value("SIGURG", SIGURG)
		ctf_enum_value("SIGXCPU", SIGXCPU)
		ctf_enum_value("SIGXFSZ", SIGXFSZ)
		ctf_enum_value("SIGVTALR", SIGVTALRM)
		ctf_enum_value("SIGPROF", SIGPROF)
		ctf_enum_value("SIGWINCH", SIGWINCH)
		ctf_enum_value("SIGIO", SIGIO)
		ctf_enum_value("SIGPOLL", SIGPOLL)
		ctf_enum_value("SIGPWR", SIGPWR)
#ifdef SIGINFO
		ctf_enum_value("SIGINFO", SIGINFO)
#endif /* #ifdef SIGINFO */
#ifdef SIGLOST
		ctf_enum_value("SIGLOST", SIGLOST)
#endif /* #ifdef SIGLOST */
		ctf_enum_value("SIGSYS", SIGSYS)
#ifdef SIGUNUSED
		ctf_enum_value("SIGUNUSED", SIGUNUSED)
#endif /* #ifdef SIGUNUSED */
	)
)

SC_LTTNG_TRACEPOINT_ENUM(lttng_clone_option_flags,
	TP_ENUM_VALUES(
		ctf_enum_value("CLONE_CHILD_CLEARTID",	LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_CHILD_CLEARTID))
		ctf_enum_value("CLONE_CHILD_SETTID",	LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_CHILD_SETTID))
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,5,0))
		ctf_enum_value("CLONE_CLEAR_SIGHAND",	LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_CLEAR_SIGHAND))
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,5,0)) */
		ctf_enum_value("CLONE_DETACHED",	LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_DETACHED))
		ctf_enum_value("CLONE_FILES",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_FILES))
		ctf_enum_value("CLONE_FS",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_FS))
		ctf_enum_value("CLONE_IO",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_IO))
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,6,0))
		ctf_enum_value("CLONE_NEWCGROUP",	LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_NEWCGROUP))
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,6,0)) */
		ctf_enum_value("CLONE_NEWIPC",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_NEWIPC))
		ctf_enum_value("CLONE_NEWNET",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_NEWNET))
		ctf_enum_value("CLONE_NEWNS",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_NEWNS))
		ctf_enum_value("CLONE_NEWPID",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_NEWPID))
		ctf_enum_value("CLONE_NEWUSER",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_NEWUSER))
		ctf_enum_value("CLONE_NEWUTS",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_NEWUTS))
		ctf_enum_value("CLONE_PARENT",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_PARENT))
		ctf_enum_value("CLONE_PARENT_SETTID",	LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_PARENT_SETTID))
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,2,0))
		ctf_enum_value("CLONE_PIDFD",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_PIDFD))
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,2,0)) */
		ctf_enum_value("CLONE_PTRACE",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_PTRACE))
		ctf_enum_value("CLONE_SETTLS",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_SETTLS))
		ctf_enum_value("CLONE_SIGHAND",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_SIGHAND))
		ctf_enum_value("CLONE_SYSVSEM",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_SYSVSEM))
		ctf_enum_value("CLONE_THREAD",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_THREAD))
		ctf_enum_value("CLONE_UNTRACED",	LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_UNTRACED))
		ctf_enum_value("CLONE_VFORK",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_VFORK))
		ctf_enum_value("CLONE_VM",		LTTNG_CLONE_OPTIONS_FLAGS_TO_CTF(CLONE_VM))
	)
)

#define LTTNG_CLONE_FLAGS_EXIT_SIGNAL					\
	lttng_kernel_static_event_field("exit_signal",			\
		lttng_kernel_static_type_enum(&__enum_lttng_clone_exit_signal_flags, \
			lttng_kernel_static_type_integer(CLONE_EXIT_SIGNAL_FLAG_RESERVED_BITS, \
				1, 0, __BYTE_ORDER, 16)),		\
		false, false)

#define LTTNG_CLONE_FLAGS_OPTIONS					\
	lttng_kernel_static_event_field("options",			\
		lttng_kernel_static_type_enum(&__enum_lttng_clone_option_flags, \
			lttng_kernel_static_type_integer(		\
				sizeof(unsigned long) * CHAR_BIT - CLONE_EXIT_SIGNAL_FLAG_RESERVED_BITS, \
				1, 0, __BYTE_ORDER, 16)),		\
		false, false)

#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#define LTTNG_CLONE_FLAGS			\
lttng_kernel_static_event_field_array(		\
	[0] = LTTNG_CLONE_FLAGS_EXIT_SIGNAL,	\
	[1] = LTTNG_CLONE_FLAGS_OPTIONS,	\
)
#else
#define LTTNG_CLONE_FLAGS			\
lttng_kernel_static_event_field_array(		\
	[0] = LTTNG_CLONE_FLAGS_OPTIONS,	\
	[1] = LTTNG_CLONE_FLAGS_EXIT_SIGNAL,	\
)
#endif

#endif /* CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM */

#define OVERRIDE_32_clone
#define OVERRIDE_64_clone
SC_LTTNG_TRACEPOINT_EVENT(clone,
	TP_PROTO(sc_exit(long ret,) unsigned long clone_flags, unsigned long newsp,
		void __user *parent_tid,
		void __user *child_tid),
	TP_ARGS(sc_exit(ret,) clone_flags, newsp, parent_tid, child_tid),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(
#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM
			ctf_custom_field(
				ctf_custom_type(
					lttng_kernel_static_type_struct(2, LTTNG_CLONE_FLAGS, lttng_alignof(unsigned long) * CHAR_BIT)
				),
				flags,
				ctf_custom_code(
					ctf_integer_type(unsigned long, clone_flags)
				)
			)
#else
			ctf_integer_hex(unsigned long, flags, clone_flags)
#endif
		)
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
			tp_locvar->fds_in = lttng_tp_mempool_alloc(				\
					tp_locvar->nr_ulong * sizeof(unsigned long));			\
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
			tp_locvar->fds_out = lttng_tp_mempool_alloc(				\
					tp_locvar->nr_ulong * sizeof(unsigned long));	\
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
			tp_locvar->fds_ex = lttng_tp_mempool_alloc(				\
					tp_locvar->nr_ulong * sizeof(unsigned long));	\
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
			lttng_kernel_static_type_integer_from_type(uint8_t, __BYTE_ORDER, 10)		\
		),											\
		_ ## name ## _length,									\
		ctf_custom_code(									\
			if (input) {									\
				ctf_integer_type(uint8_t, tp_locvar->nr_bytes)				\
				ctf_align(uint8_t)							\
			} else {									\
				ctf_integer_type(uint8_t, 0)						\
				ctf_align(uint8_t)							\
			}										\
		)											\
	)												\
	ctf_custom_field(										\
		ctf_custom_type(									\
			lttng_kernel_static_type_sequence(NULL,						\
				lttng_kernel_static_type_integer_from_type(uint8_t, __BYTE_ORDER, 16),	\
				0,									\
				none)									\
		),											\
		name,											\
		ctf_custom_code(									\
			unsigned int src;								\
			unsigned int nr_bytes_out = 0;							\
													\
			if (!input)									\
				goto skip_##name;							\
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
			lttng_kernel_static_type_integer_from_type(uint8_t, __BYTE_ORDER, 10)		\
		),											\
		_ ## name ## _length,									\
		ctf_custom_code(									\
			if (input) {									\
				ctf_integer_type(uint8_t, tp_locvar->nr_bytes)				\
				ctf_align(uint8_t)							\
			} else {									\
				ctf_integer_type(uint8_t, 0)						\
				ctf_align(uint8_t)							\
			}										\
		)											\
	)												\
	ctf_custom_field(										\
		ctf_custom_type(									\
			lttng_kernel_static_type_sequence(NULL,						\
				lttng_kernel_static_type_integer_from_type(uint8_t, __BYTE_ORDER, 16),	\
				0,									\
				none)									\
		),											\
		name,											\
		ctf_custom_code(									\
			unsigned int src, nr_bytes_out = 0;						\
													\
			if (!input)									\
				goto skip_##name;							\
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
	lttng_tp_mempool_free(tp_locvar->fds_in);	\
	lttng_tp_mempool_free(tp_locvar->fds_out);	\
	lttng_tp_mempool_free(tp_locvar->fds_ex);

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM)
/*
 * Instead of extracting the user-space pointers of the 3 fd_set,
 * extract the bitmask of the FDs in the sets (in, out, ex) in the form
 * of an array of uint8_t (1024 FDs is the limit in the kernel).
 */
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
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM) */

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM64) || defined(CONFIG_ARM)
/*
 * Instead of extracting the user-space pointers of the 3 fd_set,
 * extract the bitmask of the FDs in the sets (in, out, ex) in the form
 * of an array of uint8_t (1024 FDs is the limit in the kernel).
 */
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

#if defined(CONFIG_X86_32) || defined(CONFIG_ARM)
/*
 * Instead of extracting the user-space pointers of the 3 fd_set,
 * extract the bitmask of the FDs in the sets (in, out, ex) in the form
 * of an array of uint8_t (1024 FDs is the limit in the kernel).
 */
#define OVERRIDE_32_pselect6_time32
SC_LTTNG_TRACEPOINT_EVENT_CODE(pselect6_time32,
	TP_PROTO(sc_exit(long ret,) int n, fd_set __user * inp, fd_set __user * outp,
		fd_set __user * exp, struct old_timespec32 __user * tvp, void __user * sig),
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
		sc_inout(ctf_integer(struct old_timespec32 *, tvp, tvp))
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
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_ARM) */

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
	PARAMS(lttng_kernel_static_type_struct_init( ARRAY_SIZE(lttng_pollfd_flag_fields), lttng_pollfd_flag_fields, 0)))

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

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM)
/*
 * Instead of printing the pointer address of the poll set, extract all the FDs
 * and flags from the poll set. For now, only output the standardized
 * set of events to limit the verbosity of the output, and also extract
 * the raw value. In the future, moving to CTF2 will allow hiding unset
 * fields and then allow extracting all the fields.
 */
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
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM) */

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM64) || defined(CONFIG_ARM)
/*
 * Instead of printing the pointer address of the poll set, extract all the FDs
 * and flags from the poll set. For now, only output the standardized
 * set of events to limit the verbosity of the output, and also extract
 * the raw value. In the future, moving to CTF2 will allow hiding unset
 * fields and then allow extracting all the fields.
 */
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

#if defined(CONFIG_X86_32) || defined(CONFIG_ARM)
/*
 * Instead of printing the pointer address of the poll set, extract all the FDs
 * and flags from the poll set. For now, only output the standardized
 * set of events to limit the verbosity of the output, and also extract
 * the raw value. In the future, moving to CTF2 will allow hiding unset
 * fields and then allow extracting all the fields.
 */
#define OVERRIDE_32_ppoll_time32
SC_LTTNG_TRACEPOINT_EVENT_CODE(ppoll_time32,
	TP_PROTO(sc_exit(long ret,) struct pollfd __user * ufds,
		unsigned int nfds, struct old_timespec32 * tsp, const sigset_t * sigmask, size_t sigsetsize),
	TP_ARGS(sc_exit(ret,) ufds, nfds, tsp, sigmask, sigsetsize),
	TP_locvar(
		LTTNG_SYSCALL_POLL_locvar
	),
	TP_code_pre(
		LTTNG_SYSCALL_POLL_code_pre
	),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(struct old_timespec32 *, tsp, tsp))
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
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_ARM) */

#include <linux/eventpoll.h>

SC_LTTNG_TRACEPOINT_ENUM(lttng_epoll_op,
	TP_ENUM_VALUES(
		ctf_enum_value("EPOLL_CTL_ADD", EPOLL_CTL_ADD)
		ctf_enum_value("EPOLL_CTL_DEL", EPOLL_CTL_DEL)
		ctf_enum_value("EPOLL_CTL_MOD", EPOLL_CTL_MOD)
	)
)

#ifdef LTTNG_CREATE_FIELD_METADATA

#ifndef ONCE_LTTNG_TRACE_EPOLL_CTL_H
#define ONCE_LTTNG_TRACE_EPOLL_CTL_H

#define LTTNG_EPOLL_NRFLAGS (POLLHUP + 1)
#define EPOLL_FLAGS_PADDING_SIZE (sizeof(uint8_t) * BITS_PER_BYTE) - \
	ilog2(LTTNG_EPOLL_NRFLAGS - 1)

#ifndef LTTNG_TRACEPOINT_TYPE_EXTERN
/*
 * Only extract the values specified by iBCS2 for now.
 */
static const struct lttng_kernel_event_field *lttng_epoll_ctl_events_fields[] = {
	/* 0x0001 */
	[ilog2(POLLIN)] = lttng_kernel_static_event_field("EPOLLIN",
			lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, 10),
			false, false),
	/* 0x0002 */
	[ilog2(POLLPRI)] = lttng_kernel_static_event_field("EPOLLPRI",
			lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, 10),
			false, false),
	/* 0x0004 */
	[ilog2(POLLOUT)] = lttng_kernel_static_event_field("EPOLLOUT",
			lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, 10),
			false, false),
	/* 0x0008 */
	[ilog2(POLLERR)] = lttng_kernel_static_event_field("EPOLLERR",
			lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, 10),
			false, false),
	/* 0x0010 */
	[ilog2(POLLHUP)] = lttng_kernel_static_event_field("EPOLLHUP",
			lttng_kernel_static_type_integer(1, 1, 0, __LITTLE_ENDIAN, 10),
			false, false),
	[ilog2(LTTNG_EPOLL_NRFLAGS)] = lttng_kernel_static_event_field("padding",
			lttng_kernel_static_type_integer(EPOLL_FLAGS_PADDING_SIZE, 1, 0, __LITTLE_ENDIAN, 10),
			false, false),
};
#endif /* LTTNG_TRACEPOINT_TYPE_EXTERN */

LTTNG_TRACEPOINT_TYPE(PARAMS(const struct lttng_kernel_type_struct lttng_epoll_ctl_events_fields_struct),
	PARAMS(lttng_kernel_static_type_struct_init(ARRAY_SIZE(lttng_epoll_ctl_events_fields), lttng_epoll_ctl_events_fields, 0)))

#ifndef LTTNG_TRACEPOINT_TYPE_EXTERN
static const struct lttng_kernel_event_field *lttng_epoll_data_fields[] = {
	[0] = lttng_kernel_static_event_field("u64",
			lttng_kernel_static_type_integer_from_type(uint64_t, __BYTE_ORDER, 16),
			false, false),
	[1] = lttng_kernel_static_event_field("fd",
			lttng_kernel_static_type_integer_from_type(int, __BYTE_ORDER, 10),
			false, false),
};
#endif /* LTTNG_TRACEPOINT_TYPE_EXTERN */

LTTNG_TRACEPOINT_TYPE(PARAMS(const struct lttng_kernel_type_struct lttng_epoll_data_fields_struct),
	PARAMS(lttng_kernel_static_type_struct_init(ARRAY_SIZE(lttng_epoll_data_fields), lttng_epoll_data_fields, 0)))

#ifndef LTTNG_TRACEPOINT_TYPE_EXTERN
static const struct lttng_kernel_event_field *epoll_ctl_fields[] = {
	[0] = lttng_kernel_static_event_field("data_union",
		(const struct lttng_kernel_type_common *) &lttng_epoll_data_fields_struct,
		false, false),
	[1] = lttng_kernel_static_event_field("raw_events",
		lttng_kernel_static_type_integer_from_type(uint32_t, __BYTE_ORDER, 16),
		false, false),
	[2] = lttng_kernel_static_event_field("events",
		(const struct lttng_kernel_type_common *) &lttng_epoll_ctl_events_fields_struct,
		false, false),
};
#endif /* LTTNG_TRACEPOINT_TYPE_EXTERN */

LTTNG_TRACEPOINT_TYPE(PARAMS(const struct lttng_kernel_type_struct epoll_ctl_fields_struct),
	PARAMS(lttng_kernel_static_type_struct_init(ARRAY_SIZE(epoll_ctl_fields), epoll_ctl_fields, 0)))

#endif /* ONCE_LTTNG_TRACE_EPOLL_CTL_H */
#endif /* LTTNG_CREATE_FIELD_METADATA */

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
					(const struct lttng_kernel_type_common *) &epoll_ctl_fields_struct
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

#ifdef LTTNG_CREATE_FIELD_METADATA
#ifndef ONCE_LTTNG_TRACE_EPOLL_H
#define ONCE_LTTNG_TRACE_EPOLL_H

#ifndef LTTNG_TRACEPOINT_TYPE_EXTERN
static const struct lttng_kernel_event_field *lttng_epoll_wait_fields[] = lttng_kernel_static_event_field_array(
	[0] = lttng_kernel_static_event_field("data_union",
			(const struct lttng_kernel_type_common *) &lttng_epoll_data_fields_struct,
			false, false),
	[1] = lttng_kernel_static_event_field("raw_events", lttng_kernel_static_type_integer_from_type(uint32_t, __BYTE_ORDER, 16),
			false, false),
	[2] = lttng_kernel_static_event_field("events",
			(const struct lttng_kernel_type_common *) &lttng_epoll_ctl_events_fields_struct,
			false, false),
);

static_assert(((ARRAY_SIZE(lttng_epoll_ctl_events_fields) - 1) + EPOLL_FLAGS_PADDING_SIZE) == sizeof(uint8_t) * BITS_PER_BYTE);

#endif /* LTTNG_TRACEPOINT_TYPE_EXTERN */

LTTNG_TRACEPOINT_TYPE(PARAMS(const struct lttng_kernel_type_struct lttng_epoll_wait_fields_struct),
	PARAMS(lttng_kernel_static_type_struct_init(ARRAY_SIZE(lttng_epoll_wait_fields), lttng_epoll_wait_fields, 0)))

#endif /* ONCE_LTTNG_TRACE_EPOLL_H */
#endif /* LTTNG_CREATE_FIELD_METADATA */

#define LTTNG_SYSCALL_EPOLL_WAIT_locvar		\
	sc_out(					\
		unsigned int fds_length;	\
		uint8_t overflow;		\
		struct epoll_event *events;	\
	)

#define LTTNG_SYSCALL_EPOLL_WAIT_code_pre					\
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
		tp_locvar->events = lttng_tp_mempool_alloc(			\
			maxalloc * sizeof(struct epoll_event));			\
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
			lttng_kernel_static_type_sequence("fds_length",			\
				(const struct lttng_kernel_type_common *) &lttng_epoll_wait_fields_struct, \
				0,							\
				none)							\
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
		lttng_tp_mempool_free(tp_locvar->events);	\
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
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_X86_64) || defined(CONFIG_ARM) */

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

#ifdef CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM
/*
 * Enumeration of the open flags, as described in the 'open'
 * system call man page.
 */
SC_LTTNG_TRACEPOINT_ENUM(lttng_file_status_flags,
	TP_ENUM_VALUES(
		ctf_enum_value("O_RDONLY", O_RDONLY)
		ctf_enum_value("O_WRONLY", O_WRONLY)
		ctf_enum_value("O_RDWR", O_RDWR)
		ctf_enum_value("O_CREAT", O_CREAT)
		ctf_enum_value("O_EXCL", O_EXCL)
		ctf_enum_value("O_NOCTTY", O_NOCTTY)
		ctf_enum_value("O_TRUNC", O_TRUNC)
		ctf_enum_value("O_APPEND", O_APPEND)
		ctf_enum_value("O_NONBLOCK", O_NONBLOCK)
		ctf_enum_value("O_DSYNC", O_DSYNC)
		ctf_enum_value("FASYNC", FASYNC)
		ctf_enum_value("O_DIRECT", O_DIRECT)
		ctf_enum_value("O_LARGEFILE", O_LARGEFILE)
		ctf_enum_value("O_DIRECTORY", O_DIRECTORY)
		ctf_enum_value("O_NOFOLLOW", O_NOFOLLOW)
		ctf_enum_value("O_NOATIME", O_NOATIME)
		ctf_enum_value("O_CLOEXEC", O_CLOEXEC)
		ctf_enum_value("O_SYNC", __O_SYNC)
		ctf_enum_value("O_PATH", O_PATH)
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,11,0))
		ctf_enum_value("O_TMPFILE", __O_TMPFILE)
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,11,0)) */
	)
)

/*
 * Enumeration of the open flags, as described in the 'open'
 * system call man page.
 */
SC_LTTNG_TRACEPOINT_ENUM(lttng_file_mode,
	TP_ENUM_VALUES(
		ctf_enum_value("S_IRWXU", S_IRWXU)
		ctf_enum_value("S_IRUSR", S_IRUSR)
		ctf_enum_value("S_IWUSR", S_IWUSR)
		ctf_enum_value("S_IXUSR", S_IXUSR)
		ctf_enum_value("S_IRWXG", S_IRWXG)
		ctf_enum_value("S_IRGRP", S_IRGRP)
		ctf_enum_value("S_IWGRP", S_IWGRP)
		ctf_enum_value("S_IXGRP", S_IXGRP)
		ctf_enum_value("S_IRWXO", S_IRWXO)
		ctf_enum_value("S_IROTH", S_IROTH)
		ctf_enum_value("S_IWOTH", S_IWOTH)
		ctf_enum_value("S_IXOTH", S_IXOTH)
		ctf_enum_value("S_ISUID", S_ISUID)
		ctf_enum_value("S_ISGID", S_ISGID)
		ctf_enum_value("S_ISVTX", S_ISVTX)
	)
)

#define OVERRIDE_32_openat
#define OVERRIDE_64_openat
SC_LTTNG_TRACEPOINT_EVENT(openat,
	TP_PROTO(sc_exit(long ret,) int dfd, const char * filename, int flags, umode_t mode),
	TP_ARGS(sc_exit(ret,) dfd, filename, flags, mode),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_integer(int, dfd, dfd))
		sc_in(ctf_user_string(filename, filename))
		sc_in(ctf_enum(lttng_file_status_flags, int, flags, flags))
		sc_in(ctf_enum(lttng_file_mode, umode_t, mode, mode))
	)
)

#define OVERRIDE_32_open
#define OVERRIDE_64_open
SC_LTTNG_TRACEPOINT_EVENT(open,
	TP_PROTO(sc_exit(long ret,) const char * filename, int flags, umode_t mode),
	TP_ARGS(sc_exit(ret,) filename, flags, mode),
	TP_FIELDS(
		sc_exit(ctf_integer(long, ret, ret))
		sc_in(ctf_user_string(filename, filename))
		sc_in(ctf_enum(lttng_file_status_flags, int, flags, flags))
		sc_in(ctf_enum(lttng_file_mode, umode_t, mode, mode))
	)
)
#endif /* CONFIG_LTTNG_EXPERIMENTAL_BITWISE_ENUM */

#endif /* CREATE_SYSCALL_TABLE */

#if (LTTNG_LINUX_VERSION_CODE < LTTNG_KERNEL_VERSION(3,9,0))

#ifndef CREATE_SYSCALL_TABLE
#define OVERRIDE_32_sigaction
#else
#define OVERRIDE_TABLE_32_sigaction
#endif /* CREATE_SYSCALL_TABLE */

#else /* (LTTNG_LINUX_VERSION_CODE < LTTNG_KERNEL_VERSION(3,9,0)) */

#ifndef CREATE_SYSCALL_TABLE

#if !defined(CONFIG_COMPAT_OLD_SIGACTION)
#define OVERRIDE_32_sigaction
#else
#ifdef CONFIG_64BIT
/*
 * From the point of view of the 64-bit ABI, old_sigaction
 * becomes compat_old_sigaction.
 */
#define old_sigaction compat_old_sigaction
#endif
#endif

#else /* CREATE_SYSCALL_TABLE */

#if !defined(CONFIG_COMPAT_OLD_SIGACTION)
#define OVERRIDE_TABLE_32_sigaction
#endif

#endif /* CREATE_SYSCALL_TABLE */

#endif /* (LTTNG_LINUX_VERSION_CODE < LTTNG_KERNEL_VERSION(3,9,0)) */
