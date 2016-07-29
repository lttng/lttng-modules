#ifndef _LTTNG_WRAPPER_TIMER_H
#define _LTTNG_WRAPPER_TIMER_H

/*
 * wrapper/timer.h
 *
 * wrapper around linux/timer.h.
 *
 * Copyright (C) 2016 Michael Jeanson <mjeanson@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <linux/version.h>
#include <linux/timer.h>
#include <lttng-kernel-version.h>


#if (LTTNG_RT_VERSION_CODE >= LTTNG_RT_KERNEL_VERSION(4,6,4,8))

#define lttng_init_timer_pinned(timer)					\
	init_timer_pinned(timer)

static inline int lttng_mod_timer_pinned(struct timer_list *timer,
		unsigned long expires)
{
	return mod_timer(timer, expires);
}

#else /* #if (LTTNG_RT_VERSION_CODE >= LTTNG_RT_KERNEL_VERSION(4,6,4,8)) */

#define lttng_init_timer_pinned(timer)					\
	init_timer(timer)

static inline int lttng_mod_timer_pinned(struct timer_list *timer,
		unsigned long expires)
{
	return mod_timer_pinned(timer, expires);
}

#endif /* #else #if (LTTNG_RT_VERSION_CODE >= LTTNG_RT_KERNEL_VERSION(4,6,4,8)) */

#endif /* _LTTNG_WRAPPER_TIMER_H */
