/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/irqflags.h
 *
 * wrapper around IRQ flags.
 *
 * Copyright (C) 2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_IRQFLAGS_H
#define _LTTNG_WRAPPER_IRQFLAGS_H

#include <linux/version.h>
#include <linux/irqflags.h>

#ifdef CONFIG_X86

static inline
int lttng_regs_irqs_disabled(struct pt_regs *regs)
{
	unsigned long flags = regs->flags;

	return raw_irqs_disabled_flags(flags);
}

#else
/*
 * lttng_regs_irqs_disabled() returns -1 if irqoff state is unknown.
 * TODO: should implement lttng_regs_irqs_disabled for each architecture
 * to add interruptible context for kprobes and kretprobes.
 */

static inline
int lttng_regs_irqs_disabled(struct pt_regs *regs)
{
	return -1;
}
#endif

#endif /* _LTTNG_WRAPPER_IRQFLAGS_H */
