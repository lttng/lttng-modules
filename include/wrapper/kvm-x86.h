/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * wrapper/kvm-x86.h
 *
 * Copyright (C) 2022 Michael Jeanson <mjeanson@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_KVM_X86_H
#define _LTTNG_WRAPPER_KVM_X86_H

#include <linux/kvm_host.h>
#include <lttng/kernel-version.h>

#if LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,18,0)

#ifdef CONFIG_KALLSYMS_ALL
#include <linux/kallsyms.h>
#include <wrapper/kallsyms.h>

static struct kvm_x86_ops *kvm_x86_ops_sym;

static inline
void lttng_kvm_x86_get_exit_info(
		struct kvm_vcpu *vcpu, u32 *reason,
		u64 *info1, u64 *info2,
		u32 *exit_int_info, u32 *exit_int_info_err_code)
{
	if (!kvm_x86_ops_sym)
		kvm_x86_ops_sym =
			(void *) kallsyms_lookup_dataptr("kvm_x86_ops");

	if (kvm_x86_ops_sym) {
		kvm_x86_ops_sym->get_exit_info(vcpu, reason, info1, info2,
				exit_int_info, exit_int_info_err_code);
	} else {
		printk_once(KERN_WARNING "LTTng: kvm_x86_ops symbol lookup failed.\n");
	}
}

static inline
u64 lttng_kvm_x86_get_segment_base(struct kvm_vcpu *vcpu, int seg)
{
	if (!kvm_x86_ops_sym)
		kvm_x86_ops_sym =
			(void *) kallsyms_lookup_dataptr("kvm_x86_ops");

	if (kvm_x86_ops_sym) {
		return kvm_x86_ops_sym->get_segment_base(vcpu, seg);
	} else {
		printk_once(KERN_WARNING "LTTng: kvm_x86_ops symbol lookup failed.\n");
		return 0;
	}
}

/*
 * Canary function to check for 'kvm_x86_ops' at compile time.
 *
 * From 'arch/x86/include/asm/kvm_host.h':
 *
 *   extern struct kvm_x86_ops kvm_x86_ops;;
 */
static inline
unsigned int __canary__kvm_x86_ops(void)
{
	return kvm_x86_ops.vm_size;
}

#endif /* CONFIG_KALLSYMS_ALL */

#endif

#endif /* _LTTNG_WRAPPER_KVM_X86_H */
