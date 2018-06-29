/* SPDX-License-Identifier: (GPL-2.0 OR LGPL-2.1)
 *
 * wrapper/irqdesc.c
 *
 * wrapper around irq_to_desc. Using KALLSYMS to get its address when
 * available, else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifdef CONFIG_KALLSYMS

#include <linux/kallsyms.h>
#include <linux/interrupt.h>
#include <linux/irqnr.h>
#include <wrapper/kallsyms.h>
#include <wrapper/irqdesc.h>

static
struct irq_desc *(*irq_to_desc_sym)(unsigned int irq);

struct irq_desc *wrapper_irq_to_desc(unsigned int irq)
{
	if (!irq_to_desc_sym)
		irq_to_desc_sym = (void *) kallsyms_lookup_funcptr("irq_to_desc"); 
	if (irq_to_desc_sym) {
		return irq_to_desc_sym(irq);
	} else {
		printk_once(KERN_WARNING "LTTng: irq_to_desc symbol lookup failed.\n");
		return NULL;
	}
}

#else

#include <linux/interrupt.h>
#include <linux/irqnr.h>

struct irq_desc *wrapper_irq_to_desc(unsigned int irq)
{
	return irq_to_desc(irq);
}

#endif
