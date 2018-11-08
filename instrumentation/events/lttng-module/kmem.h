/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM kmem

#if !defined(LTTNG_TRACE_KMEM_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_KMEM_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/types.h>
#include <linux/version.h>

LTTNG_TRACEPOINT_EVENT_CLASS(kmem_alloc,

	TP_PROTO(unsigned long call_site,
		 const void *ptr,
		 size_t bytes_req,
		 size_t bytes_alloc,
		 gfp_t gfp_flags),

	TP_ARGS(call_site, ptr, bytes_req, bytes_alloc, gfp_flags),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, call_site, call_site)
		ctf_integer_hex(const void *, ptr, ptr)
		ctf_integer(size_t, bytes_req, bytes_req)
		ctf_integer(size_t, bytes_alloc, bytes_alloc)
		ctf_integer(gfp_t, gfp_flags, gfp_flags)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(kmem_alloc, kmalloc,

	kmem_kmalloc,

	TP_PROTO(unsigned long call_site, const void *ptr,
		 size_t bytes_req, size_t bytes_alloc, gfp_t gfp_flags),

	TP_ARGS(call_site, ptr, bytes_req, bytes_alloc, gfp_flags)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(kmem_alloc, kmem_cache_alloc,

	TP_PROTO(unsigned long call_site, const void *ptr,
		 size_t bytes_req, size_t bytes_alloc, gfp_t gfp_flags),

	TP_ARGS(call_site, ptr, bytes_req, bytes_alloc, gfp_flags)
)

LTTNG_TRACEPOINT_EVENT_CLASS(kmem_alloc_node,

	TP_PROTO(unsigned long call_site,
		 const void *ptr,
		 size_t bytes_req,
		 size_t bytes_alloc,
		 gfp_t gfp_flags,
		 int node),

	TP_ARGS(call_site, ptr, bytes_req, bytes_alloc, gfp_flags, node),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, call_site, call_site)
		ctf_integer_hex(const void *, ptr, ptr)
		ctf_integer(size_t, bytes_req, bytes_req)
		ctf_integer(size_t, bytes_alloc, bytes_alloc)
		ctf_integer(gfp_t, gfp_flags, gfp_flags)
		ctf_integer(int, node, node)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(kmem_alloc_node, kmalloc_node,

	kmem_kmalloc_node,

	TP_PROTO(unsigned long call_site, const void *ptr,
		 size_t bytes_req, size_t bytes_alloc,
		 gfp_t gfp_flags, int node),

	TP_ARGS(call_site, ptr, bytes_req, bytes_alloc, gfp_flags, node)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(kmem_alloc_node, kmem_cache_alloc_node,

	TP_PROTO(unsigned long call_site, const void *ptr,
		 size_t bytes_req, size_t bytes_alloc,
		 gfp_t gfp_flags, int node),

	TP_ARGS(call_site, ptr, bytes_req, bytes_alloc, gfp_flags, node)
)

LTTNG_TRACEPOINT_EVENT_CLASS(kmem_free,

	TP_PROTO(unsigned long call_site, const void *ptr),

	TP_ARGS(call_site, ptr),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, call_site, call_site)
		ctf_integer_hex(const void *, ptr, ptr)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(kmem_free, kfree,

	kmem_kfree,

	TP_PROTO(unsigned long call_site, const void *ptr),

	TP_ARGS(call_site, ptr)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(kmem_free, kmem_cache_free,

	TP_PROTO(unsigned long call_site, const void *ptr),

	TP_ARGS(call_site, ptr)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
LTTNG_TRACEPOINT_EVENT_MAP(mm_page_free, kmem_mm_page_free,
#else
LTTNG_TRACEPOINT_EVENT_MAP(mm_page_free_direct, kmem_mm_page_free_direct,
#endif

	TP_PROTO(struct page *page, unsigned int order),

	TP_ARGS(page, order),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn, page_to_pfn(page))
		ctf_integer(unsigned int, order, order)
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0))
LTTNG_TRACEPOINT_EVENT_MAP(mm_page_free_batched, kmem_mm_page_free_batched,

	TP_PROTO(struct page *page),

	TP_ARGS(page),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn, page_to_pfn(page))
	)
)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
LTTNG_TRACEPOINT_EVENT_MAP(mm_page_free_batched, kmem_mm_page_free_batched,

	TP_PROTO(struct page *page, int cold),

	TP_ARGS(page, cold),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn, page_to_pfn(page))
		ctf_integer(int, cold, cold)
	)
)
#else
LTTNG_TRACEPOINT_EVENT_MAP(mm_pagevec_free, kmem_pagevec_free,

	TP_PROTO(struct page *page, int cold),

	TP_ARGS(page, cold),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn, page_to_pfn(page))
		ctf_integer(int, cold, cold)
	)
)
#endif

LTTNG_TRACEPOINT_EVENT_MAP(mm_page_alloc, kmem_mm_page_alloc,

	TP_PROTO(struct page *page, unsigned int order,
			gfp_t gfp_flags, int migratetype),

	TP_ARGS(page, order, gfp_flags, migratetype),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn,
			page ? page_to_pfn(page) : -1UL)
		ctf_integer(unsigned int, order, order)
		ctf_integer(gfp_t, gfp_flags, gfp_flags)
		ctf_integer(int, migratetype, migratetype)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(kmem_mm_page,

	TP_PROTO(struct page *page, unsigned int order, int migratetype),

	TP_ARGS(page, order, migratetype),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn,
			page ? page_to_pfn(page) : -1UL)
		ctf_integer(unsigned int, order, order)
		ctf_integer(int, migratetype, migratetype)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(kmem_mm_page, mm_page_alloc_zone_locked,

	kmem_mm_page_alloc_zone_locked,

	TP_PROTO(struct page *page, unsigned int order, int migratetype),

	TP_ARGS(page, order, migratetype)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(kmem_mm_page, mm_page_pcpu_drain,

	kmem_mm_page_pcpu_drain,

	TP_PROTO(struct page *page, unsigned int order, int migratetype),

	TP_ARGS(page, order, migratetype)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,2)	\
	|| LTTNG_KERNEL_RANGE(3,14,36, 3,15,0)		\
	|| LTTNG_KERNEL_RANGE(3,16,35, 3,17,0)		\
	|| LTTNG_KERNEL_RANGE(3,18,10, 3,19,0)		\
	|| LTTNG_DEBIAN_KERNEL_RANGE(3,16,7,9,0,0, 3,17,0,0,0,0)	\
	|| LTTNG_UBUNTU_KERNEL_RANGE(3,13,11,50, 3,14,0,0)	\
	|| LTTNG_UBUNTU_KERNEL_RANGE(3,16,7,34, 3,17,0,0))

LTTNG_TRACEPOINT_EVENT_MAP(mm_page_alloc_extfrag,

	kmem_mm_page_alloc_extfrag,

	TP_PROTO(struct page *page,
		int alloc_order, int fallback_order,
		int alloc_migratetype, int fallback_migratetype),

	TP_ARGS(page,
		alloc_order, fallback_order,
		alloc_migratetype, fallback_migratetype),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn, page_to_pfn(page))
		ctf_integer(int, alloc_order, alloc_order)
		ctf_integer(int, fallback_order, fallback_order)
		ctf_integer(int, alloc_migratetype, alloc_migratetype)
		ctf_integer(int, fallback_migratetype, fallback_migratetype)
		ctf_integer(int, change_ownership,
			(alloc_migratetype == get_pageblock_migratetype(page)))
	)
)

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,30))

LTTNG_TRACEPOINT_EVENT_MAP(mm_page_alloc_extfrag,

	kmem_mm_page_alloc_extfrag,

	TP_PROTO(struct page *page,
		int alloc_order, int fallback_order,
		int alloc_migratetype, int fallback_migratetype, int new_migratetype),

	TP_ARGS(page,
		alloc_order, fallback_order,
		alloc_migratetype, fallback_migratetype, new_migratetype),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn, page_to_pfn(page))
		ctf_integer(int, alloc_order, alloc_order)
		ctf_integer(int, fallback_order, fallback_order)
		ctf_integer(int, alloc_migratetype, alloc_migratetype)
		ctf_integer(int, fallback_migratetype, fallback_migratetype)
		ctf_integer(int, change_ownership, (new_migratetype == alloc_migratetype))
	)
)

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))

LTTNG_TRACEPOINT_EVENT_MAP(mm_page_alloc_extfrag,

	kmem_mm_page_alloc_extfrag,

	TP_PROTO(struct page *page,
			int alloc_order, int fallback_order,
			int alloc_migratetype, int fallback_migratetype,
			int change_ownership),

	TP_ARGS(page,
		alloc_order, fallback_order,
		alloc_migratetype, fallback_migratetype,
		change_ownership),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn, page_to_pfn(page))
		ctf_integer(int, alloc_order, alloc_order)
		ctf_integer(int, fallback_order, fallback_order)
		ctf_integer(int, alloc_migratetype, alloc_migratetype)
		ctf_integer(int, fallback_migratetype, fallback_migratetype)
		ctf_integer(int, change_ownership, change_ownership)
	)
)

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

LTTNG_TRACEPOINT_EVENT_MAP(mm_page_alloc_extfrag,

	kmem_mm_page_alloc_extfrag,

	TP_PROTO(struct page *page,
			int alloc_order, int fallback_order,
			int alloc_migratetype, int fallback_migratetype),

	TP_ARGS(page,
		alloc_order, fallback_order,
		alloc_migratetype, fallback_migratetype),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn, page_to_pfn(page))
		ctf_integer(int, alloc_order, alloc_order)
		ctf_integer(int, fallback_order, fallback_order)
		ctf_integer(int, alloc_migratetype, alloc_migratetype)
		ctf_integer(int, fallback_migratetype, fallback_migratetype)
	)
)

#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

#endif /* LTTNG_TRACE_KMEM_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
