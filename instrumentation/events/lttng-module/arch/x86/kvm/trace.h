#if !defined(LTTNG_TRACE_KVM_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_KVM_H

#include <probes/lttng-tracepoint-event.h>
#include <asm/vmx.h>
#include <asm/svm.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
#include <asm/clocksource.h>
#endif
#include <linux/version.h>
#include <../arch/x86/kvm/lapic.h>
#include <../arch/x86/kvm/kvm_cache_regs.h>

#undef TRACE_SYSTEM
#define TRACE_SYSTEM kvm_x86

/*
 * Tracepoint for guest mode entry.
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_entry, kvm_x86_entry,
	TP_PROTO(unsigned int vcpu_id),
	TP_ARGS(vcpu_id),

	TP_FIELDS(
		ctf_integer(unsigned int, vcpu_id, vcpu_id)
	)
)

/*
 * Tracepoint for hypercall.
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_hypercall, kvm_x86_hypercall,
	TP_PROTO(unsigned long nr, unsigned long a0, unsigned long a1,
		 unsigned long a2, unsigned long a3),
	TP_ARGS(nr, a0, a1, a2, a3),

	TP_FIELDS(
		ctf_integer(unsigned long, nr, nr)
		ctf_integer(unsigned long, a0, a0)
		ctf_integer(unsigned long, a1, a1)
		ctf_integer(unsigned long, a2, a2)
		ctf_integer(unsigned long, a3, a3)
	)
)

/*
 * Tracepoint for hypercall.
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_hv_hypercall, kvm_x86_hv_hypercall,
	TP_PROTO(__u16 code, bool fast, __u16 rep_cnt, __u16 rep_idx,
		 __u64 ingpa, __u64 outgpa),
	TP_ARGS(code, fast, rep_cnt, rep_idx, ingpa, outgpa),

	TP_FIELDS(
		ctf_integer(__u16, rep_cnt, rep_cnt)
		ctf_integer(__u16, rep_idx, rep_idx)
		ctf_integer(__u64, ingpa, ingpa)
		ctf_integer(__u64, outgpa, outgpa)
		ctf_integer(__u16, code, code)
		ctf_integer(bool, fast, fast)
	)
)

/*
 * Tracepoint for PIO.
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_pio, kvm_x86_pio,
	TP_PROTO(unsigned int rw, unsigned int port, unsigned int size,
		 unsigned int count),
	TP_ARGS(rw, port, size, count),

	TP_FIELDS(
		ctf_integer(unsigned int, rw, rw)
		ctf_integer(unsigned int, port, port)
		ctf_integer(unsigned int, size, size)
		ctf_integer(unsigned int, count, count)
	)
)

/*
 * Tracepoint for cpuid.
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_cpuid, kvm_x86_cpuid,
	TP_PROTO(unsigned int function, unsigned long rax, unsigned long rbx,
		 unsigned long rcx, unsigned long rdx),
	TP_ARGS(function, rax, rbx, rcx, rdx),

	TP_FIELDS(
		ctf_integer(unsigned int, function, function)
		ctf_integer(unsigned long, rax, rax)
		ctf_integer(unsigned long, rbx, rbx)
		ctf_integer(unsigned long, rcx, rcx)
		ctf_integer(unsigned long, rdx, rdx)
	)
)

/*
 * Tracepoint for apic access.
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_apic, kvm_x86_apic,
	TP_PROTO(unsigned int rw, unsigned int reg, unsigned int val),
	TP_ARGS(rw, reg, val),

	TP_FIELDS(
		ctf_integer(unsigned int, rw, rw)
		ctf_integer(unsigned int, reg, reg)
		ctf_integer(unsigned int, val, val)
	)
)

#define trace_kvm_apic_read(reg, val)		trace_kvm_apic(0, reg, val)
#define trace_kvm_apic_write(reg, val)		trace_kvm_apic(1, reg, val)

/*
 * Tracepoint for kvm guest exit:
 */
LTTNG_TRACEPOINT_EVENT_CODE_MAP(kvm_exit, kvm_x86_exit,
	TP_PROTO(unsigned int exit_reason, struct kvm_vcpu *vcpu, u32 isa),
	TP_ARGS(exit_reason, vcpu, isa),

	TP_locvar(
		u64 info1, info2;
	),

	TP_code_pre(
		kvm_x86_ops->get_exit_info(vcpu, &tp_locvar->info1,
				&tp_locvar->info2);
	),

	TP_FIELDS(
		ctf_integer(unsigned int, exit_reason, exit_reason)
		ctf_integer(unsigned long, guest_rip, kvm_rip_read(vcpu))
		ctf_integer(u32, isa, isa)
		ctf_integer(u64, info1, tp_locvar->info1)
		ctf_integer(u64, info2, tp_locvar->info2)
	),

	TP_code_post()
)

/*
 * Tracepoint for kvm interrupt injection:
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_inj_virq, kvm_x86_inj_virq,
	TP_PROTO(unsigned int irq),
	TP_ARGS(irq),

	TP_FIELDS(
		ctf_integer(unsigned int, irq, irq)
	)
)

/*
 * Tracepoint for kvm interrupt injection:
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_inj_exception, kvm_x86_inj_exception,
	TP_PROTO(unsigned exception, bool has_error, unsigned error_code),
	TP_ARGS(exception, has_error, error_code),

	TP_FIELDS(
		ctf_integer(u8, exception, exception)
		ctf_integer(u8, has_error, has_error)
		ctf_integer(u32, error_code, error_code)
	)
)

/*
 * Tracepoint for page fault.
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_page_fault, kvm_x86_page_fault,
	TP_PROTO(unsigned long fault_address, unsigned int error_code),
	TP_ARGS(fault_address, error_code),

	TP_FIELDS(
		ctf_integer(unsigned long, fault_address, fault_address)
		ctf_integer(unsigned int, error_code, error_code)
	)
)

/*
 * Tracepoint for guest MSR access.
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_msr, kvm_x86_msr,
	TP_PROTO(unsigned write, u32 ecx, u64 data, bool exception),
	TP_ARGS(write, ecx, data, exception),

	TP_FIELDS(
		ctf_integer(unsigned, write, write)
		ctf_integer(u32, ecx, ecx)
		ctf_integer(u64, data, data)
		ctf_integer(u8, exception, exception)
	)
)

#define trace_kvm_msr_read(ecx, data)      trace_kvm_msr(0, ecx, data, false)
#define trace_kvm_msr_write(ecx, data)     trace_kvm_msr(1, ecx, data, false)
#define trace_kvm_msr_read_ex(ecx)         trace_kvm_msr(0, ecx, 0, true)
#define trace_kvm_msr_write_ex(ecx, data)  trace_kvm_msr(1, ecx, data, true)

/*
 * Tracepoint for guest CR access.
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_cr, kvm_x86_cr,
	TP_PROTO(unsigned int rw, unsigned int cr, unsigned long val),
	TP_ARGS(rw, cr, val),

	TP_FIELDS(
		ctf_integer(unsigned int, rw, rw)
		ctf_integer(unsigned int, cr, cr)
		ctf_integer(unsigned long, val, val)
	)
)

#define trace_kvm_cr_read(cr, val)		trace_kvm_cr(0, cr, val)
#define trace_kvm_cr_write(cr, val)		trace_kvm_cr(1, cr, val)

LTTNG_TRACEPOINT_EVENT_MAP(kvm_pic_set_irq, kvm_x86_pic_set_irq,
	    TP_PROTO(__u8 chip, __u8 pin, __u8 elcr, __u8 imr, bool coalesced),
	    TP_ARGS(chip, pin, elcr, imr, coalesced),

	TP_FIELDS(
		ctf_integer(__u8, chip, chip)
		ctf_integer(__u8, pin, pin)
		ctf_integer(__u8, elcr, elcr)
		ctf_integer(__u8, imr, imr)
		ctf_integer(bool, coalesced, coalesced)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(kvm_apic_ipi, kvm_x86_apic_ipi,
	    TP_PROTO(__u32 icr_low, __u32 dest_id),
	    TP_ARGS(icr_low, dest_id),

	TP_FIELDS(
		ctf_integer(__u32, icr_low, icr_low)
		ctf_integer(__u32, dest_id, dest_id)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(kvm_apic_accept_irq, kvm_x86_apic_accept_irq,
	    TP_PROTO(__u32 apicid, __u16 dm, __u8 tm, __u8 vec, bool coalesced),
	    TP_ARGS(apicid, dm, tm, vec, coalesced),

	TP_FIELDS(
		ctf_integer(__u32, apicid, apicid)
		ctf_integer(__u16, dm, dm)
		ctf_integer(__u8, tm, tm)
		ctf_integer(__u8, vec, vec)
		ctf_integer(bool, coalesced, coalesced)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(kvm_eoi, kvm_x86_eoi,
	    TP_PROTO(struct kvm_lapic *apic, int vector),
	    TP_ARGS(apic, vector),

	TP_FIELDS(
		ctf_integer(__u32, apicid, apic->vcpu->vcpu_id)
		ctf_integer(int, vector, vector)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(kvm_pv_eoi, kvm_x86_pv_eoi,
	    TP_PROTO(struct kvm_lapic *apic, int vector),
	    TP_ARGS(apic, vector),

	TP_FIELDS(
		ctf_integer(__u32, apicid, apic->vcpu->vcpu_id)
		ctf_integer(int, vector, vector)
	)
)

/*
 * Tracepoint for nested VMRUN
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_nested_vmrun, kvm_x86_nested_vmrun,
	    TP_PROTO(__u64 rip, __u64 vmcb, __u64 nested_rip, __u32 int_ctl,
		     __u32 event_inj, bool npt),
	    TP_ARGS(rip, vmcb, nested_rip, int_ctl, event_inj, npt),

	TP_FIELDS(
		ctf_integer(__u64, rip, rip)
		ctf_integer(__u64, vmcb, vmcb)
		ctf_integer(__u64, nested_rip, nested_rip)
		ctf_integer(__u32, int_ctl, int_ctl)
		ctf_integer(__u32, event_inj, event_inj)
		ctf_integer(bool, npt, npt)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(kvm_nested_intercepts, kvm_x86_nested_intercepts,
	    TP_PROTO(__u16 cr_read, __u16 cr_write, __u32 exceptions, __u64 intercept),
	    TP_ARGS(cr_read, cr_write, exceptions, intercept),

	TP_FIELDS(
		ctf_integer(__u16, cr_read, cr_read)
		ctf_integer(__u16, cr_write, cr_write)
		ctf_integer(__u32, exceptions, exceptions)
		ctf_integer(__u64, intercept, intercept)
	)
)
/*
 * Tracepoint for #VMEXIT while nested
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_nested_vmexit, kvm_x86_nested_vmexit,
	    TP_PROTO(__u64 rip, __u32 exit_code,
		     __u64 exit_info1, __u64 exit_info2,
		     __u32 exit_int_info, __u32 exit_int_info_err, __u32 isa),
	    TP_ARGS(rip, exit_code, exit_info1, exit_info2,
		    exit_int_info, exit_int_info_err, isa),

	TP_FIELDS(
		ctf_integer(__u64, rip, rip)
		ctf_integer(__u32, exit_code, exit_code)
		ctf_integer(__u64, exit_info1, exit_info1)
		ctf_integer(__u64, exit_info2, exit_info2)
		ctf_integer(__u32, exit_int_info, exit_int_info)
		ctf_integer(__u32, exit_int_info_err, exit_int_info_err)
		ctf_integer(__u32, isa, isa)
	)
)

/*
 * Tracepoint for #VMEXIT reinjected to the guest
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_nested_vmexit_inject, kvm_x86_nested_vmexit_inject,
	    TP_PROTO(__u32 exit_code,
		     __u64 exit_info1, __u64 exit_info2,
		     __u32 exit_int_info, __u32 exit_int_info_err, __u32 isa),
	    TP_ARGS(exit_code, exit_info1, exit_info2,
		    exit_int_info, exit_int_info_err, isa),

	TP_FIELDS(
		ctf_integer(__u32, exit_code, exit_code)
		ctf_integer(__u64, exit_info1, exit_info1)
		ctf_integer(__u64, exit_info2, exit_info2)
		ctf_integer(__u32, exit_int_info, exit_int_info)
		ctf_integer(__u32, exit_int_info_err, exit_int_info_err)
		ctf_integer(__u32, isa, isa)
	)
)

/*
 * Tracepoint for nested #vmexit because of interrupt pending
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_nested_intr_vmexit, kvm_x86_nested_intr_vmexit,
	    TP_PROTO(__u64 rip),
	    TP_ARGS(rip),

	TP_FIELDS(
		ctf_integer(__u64, rip, rip)
	)
)

/*
 * Tracepoint for nested #vmexit because of interrupt pending
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_invlpga, kvm_x86_invlpga,
	    TP_PROTO(__u64 rip, int asid, u64 address),
	    TP_ARGS(rip, asid, address),

	TP_FIELDS(
		ctf_integer(__u64, rip, rip)
		ctf_integer(int, asid, asid)
		ctf_integer(__u64, address, address)
	)
)

/*
 * Tracepoint for nested #vmexit because of interrupt pending
 */
LTTNG_TRACEPOINT_EVENT_MAP(kvm_skinit, kvm_x86_skinit,
	    TP_PROTO(__u64 rip, __u32 slb),
	    TP_ARGS(rip, slb),

	TP_FIELDS(
		ctf_integer(__u64, rip, rip)
		ctf_integer(__u32, slb, slb)
	)
)

#define KVM_EMUL_INSN_F_CR0_PE (1 << 0)
#define KVM_EMUL_INSN_F_EFL_VM (1 << 1)
#define KVM_EMUL_INSN_F_CS_D   (1 << 2)
#define KVM_EMUL_INSN_F_CS_L   (1 << 3)

#define kvm_trace_symbol_emul_flags	                  \
	{ 0,   			    "real" },		  \
	{ KVM_EMUL_INSN_F_CR0_PE			  \
	  | KVM_EMUL_INSN_F_EFL_VM, "vm16" },		  \
	{ KVM_EMUL_INSN_F_CR0_PE,   "prot16" },		  \
	{ KVM_EMUL_INSN_F_CR0_PE			  \
	  | KVM_EMUL_INSN_F_CS_D,   "prot32" },		  \
	{ KVM_EMUL_INSN_F_CR0_PE			  \
	  | KVM_EMUL_INSN_F_CS_L,   "prot64" }

#define kei_decode_mode(mode) ({			\
	u8 flags = 0xff;				\
	switch (mode) {					\
	case X86EMUL_MODE_REAL:				\
		flags = 0;				\
		break;					\
	case X86EMUL_MODE_VM86:				\
		flags = KVM_EMUL_INSN_F_EFL_VM;		\
		break;					\
	case X86EMUL_MODE_PROT16:			\
		flags = KVM_EMUL_INSN_F_CR0_PE;		\
		break;					\
	case X86EMUL_MODE_PROT32:			\
		flags = KVM_EMUL_INSN_F_CR0_PE		\
			| KVM_EMUL_INSN_F_CS_D;		\
		break;					\
	case X86EMUL_MODE_PROT64:			\
		flags = KVM_EMUL_INSN_F_CR0_PE		\
			| KVM_EMUL_INSN_F_CS_L;		\
		break;					\
	}						\
	flags;						\
	})

LTTNG_TRACEPOINT_EVENT_MAP(kvm_emulate_insn, kvm_x86_emulate_insn,
	TP_PROTO(struct kvm_vcpu *vcpu, __u8 failed),
	TP_ARGS(vcpu, failed),

	TP_FIELDS(
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,1,0))
		ctf_integer(__u64, rip, vcpu->arch.emulate_ctxt.decode.fetch.start)
		ctf_integer(__u32, csbase, kvm_x86_ops->get_segment_base(vcpu, VCPU_SREG_CS))
		ctf_integer(__u8, len, vcpu->arch.emulate_ctxt.decode.eip
				- vcpu->arch.emulate_ctxt.decode.fetch.start)
		ctf_array(__u8, insn, vcpu->arch.emulate_ctxt.decode.fetch.data, 15)
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3,17,0))
		ctf_integer(__u64, rip, vcpu->arch.emulate_ctxt.fetch.start)
		ctf_integer(__u32, csbase, kvm_x86_ops->get_segment_base(vcpu, VCPU_SREG_CS))
		ctf_integer(__u8, len, vcpu->arch.emulate_ctxt._eip
				- vcpu->arch.emulate_ctxt.fetch.start)
		ctf_array(__u8, insn, vcpu->arch.emulate_ctxt.fetch.data, 15)
#else
		ctf_integer(__u64, rip, vcpu->arch.emulate_ctxt._eip -
				(vcpu->arch.emulate_ctxt.fetch.ptr -
					vcpu->arch.emulate_ctxt.fetch.data))
		ctf_integer(__u32, csbase, kvm_x86_ops->get_segment_base(vcpu, VCPU_SREG_CS))
		ctf_integer(__u8, len, vcpu->arch.emulate_ctxt.fetch.ptr -
				vcpu->arch.emulate_ctxt.fetch.data)
		ctf_array(__u8, insn, vcpu->arch.emulate_ctxt.fetch.data, 15)
#endif
		ctf_integer(__u8, flags, kei_decode_mode(vcpu->arch.emulate_ctxt.mode))
		ctf_integer(__u8, failed, failed)
	)
)

#define trace_kvm_emulate_insn_start(vcpu) trace_kvm_emulate_insn(vcpu, 0)
#define trace_kvm_emulate_insn_failed(vcpu) trace_kvm_emulate_insn(vcpu, 1)

LTTNG_TRACEPOINT_EVENT_MAP(
	vcpu_match_mmio, kvm_x86_vcpu_match_mmio,
	TP_PROTO(gva_t gva, gpa_t gpa, bool write, bool gpa_match),
	TP_ARGS(gva, gpa, write, gpa_match),

	TP_FIELDS(
		ctf_integer(gva_t, gva, gva)
		ctf_integer(gpa_t, gpa, gpa)
		ctf_integer(bool, write, write)
		ctf_integer(bool, gpa_match, gpa_match)
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0))
LTTNG_TRACEPOINT_EVENT_MAP(kvm_write_tsc_offset, kvm_x86_write_tsc_offset,
	TP_PROTO(unsigned int vcpu_id, __u64 previous_tsc_offset,
		 __u64 next_tsc_offset),
	TP_ARGS(vcpu_id, previous_tsc_offset, next_tsc_offset),

	TP_FIELDS(
		ctf_integer(unsigned int, vcpu_id, vcpu_id)
		ctf_integer(__u64, previous_tsc_offset, previous_tsc_offset)
		ctf_integer(__u64, next_tsc_offset, next_tsc_offset)
	)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
#ifdef CONFIG_X86_64

LTTNG_TRACEPOINT_EVENT_MAP(kvm_update_master_clock, kvm_x86_update_master_clock,
	TP_PROTO(bool use_master_clock, unsigned int host_clock, bool offset_matched),
	TP_ARGS(use_master_clock, host_clock, offset_matched),

	TP_FIELDS(
		ctf_integer(bool, use_master_clock, use_master_clock)
		ctf_integer(unsigned int, host_clock, host_clock)
		ctf_integer(bool, offset_matched, offset_matched)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(kvm_track_tsc, kvm_x86_track_tsc,
	TP_PROTO(unsigned int vcpu_id, unsigned int nr_matched,
		 unsigned int online_vcpus, bool use_master_clock,
		 unsigned int host_clock),
	TP_ARGS(vcpu_id, nr_matched, online_vcpus, use_master_clock,
		host_clock),

	TP_FIELDS(
		ctf_integer(unsigned int, vcpu_id, vcpu_id)
		ctf_integer(unsigned int, nr_vcpus_matched_tsc, nr_matched)
		ctf_integer(unsigned int, online_vcpus, online_vcpus)
		ctf_integer(bool, use_master_clock, use_master_clock)
		ctf_integer(unsigned int, host_clock, host_clock)
	)
)

#endif /* CONFIG_X86_64 */
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0) */

#endif /* LTTNG_TRACE_KVM_H */

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH ../instrumentation/events/lttng-module/arch/x86/kvm
#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_FILE trace

/* This part must be outside protection */
#include <probes/define_trace.h>
