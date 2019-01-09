/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-probe-user.c
 *
 * Copyright (C) 2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/uaccess.h>
#include <linux/module.h>
#include <wrapper/uaccess.h>
#include <probes/lttng-probe-user.h>

/*
 * Calculate string length. Include final null terminating character if there is
 * one, or ends at first fault. Disabling page faults ensures that we can safely
 * call this from pretty much any context, including those where the caller
 * holds mmap_sem, or any lock which nests in mmap_sem.
 */
long lttng_strlen_user_inatomic(const char *addr)
{
	long count = 0;
	mm_segment_t old_fs;

	if (!addr)
		return 0;

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	pagefault_disable();
	for (;;) {
		char v;
		unsigned long ret;

		if (unlikely(!lttng_access_ok(VERIFY_READ,
				(__force const char __user *) addr,
				sizeof(v))))
			break;
		ret = __copy_from_user_inatomic(&v,
			(__force const char __user *)(addr),
			sizeof(v));
		if (unlikely(ret > 0))
			break;
		count++;
		if (unlikely(!v))
			break;
		addr++;
	}
	pagefault_enable();
	set_fs(old_fs);
	return count;
}
EXPORT_SYMBOL_GPL(lttng_strlen_user_inatomic);
