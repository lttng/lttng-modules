/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only) */

#ifndef CREATE_SYSCALL_TABLE

# ifndef CONFIG_UID16
#  define OVERRIDE_32_getgroups16
#  define OVERRIDE_32_setgroups16
#  define OVERRIDE_32_lchown16
#  define OVERRIDE_32_getresuid16
#  define OVERRIDE_32_getresgid16
#  define OVERRIDE_32_chown16
# endif

#ifdef CONFIG_X86_64
#ifdef CONFIG_COMPAT_OLD_SIGACTION
/*
 * From the point of view of the 64-bit ABI, old_sigaction
 * becomes compat_old_sigaction.
 */
#define old_sigaction compat_old_sigaction
#else /* CONFIG_COMPAT_OLD_SIGACTION */
/*
 * The target 64-bit kernel does not enable the support for
 * the 32-bit compat version of OLD_SIGACTION. Defining
 * OVERRIDE_32_sigaction ensures we don't build a tracepoint
 * for this syscall.
 */
#define OVERRIDE_32_sigaction
#endif /* CONFIG_COMPAT_OLD_SIGACTION */
#endif

/*
 * Override 'pipe' to set the output field 'fildes' to an array of 2 integers
 * instead of the default integer pointer.
 */
#define OVERRIDE_32_pipe
#define OVERRIDE_64_pipe
SC_LTTNG_TRACEPOINT_EVENT(pipe,
    TP_PROTO(sc_exit(long ret,) int * fildes),
    TP_ARGS(sc_exit(ret,) fildes),
    TP_FIELDS(sc_exit(ctf_integer(long, ret, ret))
        sc_out(ctf_user_array(int, fildes, fildes, 2))
    )
)

#else	/* CREATE_SYSCALL_TABLE */

# ifndef CONFIG_UID16
#  define OVERRIDE_TABLE_32_getgroups16
#  define OVERRIDE_TABLE_32_setgroups16
#  define OVERRIDE_TABLE_32_lchown16
#  define OVERRIDE_TABLE_32_getresuid16
#  define OVERRIDE_TABLE_32_getresgid16
#  define OVERRIDE_TABLE_32_chown16
# endif

#define OVERRIDE_TABLE_32_execve
TRACE_SYSCALL_TABLE(execve, execve, 11, 3)
#define OVERRIDE_TABLE_32_clone
TRACE_SYSCALL_TABLE(clone, clone, 120, 5)
#define OVERRIDE_TABLE_32_getcpu
TRACE_SYSCALL_TABLE(getcpu, getcpu, 318, 3)

#endif /* CREATE_SYSCALL_TABLE */


