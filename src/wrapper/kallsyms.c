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

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,7,0))

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

#ifdef LTTNG_CONFIG_PPC64_ELF_ABI_V2
static
void kallsyms_pre_arch_adjust(struct kprobe *probe)
{
	/*
	 * With powerpc64 ABIv2, we need the global entry point of
	 * kallsyms_lookup_name to call it later, while kprobe_register would
	 * automatically adjust the global entry point to the local entry point,
	 * when a kprobe was registered at a function entry. So we add 4 bytes
	 * which is the length of one instruction to kallsyms_lookup_name to
	 * avoid the adjustment.
	 */
	probe->offset = 4;
}
#else
static
void kallsyms_pre_arch_adjust(struct kprobe *probe)
{
}
#endif

#ifdef LTTNG_CONFIG_PPC64_ELF_ABI_V2
static
unsigned long kallsyms_get_arch_call_addr(const struct kprobe *probe)
{
	/* Substract 4 bytes to get what we originally want */
	return (unsigned long)(((char *)probe->addr) - 4);
}

#elif defined(LTTNG_CONFIG_PPC64_ELF_ABI_V1)
# if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,18,0))
# include <asm/elf.h>
# define LTTNG_FUNC_DESC_TYPE struct func_desc
# define LTTNG_FUNC_DESC_ADDR_NAME addr
# else
# include <asm/types.h>
# define LTTNG_FUNC_DESC_TYPE func_descr_t
# define LTTNG_FUNC_DESC_ADDR_NAME entry

static
LTTNG_FUNC_DESC_TYPE kallsyms_lookup_name_func_desc;
# endif

static
unsigned long kallsyms_get_arch_call_addr(const struct kprobe *probe)
{
	/*
	 * Build a function descriptor from the address of
	 * 'kallsyms_lookup_name' returned by kprobe and the toc of
	 * 'sprint_symbol' which is in the same compile unit and exported. I
	 * hate this on so many levels but it works.
	 */
	kallsyms_lookup_name_func_desc.LTTNG_FUNC_DESC_ADDR_NAME = (unsigned long) probe.addr;
	kallsyms_lookup_name_func_desc.toc = ((LTTNG_FUNC_DESC_TYPE *) &sprint_symbol)->toc;
	return (unsigned long) &kallsyms_lookup_name_func_desc;
}
#elif defined(CONFIG_ARM) && defined(CONFIG_THUMB2_KERNEL)
static
unsigned long kallsyms_get_arch_call_addr(const struct kprobe *probe)
{
	unsigned long addr = (unsigned long)probe->addr;

	if (addr)
		addr |= 1; /* set bit 0 in address for thumb mode */
	return addr;
}
#else
static
unsigned long kallsyms_get_arch_call_addr(const struct kprobe *probe)
{
	return (unsigned long)probe->addr;
}
#endif

static
unsigned long do_get_kallsyms(void)
{
	struct kprobe probe;
	int ret;
	unsigned long addr;

	memset(&probe, 0, sizeof(probe));
	probe.pre_handler = dummy_kprobe_handler;
	probe.symbol_name = "kallsyms_lookup_name";
	kallsyms_pre_arch_adjust(&probe);
	ret = register_kprobe(&probe);
	if (ret)
		return 0;
	addr = kallsyms_get_arch_call_addr(&probe);
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
		printk_once(KERN_WARNING "LTTng: requires kallsyms_lookup_name\n");
		return 0;
	}
}
EXPORT_SYMBOL_GPL(wrapper_kallsyms_lookup_name);

#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,4,0) && defined(CONFIG_ANDROID))
MODULE_IMPORT_NS(VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver);
#endif
