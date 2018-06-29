/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/ftrace.h
 *
 * wrapper around vmalloc_sync_all. Using KALLSYMS to get its address when
 * available, else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_FTRACE_H
#define _LTTNG_WRAPPER_FTRACE_H

#include <linux/ftrace.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,12,0))
#include <../kernel/trace/trace.h>
#endif

#ifdef CONFIG_KALLSYMS

#include <linux/kallsyms.h>
#include <wrapper/kallsyms.h>

static inline
int wrapper_register_ftrace_function_probe(char *glob,
		struct ftrace_probe_ops *ops, void *data)
{
	int (*register_ftrace_function_probe_sym)(char *glob,
			struct ftrace_probe_ops *ops, void *data);

	register_ftrace_function_probe_sym = (void *) kallsyms_lookup_funcptr("register_ftrace_function_probe");
	if (register_ftrace_function_probe_sym) {
		return register_ftrace_function_probe_sym(glob, ops, data);
	} else {
		printk_once(KERN_WARNING "LTTng: register_ftrace_function_probe symbol lookup failed.\n");
		return -EINVAL;
	}
}

static inline
void wrapper_unregister_ftrace_function_probe(char *glob,
		struct ftrace_probe_ops *ops, void *data)
{
	void (*unregister_ftrace_function_probe_sym)(char *glob,
			struct ftrace_probe_ops *ops, void *data);

	unregister_ftrace_function_probe_sym = (void *) kallsyms_lookup_funcptr("unregister_ftrace_function_probe");
	if (unregister_ftrace_function_probe_sym) {
		unregister_ftrace_function_probe_sym(glob, ops, data);
	} else {
		printk_once(KERN_WARNING "LTTng: unregister_ftrace_function_probe symbol lookup failed.\n");
		WARN_ON(1);
	}
}

#else

static inline
int wrapper_register_ftrace_function_probe(char *glob,
		struct ftrace_probe_ops *ops, void *data)
{
	return register_ftrace_function_probe(glob, ops, data);
}

static inline
void wrapper_unregister_ftrace_function_probe(char *glob,
		struct ftrace_probe_ops *ops, void *data)
{
	return unregister_ftrace_function_probe(glob, ops, data);
}
#endif

#endif /* _LTTNG_WRAPPER_FTRACE_H */
