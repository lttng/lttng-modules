// SPDX-FileCopyrightText: 2013 Mohamad Gebai <mohamad.gebai@polymtl.ca>
//
// SPDX-License-Identifier: GPL-2.0-only

#if !defined(LTTNG_TRACE_KVM_MMU_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_KVM_MMU_H

#include <lttng/tracepoint-event.h>
#include <lttng/kernel-version.h>

#include <linux/trace_events.h>

#undef TRACE_SYSTEM
#define TRACE_SYSTEM kvm_mmu

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,1,0) || \
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,147,0,0, 4,19,0,0,0,0))

#define LTTNG_KVM_MMU_PAGE_FIELDS \
	ctf_integer(__u64, gfn, (sp)->gfn) \
	ctf_integer(__u32, role, (sp)->role.word) \
	ctf_integer(__u32, root_count, (sp)->root_count) \
	ctf_integer(bool, unsync, (sp)->unsync)

#else

#define LTTNG_KVM_MMU_PAGE_FIELDS \
	ctf_integer(unsigned long, mmu_valid_gen, (sp)->mmu_valid_gen) \
	ctf_integer(__u64, gfn, (sp)->gfn) \
	ctf_integer(__u32, role, (sp)->role.word) \
	ctf_integer(__u32, root_count, (sp)->root_count) \
	ctf_integer(bool, unsync, (sp)->unsync)

#endif

/*
 * A pagetable walk has started
 */
LTTNG_TRACEPOINT_EVENT(
	kvm_mmu_pagetable_walk,
	TP_PROTO(u64 addr, u32 pferr),
	TP_ARGS(addr, pferr),

	TP_FIELDS(
		ctf_integer_hex(__u64, addr, addr)
		ctf_integer(__u32, pferr, pferr)
	)
)

/* We just walked a paging element */
LTTNG_TRACEPOINT_EVENT(
	kvm_mmu_paging_element,
	TP_PROTO(u64 pte, int level),
	TP_ARGS(pte, level),

	TP_FIELDS(
		ctf_integer(__u64, pte, pte)
		ctf_integer(__u32, level, level)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(kvm_mmu_set_bit_class,

	TP_PROTO(unsigned long table_gfn, unsigned index, unsigned size),

	TP_ARGS(table_gfn, index, size),

	TP_FIELDS(
		ctf_integer(__u64, gpa,
			((u64)table_gfn << PAGE_SHIFT) + index * size)
	)
)

/* We set a pte accessed bit */
LTTNG_TRACEPOINT_EVENT_INSTANCE(kvm_mmu_set_bit_class, kvm_mmu_set_accessed_bit,

	TP_PROTO(unsigned long table_gfn, unsigned index, unsigned size),

	TP_ARGS(table_gfn, index, size)
)

/* We set a pte dirty bit */
LTTNG_TRACEPOINT_EVENT_INSTANCE(kvm_mmu_set_bit_class, kvm_mmu_set_dirty_bit,

	TP_PROTO(unsigned long table_gfn, unsigned index, unsigned size),

	TP_ARGS(table_gfn, index, size)
)

LTTNG_TRACEPOINT_EVENT(
	kvm_mmu_walker_error,
	TP_PROTO(u32 pferr),
	TP_ARGS(pferr),

	TP_FIELDS(
		ctf_integer(__u32, pferr, pferr)
	)
)

LTTNG_TRACEPOINT_EVENT(
	kvm_mmu_get_page,
	TP_PROTO(struct kvm_mmu_page *sp, bool created),
	TP_ARGS(sp, created),

	TP_FIELDS(
		LTTNG_KVM_MMU_PAGE_FIELDS
		ctf_integer(bool, created, created)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(kvm_mmu_page_class,

	TP_PROTO(struct kvm_mmu_page *sp),
	TP_ARGS(sp),

	TP_FIELDS(
		LTTNG_KVM_MMU_PAGE_FIELDS
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(kvm_mmu_page_class, kvm_mmu_sync_page,
	TP_PROTO(struct kvm_mmu_page *sp),

	TP_ARGS(sp)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(kvm_mmu_page_class, kvm_mmu_unsync_page,
	TP_PROTO(struct kvm_mmu_page *sp),

	TP_ARGS(sp)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(kvm_mmu_page_class, kvm_mmu_prepare_zap_page,
	TP_PROTO(struct kvm_mmu_page *sp),

	TP_ARGS(sp)
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,10,0) || \
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,305,0,0, 4,19,0,0,0,0))

LTTNG_TRACEPOINT_EVENT_MAP(
	mark_mmio_spte,

	kvm_mmu_mark_mmio_spte,

	TP_PROTO(u64 *sptep, gfn_t gfn, u64 spte),
	TP_ARGS(sptep, gfn, spte),

	TP_FIELDS(
		ctf_integer_hex(void *, sptep, sptep)
		ctf_integer(gfn_t, gfn, gfn)
		ctf_integer(unsigned, access, spte & ACC_ALL)
		ctf_integer(unsigned int, gen, get_mmio_spte_generation(spte))
	)
)

#else

LTTNG_TRACEPOINT_EVENT_MAP(
	mark_mmio_spte,

	kvm_mmu_mark_mmio_spte,

	TP_PROTO(u64 *sptep, gfn_t gfn, unsigned access, unsigned int gen),
	TP_ARGS(sptep, gfn, access, gen),

	TP_FIELDS(
		ctf_integer_hex(void *, sptep, sptep)
		ctf_integer(gfn_t, gfn, gfn)
		ctf_integer(unsigned, access, access)
		ctf_integer(unsigned int, gen, gen)
	)
)
#endif

LTTNG_TRACEPOINT_EVENT_MAP(
	handle_mmio_page_fault,

	kvm_mmu_handle_mmio_page_fault,

	TP_PROTO(u64 addr, gfn_t gfn, unsigned access),
	TP_ARGS(addr, gfn, access),

	TP_FIELDS(
		ctf_integer_hex(u64, addr, addr)
		ctf_integer(gfn_t, gfn, gfn)
		ctf_integer(unsigned, access, access)
	)
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,16,0))
LTTNG_TRACEPOINT_EVENT_MAP(
	fast_page_fault,

	kvm_mmu_fast_page_fault,

	TP_PROTO(struct kvm_vcpu *vcpu, struct kvm_page_fault *fault,
		 u64 *sptep, u64 old_spte, int ret),
	TP_ARGS(vcpu, fault, sptep, old_spte, ret),

	TP_FIELDS(
		ctf_integer(int, vcpu_id, vcpu->vcpu_id)
		ctf_integer(gpa_t, cr2_or_gpa, fault->addr)
		ctf_integer(u32, error_code, fault->error_code)
		ctf_integer_hex(u64 *, sptep, sptep)
		ctf_integer(u64, old_spte, old_spte)
		ctf_integer(u64, new_spte, *sptep)
		ctf_integer(int, ret, ret)
	)
)
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,10,0) || \
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,305,0,0, 4,19,0,0,0,0))
LTTNG_TRACEPOINT_EVENT_MAP(
	fast_page_fault,

	kvm_mmu_fast_page_fault,

	TP_PROTO(struct kvm_vcpu *vcpu, gpa_t cr2_or_gpa, u32 error_code,
		 u64 *sptep, u64 old_spte, int ret),
	TP_ARGS(vcpu, cr2_or_gpa, error_code, sptep, old_spte, ret),

	TP_FIELDS(
		ctf_integer(int, vcpu_id, vcpu->vcpu_id)
		ctf_integer(gpa_t, cr2_or_gpa, cr2_or_gpa)
		ctf_integer(u32, error_code, error_code)
		ctf_integer_hex(u64 *, sptep, sptep)
		ctf_integer(u64, old_spte, old_spte)
		ctf_integer(u64, new_spte, *sptep)
		ctf_integer(int, ret, ret)
	)
)
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,6,0) || \
	LTTNG_KERNEL_RANGE(4,19,103, 4,20,0) || \
	LTTNG_KERNEL_RANGE(5,4,19, 5,5,0) || \
	LTTNG_KERNEL_RANGE(5,5,3, 5,6,0) || \
	LTTNG_UBUNTU_KERNEL_RANGE(4,15,18,92, 4,16,0,0) || \
	LTTNG_UBUNTU_KERNEL_RANGE(5,0,21,44, 5,1,0,0) || \
	LTTNG_UBUNTU_KERNEL_RANGE(5,3,18,43, 5,3,18,45) || \
	LTTNG_UBUNTU_KERNEL_RANGE(5,3,18,46, 5,4,0,0))
LTTNG_TRACEPOINT_EVENT_MAP(
	fast_page_fault,

	kvm_mmu_fast_page_fault,

	TP_PROTO(struct kvm_vcpu *vcpu, gpa_t cr2_or_gpa, u32 error_code,
		 u64 *sptep, u64 old_spte, bool retry),
	TP_ARGS(vcpu, cr2_or_gpa, error_code, sptep, old_spte, retry),

	TP_FIELDS(
		ctf_integer(int, vcpu_id, vcpu->vcpu_id)
		ctf_integer(gpa_t, cr2_or_gpa, cr2_or_gpa)
		ctf_integer(u32, error_code, error_code)
		ctf_integer_hex(u64 *, sptep, sptep)
		ctf_integer(u64, old_spte, old_spte)
		ctf_integer(u64, new_spte, *sptep)
		ctf_integer(bool, retry, retry)
	)
)
#else
LTTNG_TRACEPOINT_EVENT_MAP(
	fast_page_fault,

	kvm_mmu_fast_page_fault,

	TP_PROTO(struct kvm_vcpu *vcpu, gva_t gva, u32 error_code,
		 u64 *sptep, u64 old_spte, bool retry),
	TP_ARGS(vcpu, gva, error_code, sptep, old_spte, retry),

	TP_FIELDS(
		ctf_integer(int, vcpu_id, vcpu->vcpu_id)
		ctf_integer(gva_t, gva, gva)
		ctf_integer(u32, error_code, error_code)
		ctf_integer_hex(u64 *, sptep, sptep)
		ctf_integer(u64, old_spte, old_spte)
		ctf_integer(u64, new_spte, *sptep)
		ctf_integer(bool, retry, retry)
	)
)
#endif

#endif /* LTTNG_TRACE_KVM_MMU_H */

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH instrumentation/events/arch/x86/kvm
#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_FILE mmutrace

/* This part must be outside protection */
#include <lttng/define_trace.h>
