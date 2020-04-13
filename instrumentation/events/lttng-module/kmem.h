/* SPDX-License-Identifier: GPL-2.0-only */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM kmem

#if !defined(LTTNG_TRACE_KMEM_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_KMEM_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/types.h>

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

LTTNG_TRACEPOINT_EVENT_MAP(mm_page_free, kmem_mm_page_free,

	TP_PROTO(struct page *page, unsigned int order),

	TP_ARGS(page, order),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn, page_to_pfn(page))
		ctf_integer(unsigned int, order, order)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(mm_page_free_batched, kmem_mm_page_free_batched,

	TP_PROTO(struct page *page),

	TP_ARGS(page),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn, page_to_pfn(page))
	)
)

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

#endif /* LTTNG_TRACE_KMEM_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
