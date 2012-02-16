#ifndef _LTTNG_WRAPPER_IRQDESC_H
#define _LTTNG_WRAPPER_IRQDESC_H

/*
 * Copyright (C) 2011 Mathieu Desnoyers (mathieu.desnoyers@efficios.com)
 *
 * wrapper around irq_to_desc. Using KALLSYMS to get its address when
 * available, else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/interrupt.h>
#include <linux/irqnr.h>

struct irq_desc *wrapper_irq_to_desc(unsigned int irq);

#endif /* _LTTNG_WRAPPER_IRQDESC_H */
