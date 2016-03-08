#include <linux/version.h>

#undef TRACE_SYSTEM
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33))
#define TRACE_SYSTEM lock
#else
#define TRACE_SYSTEM lockdep
#define TRACE_INCLUDE_FILE lock
#if defined(_TRACE_LOCKDEP_H)
#define LTTNG_TRACE_LOCK_H
#endif
#endif

#if !defined(LTTNG_TRACE_LOCK_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_LOCK_H

#include <linux/lockdep.h>
#include "../../../probes/lttng-tracepoint-event.h"

#ifdef CONFIG_LOCKDEP

LTTNG_TRACEPOINT_EVENT(lock_acquire,

	TP_PROTO(struct lockdep_map *lock, unsigned int subclass,
		int trylock, int read, int check,
		struct lockdep_map *next_lock, unsigned long ip),

	TP_ARGS(lock, subclass, trylock, read, check, next_lock, ip),

	TP_FIELDS(
		ctf_integer(unsigned int, flags, (trylock ? 1 : 0) | (read ? 2 : 0))
		ctf_string(name, lock->name)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
		ctf_integer_hex(void *, lockdep_addr, lock)
#endif
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35))

LTTNG_TRACEPOINT_EVENT_CLASS(lock,

	TP_PROTO(struct lockdep_map *lock, unsigned long ip),

	TP_ARGS(lock, ip),

	TP_FIELDS(
		ctf_string(name, lock->name)
		ctf_integer_hex(void *, lockdep_addr, lock)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(lock, lock_release,

	TP_PROTO(struct lockdep_map *lock, unsigned long ip),

	TP_ARGS(lock, ip)
)

#ifdef CONFIG_LOCK_STAT

LTTNG_TRACEPOINT_EVENT_INSTANCE(lock, lock_contended,

	TP_PROTO(struct lockdep_map *lock, unsigned long ip),

	TP_ARGS(lock, ip)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(lock, lock_acquired,

	TP_PROTO(struct lockdep_map *lock, unsigned long ip),

	TP_ARGS(lock, ip)
)

#endif

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)) */

LTTNG_TRACEPOINT_EVENT(lock_release,

	TP_PROTO(struct lockdep_map *lock, int nested, unsigned long ip),

	TP_ARGS(lock, nested, ip),

	TP_FIELDS(
		ctf_string(name, lock->name)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
		ctf_integer_hex(void *, lockdep_addr, lock)
#endif
	)
)

#ifdef CONFIG_LOCK_STAT

LTTNG_TRACEPOINT_EVENT(lock_contended,

	TP_PROTO(struct lockdep_map *lock, unsigned long ip),

	TP_ARGS(lock, ip),

	TP_FIELDS(
		ctf_string(name, lock->name)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
		ctf_integer_hex(void *, lockdep_addr, lock)
#endif
	)
)

LTTNG_TRACEPOINT_EVENT(lock_acquired,

	TP_PROTO(struct lockdep_map *lock, unsigned long ip, s64 waittime),

	TP_ARGS(lock, ip, waittime),

	TP_FIELDS(
		ctf_string(name, lock->name)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
		ctf_integer(s64, wait_nsec, wait_nsec)
		ctf_integer_hex(void *, lockdep_addr, lock)
#else
		ctf_integer(unsigned long, wait_usec, (unsigned long) waittime)
		ctf_integer(unsigned long, wait_nsec_rem, do_div(waittime, NSEC_PER_USEC))
#endif
	)
)

#endif

#endif /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)) */

#endif

#endif /* LTTNG_TRACE_LOCK_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
