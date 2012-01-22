#ifndef _LTTNG_WRAPPER_POLL_H
#define _LTTNG_WRAPPER_POLL_H

/*
 * Copyright (C) 2011 Mathieu Desnoyers (mathieu.desnoyers@efficios.com)
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/poll.h>

/*
 * Note: poll_wait_set_exclusive() is defined as no-op. Thundering herd
 * effect can be noticed with large number of consumer threads.
 */

#define poll_wait_set_exclusive(poll_table)

#endif /* _LTTNG_WRAPPER_POLL_H */
