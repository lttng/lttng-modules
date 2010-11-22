/*
 * ltt/probes/kvm-trace.c
 *
 * KVM tracepoint probes.
 *
 * (C) Copyright 2010 - Julien Desfossez <julien.desfossez@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 *
 */

#include <linux/module.h>
#include <asm/vmx.h>
#include <linux/kvm_host.h>
#include <asm/kvm-trace.h>

void probe_kvm_entry(void *_data, unsigned int vcpu_id)
{
	trace_mark_tp(kvm, kvm_entry, kvm_entry,
		probe_kvm_entry,
		"vcpu %u",
		vcpu_id);
}

void probe_kvm_hypercall(void *_data, unsigned long nr, unsigned long a0,
		unsigned long a1, unsigned long a2, unsigned long a3)
{
	trace_mark_tp(kvm, kvm_hypercall, kvm_hypercall,
		probe_kvm_hypercall,
		"nr 0x%lx a0 0x%lx a1 0x%lx a2 0x%lx a3 0x%lx",
		nr, a0, a1, a2, a3);
}

void probe_kvm_pio(void *_data, unsigned int rw, unsigned int port,
		unsigned int size, unsigned int count)
{
	trace_mark_tp(kvm, kvm_pio, kvm_pio,
		probe_kvm_pio,
		"pio_%s at 0x%x size %d count %d",
		rw ? "write" : "read",
		port, size, count);
}

void probe_kvm_cpuid(void *_data, unsigned int function, unsigned long rax,
		unsigned long rbx, unsigned long rcx, unsigned long rdx)
{
	trace_mark_tp(kvm, kvm_cpuid, kvm_cpuid,
		probe_kvm_cpuid,
		"func %x rax %lx rbx %lx rcx %lx rdx %lx",
		function, rax, rbx, rcx, rdx);
}

void probe_kvm_apic (void *_data, unsigned int rw, unsigned int reg,
		unsigned int val)
{
	trace_mark_tp(kvm, kvm_apic, kvm_apic,
		probe_kvm_apic,
		"apic_%s  reg %d = 0x%x",
		rw ? "write" : "read",
		reg, val);
}

void probe_kvm_exit(void *_data, unsigned int exit_reason, struct kvm_vcpu *vcpu)
{
	trace_mark_tp(kvm, kvm_exit, kvm_exit,
		probe_kvm_exit,
		"reason %d",
		exit_reason);
}

void probe_kvm_inj_virq(void *_data, unsigned int irq)
{
	trace_mark_tp(kvm, kvm_inj_virq, kvm_inj_virq,
		probe_kvm_inj_virq,
		"irq %u", irq);
}

void probe_kvm_page_fault(void *_data, unsigned long fault_address,
		unsigned int error_code)
{
	trace_mark_tp(kvm, kvm_page_fault, kvm_page_fault,
		probe_kvm_page_fault,
		"address %lx error_code %x",
		fault_address, error_code);
}

void probe_kvm_msr(void *_data, unsigned int rw, __u32 ecx, __u64 data, bool exception)
{
	trace_mark_tp(kvm, kvm_msr, kvm_msr,
		probe_kvm_msr,
		"msr_%s %x = 0x%llx",
		rw ? "read" : "write",
		ecx, data);
}

void probe_kvm_cr(void *_data, unsigned int rw, unsigned int cr, unsigned long val)
{
	trace_mark_tp(kvm, kvm_cr, kvm_cr,
		probe_kvm_cr,
		"cr_%s %x = 0x%lx",
		rw ? "read" : "write",
		cr, val);
}

void probe_kvm_pic_set_irq(void *_data, __u8 chip, __u8 pin, __u8 elcr,
		__u8 imr, bool coalesced)
{
	trace_mark_tp(kvm, kvm_pic_set_irq, kvm_pic_set_irq,
		probe_kvm_pic_set_irq,
		"chip %u pin %u (%s%s)%s",
		chip, pin,
		(elcr & (1 << pin)) ? "level":"edge",
		(imr & (1 << pin)) ? "|masked":"",
		coalesced ? " (coalesced)" : "");
}

void probe_kvm_apic_ipi(void *_data, __u32 icr_low, __u32 dest_id)
{
	trace_mark_tp(kvm, kvm_apic_ipi, kvm_apic_ipi,
		probe_kvm_apic_ipi,
		"dst %x vec %u (%x|%s|%s|%s|%x)",
		dest_id, icr_low,
		icr_low >> 8 & 0x7,
		(icr_low & (1<<11)) ? "logical" : "physical",
		(icr_low & (1<<14)) ? "assert" : "de-assert",
		(icr_low & (1<<15)) ? "level" : "edge",
		icr_low >> 18 & 0x3);
}

void probe_kvm_apic_accept_irq(void *_data, __u32 apicid, __u16 dm, __u8 tm,
		__u8 vec, bool coalesced)
{
	trace_mark_tp(kvm, kvm_apic_accept_irq, kvm_apic_accept_irq,
		probe_kvm_apic_accept_irq,
		"apicid %x vec %u (%x|%s)%s",
		apicid, vec, dm >> 8 & 0x7,
		tm ? "level" : "edge",
		coalesced ? " (coalesced)" : "");
}

void probe_kvm_nested_vmrun(void *_data, __u64 rip, __u64 vmcb, __u64 nested_rip,
		__u32 int_ctl, __u32 event_inj, bool npt)
{
	trace_mark_tp(kvm, kvm_nested_vmrun, kvm_nested_vmrun,
		probe_kvm_nested_vmrun,
		"rip 0x%Lx vmcb 0x%Lx nrip 0x%Lx int_ctl 0x%x "
		    "event_inj 0x%x npt %s",
		(unsigned long long)rip,
		(unsigned long long)vmcb,
		(unsigned long long)nested_rip,
		int_ctl, event_inj,
		npt ? "on" : "off");
}

void probe_kvm_nested_vmexit(void *_data, __u64 rip, __u32 exit_code,
		__u64 exit_info1, __u64 exit_info2,
		__u32 exit_int_info, __u32 exit_int_info_err)
{
	trace_mark_tp(kvm, kvm_nested_vmexit, kvm_nested_vmexit,
		probe_kvm_nested_vmexit,
		"rip 0x%Lx reason %d ext_inf1 0x%Lx "
	        "ext_inf2 0x%Lx ext_int 0x%x ext_int_err 0x%x",
		(unsigned long long)rip,
		exit_code,
		(unsigned long long)exit_info1,
		(unsigned long long)exit_info2,
		exit_int_info, exit_int_info_err);
}

void probe_kvm_nested_vmexit_inject(void *_data, __u32 exit_code,
		__u64 exit_info1, __u64 exit_info2,
		__u32 exit_int_info, __u32 exit_int_info_err)
{
	trace_mark_tp(kvm, kvm_nested_vmexit_inject, kvm_nested_vmexit_inject,
		probe_kvm_nested_vmexit_inject,
		"reason %d ext_inf1 0x%Lx "
			"ext_inf2 0x%Lx ext_int 0x%x ext_int_err 0x%x",
		exit_code,
		(unsigned long long)exit_info1,
		(unsigned long long)exit_info2,
		exit_int_info, exit_int_info_err);
}

void probe_kvm_nested_intr_vmexit(void *_data, __u64 rip)
{
	trace_mark_tp(kvm, kvm_nested_intr_vmexit, kvm_nested_intr_vmexit,
		probe_kvm_nested_intr_vmexit,
		"rip 0x%Lx", (unsigned long long)rip );
}

void probe_kvm_invlpga(void *_data, __u64 rip, int asid, u64 address)
{
	trace_mark_tp(kvm, kvm_invlpga, kvm_invlpga,
		probe_kvm_invlpga,
		"rip 0x%Lx asid %d address 0x%Lx",
		(unsigned long long)rip,
		asid,
		(unsigned long long)address);
}

void probe_kvm_skinit(void *_data, __u64 rip, __u32 slb)
{
	trace_mark_tp(kvm, kvm_skinit, kvm_skinit,
		probe_kvm_skinit,
		"rip 0x%Lx slb 0x%x",
		(unsigned long long)rip, slb);
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Julien Desfossez");
MODULE_DESCRIPTION("KVM Tracepoint Probes");
