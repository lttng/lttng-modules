/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/probe-user.h
 *
 * Copyright (C) 2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_PROBE_USER_H
#define _LTTNG_PROBE_USER_H

#include <wrapper/uaccess.h>

/*
 * Calculate string length. Include final null terminating character if there is
 * one, or ends at first fault.
 */
long lttng_strlen_user_inatomic(const char *addr);

/*
 * We use __copy_from_user_inatomic to copy userspace data after
 * checking with access_ok() and disabling page faults.
 *
 * Return 0 if OK, nonzero on error.
 */
static inline
unsigned long lttng_copy_from_user_check_nofault(void *dest,
						 const void __user *src,
						 unsigned long len)
{
	unsigned long ret;

	if (!lttng_access_ok(VERIFY_READ, src, len))
		return 1;
	pagefault_disable();
	ret = __copy_from_user_inatomic(dest, src, len);
	pagefault_enable();
	return ret;
}

#endif /* _LTTNG_PROBE_USER_H */
