// SPDX-FileCopyrightText: 2012 Paul Woegerer <paul_woegerer@mentor.com>
//
// SPDX-License-Identifier: GPL-2.0-only

#undef TRACE_SYSTEM
#define TRACE_SYSTEM kmem

#if !defined(LTTNG_TRACE_KMEM_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_KMEM_H

#include <lttng/tracepoint-event.h>
#include <linux/types.h>
#include <lttng/kernel-version.h>

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,0,0))
#include <../../mm/slab.h>
#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,1,0))
LTTNG_TRACEPOINT_EVENT_MAP(kmalloc,

	kmem_kmalloc,

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
		ctf_integer(bool, accounted, (IS_ENABLED(CONFIG_MEMCG_KMEM) &&
			(gfp_flags & __GFP_ACCOUNT) ? true : false))
	)
)

LTTNG_TRACEPOINT_EVENT(kmem_cache_alloc,

	TP_PROTO(unsigned long call_site,
		 const void *ptr,
		 struct kmem_cache *s,
		 gfp_t gfp_flags,
		 int node),

	TP_ARGS(call_site, ptr, s, gfp_flags, node),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, call_site, call_site)
		ctf_integer_hex(const void *, ptr, ptr)
		ctf_integer(size_t, bytes_req, s->object_size)
		ctf_integer(size_t, bytes_alloc, s->size)
		ctf_integer(gfp_t, gfp_flags, gfp_flags)
		ctf_integer(int, node, node)
		ctf_integer(bool, accounted, IS_ENABLED(CONFIG_MEMCG_KMEM) ?
			((gfp_flags & __GFP_ACCOUNT) ||
			(s->flags & SLAB_ACCOUNT)) : false)
	)
)
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,0,0) || \
	LTTNG_RHEL_KERNEL_RANGE(5,14,0,163,0,0, 5,15,0,0,0,0))

LTTNG_TRACEPOINT_EVENT_CLASS(kmem_alloc,

	TP_PROTO(unsigned long call_site,
		 const void *ptr,
		 struct kmem_cache *s,
		 size_t bytes_req,
		 size_t bytes_alloc,
		 gfp_t gfp_flags),

	TP_ARGS(call_site, ptr, s, bytes_req, bytes_alloc, gfp_flags),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, call_site, call_site)
		ctf_integer_hex(const void *, ptr, ptr)
		ctf_integer(size_t, bytes_req, bytes_req)
		ctf_integer(size_t, bytes_alloc, bytes_alloc)
		ctf_integer(gfp_t, gfp_flags, gfp_flags)
		ctf_integer(bool, accounted, IS_ENABLED(CONFIG_MEMCG_KMEM) ?
			((gfp_flags & __GFP_ACCOUNT) ||
			(s && s->flags & SLAB_ACCOUNT)) : false)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(kmem_alloc, kmalloc,

	kmem_kmalloc,

	TP_PROTO(unsigned long call_site, const void *ptr, struct kmem_cache *s,
		 size_t bytes_req, size_t bytes_alloc, gfp_t gfp_flags),

	TP_ARGS(call_site, ptr, s, bytes_req, bytes_alloc, gfp_flags)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(kmem_alloc, kmem_cache_alloc,

	TP_PROTO(unsigned long call_site, const void *ptr, struct kmem_cache *s,
		 size_t bytes_req, size_t bytes_alloc, gfp_t gfp_flags),

	TP_ARGS(call_site, ptr, s, bytes_req, bytes_alloc, gfp_flags)
)
#else
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
#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,0,0) || \
	LTTNG_RHEL_KERNEL_RANGE(5,14,0,163,0,0, 5,15,0,0,0,0))

LTTNG_TRACEPOINT_EVENT_CLASS(kmem_alloc_node,

	TP_PROTO(unsigned long call_site,
		 const void *ptr,
		 struct kmem_cache *s,
		 size_t bytes_req,
		 size_t bytes_alloc,
		 gfp_t gfp_flags,
		 int node),

	TP_ARGS(call_site, ptr, s, bytes_req, bytes_alloc, gfp_flags, node),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, call_site, call_site)
		ctf_integer_hex(const void *, ptr, ptr)
		ctf_integer(size_t, bytes_req, bytes_req)
		ctf_integer(size_t, bytes_alloc, bytes_alloc)
		ctf_integer(gfp_t, gfp_flags, gfp_flags)
		ctf_integer(int, node, node)
		ctf_integer(bool, accounted, IS_ENABLED(CONFIG_MEMCG_KMEM) ?
			((gfp_flags & __GFP_ACCOUNT) ||
			(s && s->flags & SLAB_ACCOUNT)) : false)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(kmem_alloc_node, kmalloc_node,

	kmem_kmalloc_node,

	TP_PROTO(unsigned long call_site, const void *ptr,
		 struct kmem_cache *s, size_t bytes_req, size_t bytes_alloc,
		 gfp_t gfp_flags, int node),

	TP_ARGS(call_site, ptr, s, bytes_req, bytes_alloc, gfp_flags, node)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(kmem_alloc_node, kmem_cache_alloc_node,

	TP_PROTO(unsigned long call_site, const void *ptr,
		 struct kmem_cache *s, size_t bytes_req, size_t bytes_alloc,
		 gfp_t gfp_flags, int node),

	TP_ARGS(call_site, ptr, s, bytes_req, bytes_alloc, gfp_flags, node)
)
#else
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
#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,12,0) || \
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,348,0,0, 4,19,0,0,0,0))
LTTNG_TRACEPOINT_EVENT_MAP(kfree,

	kmem_kfree,

	TP_PROTO(unsigned long call_site, const void *ptr),

	TP_ARGS(call_site, ptr),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, call_site, call_site)
		ctf_integer_hex(const void *, ptr, ptr)
	)
)
#else
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
#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,1,0))
LTTNG_TRACEPOINT_EVENT(kmem_cache_free,

	TP_PROTO(unsigned long call_site, const void *ptr, const struct kmem_cache *s),

	TP_ARGS(call_site, ptr, s),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, call_site, call_site)
		ctf_integer_hex(const void *, ptr, ptr)
		ctf_string(name, s->name)
	)
)
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,12,0))
LTTNG_TRACEPOINT_EVENT(kmem_cache_free,

	TP_PROTO(unsigned long call_site, const void *ptr, const char *name),

	TP_ARGS(call_site, ptr, name),

	TP_FIELDS(
		ctf_integer_hex(unsigned long, call_site, call_site)
		ctf_integer_hex(const void *, ptr, ptr)
		ctf_string(name, name)
	)
)
#endif

LTTNG_TRACEPOINT_EVENT_MAP(mm_page_free, kmem_mm_page_free,

	TP_PROTO(struct page *page, unsigned int order),

	TP_ARGS(page, order),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn, page_to_pfn(page))
		ctf_integer(unsigned int, order, order)
	)
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,15,0))
LTTNG_TRACEPOINT_EVENT_MAP(mm_page_free_batched, kmem_mm_page_free_batched,

	TP_PROTO(struct page *page),

	TP_ARGS(page),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn, page_to_pfn(page))
	)
)
#else
LTTNG_TRACEPOINT_EVENT_MAP(mm_page_free_batched, kmem_mm_page_free_batched,

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

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,19,0) || \
	LTTNG_KERNEL_RANGE(5,15,171, 5,16,0) || \
	LTTNG_RHEL_KERNEL_RANGE(5,14,0,163,0,0, 5,15,0,0,0,0))

LTTNG_TRACEPOINT_EVENT_CLASS(kmem_mm_page,

	TP_PROTO(struct page *page, unsigned int order, int migratetype,
			int percpu_refill),

	TP_ARGS(page, order, migratetype, percpu_refill),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(unsigned long, pfn,
			page ? page_to_pfn(page) : -1UL)
		ctf_integer(unsigned int, order, order)
		ctf_integer(int, migratetype, migratetype)
		ctf_integer(int, percpu_refill, percpu_refill)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(kmem_mm_page, mm_page_alloc_zone_locked,

	kmem_mm_page_alloc_zone_locked,

	TP_PROTO(struct page *page, unsigned int order, int migratetype,
			int percpu_refill),

	TP_ARGS(page, order, migratetype, percpu_refill)
)

LTTNG_TRACEPOINT_EVENT_MAP(mm_page_pcpu_drain,

	kmem_mm_page_pcpu_drain,

	TP_PROTO(struct page *page, unsigned int order, int migratetype),

	TP_ARGS(page, order, migratetype),

	TP_FIELDS(
		ctf_integer(unsigned long, pfn,
			page ? page_to_pfn(page) : -1UL)
		ctf_integer(unsigned int, order, order)
		ctf_integer(int, migratetype, migratetype)
	)
)
#else
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
#endif

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
#include <lttng/define_trace.h>
