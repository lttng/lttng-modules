#ifndef _LTTNG_WRAPPER_RANDOM_H
#define _LTTNG_WRAPPER_RANDOM_H

/*
 * Copyright (C) 2011 Mathieu Desnoyers (mathieu.desnoyers@efficios.com)
 *
 * wrapper around bootid read. Using KALLSYMS to get its address when
 * available, else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#define BOOT_ID_LEN	37

int wrapper_get_bootid(char *bootid);

#endif /* _LTTNG_WRAPPER_RANDOM_H */
