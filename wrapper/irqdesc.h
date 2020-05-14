/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/irqdesc.h
 *
 * wrapper around irq_to_desc. Using KALLSYMS to get its address when
 * available, else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_IRQDESC_H
#define _LTTNG_WRAPPER_IRQDESC_H

#include <linux/interrupt.h>
#include <linux/irqnr.h>

struct irq_desc *wrapper_irq_to_desc(unsigned int irq);

/*
 * Canary function to check for 'irq_to_desc()' at compile time.
 *
 * From 'include/linux/irqnr.h':
 *
 *   extern struct irq_desc *irq_to_desc(unsigned int irq);
 */
static inline
struct irq_desc *__canary__irq_to_desc(unsigned int irq)
{
	return irq_to_desc(irq);
}

#endif /* _LTTNG_WRAPPER_IRQDESC_H */
