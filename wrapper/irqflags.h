#ifndef _LTTNG_WRAPPER_IRQFLAGS_H
#define _LTTNG_WRAPPER_IRQFLAGS_H

/*
 * wrapper/irqflags.h
 *
 * wrapper around IRQ flags.
 *
 * Copyright (C) 2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

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
