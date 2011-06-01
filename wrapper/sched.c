/*
 * Copyright (C) 2011 Mathieu Desnoyers (mathieu.desnoyers@efficios.com)
 *
 * Wrapper around task_prio call.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/kallsyms.h>
#include <linux/sched.h>

void (*wrapper_task_prio_sym)(struct task_struct *t);

int wrapper_task_prio_init(void)
{
	wrapper_task_prio_sym = (void *) kallsyms_lookup_name("task_prio");
	if (!wrapper_task_prio_sym) {
		printk(KERN_WARNING "LTTng: task_prio symbol lookup failed.\n");
		return -EINVAL;
	}
	return 0;
}
