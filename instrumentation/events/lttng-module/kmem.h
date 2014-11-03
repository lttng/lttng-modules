#undef TRACE_SYSTEM
#define TRACE_SYSTEM kmem

#if !defined(LTTNG_TRACE_KMEM_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_KMEM_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/types.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
#include <trace/events/gfpflags.h>
#endif

LTTNG_TRACEPOINT_EVENT_CLASS(kmem_alloc,

	TP_PROTO(unsigned long call_site,
		 const void *ptr,
		 size_t bytes_req,
		 size_t bytes_alloc,
		 gfp_t gfp_flags),

	TP_ARGS(call_site, ptr, bytes_req, bytes_alloc, gfp_flags),

	TP_STRUCT__entry(
		__field_hex(	unsigned long,	call_site	)
		__field_hex(	const void *,	ptr		)
		__field(	size_t,		bytes_req	)
		__field(	size_t,		bytes_alloc	)
		__field(	gfp_t,		gfp_flags	)
	),

	TP_fast_assign(
		tp_assign(call_site, call_site)
		tp_assign(ptr, ptr)
		tp_assign(bytes_req, bytes_req)
		tp_assign(bytes_alloc, bytes_alloc)
		tp_assign(gfp_flags, gfp_flags)
	),

	TP_printk("call_site=%lx ptr=%p bytes_req=%zu bytes_alloc=%zu gfp_flags=%s",
		__entry->call_site,
		__entry->ptr,
		__entry->bytes_req,
		__entry->bytes_alloc,
		show_gfp_flags(__entry->gfp_flags))
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

	TP_STRUCT__entry(
		__field_hex(	unsigned long,	call_site	)
		__field_hex(	const void *,	ptr		)
		__field(	size_t,		bytes_req	)
		__field(	size_t,		bytes_alloc	)
		__field(	gfp_t,		gfp_flags	)
		__field(	int,		node		)
	),

	TP_fast_assign(
		tp_assign(call_site, call_site)
		tp_assign(ptr, ptr)
		tp_assign(bytes_req, bytes_req)
		tp_assign(bytes_alloc, bytes_alloc)
		tp_assign(gfp_flags, gfp_flags)
		tp_assign(node, node)
	),

	TP_printk("call_site=%lx ptr=%p bytes_req=%zu bytes_alloc=%zu gfp_flags=%s node=%d",
		__entry->call_site,
		__entry->ptr,
		__entry->bytes_req,
		__entry->bytes_alloc,
		show_gfp_flags(__entry->gfp_flags),
		__entry->node)
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

	TP_STRUCT__entry(
		__field_hex(	unsigned long,	call_site	)
		__field_hex(	const void *,	ptr		)
	),

	TP_fast_assign(
		tp_assign(call_site, call_site)
		tp_assign(ptr, ptr)
	),

	TP_printk("call_site=%lx ptr=%p", __entry->call_site, __entry->ptr)
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

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
LTTNG_TRACEPOINT_EVENT_MAP(mm_page_free, kmem_mm_page_free,
#else
LTTNG_TRACEPOINT_EVENT_MAP(mm_page_free_direct, kmem_mm_page_free_direct,
#endif

	TP_PROTO(struct page *page, unsigned int order),

	TP_ARGS(page, order),

	TP_STRUCT__entry(
		__field_hex(	struct page *,	page		)
		__field(	unsigned int,	order		)
	),

	TP_fast_assign(
		tp_assign(page, page)
		tp_assign(order, order)
	),

	TP_printk("page=%p pfn=%lu order=%d",
			__entry->page,
			page_to_pfn(__entry->page),
			__entry->order)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
LTTNG_TRACEPOINT_EVENT_MAP(mm_page_free_batched, kmem_mm_page_free_batched,
#else
LTTNG_TRACEPOINT_EVENT_MAP(mm_pagevec_free, kmem_pagevec_free,
#endif

	TP_PROTO(struct page *page, int cold),

	TP_ARGS(page, cold),

	TP_STRUCT__entry(
		__field_hex(	struct page *,	page		)
		__field(	int,		cold		)
	),

	TP_fast_assign(
		tp_assign(page, page)
		tp_assign(cold, cold)
	),

	TP_printk("page=%p pfn=%lu order=0 cold=%d",
			__entry->page,
			page_to_pfn(__entry->page),
			__entry->cold)
)

LTTNG_TRACEPOINT_EVENT_MAP(mm_page_alloc, kmem_mm_page_alloc,

	TP_PROTO(struct page *page, unsigned int order,
			gfp_t gfp_flags, int migratetype),

	TP_ARGS(page, order, gfp_flags, migratetype),

	TP_STRUCT__entry(
		__field_hex(	struct page *,	page		)
		__field(	unsigned int,	order		)
		__field(	gfp_t,		gfp_flags	)
		__field(	int,		migratetype	)
	),

	TP_fast_assign(
		tp_assign(page, page)
		tp_assign(order, order)
		tp_assign(gfp_flags, gfp_flags)
		tp_assign(migratetype, migratetype)
	),

	TP_printk("page=%p pfn=%lu order=%d migratetype=%d gfp_flags=%s",
		__entry->page,
		__entry->page ? page_to_pfn(__entry->page) : 0,
		__entry->order,
		__entry->migratetype,
		show_gfp_flags(__entry->gfp_flags))
)

LTTNG_TRACEPOINT_EVENT_CLASS(kmem_mm_page,

	TP_PROTO(struct page *page, unsigned int order, int migratetype),

	TP_ARGS(page, order, migratetype),

	TP_STRUCT__entry(
		__field_hex(	struct page *,	page		)
		__field(	unsigned int,	order		)
		__field(	int,		migratetype	)
	),

	TP_fast_assign(
		tp_assign(page, page)
		tp_assign(order, order)
		tp_assign(migratetype, migratetype)
	),

	TP_printk("page=%p pfn=%lu order=%u migratetype=%d percpu_refill=%d",
		__entry->page,
		__entry->page ? page_to_pfn(__entry->page) : 0,
		__entry->order,
		__entry->migratetype,
		__entry->order == 0)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(kmem_mm_page, mm_page_alloc_zone_locked,

	kmem_mm_page_alloc_zone_locked,

	TP_PROTO(struct page *page, unsigned int order, int migratetype),

	TP_ARGS(page, order, migratetype)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_PRINT_MAP(kmem_mm_page, mm_page_pcpu_drain,

	kmem_mm_page_pcpu_drain,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33))
	TP_PROTO(struct page *page, unsigned int order, int migratetype),
#else
	TP_PROTO(struct page *page, int order, int migratetype),
#endif

	TP_ARGS(page, order, migratetype),

	TP_printk("page=%p pfn=%lu order=%d migratetype=%d",
		__entry->page, page_to_pfn(__entry->page),
		__entry->order, __entry->migratetype)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,2)	\
	|| LTTNG_KERNEL_RANGE(3,14,36, 3,15,0))

LTTNG_TRACEPOINT_EVENT(mm_page_alloc_extfrag,

	TP_PROTO(struct page *page,
		int alloc_order, int fallback_order,
		int alloc_migratetype, int fallback_migratetype),

	TP_ARGS(page,
		alloc_order, fallback_order,
		alloc_migratetype, fallback_migratetype),

	TP_STRUCT__entry(
		__field_hex(	struct page *,	page			)
		__field(	int,		alloc_order		)
		__field(	int,		fallback_order		)
		__field(	int,		alloc_migratetype	)
		__field(	int,		fallback_migratetype	)
		__field(	int,		change_ownership	)
	),

	TP_fast_assign(
		tp_assign(page, page)
		tp_assign(alloc_order, alloc_order)
		tp_assign(fallback_order, fallback_order)
		tp_assign(alloc_migratetype, alloc_migratetype)
		tp_assign(fallback_migratetype, fallback_migratetype)
		tp_assign(change_ownership,
			(alloc_migratetype == get_pageblock_migratetype(page)))
	),

	TP_printk("page=%p pfn=%lu alloc_order=%d fallback_order=%d pageblock_order=%d alloc_migratetype=%d fallback_migratetype=%d fragmenting=%d change_ownership=%d",
		__entry->page,
		page_to_pfn(__entry->page),
		__entry->alloc_order,
		__entry->fallback_order,
		pageblock_order,
		__entry->alloc_migratetype,
		__entry->fallback_migratetype,
		__entry->fallback_order < pageblock_order,
		__entry->change_ownership)
)

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,30))

LTTNG_TRACEPOINT_EVENT(mm_page_alloc_extfrag,

	TP_PROTO(struct page *page,
		int alloc_order, int fallback_order,
		int alloc_migratetype, int fallback_migratetype, int new_migratetype),

	TP_ARGS(page,
		alloc_order, fallback_order,
		alloc_migratetype, fallback_migratetype, new_migratetype),

	TP_STRUCT__entry(
		__field_hex(	struct page *,	page			)
		__field(	int,		alloc_order		)
		__field(	int,		fallback_order		)
		__field(	int,		alloc_migratetype	)
		__field(	int,		fallback_migratetype	)
		__field(	int,		change_ownership	)
	),

	TP_fast_assign(
		tp_assign(page, page)
		tp_assign(alloc_order, alloc_order)
		tp_assign(fallback_order, fallback_order)
		tp_assign(alloc_migratetype, alloc_migratetype)
		tp_assign(fallback_migratetype, fallback_migratetype)
		tp_assign(change_ownership, (new_migratetype == alloc_migratetype))
	),

	TP_printk("page=%p pfn=%lu alloc_order=%d fallback_order=%d pageblock_order=%d alloc_migratetype=%d fallback_migratetype=%d fragmenting=%d change_ownership=%d",
		__entry->page,
		page_to_pfn(__entry->page),
		__entry->alloc_order,
		__entry->fallback_order,
		pageblock_order,
		__entry->alloc_migratetype,
		__entry->fallback_migratetype,
		__entry->fallback_order < pageblock_order,
		__entry->change_ownership)
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

	TP_STRUCT__entry(
		__field_hex(	struct page *,	page			)
		__field(	int,		alloc_order		)
		__field(	int,		fallback_order		)
		__field(	int,		alloc_migratetype	)
		__field(	int,		fallback_migratetype	)
		__field(	int,		change_ownership	)
	),

	TP_fast_assign(
		tp_assign(page, page)
		tp_assign(alloc_order, alloc_order)
		tp_assign(fallback_order, fallback_order)
		tp_assign(alloc_migratetype, alloc_migratetype)
		tp_assign(fallback_migratetype, fallback_migratetype)
		tp_assign(change_ownership, change_ownership)
	),

	TP_printk("page=%p pfn=%lu alloc_order=%d fallback_order=%d pageblock_order=%d alloc_migratetype=%d fallback_migratetype=%d fragmenting=%d change_ownership=%d",
		__entry->page,
		page_to_pfn(__entry->page),
		__entry->alloc_order,
		__entry->fallback_order,
		pageblock_order,
		__entry->alloc_migratetype,
		__entry->fallback_migratetype,
		__entry->fallback_order < pageblock_order,
		__entry->change_ownership)
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

	TP_STRUCT__entry(
		__field_hex(	struct page *,	page			)
		__field(	int,		alloc_order		)
		__field(	int,		fallback_order		)
		__field(	int,		alloc_migratetype	)
		__field(	int,		fallback_migratetype	)
	),

	TP_fast_assign(
		tp_assign(page, page)
		tp_assign(alloc_order, alloc_order)
		tp_assign(fallback_order, fallback_order)
		tp_assign(alloc_migratetype, alloc_migratetype)
		tp_assign(fallback_migratetype, fallback_migratetype)
	),

	TP_printk("page=%p pfn=%lu alloc_order=%d fallback_order=%d pageblock_order=%d alloc_migratetype=%d fallback_migratetype=%d fragmenting=%d change_ownership=%d",
		__entry->page,
		page_to_pfn(__entry->page),
		__entry->alloc_order,
		__entry->fallback_order,
		pageblock_order,
		__entry->alloc_migratetype,
		__entry->fallback_migratetype,
		__entry->fallback_order < pageblock_order,
		__entry->alloc_migratetype == __entry->fallback_migratetype)
)

#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */

#endif

#endif /* LTTNG_TRACE_KMEM_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
