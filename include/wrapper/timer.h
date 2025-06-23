/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/timer.h
 *
 * wrapper around linux/timer.h.
 *
 * Copyright (C) 2016 Michael Jeanson <mjeanson@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_TIMER_H
#define _LTTNG_WRAPPER_TIMER_H

#include <lttng/kernel-version.h>
#include <linux/timer.h>

/*
 * In the olden days, pinned timers were initialized normaly with init_timer()
 * and then modified with mod_timer_pinned().
 *
 * Then came kernel 4.8.0 and they had to be initilized as pinned with
 * init_timer_pinned() and then modified as regular timers with mod_timer().
 *
 * Then came kernel 4.15.0 with a new timer API where init_timer() is no more.
 * It's replaced by timer_setup() where pinned is now part of timer flags.
 */

static inline int lttng_timer_delete_sync(struct timer_list *timer)
{
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,15,0))
	return timer_delete_sync(timer);
#else
	return del_timer_sync(timer);
#endif
}


#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,16,0))

#define lttng_timer_container_of(var, callback_timer, timer_fieldname) \
	timer_container_of(var, callback_timer, timer_fieldname)

#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0))

#define lttng_timer_container_of(var, callback_timer, timer_fieldname) \
	from_timer(var, callback_timer, timer_fieldname)

#else

/* timer_fieldname is unused prior to 4.15. */
#define lttng_timer_container_of(var, timer_data, timer_fieldname) \
	((typeof(var))timer_data)

#endif


#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0))

#define LTTNG_TIMER_PINNED		TIMER_PINNED
#define LTTNG_TIMER_FUNC_ARG_TYPE	struct timer_list *

#define lttng_mod_timer_pinned(timer, expires)  \
	mod_timer(timer, expires)

#define lttng_timer_setup(timer, callback, flags, unused) \
	timer_setup(timer, callback, flags)


#else /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0) */


# if (LTTNG_RT_VERSION_CODE >= LTTNG_RT_KERNEL_VERSION(4,6,4,8) \
	|| LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,8,0))

#define lttng_init_timer_pinned(timer) \
	init_timer_pinned(timer)

#define lttng_mod_timer_pinned(timer, expires) \
	mod_timer(timer, expires)

# else /* LTTNG_RT_VERSION_CODE >= LTTNG_RT_KERNEL_VERSION(4,6,4,8) */

#define lttng_init_timer_pinned(timer) \
	init_timer(timer)

#define lttng_mod_timer_pinned(timer, expires) \
	mod_timer_pinned(timer, expires)

# endif /* LTTNG_RT_VERSION_CODE >= LTTNG_RT_KERNEL_VERSION(4,6,4,8) */


#define LTTNG_TIMER_PINNED		TIMER_PINNED
#define LTTNG_TIMER_FUNC_ARG_TYPE	unsigned long

static inline void lttng_timer_setup(struct timer_list *timer,
		void (*function)(LTTNG_TIMER_FUNC_ARG_TYPE),
		unsigned int flags, void *data)
{
	if (flags & LTTNG_TIMER_PINNED)
		lttng_init_timer_pinned(timer);
	else
		init_timer(timer);

	timer->function = function;
	timer->data = (unsigned long)data;
}

#endif /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0) */

#endif /* _LTTNG_WRAPPER_TIMER_H */
