/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * wrapper/random.h
 *
 * wrapper around bootid read. Read the boot id through the /proc filesystem.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_RANDOM_H
#define _LTTNG_WRAPPER_RANDOM_H

#include <lttng/clock.h>

#define BOOT_ID_LEN	LTTNG_MODULES_UUID_STR_LEN

int wrapper_get_bootid(char *bootid);

#endif /* _LTTNG_WRAPPER_RANDOM_H */
