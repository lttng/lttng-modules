/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/uuid.h
 *
 * Copyright (C) 2020 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_UUID_H
#define _LTTNG_WRAPPER_UUID_H

#include <linux/version.h>
#include <linux/uuid.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0))
static inline
void lttng_guid_gen(guid_t *u)
{
	return guid_gen(u);
}
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)) */

typedef uuid_le guid_t;

static inline
void lttng_guid_gen(guid_t *u)
{
	return uuid_le_gen(u);
}
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)) */

#endif /* _LTTNG_WRAPPER_UUID_H */
