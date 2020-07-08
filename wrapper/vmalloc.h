/* SPDX-License-Identifier: (GPL-2.0 or LGPL-2.1)
 *
 * wrapper/vmalloc.h
 *
 * wrapper around vmalloc_sync_all. Using KALLSYMS to get its address when
 * available, else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_WRAPPER_VMALLOC_H
#define _LTTNG_WRAPPER_VMALLOC_H

#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>

#ifdef CONFIG_KALLSYMS

#include <linux/kallsyms.h>
#include <wrapper/kallsyms.h>
#include <lttng-kernel-version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0))

/*
 * wrapper_vmalloc_sync_mappings was removed in v5.8, the vmalloc mappings
 * are now synchronized when they are created or torn down.
 */
static inline
void wrapper_vmalloc_sync_mappings(void)
{}

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0) \
  || LTTNG_KERNEL_RANGE(5,5,12, 5,6,0)            \
  || LTTNG_KERNEL_RANGE(5,4,28, 5,5,0)            \
  || LTTNG_KERNEL_RANGE(5,2,37, 5,3,0)            \
  || LTTNG_KERNEL_RANGE(4,19,113, 4,20,0)         \
  || LTTNG_KERNEL_RANGE(4,14,175, 4,15,0)         \
  || LTTNG_KERNEL_RANGE(4,9,218, 4,10,0)          \
  || LTTNG_KERNEL_RANGE(4,4,218, 4,5,0))	  \
  || LTTNG_UBUNTU_KERNEL_RANGE(4,15,18,97, 4,16,0,0) \
  || LTTNG_UBUNTU_KERNEL_RANGE(5,0,21,48, 5,1,0,0)   \
  || LTTNG_UBUNTU_KERNEL_RANGE(5,3,18,52, 5,4,0,0)

static inline
void wrapper_vmalloc_sync_mappings(void)
{
	void (*vmalloc_sync_mappings_sym)(void);

	vmalloc_sync_mappings_sym = (void *) kallsyms_lookup_funcptr("vmalloc_sync_mappings");
	if (vmalloc_sync_mappings_sym) {
		vmalloc_sync_mappings_sym();
	} else {
#ifdef CONFIG_X86
		/*
		 * Only x86 needs vmalloc_sync_mappings to make sure LTTng does not
		 * trigger recursive page faults.
		 */
		printk_once(KERN_WARNING "LTTng: vmalloc_sync_mappings symbol lookup failed.\n");
		printk_once(KERN_WARNING "Page fault handler and NMI tracing might trigger faults.\n");
#endif
	}
}

/*
 * Canary function to check for 'vmalloc_sync_mappings()' at compile time.
 *
 * From 'include/linux/vmalloc.h':
 *
 *   void vmalloc_sync_mappings(void);
 */
static inline
void __canary__vmalloc_sync_mappings(void)
{
	vmalloc_sync_mappings();
}

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)) */

/*
 * Map vmalloc_sync_mappings to vmalloc_sync_all() on kernels before 5.6.
 */
static inline
void wrapper_vmalloc_sync_mappings(void)
{
	void (*vmalloc_sync_all_sym)(void);

	vmalloc_sync_all_sym = (void *) kallsyms_lookup_funcptr("vmalloc_sync_all");
	if (vmalloc_sync_all_sym) {
		vmalloc_sync_all_sym();
	} else {
#ifdef CONFIG_X86
		/*
		 * Only x86 needs vmalloc_sync_all to make sure LTTng does not
		 * trigger recursive page faults.
		 */
		printk_once(KERN_WARNING "LTTng: vmalloc_sync_all symbol lookup failed.\n");
		printk_once(KERN_WARNING "Page fault handler and NMI tracing might trigger faults.\n");
#endif
	}
}

/*
 * Canary function to check for 'vmalloc_sync_all()' at compile time.
 *
 * From 'include/linux/vmalloc.h':
 *
 *   void vmalloc_sync_all(void);
 */
static inline
void __canary__vmalloc_sync_all(void)
{
	vmalloc_sync_all();
}

#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)) */

#else /* CONFIG_KALLSYMS */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0))

/*
 * wrapper_vmalloc_sync_mappings was removed in v5.8, the vmalloc mappings
 * are now synchronized when they are created or torn down.
 */
static inline
void wrapper_vmalloc_sync_mappings(void)
{}

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0) \
  || LTTNG_KERNEL_RANGE(5,5,12, 5,6,0)            \
  || LTTNG_KERNEL_RANGE(5,4,28, 5,5,0)            \
  || LTTNG_KERNEL_RANGE(5,2,37, 5,3,0)            \
  || LTTNG_KERNEL_RANGE(4,19,113, 4,20,0)         \
  || LTTNG_KERNEL_RANGE(4,14,175, 4,15,0)         \
  || LTTNG_KERNEL_RANGE(4,9,218, 4,10,0)          \
  || LTTNG_KERNEL_RANGE(4,4,218, 4,5,0))	  \
  || LTTNG_UBUNTU_KERNEL_RANGE(4,15,18,97, 4,18,0,0) \
  || LTTNG_UBUNTU_KERNEL_RANGE(5,0,21,48, 5,1,0,0)   \
  || LTTNG_UBUNTU_KERNEL_RANGE(5,3,18,52, 5,4,0,0)

static inline
void wrapper_vmalloc_sync_mappings(void)
{
	return vmalloc_sync_mappings();
}

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)) */

static inline
void wrapper_vmalloc_sync_mappings(void)
{
	return vmalloc_sync_all();
}

#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)) */

#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,12,0))
static inline
void *lttng_kvmalloc_node(unsigned long size, gfp_t flags, int node)
{
	void *ret;

	ret = kvmalloc_node(size, flags, node);
	if (is_vmalloc_addr(ret)) {
		/*
		 * Make sure we don't trigger recursive page faults in the
		 * tracing fast path.
		 */
		wrapper_vmalloc_sync_mappings();
	}
	return ret;
}

static inline
void *lttng_kvzalloc_node(unsigned long size, gfp_t flags, int node)
{
	return lttng_kvmalloc_node(size, flags | __GFP_ZERO, node);
}

static inline
void *lttng_kvmalloc(unsigned long size, gfp_t flags)
{
	return lttng_kvmalloc_node(size, flags, NUMA_NO_NODE);
}

static inline
void *lttng_kvzalloc(unsigned long size, gfp_t flags)
{
	return lttng_kvzalloc_node(size, flags, NUMA_NO_NODE);
}

static inline
void lttng_kvfree(const void *addr)
{
	kvfree(addr);
}

#else

#include <linux/slab.h>

static inline
void print_vmalloc_node_range_warning(void)
{
	printk_once(KERN_WARNING "LTTng: __vmalloc_node_range symbol lookup failed.\n");
	printk_once(KERN_WARNING "Tracer performance will be degraded on NUMA systems.\n");
	printk_once(KERN_WARNING "Please rebuild your kernel with CONFIG_KALLSYMS enabled.\n");
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))

/*
 * kallsyms wrapper of __vmalloc_node with a fallback to kmalloc_node.
 */
static inline
void *__lttng_vmalloc_node_range(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, unsigned long vm_flags, int node,
			const void *caller)
{
#ifdef CONFIG_KALLSYMS
	/*
	 * If we have KALLSYMS, get * __vmalloc_node_range which is not exported.
	 */
	void *(*lttng__vmalloc_node_range)(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, unsigned long vm_flags, int node,
			const void *caller);

	lttng__vmalloc_node_range = (void *) kallsyms_lookup_funcptr("__vmalloc_node_range");
	if (lttng__vmalloc_node_range)
		return lttng__vmalloc_node_range(size, align, start, end, gfp_mask, prot,
				vm_flags, node, caller);
#endif
	if (node != NUMA_NO_NODE)
		print_vmalloc_node_range_warning();
	return __vmalloc(size, gfp_mask, prot);
}

/*
 * Canary function to check for '__vmalloc_node_range()' at compile time.
 *
 * From 'include/linux/vmalloc.h':
 *
 *   extern void *__vmalloc_node_range(unsigned long size, unsigned long align,
 *                           unsigned long start, unsigned long end, gfp_t gfp_mask,
 *                           pgprot_t prot, unsigned long vm_flags, int node,
 *                           const void *caller);
 */
static inline
void *__canary____lttng_vmalloc_node_range(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, unsigned long vm_flags, int node,
			const void *caller)
{
	return __vmalloc_node_range(size, align, start, end, gfp_mask, prot,
			vm_flags, node, caller);
}

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))

/*
 * kallsyms wrapper of __vmalloc_node with a fallback to kmalloc_node.
 */
static inline
void *__lttng_vmalloc_node_range(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, unsigned long vm_flags, int node,
			const void *caller)
{
#ifdef CONFIG_KALLSYMS
	/*
	 * If we have KALLSYMS, get * __vmalloc_node_range which is not exported.
	 */
	void *(*lttng__vmalloc_node_range)(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, int node, const void *caller);

	lttng__vmalloc_node_range = (void *) kallsyms_lookup_funcptr("__vmalloc_node_range");
	if (lttng__vmalloc_node_range)
		return lttng__vmalloc_node_range(size, align, start, end, gfp_mask, prot,
				node, caller);
#endif
	if (node != NUMA_NO_NODE)
		print_vmalloc_node_range_warning();
	return __vmalloc(size, gfp_mask, prot);
}

/*
 * Canary function to check for '__vmalloc_node_range()' at compile time.
 *
 * From 'include/linux/vmalloc.h':
 *
 *   extern void *__vmalloc_node_range(unsigned long size, unsigned long align,
 *                           unsigned long start, unsigned long end, gfp_t gfp_mask,
 *                           pgprot_t prot, unsigned long vm_flags, int node,
 *                           const void *caller);
 */
static inline
void *__canary____lttng_vmalloc_node_range(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, int node, const void *caller)
{
	return __vmalloc_node_range(size, align, start, end, gfp_mask, prot,
			node, caller);
}

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))

/*
 * kallsyms wrapper of __vmalloc_node with a fallback to kmalloc_node.
 */
static inline
void *__lttng_vmalloc_node_range(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, unsigned long vm_flags, int node,
			void *caller)
{
#ifdef CONFIG_KALLSYMS
	/*
	 * If we have KALLSYMS, get * __vmalloc_node_range which is not exported.
	 */
	void *(*lttng__vmalloc_node_range)(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, int node, void *caller);

	lttng__vmalloc_node_range = (void *) kallsyms_lookup_funcptr("__vmalloc_node_range");
	if (lttng__vmalloc_node_range)
		return lttng__vmalloc_node_range(size, align, start, end, gfp_mask, prot,
				node, caller);
#endif
	if (node != NUMA_NO_NODE)
		print_vmalloc_node_range_warning();
	return __vmalloc(size, gfp_mask, prot);
}

/*
 * Canary function to check for '__vmalloc_node_range()' at compile time.
 *
 * From 'include/linux/vmalloc.h':
 *
 *   extern void *__vmalloc_node_range(unsigned long size, unsigned long align,
 *                           unsigned long start, unsigned long end, gfp_t gfp_mask,
 *                           pgprot_t prot, unsigned long vm_flags, int node,
 *                           void *caller);
 */
static inline
void *__canary____lttng_vmalloc_node_range(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, int node, void *caller)
{
	return __vmalloc_node_range(size, align, start, end, gfp_mask, prot,
			node, caller);
}

#else /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38)) */

/*
 * Basic fallback for kernel prior to 2.6.38 without __vmalloc_node_range()
 */
static inline
void *__lttng_vmalloc_node_range(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, unsigned long vm_flags, int node,
			void *caller)
{
	return __vmalloc(size, gfp_mask, prot);
}

#endif

/**
 * lttng_kvmalloc_node - attempt to allocate physically contiguous memory, but upon
 * failure, fall back to non-contiguous (vmalloc) allocation.
 * @size: size of the request.
 * @flags: gfp mask for the allocation - must be compatible with GFP_KERNEL.
 *
 * Uses kmalloc to get the memory but if the allocation fails then falls back
 * to the vmalloc allocator. Use lttng_kvfree to free the memory.
 *
 * Reclaim modifiers - __GFP_NORETRY, __GFP_REPEAT and __GFP_NOFAIL are not supported
 */
static inline
void *lttng_kvmalloc_node(unsigned long size, gfp_t flags, int node)
{
	void *ret;

	/*
	 * vmalloc uses GFP_KERNEL for some internal allocations (e.g page tables)
	 * so the given set of flags has to be compatible.
	 */
	WARN_ON_ONCE((flags & GFP_KERNEL) != GFP_KERNEL);

	/*
	 * If the allocation fits in a single page, do not fallback.
	 */
	if (size <= PAGE_SIZE) {
		return kmalloc_node(size, flags, node);
	}

	/*
	 * Make sure that larger requests are not too disruptive - no OOM
	 * killer and no allocation failure warnings as we have a fallback
	 */
	ret = kmalloc_node(size, flags | __GFP_NOWARN | __GFP_NORETRY, node);
	if (!ret) {
		ret = __lttng_vmalloc_node_range(size, 1,
				VMALLOC_START, VMALLOC_END,
				flags | __GFP_HIGHMEM, PAGE_KERNEL, 0,
				node, __builtin_return_address(0));
		/*
		 * Make sure we don't trigger recursive page faults in the
		 * tracing fast path.
		 */
		wrapper_vmalloc_sync_mappings();
	}
	return ret;
}

static inline
void *lttng_kvzalloc_node(unsigned long size, gfp_t flags, int node)
{
	return lttng_kvmalloc_node(size, flags | __GFP_ZERO, node);
}

static inline
void *lttng_kvmalloc(unsigned long size, gfp_t flags)
{
	return lttng_kvmalloc_node(size, flags, NUMA_NO_NODE);
}

static inline
void *lttng_kvzalloc(unsigned long size, gfp_t flags)
{
	return lttng_kvzalloc_node(size, flags, NUMA_NO_NODE);
}

static inline
void lttng_kvfree(const void *addr)
{
	if (is_vmalloc_addr(addr)) {
		vfree(addr);
	} else {
		kfree(addr);
	}
}
#endif

#endif /* _LTTNG_WRAPPER_VMALLOC_H */
