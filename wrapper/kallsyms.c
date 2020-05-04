/* SPDX-License-Identifier: (GPL-2.0-only OR LGPL-2.1-only)
 *
 * wrapper/kallsyms.c
 *
 * Wrapper around kallsyms. Using kprobes to get its address when available.
 *
 * Can we mainline LTTng already so we don't have to waste our time doing this
 * kind of hack ?
 *
 * Copyright (C) 2020 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/kprobes.h>
#include <linux/module.h>
#include <wrapper/kallsyms.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0))

#ifndef CONFIG_KPROBES
# error "LTTng-modules requires CONFIG_KPROBES on kernels >= 5.7.0"
#endif

static
unsigned long (*kallsyms_lookup_name_sym)(const char *name);

static
int dummy_kprobe_handler(struct kprobe *p, struct pt_regs *regs)
{
	return 0;
}

static
unsigned long do_get_kallsyms(void)
{
	struct kprobe probe;
	int ret;
	unsigned long addr;

	memset(&probe, 0, sizeof(probe));
	probe.pre_handler = dummy_kprobe_handler;
	probe.symbol_name = "kallsyms_lookup_name";
	ret = register_kprobe(&probe);
	if (ret)
		return 0;
	addr = (unsigned long)probe.addr;
#ifdef CONFIG_ARM
#ifdef CONFIG_THUMB2_KERNEL
	if (addr)
		addr |= 1; /* set bit 0 in address for thumb mode */
#endif
#endif
	unregister_kprobe(&probe);
	return addr;
}

unsigned long wrapper_kallsyms_lookup_name(const char *name)
{
	if (!kallsyms_lookup_name_sym) {
		kallsyms_lookup_name_sym = (void *)do_get_kallsyms();
	}
	if (kallsyms_lookup_name_sym)
		return kallsyms_lookup_name_sym(name);
	else {
		printk_once(KERN_WARNING "LTTng requires kallsyms_lookup_name\n");
		return 0;
	}
}
EXPORT_SYMBOL_GPL(wrapper_kallsyms_lookup_name);

#endif
