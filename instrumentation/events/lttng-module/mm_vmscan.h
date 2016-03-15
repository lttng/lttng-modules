#undef TRACE_SYSTEM
#define TRACE_SYSTEM mm_vmscan

#if !defined(LTTNG_TRACE_MM_VMSCAN_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_MM_VMSCAN_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/memcontrol.h>
#include <trace/events/gfpflags.h>
#include <linux/version.h>

#ifndef _TRACE_VMSCAN_DEF
#define _TRACE_VMSCAN_DEF
#define RECLAIM_WB_ANON		0x0001u
#define RECLAIM_WB_FILE		0x0002u
#define RECLAIM_WB_MIXED	0x0010u
#define RECLAIM_WB_SYNC		0x0004u /* Unused, all reclaim async */
#define RECLAIM_WB_ASYNC	0x0008u

#if ((LINUX_VERSION_CODE <= KERNEL_VERSION(3,0,38)) || \
	LTTNG_KERNEL_RANGE(3,1,0, 3,2,0))
typedef int isolate_mode_t;
#endif

#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0))

#include <linux/mm_inline.h>

#define trace_reclaim_flags(page) ( \
	(page_is_file_cache(page) ? RECLAIM_WB_FILE : RECLAIM_WB_ANON) | \
	(RECLAIM_WB_ASYNC) \
	)

#define trace_shrink_flags(file) \
	( \
		(file ? RECLAIM_WB_FILE : RECLAIM_WB_ANON) | \
		(RECLAIM_WB_ASYNC) \
	)

#endif

LTTNG_TRACEPOINT_EVENT(mm_vmscan_kswapd_sleep,

	TP_PROTO(int nid),

	TP_ARGS(nid),

	TP_FIELDS(
		ctf_integer(int, nid, nid)
	)
)

LTTNG_TRACEPOINT_EVENT(mm_vmscan_kswapd_wake,

	TP_PROTO(int nid, int order),

	TP_ARGS(nid, order),

	TP_FIELDS(
		ctf_integer(int, nid, nid)
		ctf_integer(int, order, order)
	)
)

LTTNG_TRACEPOINT_EVENT(mm_vmscan_wakeup_kswapd,

	TP_PROTO(int nid, int zid, int order),

	TP_ARGS(nid, zid, order),

	TP_FIELDS(
		ctf_integer(int, nid, nid)
		ctf_integer(int, zid, zid)
		ctf_integer(int, order, order)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(mm_vmscan_direct_reclaim_begin_template,

	TP_PROTO(int order, int may_writepage, gfp_t gfp_flags),

	TP_ARGS(order, may_writepage, gfp_flags),

	TP_FIELDS(
		ctf_integer(int, order, order)
		ctf_integer(int, may_writepage, may_writepage)
		ctf_integer(gfp_t, gfp_flags, gfp_flags)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(mm_vmscan_direct_reclaim_begin_template, mm_vmscan_direct_reclaim_begin,

	TP_PROTO(int order, int may_writepage, gfp_t gfp_flags),

	TP_ARGS(order, may_writepage, gfp_flags)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(mm_vmscan_direct_reclaim_begin_template, mm_vmscan_memcg_reclaim_begin,

	TP_PROTO(int order, int may_writepage, gfp_t gfp_flags),

	TP_ARGS(order, may_writepage, gfp_flags)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(mm_vmscan_direct_reclaim_begin_template, mm_vmscan_memcg_softlimit_reclaim_begin,

	TP_PROTO(int order, int may_writepage, gfp_t gfp_flags),

	TP_ARGS(order, may_writepage, gfp_flags)
)

LTTNG_TRACEPOINT_EVENT_CLASS(mm_vmscan_direct_reclaim_end_template,

	TP_PROTO(unsigned long nr_reclaimed),

	TP_ARGS(nr_reclaimed),

	TP_FIELDS(
		ctf_integer(unsigned long, nr_reclaimed, nr_reclaimed)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(mm_vmscan_direct_reclaim_end_template, mm_vmscan_direct_reclaim_end,

	TP_PROTO(unsigned long nr_reclaimed),

	TP_ARGS(nr_reclaimed)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(mm_vmscan_direct_reclaim_end_template, mm_vmscan_memcg_reclaim_end,

	TP_PROTO(unsigned long nr_reclaimed),

	TP_ARGS(nr_reclaimed)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(mm_vmscan_direct_reclaim_end_template, mm_vmscan_memcg_softlimit_reclaim_end,

	TP_PROTO(unsigned long nr_reclaimed),

	TP_ARGS(nr_reclaimed)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0))
LTTNG_TRACEPOINT_EVENT_MAP(mm_shrink_slab_start,

	mm_vmscan_shrink_slab_start,

	TP_PROTO(struct shrinker *shr, struct shrink_control *sc,
		long nr_objects_to_shrink, unsigned long pgs_scanned,
		unsigned long lru_pgs, unsigned long cache_items,
		unsigned long long delta, unsigned long total_scan),

	TP_ARGS(shr, sc, nr_objects_to_shrink, pgs_scanned, lru_pgs,
		cache_items, delta, total_scan),

	TP_FIELDS(
		ctf_integer_hex(struct shrinker *, shr, shr)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
		ctf_integer_hex(void *, shrink, shr->scan_objects)
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
		ctf_integer_hex(void *, shrink, shr->shrink)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
		ctf_integer(long, nr_objects_to_shrink, nr_objects_to_shrink)
		ctf_integer(gfp_t, gfp_flags, sc->gfp_mask)
		ctf_integer(unsigned long, pgs_scanned, pgs_scanned)
		ctf_integer(unsigned long, lru_pgs, lru_pgs)
		ctf_integer(unsigned long, cache_items, cache_items)
		ctf_integer(unsigned long long, delta, delta)
		ctf_integer(unsigned long, total_scan, total_scan)
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0))
LTTNG_TRACEPOINT_EVENT_MAP(mm_shrink_slab_end,

	mm_vmscan_shrink_slab_end,

	TP_PROTO(struct shrinker *shr, int nid, int shrinker_retval,
		long unused_scan_cnt, long new_scan_cnt, long total_scan),

	TP_ARGS(shr, nid, shrinker_retval, unused_scan_cnt, new_scan_cnt,
		total_scan),

	TP_FIELDS(
		ctf_integer_hex(struct shrinker *, shr, shr)
		ctf_integer(int, nid, nid)
		ctf_integer_hex(void *, shrink, shr->scan_objects)
		ctf_integer(long, unused_scan, unused_scan_cnt)
		ctf_integer(long, new_scan, new_scan_cnt)
		ctf_integer(int, retval, shrinker_retval)
		ctf_integer(long, total_scan, total_scan)
	)
)
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)) */
LTTNG_TRACEPOINT_EVENT_MAP(mm_shrink_slab_end,

	mm_vmscan_shrink_slab_end,

	TP_PROTO(struct shrinker *shr, int shrinker_retval,
		long unused_scan_cnt, long new_scan_cnt),

	TP_ARGS(shr, shrinker_retval, unused_scan_cnt, new_scan_cnt),

	TP_FIELDS(
		ctf_integer_hex(struct shrinker *, shr, shr)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0))
		ctf_integer_hex(void *, shrink, shr->scan_objects)
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
		ctf_integer_hex(void *, shrink, shr->shrink)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)) */
		ctf_integer(long, unused_scan, unused_scan_cnt)
		ctf_integer(long, new_scan, new_scan_cnt)
		ctf_integer(int, retval, shrinker_retval)
		ctf_integer(long, total_scan, new_scan_cnt - unused_scan_cnt)
	)
)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)) */
#endif

LTTNG_TRACEPOINT_EVENT_CLASS(mm_vmscan_lru_isolate_template,

	TP_PROTO(int order,
		unsigned long nr_requested,
		unsigned long nr_scanned,
		unsigned long nr_taken,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
		unsigned long nr_lumpy_taken,
		unsigned long nr_lumpy_dirty,
		unsigned long nr_lumpy_failed,
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0))
		isolate_mode_t isolate_mode
#else
		isolate_mode_t isolate_mode,
		int file
#endif
	),

	TP_ARGS(order, nr_requested, nr_scanned, nr_taken,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
		nr_lumpy_taken, nr_lumpy_dirty, nr_lumpy_failed,
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0))
		isolate_mode
#else
		isolate_mode, file
#endif
	),


	TP_FIELDS(
		ctf_integer(int, order, order)
		ctf_integer(unsigned long, nr_requested, nr_requested)
		ctf_integer(unsigned long, nr_scanned, nr_scanned)
		ctf_integer(unsigned long, nr_taken, nr_taken)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
		ctf_integer(unsigned long, nr_lumpy_taken, nr_lumpy_taken)
		ctf_integer(unsigned long, nr_lumpy_dirty, nr_lumpy_dirty)
		ctf_integer(unsigned long, nr_lumpy_failed, nr_lumpy_failed)
#endif
		ctf_integer(isolate_mode_t, isolate_mode, isolate_mode)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
		ctf_integer(int, file, file)
#endif
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(mm_vmscan_lru_isolate_template, mm_vmscan_lru_isolate,

	TP_PROTO(int order,
		unsigned long nr_requested,
		unsigned long nr_scanned,
		unsigned long nr_taken,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
		unsigned long nr_lumpy_taken,
		unsigned long nr_lumpy_dirty,
		unsigned long nr_lumpy_failed,
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0))
		isolate_mode_t isolate_mode
#else
		isolate_mode_t isolate_mode,
		int file
#endif
	),

	TP_ARGS(order, nr_requested, nr_scanned, nr_taken,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
		nr_lumpy_taken, nr_lumpy_dirty, nr_lumpy_failed,
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0))
		isolate_mode
#else
		isolate_mode, file
#endif
	)

)

LTTNG_TRACEPOINT_EVENT_INSTANCE(mm_vmscan_lru_isolate_template, mm_vmscan_memcg_isolate,

	TP_PROTO(int order,
		unsigned long nr_requested,
		unsigned long nr_scanned,
		unsigned long nr_taken,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
		unsigned long nr_lumpy_taken,
		unsigned long nr_lumpy_dirty,
		unsigned long nr_lumpy_failed,
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0))
		isolate_mode_t isolate_mode
#else
		isolate_mode_t isolate_mode,
		int file
#endif
	),

	TP_ARGS(order, nr_requested, nr_scanned, nr_taken,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
		nr_lumpy_taken, nr_lumpy_dirty, nr_lumpy_failed,
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0))
		isolate_mode
#else
		isolate_mode, file
#endif
	)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0))
LTTNG_TRACEPOINT_EVENT(mm_vmscan_writepage,

	TP_PROTO(struct page *page),

	TP_ARGS(page),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(int, reclaim_flags, trace_reclaim_flags(page))
	)
)
#else
LTTNG_TRACEPOINT_EVENT(mm_vmscan_writepage,

	TP_PROTO(struct page *page,
		int reclaim_flags),

	TP_ARGS(page, reclaim_flags),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(int, reclaim_flags, reclaim_flags)
	)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0))
LTTNG_TRACEPOINT_EVENT(mm_vmscan_lru_shrink_inactive,

	TP_PROTO(struct zone *zone,
		unsigned long nr_scanned, unsigned long nr_reclaimed,
		int priority, int file),

	TP_ARGS(zone, nr_scanned, nr_reclaimed, priority, file),

	TP_FIELDS(
		ctf_integer(int, nid, zone_to_nid(zone))
		ctf_integer(int, zid, zone_idx(zone))
		ctf_integer(unsigned long, nr_scanned, nr_scanned)
		ctf_integer(unsigned long, nr_reclaimed, nr_reclaimed)
		ctf_integer(int, priority, priority)
		ctf_integer(int, reclaim_flags, trace_shrink_flags(file))
	)
)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
LTTNG_TRACEPOINT_EVENT(mm_vmscan_lru_shrink_inactive,

	TP_PROTO(int nid, int zid,
			unsigned long nr_scanned, unsigned long nr_reclaimed,
			int priority, int reclaim_flags),

	TP_ARGS(nid, zid, nr_scanned, nr_reclaimed, priority, reclaim_flags),

	TP_FIELDS(
		ctf_integer(int, nid, nid)
		ctf_integer(int, zid, zid)
		ctf_integer(unsigned long, nr_scanned, nr_scanned)
		ctf_integer(unsigned long, nr_reclaimed, nr_reclaimed)
		ctf_integer(int, priority, priority)
		ctf_integer(int, reclaim_flags, reclaim_flags)
	)
)
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
LTTNG_TRACEPOINT_EVENT_MAP(replace_swap_token,

	mm_vmscan_replace_swap_token,

	TP_PROTO(struct mm_struct *old_mm,
		 struct mm_struct *new_mm),

	TP_ARGS(old_mm, new_mm),

	TP_FIELDS(
		ctf_integer_hex(struct mm_struct *, old_mm, old_mm)
		ctf_integer(unsigned int, old_prio, old_mm ? old_mm->token_priority : 0)
		ctf_integer_hex(struct mm_struct *, new_mm, new_mm)
		ctf_integer(unsigned int, new_prio, new_mm->token_priority)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(mm_vmscan_put_swap_token_template,
	TP_PROTO(struct mm_struct *swap_token_mm),

	TP_ARGS(swap_token_mm),

	TP_FIELDS(
		ctf_integer_hex(struct mm_struct*, swap_token_mm, swap_token_mm)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(mm_vmscan_put_swap_token_template, put_swap_token,

	mm_vmscan_put_swap_token,

	TP_PROTO(struct mm_struct *swap_token_mm),
	TP_ARGS(swap_token_mm)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(mm_vmscan_put_swap_token_template, disable_swap_token,

	mm_vmscan_disable_swap_token,

	TP_PROTO(struct mm_struct *swap_token_mm),
	TP_ARGS(swap_token_mm)
)

LTTNG_TRACEPOINT_EVENT_MAP(update_swap_token_priority,

	mm_vmscan_update_swap_token_priority,

	TP_PROTO(struct mm_struct *mm,
		 unsigned int old_prio,
		 struct mm_struct *swap_token_mm),

	TP_ARGS(mm, old_prio, swap_token_mm),

	TP_FIELDS(
		ctf_integer_hex(struct mm_struct *, mm, mm)
		ctf_integer(unsigned int, old_prio, old_prio)
		ctf_integer(unsigned int, new_prio, mm->token_priority)
		ctf_integer_hex(struct mm_struct *, swap_token_mm, swap_token_mm)
		ctf_integer(unsigned int, swap_token_prio, swap_token_mm ? swap_token_mm->token_priority : 0)
	)
)
#endif

#endif /* LTTNG_TRACE_MM_VMSCAN_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
