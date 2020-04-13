/* SPDX-License-Identifier: GPL-2.0-only */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM mm_vmscan

#if !defined(LTTNG_TRACE_MM_VMSCAN_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_MM_VMSCAN_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/memcontrol.h>

#ifndef _TRACE_VMSCAN_DEF
#define _TRACE_VMSCAN_DEF
#define RECLAIM_WB_ANON		0x0001u
#define RECLAIM_WB_FILE		0x0002u
#define RECLAIM_WB_MIXED	0x0010u
#define RECLAIM_WB_SYNC		0x0004u /* Unused, all reclaim async */
#define RECLAIM_WB_ASYNC	0x0008u
#endif

#include <linux/mm_inline.h>

#define trace_reclaim_flags(file) ( \
	(file ? RECLAIM_WB_FILE : RECLAIM_WB_ANON) | \
	(RECLAIM_WB_ASYNC) \
	)

LTTNG_TRACEPOINT_EVENT(mm_vmscan_kswapd_sleep,

	TP_PROTO(int nid),

	TP_ARGS(nid),

	TP_FIELDS(
		ctf_integer(int, nid, nid)
	)
)

LTTNG_TRACEPOINT_EVENT(mm_vmscan_kswapd_wake,

	TP_PROTO(int nid, int zid, int order),

	TP_ARGS(nid, zid, order),

	TP_FIELDS(
		ctf_integer(int, nid, nid)
		ctf_integer(int, zid, zid)
		ctf_integer(int, order, order)
	)
)

LTTNG_TRACEPOINT_EVENT(mm_vmscan_wakeup_kswapd,

	TP_PROTO(int nid, int zid, int order, gfp_t gfp_flags),

	TP_ARGS(nid, zid, order, gfp_flags),

	TP_FIELDS(
		ctf_integer(int, nid, nid)
		ctf_integer(int, zid, zid)
		ctf_integer(int, order, order)
		ctf_integer(gfp_t, gfp_flags, gfp_flags)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(mm_vmscan_direct_reclaim_begin_template,

	TP_PROTO(int order, gfp_t gfp_flags),

	TP_ARGS(order, gfp_flags),

	TP_FIELDS(
		ctf_integer(int, order, order)
		ctf_integer(gfp_t, gfp_flags, gfp_flags)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(mm_vmscan_direct_reclaim_begin_template, mm_vmscan_direct_reclaim_begin,

	TP_PROTO(int order, gfp_t gfp_flags),

	TP_ARGS(order, gfp_flags)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(mm_vmscan_direct_reclaim_begin_template, mm_vmscan_memcg_reclaim_begin,

	TP_PROTO(int order, gfp_t gfp_flags),

	TP_ARGS(order, gfp_flags)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(mm_vmscan_direct_reclaim_begin_template, mm_vmscan_memcg_softlimit_reclaim_begin,

	TP_PROTO(int order, gfp_t gfp_flags),

	TP_ARGS(order, gfp_flags)
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

LTTNG_TRACEPOINT_EVENT_MAP(mm_shrink_slab_start,

	mm_vmscan_shrink_slab_start,

	TP_PROTO(struct shrinker *shr, struct shrink_control *sc,
		long nr_objects_to_shrink, unsigned long cache_items,
		unsigned long long delta, unsigned long total_scan,
		int priority),

	TP_ARGS(shr, sc, nr_objects_to_shrink, cache_items, delta, total_scan,
		priority),

	TP_FIELDS(
		ctf_integer_hex(struct shrinker *, shr, shr)
		ctf_integer_hex(void *, shrink, shr->scan_objects)
		ctf_integer(int, nid, sc->nid)
		ctf_integer(long, nr_objects_to_shrink, nr_objects_to_shrink)
		ctf_integer(gfp_t, gfp_flags, sc->gfp_mask)
		ctf_integer(unsigned long, cache_items, cache_items)
		ctf_integer(unsigned long long, delta, delta)
		ctf_integer(unsigned long, total_scan, total_scan)
		ctf_integer(int, priority, priority)
	)
)

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

LTTNG_TRACEPOINT_EVENT(mm_vmscan_lru_isolate,

	TP_PROTO(int classzone_idx,
		int order,
		unsigned long nr_requested,
		unsigned long nr_scanned,
		unsigned long nr_skipped,
		unsigned long nr_taken,
		isolate_mode_t isolate_mode,
		int lru
	),

	TP_ARGS(classzone_idx, order, nr_requested, nr_scanned, nr_skipped,
		nr_taken, isolate_mode, lru
	),

	TP_FIELDS(
		ctf_integer(int, classzone_idx, classzone_idx)
		ctf_integer(int, order, order)
		ctf_integer(unsigned long, nr_requested, nr_requested)
		ctf_integer(unsigned long, nr_scanned, nr_scanned)
		ctf_integer(unsigned long, nr_skipped, nr_skipped)
		ctf_integer(unsigned long, nr_taken, nr_taken)
		ctf_integer(isolate_mode_t, isolate_mode, isolate_mode)
		ctf_integer(int, lru, lru)
	)
)

LTTNG_TRACEPOINT_EVENT(mm_vmscan_writepage,

	TP_PROTO(struct page *page),

	TP_ARGS(page),

	TP_FIELDS(
		ctf_integer_hex(struct page *, page, page)
		ctf_integer(int, reclaim_flags, trace_reclaim_flags(
				page_is_file_lru(page)))
	)
)

LTTNG_TRACEPOINT_EVENT(mm_vmscan_lru_shrink_inactive,

	TP_PROTO(int nid,
		unsigned long nr_scanned, unsigned long nr_reclaimed,
		struct reclaim_stat *stat, int priority, int file),

	TP_ARGS(nid, nr_scanned, nr_reclaimed, stat, priority, file),

	TP_FIELDS(
		ctf_integer(int, nid, nid)
		ctf_integer(unsigned long, nr_scanned, nr_scanned)
		ctf_integer(unsigned long, nr_reclaimed, nr_reclaimed)
		ctf_integer(unsigned long, nr_dirty, stat->nr_dirty)
		ctf_integer(unsigned long, nr_writeback, stat->nr_writeback)
		ctf_integer(unsigned long, nr_congested, stat->nr_congested)
		ctf_integer(unsigned long, nr_immediate, stat->nr_immediate)
		ctf_integer(unsigned long, nr_activate_anon, stat->nr_activate[0])
		ctf_integer(unsigned long, nr_activate_file, stat->nr_activate[1])
		ctf_integer(unsigned long, nr_ref_keep, stat->nr_ref_keep)
		ctf_integer(unsigned long, nr_unmap_fail, stat->nr_unmap_fail)
		ctf_integer(int, priority, priority)
		ctf_integer(int, reclaim_flags, trace_reclaim_flags(file))
	)
)

#endif /* LTTNG_TRACE_MM_VMSCAN_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
