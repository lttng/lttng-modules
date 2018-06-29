/* SPDX-License-Identifier: (GPL-2.0 OR LGPL-2.1)
 *
 * wrapper/random.h
 *
 * wrapper around bootid read. Using KALLSYMS to get its address when
 * available, else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_RANDOM_H
#define _LTTNG_WRAPPER_RANDOM_H

#include <lttng-clock.h>

#define BOOT_ID_LEN	LTTNG_MODULES_UUID_STR_LEN

int wrapper_get_bootid(char *bootid);

#endif /* _LTTNG_WRAPPER_RANDOM_H */
