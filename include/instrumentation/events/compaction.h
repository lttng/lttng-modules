/* SPDX-License-Identifier: GPL-2.0-only */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM compaction

#if !defined(LTTNG_TRACE_COMPACTION_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_COMPACTION_H

#include <lttng/tracepoint-event.h>
#include <linux/types.h>
#include <lttng/kernel-version.h>

LTTNG_TRACEPOINT_EVENT_CLASS(compaction_isolate_template,

	TP_PROTO(unsigned long start_pfn,
		unsigned long end_pfn,
		unsigned long nr_scanned,
		unsigned long nr_taken),

	TP_ARGS(start_pfn, end_pfn, nr_scanned, nr_taken),

	TP_FIELDS(
		ctf_integer(unsigned long, start_pfn, start_pfn)
		ctf_integer(unsigned long, end_pfn, end_pfn)
		ctf_integer(unsigned long, nr_scanned, nr_scanned)
		ctf_integer(unsigned long, nr_taken, nr_taken)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(compaction_isolate_template,

	mm_compaction_isolate_migratepages,

	compaction_isolate_migratepages,

	TP_PROTO(unsigned long start_pfn,
		unsigned long end_pfn,
		unsigned long nr_scanned,
		unsigned long nr_taken),

	TP_ARGS(start_pfn, end_pfn, nr_scanned, nr_taken)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(compaction_isolate_template,

	mm_compaction_isolate_freepages,

	compaction_isolate_freepages,

	TP_PROTO(unsigned long start_pfn,
		unsigned long end_pfn,
		unsigned long nr_scanned,
		unsigned long nr_taken),

	TP_ARGS(start_pfn, end_pfn, nr_scanned, nr_taken)
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,9,0))
LTTNG_TRACEPOINT_EVENT_MAP(mm_compaction_migratepages,

	compaction_migratepages,

	TP_PROTO(unsigned int nr_migratepages,
		unsigned int nr_succeeded),

	TP_ARGS(nr_migratepages, nr_succeeded),

	TP_FIELDS(
		ctf_integer(unsigned long, nr_migrated, nr_succeeded)
		ctf_integer(unsigned long, nr_failed, nr_migratepages - nr_succeeded)
	)
)
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,18,0) || \
    LTTNG_RHEL_KERNEL_RANGE(5,14,0,163,0,0, 5,15,0,0,0,0))
LTTNG_TRACEPOINT_EVENT_MAP(mm_compaction_migratepages,

	compaction_migratepages,

	TP_PROTO(struct compact_control *cc,
		unsigned int nr_succeeded),

	TP_ARGS(cc, nr_succeeded),

	TP_FIELDS(
		ctf_integer(unsigned long, nr_migrated, nr_succeeded)
		ctf_integer(unsigned long, nr_failed, cc->nr_migratepages - nr_succeeded)
	)
)
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,17,0))
LTTNG_TRACEPOINT_EVENT_MAP(mm_compaction_migratepages,

	compaction_migratepages,

	TP_PROTO(unsigned long nr_all,
		unsigned int nr_succeeded),

	TP_ARGS(nr_all, nr_succeeded),

	TP_FIELDS(
		ctf_integer(unsigned long, nr_migrated, nr_succeeded)
		ctf_integer(unsigned long, nr_failed, nr_all - nr_succeeded)
	)
)
#else
LTTNG_TRACEPOINT_EVENT_CODE_MAP(mm_compaction_migratepages,

	compaction_migratepages,

	TP_PROTO(unsigned long nr_all,
		int migrate_rc,
		struct list_head *migratepages),

	TP_ARGS(nr_all, migrate_rc, migratepages),

	TP_locvar(
		unsigned long nr_failed;
	),

	TP_code_pre(
		tp_locvar->nr_failed = 0;

		{
			struct list_head *page_lru;

			if (migrate_rc >= 0)
				tp_locvar->nr_failed = migrate_rc;
			else
				list_for_each(page_lru, migratepages)
					tp_locvar->nr_failed++;
		}
	),

	TP_FIELDS(
		ctf_integer(unsigned long, nr_migrated, nr_all - tp_locvar->nr_failed)
		ctf_integer(unsigned long, nr_failed, tp_locvar->nr_failed)
	),

	TP_code_post()
)
#endif /* #else #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(3,16,0)) */

#endif /* LTTNG_TRACE_COMPACTION_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
