/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-syscalls-extractor.c
 *
 * Dump syscall metadata to debugfs.
 *
 * Copyright 2011 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright 2011 EfficiOS Inc.
 * Copyright 2026 Michael Jeanson <mjeanson@efficios.com>
 */

#include <asm/syscall.h>
#include <linux/debugfs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <trace/syscall.h>
#include <wrapper/kallsyms.h>

#ifndef CONFIG_FTRACE_SYSCALLS
#error "You need to set CONFIG_FTRACE_SYSCALLS=y"
#endif

#ifndef CONFIG_KALLSYMS_ALL
#error "You need to set CONFIG_KALLSYMS_ALL=y"
#endif

#ifndef CONFIG_DEBUG_FS
#error "You need to set CONFIG_DEBUG_FS=y"
#endif

static struct syscall_metadata **__start_syscalls_metadata;
static struct syscall_metadata **__stop_syscalls_metadata;

static
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

static
int lse_debug_show(struct seq_file *m, void *p)
{
	struct syscall_metadata *meta;
	int i;

	__start_syscalls_metadata = (void *) wrapper_kallsyms_lookup_name("__start_syscalls_metadata");
	__stop_syscalls_metadata = (void *) wrapper_kallsyms_lookup_name("__stop_syscalls_metadata");

	for (i = 0; i < NR_syscalls; i++) {
		int j;

		meta = find_syscall_meta(i);
		if (!meta)
			continue;
		seq_printf(m, "syscall %s nr %d nbargs %d ",
			meta->name, meta->syscall_nr, meta->nb_args);
		seq_printf(m, KERN_CONT "types: (");
		for (j = 0; j < meta->nb_args; j++) {
			if (j > 0)
				seq_printf(m, KERN_CONT ", ");
			seq_printf(m, KERN_CONT "%s", meta->types[j]);
		}
		seq_printf(m, KERN_CONT ") ");
		seq_printf(m, KERN_CONT "args: (");
		for (j = 0; j < meta->nb_args; j++) {
			if (j > 0)
				seq_printf(m, KERN_CONT ", ");
			seq_printf(m, KERN_CONT "%s", meta->args[j]);
		}
		seq_printf(m, KERN_CONT ")\n");
	}

        return 0;
}

static
int lse_debug_open(struct inode *inode, struct file *file)
{
        return single_open(file, lse_debug_show, inode->i_private);
}

static const struct file_operations lse_debug_ops = {
        .open           = lse_debug_open,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};


static struct dentry *lttng_dir = NULL;

static
int __init lttng_syscalls_extractor_init(void)
{
	lttng_dir = debugfs_create_dir("lttng", NULL);

	debugfs_create_file("syscalls-extractor", 0444, lttng_dir, NULL, &lse_debug_ops);

	return 0;
}
module_init(lttng_syscalls_extractor_init);

static
void __exit lttng_syscalls_extractor_exit(void)
{
	debugfs_remove(lttng_dir);
}
module_exit(lttng_syscalls_extractor_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng Syscalls Extractor");
