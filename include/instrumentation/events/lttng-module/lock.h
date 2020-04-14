/* SPDX-License-Identifier: GPL-2.0-only */

#undef TRACE_SYSTEM
#define TRACE_SYSTEM lock

#if !defined(LTTNG_TRACE_LOCK_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_LOCK_H

#include <linux/lockdep.h>
#include <lttng/lttng-tracepoint-event.h>

#ifdef CONFIG_LOCKDEP

LTTNG_TRACEPOINT_EVENT(lock_acquire,

	TP_PROTO(struct lockdep_map *lock, unsigned int subclass,
		int trylock, int read, int check,
		struct lockdep_map *next_lock, unsigned long ip),

	TP_ARGS(lock, subclass, trylock, read, check, next_lock, ip),

	TP_FIELDS(
		ctf_integer(unsigned int, flags, (trylock ? 1 : 0) | (read ? 2 : 0))
		ctf_string(name, lock->name)
		ctf_integer_hex(void *, lockdep_addr, lock)
	)
)

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

#endif /* CONFIG_LOCK_STAT */

#endif /* CONFIG_LOCKDEP */

#endif /* LTTNG_TRACE_LOCK_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
