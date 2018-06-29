/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/uuid.h
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_UUID_H
#define _LTTNG_WRAPPER_UUID_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35))
#include <linux/uuid.h>
#else

#include <linux/random.h>

typedef struct {
	__u8 b[16];
} uuid_le;

static inline
void uuid_le_gen(uuid_le *u)
{
	generate_random_uuid(u->b);
}

#endif
#endif /* _LTTNG_WRAPPER_UUID_H */
