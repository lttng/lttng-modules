/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/poll.h
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_POLL_H
#define _LTTNG_WRAPPER_POLL_H

#include <linux/poll.h>

/*
 * Note: poll_wait_set_exclusive() is defined as no-op. Thundering herd
 * effect can be noticed with large number of consumer threads.
 */

#define poll_wait_set_exclusive(poll_table)

#endif /* _LTTNG_WRAPPER_POLL_H */
