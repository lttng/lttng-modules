/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-probe-user.h
 *
 * Copyright (C) 2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_PROBE_USER_H
#define _LTTNG_PROBE_USER_H

/*
 * Calculate string length. Include final null terminating character if there is
 * one, or ends at first fault.
 */
long lttng_strlen_user_inatomic(const char *addr);

#endif /* _LTTNG_PROBE_USER_H */
