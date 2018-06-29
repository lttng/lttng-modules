/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/irq.h
 *
 * wrapper around linux/irq.h.
 *
 * Copyright (C) 2013 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_IRQ_H
#define _LTTNG_WRAPPER_IRQ_H

#include <linux/version.h>

/*
 * Starting from the 3.12 Linux kernel, all architectures use the
 * generic hard irqs system. More details can be seen at commit
 * 0244ad004a54e39308d495fee0a2e637f8b5c317 in the Linux kernel GIT.
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0) \
	|| defined(CONFIG_GENERIC_HARDIRQS))
# define CONFIG_LTTNG_HAS_LIST_IRQ
#endif

#endif /* _LTTNG_WRAPPER_IRQ_H */
