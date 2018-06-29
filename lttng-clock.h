/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-clock.h
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_CLOCK_H
#define _LTTNG_CLOCK_H

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
