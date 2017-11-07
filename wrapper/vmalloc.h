#ifndef _LTTNG_WRAPPER_VMALLOC_H
#define _LTTNG_WRAPPER_VMALLOC_H

/*
 * wrapper/vmalloc.h
 *
 * wrapper around vmalloc_sync_all. Using KALLSYMS to get its address when
 * available, else we need to have a kernel that exports this function to GPL
 * modules.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>

#ifdef CONFIG_KALLSYMS

#include <linux/kallsyms.h>
#include <wrapper/kallsyms.h>

static inline
void wrapper_vmalloc_sync_all(void)
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
#else

static inline
void wrapper_vmalloc_sync_all(void)
{
	return vmalloc_sync_all();
}
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
		wrapper_vmalloc_sync_all();
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
		wrapper_vmalloc_sync_all();
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
