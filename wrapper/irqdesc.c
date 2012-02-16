/*
 * Copyright (C) 2011 Mathieu Desnoyers (mathieu.desnoyers@efficios.com)
 *
 * wrapper around irq_to_desc. Using KALLSYMS to get its address when
 * available, else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#ifdef CONFIG_KALLSYMS

#include <linux/kallsyms.h>
#include <linux/interrupt.h>
#include <linux/irqnr.h>
#include "kallsyms.h"
#include "irqdesc.h"

static
struct irq_desc *(*irq_to_desc_sym)(unsigned int irq);

struct irq_desc *wrapper_irq_to_desc(unsigned int irq)
{
	if (!irq_to_desc_sym)
		irq_to_desc_sym = (void *) kallsyms_lookup_funcptr("irq_to_desc"); 
	if (irq_to_desc_sym) {
		return irq_to_desc_sym(irq);
	} else {
		printk(KERN_WARNING "LTTng: irq_to_desc symbol lookup failed.\n");
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
