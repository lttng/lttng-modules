#ifndef _LTT_WRAPPER_VMALLOC_H
#define _LTT_WRAPPER_VMALLOC_H

/*
 * Copyright (C) 2011 Mathieu Desnoyers (mathieu.desnoyers@efficios.com)
 *
 * Wrapper around task_prio call.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#ifdef CONFIG_KALLSYMS

#include "../ltt-events.h"

static inline
int wrapper_task_prio(struct task_struct *t)
{
	return wrapper_task_prio_sym(t);
}
#else

#include <linux/sched.h>

static inline
int wrapper_task_prio(struct task_struct *t)
{
	return task_prio(t);
}
#endif

#endif /* _LTT_WRAPPER_VMALLOC_H */
