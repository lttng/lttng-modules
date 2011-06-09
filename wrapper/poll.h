#ifndef _LTTNG_WRAPPER_POLL_H
#define _LTTNG_WRAPPER_POLL_H

/*
 * Copyright (C) 2011 Mathieu Desnoyers (mathieu.desnoyers@efficios.com)
 *
 * wrapper around poll __pollwait and poll_get_entry. Using KALLSYMS to get its
 * address when available, else we need to have a kernel that exports this
 * function to GPL modules.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/poll.h>

void wrapper_pollwait_exclusive(struct file *filp,
			 wait_queue_head_t *wait_address,
			 poll_table *p);

#endif /* _LTTNG_WRAPPER_POLL_H */
