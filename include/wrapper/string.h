// SPDX-FileCopyrightText: 2026 Michael Jeanson <mjeanson@efficios.com>
// SPDX-License-Identifier: GPL-2.0-only

#ifndef _LTTNG_WRAPPER_STRING_H
#define _LTTNG_WRAPPER_STRING_H

#include <lttng/kernel-version.h>
#include <linux/string.h>

/**
 * strscpy_pad() - Copy a C-string into a sized buffer
 * @dst: Where to copy the string to
 * @src: Where to copy the string from
 * @size: Size of destination buffer
 *
 * Copy the string, or as much of it as fits, into the dest buffer. The
 * behavior is undefined if the string buffers overlap. The destination
 * buffer is always %NUL terminated, unless it's zero-sized.
 *
 * If the source string is shorter than the destination buffer, the
 * remaining bytes in the buffer will be filled with %NUL bytes.
 *
 * For full explanation of why you may want to consider using the
 * 'strscpy' functions please see the function docstring for strscpy().
 *
 * Returns:
 * * The number of characters copied (not including the trailing %NULs)
 * * -E2BIG if count is 0 or @src was truncated.
 */
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,2,0))
static inline
ssize_t lttng_strscpy_pad(char *dst, const char *src, size_t count)
{
	return strscpy_pad(dst, src, count);
}
#else
static inline
ssize_t lttng_strscpy_pad(char *dst, const char *src, size_t count)
{
	ssize_t ret;

	if (count == 0)
		 return -E2BIG;

	strncpy(dst, src, count);

	ret = strnlen(dst, count);
	if (ret >= count) {
		dst[count - 1] = '\0';
		ret = -E2BIG;
	}

	return ret;
}
#endif

#endif /* _LTTNG_WRAPPER_STRING_H */
