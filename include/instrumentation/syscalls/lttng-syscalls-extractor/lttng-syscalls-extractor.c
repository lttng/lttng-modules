/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-syscalls-extractor.c
 *
 * Dump syscall metadata to console.
 *
 * Copyright 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright 2011 Julien Desfossez <julien.desfossez@polymtl.ca>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/kallsyms.h>
#include <linux/dcache.h>
#include <linux/trace_events.h>
#include <trace/syscall.h>
#include <asm/syscall.h>

#ifndef CONFIG_FTRACE_SYSCALLS
#error "You need to set CONFIG_FTRACE_SYSCALLS=y"
#endif

#ifndef CONFIG_KALLSYMS_ALL
#error "You need to set CONFIG_KALLSYMS_ALL=y"
#endif

/*
 * The 'ident' parameter is prepended to each printk line to help
 * extract the proper lines from dmesg.
 */
static char *ident = "";
module_param(ident, charp, 0);

static struct syscall_metadata **__start_syscalls_metadata;
static struct syscall_metadata **__stop_syscalls_metadata;

static __init
struct syscall_metadata *find_syscall_meta(unsigned long syscall)
{
	struct syscall_metadata **iter;

	for (iter = __start_syscalls_metadata;
			iter < __stop_syscalls_metadata; iter++) {
		if ((*iter)->syscall_nr == syscall)
			return (*iter);
	}
	return NULL;
}

int init_module(void)
{
	struct syscall_metadata *meta;
	int i;

	__start_syscalls_metadata = (void *) kallsyms_lookup_name("__start_syscalls_metadata");
	__stop_syscalls_metadata = (void *) kallsyms_lookup_name("__stop_syscalls_metadata");

	printk("%s---START---\n", ident);
	for (i = 0; i < NR_syscalls; i++) {
		int j;

		meta = find_syscall_meta(i);
		if (!meta)
			continue;
		printk("%ssyscall %s nr %d nbargs %d ",
			ident, meta->name, meta->syscall_nr, meta->nb_args);
		printk(KERN_CONT "types: (");
		for (j = 0; j < meta->nb_args; j++) {
			if (j > 0)
				printk(KERN_CONT ", ");
			printk(KERN_CONT "%s", meta->types[j]);
		}
		printk(KERN_CONT ") ");
		printk(KERN_CONT "args: (");
		for (j = 0; j < meta->nb_args; j++) {
			if (j > 0)
				printk(KERN_CONT ", ");
			printk(KERN_CONT "%s", meta->args[j]);
		}
		printk(KERN_CONT ")\n");
	}
	printk("%s---END---\n", ident);

	/*
	 * This module always fails to load.
	 */
	return -1;
}

void cleanup_module(void)
{
}

MODULE_LICENSE("GPL");
