/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * lttng-kallsyms.h
 *
 * Copyright (C) 2019 Geneviève Bastien <gbastien@versatic.net>
 */

#ifndef _LTTNG_KALLSYMS_H
#define _LTTNG_KALLSYMS_H

#include <lttng-events.h>

#ifdef CONFIG_KALLSYMS

int lttng_enumerate_kernel_symbols(struct lttng_session *session);

#else /* !CONFIG_KALLSYMS */

static inline int lttng_enumerate_kernel_symbols(struct lttng_session *session)
{
	return 0;
}

#endif /* !CONFIG_KALLSYMS */

#endif /* _LTTNG_KALLSYMS_H */
