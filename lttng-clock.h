#ifndef _LTTNG_CLOCK_H
#define _LTTNG_CLOCK_H

/*
 * lttng-clock.h
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

#include <linux/module.h>

#define LTTNG_MODULES_UUID_STR_LEN	37

struct lttng_trace_clock {
	u64 (*read64)(void);
	u64 (*freq)(void);
	int (*uuid)(char *uuid);
	const char *(*name)(void);
	const char *(*description)(void);
};

int lttng_clock_register_plugin(struct lttng_trace_clock *ltc,
		struct module *mod);
void lttng_clock_unregister_plugin(struct lttng_trace_clock *ltc,
		struct module *mod);

#endif /* _LTTNG_TRACE_CLOCK_H */
