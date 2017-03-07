#undef TRACE_SYSTEM
#define TRACE_SYSTEM timer

#if !defined(LTTNG_TRACE_TIMER_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_TIMER_H

#include <probes/lttng-tracepoint-event.h>

#ifndef _TRACE_TIMER_DEF_
#define _TRACE_TIMER_DEF_
#include <linux/hrtimer.h>
#include <linux/timer.h>
#include <linux/version.h>

struct timer_list;

#endif /* _TRACE_TIMER_DEF_ */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0))
#define lttng_ktime_get_tv64(kt)	(kt)
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)) */
#define lttng_ktime_get_tv64(kt)	((kt).tv64)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)) */

LTTNG_TRACEPOINT_EVENT_CLASS(timer_class,

	TP_PROTO(struct timer_list *timer),

	TP_ARGS(timer),

	TP_FIELDS(
		ctf_integer_hex(void *, timer, timer)
	)
)

/**
 * timer_init - called when the timer is initialized
 * @timer:	pointer to struct timer_list
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(timer_class, timer_init,

	TP_PROTO(struct timer_list *timer),

	TP_ARGS(timer)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,2,0))
/**
 * timer_start - called when the timer is started
 * @timer:	pointer to struct timer_list
 * @expires:	the timers expiry time
 * @flags:	the timers expiry time
 */
LTTNG_TRACEPOINT_EVENT(timer_start,

	TP_PROTO(struct timer_list *timer, unsigned long expires,
		unsigned int flags),

	TP_ARGS(timer, expires, flags),

	TP_FIELDS(
		ctf_integer_hex(void *, timer, timer)
		ctf_integer_hex(void *, function, timer->function)
		ctf_integer(unsigned long, expires, expires)
		ctf_integer(unsigned long, now, jiffies)
		ctf_integer(unsigned int, flags, flags)
	)
)
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,2,0)) */
/**
 * timer_start - called when the timer is started
 * @timer:	pointer to struct timer_list
 * @expires:	the timers expiry time
 */
LTTNG_TRACEPOINT_EVENT(timer_start,

	TP_PROTO(struct timer_list *timer, unsigned long expires),

	TP_ARGS(timer, expires),

	TP_FIELDS(
		ctf_integer_hex(void *, timer, timer)
		ctf_integer_hex(void *, function, timer->function)
		ctf_integer(unsigned long, expires, expires)
		ctf_integer(unsigned long, now, jiffies)
	)
)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,2,0)) */

/**
 * timer_expire_entry - called immediately before the timer callback
 * @timer:	pointer to struct timer_list
 *
 * Allows to determine the timer latency.
 */
LTTNG_TRACEPOINT_EVENT(timer_expire_entry,

	TP_PROTO(struct timer_list *timer),

	TP_ARGS(timer),

	TP_FIELDS(
		ctf_integer_hex(void *, timer, timer)
		ctf_integer(unsigned long, now, jiffies)
		ctf_integer_hex(void *, function, timer->function)
	)
)

/**
 * timer_expire_exit - called immediately after the timer callback returns
 * @timer:	pointer to struct timer_list
 *
 * When used in combination with the timer_expire_entry tracepoint we can
 * determine the runtime of the timer callback function.
 *
 * NOTE: Do NOT derefernce timer in TP_fast_assign. The pointer might
 * be invalid. We solely track the pointer.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(timer_class, timer_expire_exit,

	TP_PROTO(struct timer_list *timer),

	TP_ARGS(timer)
)

/**
 * timer_cancel - called when the timer is canceled
 * @timer:	pointer to struct timer_list
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE(timer_class, timer_cancel,

	TP_PROTO(struct timer_list *timer),

	TP_ARGS(timer)
)

/**
 * hrtimer_init - called when the hrtimer is initialized
 * @timer:	pointer to struct hrtimer
 * @clockid:	the hrtimers clock
 * @mode:	the hrtimers mode
 */
LTTNG_TRACEPOINT_EVENT_MAP(hrtimer_init,

	timer_hrtimer_init,

	TP_PROTO(struct hrtimer *hrtimer, clockid_t clockid,
		 enum hrtimer_mode mode),

	TP_ARGS(hrtimer, clockid, mode),

	TP_FIELDS(
		ctf_integer_hex(void *, hrtimer, hrtimer)
		ctf_integer(clockid_t, clockid, clockid)
		ctf_integer(enum hrtimer_mode, mode, mode)
	)
)

/**
 * hrtimer_start - called when the hrtimer is started
 * @timer: pointer to struct hrtimer
 */
LTTNG_TRACEPOINT_EVENT_MAP(hrtimer_start,

	timer_hrtimer_start,

	TP_PROTO(struct hrtimer *hrtimer),

	TP_ARGS(hrtimer),

	TP_FIELDS(
		ctf_integer_hex(void *, hrtimer, hrtimer)
		ctf_integer_hex(void *, function, hrtimer->function)
		ctf_integer(s64, expires,
			lttng_ktime_get_tv64(hrtimer_get_expires(hrtimer)))
		ctf_integer(s64, softexpires,
			lttng_ktime_get_tv64(hrtimer_get_softexpires(hrtimer)))
	)
)

/**
 * htimmer_expire_entry - called immediately before the hrtimer callback
 * @timer:	pointer to struct hrtimer
 * @now:	pointer to variable which contains current time of the
 *		timers base.
 *
 * Allows to determine the timer latency.
 */
LTTNG_TRACEPOINT_EVENT_MAP(hrtimer_expire_entry,

	timer_hrtimer_expire_entry,

	TP_PROTO(struct hrtimer *hrtimer, ktime_t *now),

	TP_ARGS(hrtimer, now),

	TP_FIELDS(
		ctf_integer_hex(void *, hrtimer, hrtimer)
		ctf_integer(s64, now, lttng_ktime_get_tv64(*now))
		ctf_integer_hex(void *, function, hrtimer->function)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(timer_hrtimer_class,

	TP_PROTO(struct hrtimer *hrtimer),

	TP_ARGS(hrtimer),

	TP_FIELDS(
		ctf_integer_hex(void *, hrtimer, hrtimer)
	)
)

/**
 * hrtimer_expire_exit - called immediately after the hrtimer callback returns
 * @timer:	pointer to struct hrtimer
 *
 * When used in combination with the hrtimer_expire_entry tracepoint we can
 * determine the runtime of the callback function.
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(timer_hrtimer_class, hrtimer_expire_exit,

	timer_hrtimer_expire_exit,

	TP_PROTO(struct hrtimer *hrtimer),

	TP_ARGS(hrtimer)
)

/**
 * hrtimer_cancel - called when the hrtimer is canceled
 * @hrtimer:	pointer to struct hrtimer
 */
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(timer_hrtimer_class, hrtimer_cancel,

	timer_hrtimer_cancel,

	TP_PROTO(struct hrtimer *hrtimer),

	TP_ARGS(hrtimer)
)

/**
 * itimer_state - called when itimer is started or canceled
 * @which:	name of the interval timer
 * @value:	the itimers value, itimer is canceled if value->it_value is
 *		zero, otherwise it is started
 * @expires:	the itimers expiry time
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0))
LTTNG_TRACEPOINT_EVENT_MAP(itimer_state,

	timer_itimer_state,

	TP_PROTO(int which, const struct itimerval *const value,
		 unsigned long long expires),

	TP_ARGS(which, value, expires),

	TP_FIELDS(
		ctf_integer(int, which, which)
		ctf_integer(unsigned long long, expires, expires)
		ctf_integer(long, value_sec, value->it_value.tv_sec)
		ctf_integer(long, value_usec, value->it_value.tv_usec)
		ctf_integer(long, interval_sec, value->it_interval.tv_sec)
		ctf_integer(long, interval_usec, value->it_interval.tv_usec)
	)
)
#else /* if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)) */
LTTNG_TRACEPOINT_EVENT_MAP(itimer_state,

	timer_itimer_state,

	TP_PROTO(int which, const struct itimerval *const value,
		 cputime_t expires),

	TP_ARGS(which, value, expires),

	TP_FIELDS(
		ctf_integer(int, which, which)
		ctf_integer(cputime_t, expires, expires)
		ctf_integer(long, value_sec, value->it_value.tv_sec)
		ctf_integer(long, value_usec, value->it_value.tv_usec)
		ctf_integer(long, interval_sec, value->it_interval.tv_sec)
		ctf_integer(long, interval_usec, value->it_interval.tv_usec)
	)
)
#endif /* #else (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)) */

/**
 * itimer_expire - called when itimer expires
 * @which:	type of the interval timer
 * @pid:	pid of the process which owns the timer
 * @now:	current time, used to calculate the latency of itimer
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0))
LTTNG_TRACEPOINT_EVENT_MAP(itimer_expire,

	timer_itimer_expire,

	TP_PROTO(int which, struct pid *pid, unsigned long long now),

	TP_ARGS(which, pid, now),

	TP_FIELDS(
		ctf_integer(int , which, which)
		ctf_integer(pid_t, pid, pid_nr(pid))
		ctf_integer(unsigned long long, now, now)
	)
)
#else /* if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)) */
LTTNG_TRACEPOINT_EVENT_MAP(itimer_expire,

	timer_itimer_expire,

	TP_PROTO(int which, struct pid *pid, cputime_t now),

	TP_ARGS(which, pid, now),

	TP_FIELDS(
		ctf_integer(int , which, which)
		ctf_integer(pid_t, pid, pid_nr(pid))
		ctf_integer(cputime_t, now, now)
	)
)
#endif /* #else (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)) */

#endif /*  LTTNG_TRACE_TIMER_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
