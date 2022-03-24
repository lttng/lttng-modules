/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/uaccess.h
 *
 * wrapper around linux/uaccess.h.
 *
 * Copyright (C) 2019 Michael Jeanson <mjeanson@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_UACCESS_H
#define _LTTNG_WRAPPER_UACCESS_H

#include <linux/uaccess.h>
#include <lttng/kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,0,0) || \
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,147,0,0, 4,19,0,0,0,0))

#define VERIFY_READ	0
#define VERIFY_WRITE	1
#define lttng_access_ok(type, addr, size) access_ok(addr, size)

#else /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,0,0) */

#define lttng_access_ok(type, addr, size) access_ok(type, addr, size)

#endif /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,0,0) */

#if LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,4,0)
static __always_inline __must_check int
lttng_copy_struct_from_user(void *dst, size_t ksize, const void __user *src,
		      size_t usize)
{
	return copy_struct_from_user(dst, ksize, src, usize);
}
#else /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,4,0) */
/**
 * lttng_check_zeroed_user: check if a userspace buffer only contains zero bytes
 * @from: Source address, in userspace.
 * @size: Size of buffer.
 *
 * This is effectively shorthand for "memchr_inv(from, 0, size) == NULL" for
 * userspace addresses (and is more efficient because we don't care where the
 * first non-zero byte is).
 *
 * Returns:
 *  * 0: There were non-zero bytes present in the buffer.
 *  * 1: The buffer was full of zero bytes.
 *  * -EFAULT: access to userspace failed.
 */
int lttng_check_zeroed_user(const void __user *from, size_t size)
{
	unsigned long val;
	uintptr_t align = (uintptr_t) from % sizeof(unsigned long);
	int ret;

	if (unlikely(size == 0))
		return 1;

	from -= align;
	size += align;

	if (!lttng_access_ok(VERIFY_READ, from, size)
		return -EFAULT;

	ret = get_user(val, (unsigned long __user *) from);
	if (ret)
		return ret;
	if (align)
		val &= ~aligned_byte_mask(align);

	while (size > sizeof(unsigned long)) {
		if (unlikely(val))
			goto done;

		from += sizeof(unsigned long);
		size -= sizeof(unsigned long);

		ret = get_user(val, (unsigned long __user *) from);
		if (ret)
			return ret;
	}

	if (size < sizeof(unsigned long))
		val &= aligned_byte_mask(size);

done:
	return (val == 0);
}

static __always_inline __must_check int
lttng_copy_struct_from_user(void *dst, size_t ksize, const void __user *src,
		      size_t usize)
{
	size_t size = min(ksize, usize);
	size_t rest = max(ksize, usize) - size;

	/* Deal with trailing bytes. */
	if (usize < ksize) {
		memset(dst + size, 0, rest);
	} else if (usize > ksize) {
		int ret = lttng_check_zeroed_user(src + size, rest);
		if (ret <= 0)
			return ret ?: -E2BIG;
	}
	/* Copy the interoperable parts of the struct. */
	if (copy_from_user(dst, src, size))
		return -EFAULT;
	return 0;
}
#endif /* LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,4,0) */

#endif /* _LTTNG_WRAPPER_UACCESS_H */
