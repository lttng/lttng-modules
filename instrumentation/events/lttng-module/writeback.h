/* SPDX-License-Identifier: GPL-2.0-only */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM writeback

#if !defined(LTTNG_TRACE_WRITEBACK_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_WRITEBACK_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/tracepoint.h>
#include <linux/backing-dev.h>
#include <linux/writeback.h>

#ifndef _TRACE_WRITEBACK_DEF_
#define _TRACE_WRITEBACK_DEF_

static inline struct backing_dev_info *lttng_inode_to_bdi(struct inode *inode)
{
	return inode_to_bdi(inode);
}

#endif

#define show_inode_state(state)					\
	__print_flags(state, "|",				\
		{I_DIRTY_SYNC,		"I_DIRTY_SYNC"},	\
		{I_DIRTY_DATASYNC,	"I_DIRTY_DATASYNC"},	\
		{I_DIRTY_PAGES,		"I_DIRTY_PAGES"},	\
		{I_NEW,			"I_NEW"},		\
		{I_WILL_FREE,		"I_WILL_FREE"},		\
		{I_FREEING,		"I_FREEING"},		\
		{I_CLEAR,		"I_CLEAR"},		\
		{I_SYNC,		"I_SYNC"},		\
		{I_DIRTY_TIME,		"I_DIRTY_TIME"},	\
		{I_DIRTY_TIME_EXPIRED,	"I_DIRTY_TIME_EXPIRED"}, \
		{I_REFERENCED,		"I_REFERENCED"}		\
	)

LTTNG_TRACEPOINT_EVENT(writeback_dirty_page,
	TP_PROTO(struct page *page, struct address_space *mapping),
	TP_ARGS(page, mapping),
	TP_FIELDS(
		ctf_array_text(char, name,
			mapping ? dev_name(lttng_inode_to_bdi(mapping->host)->dev) : "(unknown)", 32)
		ctf_integer(unsigned long, ino, mapping ? mapping->host->i_ino : 0)
		ctf_integer(pgoff_t, index, page->index)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(writeback_dirty_inode_template,
	TP_PROTO(struct inode *inode, int flags),
	TP_ARGS(inode, flags),
	TP_FIELDS(
		/* may be called for files on pseudo FSes w/ unregistered bdi */
		ctf_array_text(char, name,
			lttng_inode_to_bdi(inode)->dev ?
				dev_name(lttng_inode_to_bdi(inode)->dev) : "(unknown)", 32)
		ctf_integer(unsigned long, ino, inode->i_ino)
		ctf_integer(unsigned long, state, inode->i_state)
		ctf_integer(unsigned long, flags, flags)
	)
)
#define LTTNG_TRACEPOINT_EVENT_WRITEBACK_DIRTY_INODE_TEMPLATE(name) \
LTTNG_TRACEPOINT_EVENT_INSTANCE(writeback_dirty_inode_template, name, \
	TP_PROTO(struct inode *inode, int flags), \
	TP_ARGS(inode, flags))
LTTNG_TRACEPOINT_EVENT_WRITEBACK_DIRTY_INODE_TEMPLATE(writeback_dirty_inode_start)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_DIRTY_INODE_TEMPLATE(writeback_dirty_inode)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_DIRTY_INODE_TEMPLATE(writeback_mark_inode_dirty)

LTTNG_TRACEPOINT_EVENT_CLASS(writeback_write_inode_template,
	TP_PROTO(struct inode *inode, struct writeback_control *wbc),
	TP_ARGS(inode, wbc),
	TP_FIELDS(
		ctf_array_text(char, name,
			dev_name(lttng_inode_to_bdi(inode)->dev), 32)
		ctf_integer(unsigned long, ino, inode->i_ino)
		ctf_integer(int, sync_mode, wbc->sync_mode)
	)
)

#define LTTNG_TRACEPOINT_EVENT_WRITEBACK_WRITE_INODE(name) \
LTTNG_TRACEPOINT_EVENT_INSTANCE(writeback_write_inode_template, name, \
	TP_PROTO(struct inode *inode, struct writeback_control *wbc), \
	TP_ARGS(inode, wbc))
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WRITE_INODE(writeback_write_inode_start)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WRITE_INODE(writeback_write_inode)

LTTNG_TRACEPOINT_EVENT_CLASS(writeback_work_class,
	TP_PROTO(struct bdi_writeback *wb, struct wb_writeback_work *work),
	TP_ARGS(wb, work),
	TP_FIELDS(
		ctf_array_text(char, name, wb->bdi->dev ? dev_name(wb->bdi->dev) :
				"(unknown)", 32)
	)
)

#define LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(name) \
LTTNG_TRACEPOINT_EVENT_INSTANCE(writeback_work_class, name, \
	TP_PROTO(struct bdi_writeback *wb, struct wb_writeback_work *work), \
	TP_ARGS(wb, work))

LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(writeback_nothread)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(writeback_queue)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(writeback_exec)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(writeback_start)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(writeback_written)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(writeback_wait)

LTTNG_TRACEPOINT_EVENT(writeback_pages_written,
	TP_PROTO(long pages_written),
	TP_ARGS(pages_written),
	TP_FIELDS(
		ctf_integer(long, pages, pages_written)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(writeback_class,
	TP_PROTO(struct bdi_writeback *wb),
	TP_ARGS(wb),
	TP_FIELDS(
		ctf_array_text(char, name,
			dev_name(wb->bdi->dev), 32)
	)
)

#undef DEFINE_WRITEBACK_EVENT
#define DEFINE_WRITEBACK_EVENT(name) \
LTTNG_TRACEPOINT_EVENT_INSTANCE(writeback_class, name, \
	TP_PROTO(struct bdi_writeback *wb), \
	TP_ARGS(wb))

#define DEFINE_WRITEBACK_EVENT_MAP(name, map) \
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(writeback_class, name, map, \
	TP_PROTO(struct bdi_writeback *wb), \
	TP_ARGS(wb))

LTTNG_TRACEPOINT_EVENT(writeback_bdi_register,
	TP_PROTO(struct backing_dev_info *bdi),
	TP_ARGS(bdi),
	TP_FIELDS(
		ctf_array_text(char, name,
			dev_name(bdi->dev), 32)
	)
)

DEFINE_WRITEBACK_EVENT(writeback_nowork)
DEFINE_WRITEBACK_EVENT(writeback_wake_background)
DEFINE_WRITEBACK_EVENT(writeback_wake_thread)
DEFINE_WRITEBACK_EVENT(writeback_wake_forker_thread)
DEFINE_WRITEBACK_EVENT(writeback_bdi_unregister)
DEFINE_WRITEBACK_EVENT(writeback_thread_start)
DEFINE_WRITEBACK_EVENT(writeback_thread_stop)

LTTNG_TRACEPOINT_EVENT_CLASS(writeback_wbc_class,
	TP_PROTO(struct writeback_control *wbc, struct backing_dev_info *bdi),
	TP_ARGS(wbc, bdi),
	TP_FIELDS(
		ctf_array_text(char, name, dev_name(bdi->dev), 32)
		ctf_integer(long, nr_to_write, wbc->nr_to_write)
		ctf_integer(long, pages_skipped, wbc->pages_skipped)
		ctf_integer(int, sync_mode, wbc->sync_mode)
		ctf_integer(int, for_kupdate, wbc->for_kupdate)
		ctf_integer(int, for_background, wbc->for_background)
		ctf_integer(int, for_reclaim, wbc->for_reclaim)
		ctf_integer(int, range_cyclic, wbc->range_cyclic)
		ctf_integer(long, range_start, (long) wbc->range_start)
		ctf_integer(long, range_end, (long) wbc->range_end)
	)
)

#undef DEFINE_WBC_EVENT
#define LTTNG_TRACEPOINT_EVENT_WBC_INSTANCE(name, map) \
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(writeback_wbc_class, name, map, \
	TP_PROTO(struct writeback_control *wbc, struct backing_dev_info *bdi), \
	TP_ARGS(wbc, bdi))
LTTNG_TRACEPOINT_EVENT_WBC_INSTANCE(wbc_writepage, writeback_wbc_writepage)

LTTNG_TRACEPOINT_EVENT(writeback_queue_io,
	TP_PROTO(struct bdi_writeback *wb,
		 struct wb_writeback_work *work,
		 int moved),
	TP_ARGS(wb, work, moved),
	TP_FIELDS(
		ctf_array_text(char, name, dev_name(wb->bdi->dev), 32)
		ctf_integer(int, moved, moved)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(global_dirty_state,

	writeback_global_dirty_state,

	TP_PROTO(struct wb_domain *domain,
		 unsigned long background_thresh,
		 unsigned long dirty_thresh
	),

	TP_ARGS(domain,
		background_thresh,
		dirty_thresh
	),

	TP_FIELDS(
		ctf_integer(unsigned long, nr_dirty, global_node_page_state(NR_FILE_DIRTY))
		ctf_integer(unsigned long, nr_writeback, global_node_page_state(NR_WRITEBACK))
		ctf_integer(unsigned long, nr_unstable, global_node_page_state(NR_UNSTABLE_NFS))
		ctf_integer(unsigned long, nr_dirtied, global_node_page_state(NR_DIRTIED))
		ctf_integer(unsigned long, nr_written, global_node_page_state(NR_WRITTEN))
		ctf_integer(unsigned long, background_thresh, background_thresh)
		ctf_integer(unsigned long, dirty_thresh, dirty_thresh)
		ctf_integer(unsigned long, dirty_limit, domain->dirty_limit)
	)
)

#define KBps(x)			((x) << (PAGE_SHIFT - 10))

LTTNG_TRACEPOINT_EVENT_MAP(bdi_dirty_ratelimit,

	writeback_bdi_dirty_ratelimit,

	TP_PROTO(struct bdi_writeback *wb,
		 unsigned long dirty_rate,
		 unsigned long task_ratelimit),

	TP_ARGS(wb, dirty_rate, task_ratelimit),

	TP_FIELDS(
		ctf_array_text(char, bdi, dev_name(wb->bdi->dev), 32)
		ctf_integer(unsigned long, write_bw, KBps(wb->bdi->wb.write_bandwidth))
		ctf_integer(unsigned long, avg_write_bw, KBps(wb->bdi->wb.avg_write_bandwidth))
		ctf_integer(unsigned long, dirty_rate, KBps(dirty_rate))
		ctf_integer(unsigned long, dirty_ratelimit, KBps(wb->bdi->wb.dirty_ratelimit))
		ctf_integer(unsigned long, task_ratelimit, KBps(task_ratelimit))
		ctf_integer(unsigned long, balanced_dirty_ratelimit,
					KBps(wb->bdi->wb.balanced_dirty_ratelimit))
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(balance_dirty_pages,

	writeback_balance_dirty_pages,

	TP_PROTO(struct wb_domain *domain,
		 struct bdi_writeback *wb,
		 struct dirty_throttle_control *sdtc,
		 unsigned long dirty_ratelimit,
		 unsigned long task_ratelimit,
		 unsigned long dirtied,
		 unsigned long period,
		 long pause,
		 unsigned long start_time),

	TP_ARGS(domain, wb, sdtc,
		dirty_ratelimit, task_ratelimit,
		dirtied, period, pause, start_time
	),

	TP_FIELDS(
		ctf_array_text(char, bdi, dev_name(wb->bdi->dev), 32)
		ctf_integer(unsigned long, limit, domain->dirty_limit)
		ctf_integer(unsigned long, setpoint,
			(domain->dirty_limit + (sdtc->thresh + sdtc->bg_thresh) / 2) / 2)
		ctf_integer(unsigned long, dirty, sdtc->dirty)
		ctf_integer(unsigned long, bdi_setpoint,
			((domain->dirty_limit + (sdtc->thresh + sdtc->bg_thresh) / 2) / 2) *
				sdtc->wb_thresh / (sdtc->thresh + 1))
		ctf_integer(unsigned long, bdi_dirty, sdtc->wb_dirty)
		ctf_integer(unsigned long, dirty_ratelimit,
			KBps(dirty_ratelimit))
		ctf_integer(unsigned long, task_ratelimit,
			KBps(task_ratelimit))
		ctf_integer(unsigned int, dirtied, dirtied)
		ctf_integer(unsigned int, dirtied_pause,
			current->nr_dirtied_pause)
		ctf_integer(unsigned long, paused,
			(jiffies - start_time) * 1000 / HZ)
		ctf_integer(long, pause, pause * 1000 / HZ)
		ctf_integer(unsigned long, period,
			period * 1000 / HZ)
		ctf_integer(long, think,
			current->dirty_paused_when == 0 ? 0 :
				(long)(jiffies - current->dirty_paused_when) * 1000/HZ)
	)
)

LTTNG_TRACEPOINT_EVENT(writeback_sb_inodes_requeue,

	TP_PROTO(struct inode *inode),
	TP_ARGS(inode),

	TP_FIELDS(
		ctf_array_text(char, name,
			dev_name(lttng_inode_to_bdi(inode)->dev), 32)
		ctf_integer(unsigned long, ino, inode->i_ino)
		ctf_integer(unsigned long, state, inode->i_state)
		ctf_integer(unsigned long, dirtied_when, inode->dirtied_when)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(writeback_congest_waited_template,

	TP_PROTO(unsigned int usec_timeout, unsigned int usec_delayed),

	TP_ARGS(usec_timeout, usec_delayed),

	TP_FIELDS(
		ctf_integer(unsigned int, usec_timeout, usec_timeout)
		ctf_integer(unsigned int, usec_delayed, usec_delayed)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(writeback_congest_waited_template, writeback_congestion_wait,

	TP_PROTO(unsigned int usec_timeout, unsigned int usec_delayed),

	TP_ARGS(usec_timeout, usec_delayed)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(writeback_congest_waited_template, writeback_wait_iff_congested,

	TP_PROTO(unsigned int usec_timeout, unsigned int usec_delayed),

	TP_ARGS(usec_timeout, usec_delayed)
)

LTTNG_TRACEPOINT_EVENT_CLASS(writeback_single_inode_template,

	TP_PROTO(struct inode *inode,
		 struct writeback_control *wbc,
		 unsigned long nr_to_write
	),

	TP_ARGS(inode, wbc, nr_to_write),

	TP_FIELDS(
		ctf_array_text(char, name,
			dev_name(lttng_inode_to_bdi(inode)->dev), 32)
		ctf_integer(unsigned long, ino, inode->i_ino)
		ctf_integer(unsigned long, state, inode->i_state)
		ctf_integer(unsigned long, dirtied_when, inode->dirtied_when)
		ctf_integer(unsigned long, writeback_index,
			inode->i_mapping->writeback_index)
		ctf_integer(long, nr_to_write, nr_to_write)
		ctf_integer(unsigned long, wrote,
			nr_to_write - wbc->nr_to_write)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(writeback_single_inode_template, writeback_single_inode,
	TP_PROTO(struct inode *inode,
		 struct writeback_control *wbc,
		 unsigned long nr_to_write),
	TP_ARGS(inode, wbc, nr_to_write)
)

#endif /* LTTNG_TRACE_WRITEBACK_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
