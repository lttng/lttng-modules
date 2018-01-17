#ifndef _LTTNG_WRAPPER_KSTRTOX_H
#define _LTTNG_WRAPPER_KSTRTOX_H

/*
 * wrapper/kstrtox.h
 *
 * Copyright (C) 2015 Michael Jeanson <mjeanson@efficios.com>
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

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,0,0))

#include <linux/uaccess.h>

/* Excludes final \0. */
#define LTTNG_MAX_UINT_CHAR 10

static inline
int __must_check lttng_kstrtouint_from_user(const char __user *ubuf,
		size_t count, unsigned int base, unsigned int *res)
{
	unsigned int _res;
	char kbuf[LTTNG_MAX_UINT_CHAR + 1], *endptr;

	memset(kbuf, 0, sizeof(kbuf));
	if (copy_from_user(kbuf, ubuf, min_t(size_t, LTTNG_MAX_UINT_CHAR, count)))
		return -EFAULT;

	_res = simple_strtoul(kbuf, &endptr, base);
	if (!endptr)
		return -EINVAL;

	*res = _res;
	return 0;
}
#else
static inline
int __must_check lttng_kstrtouint_from_user(const char __user *ubuf,
		size_t count, unsigned int base, unsigned int *res)
{
	return kstrtouint_from_user(ubuf, count, base, res);
}
#endif

#endif /* _LTTNG_WRAPPER_KSTRTOX_H */
