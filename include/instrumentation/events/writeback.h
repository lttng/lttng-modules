// SPDX-FileCopyrightText: 2012 Andrew Gabbasov <andrew_gabbasov@mentor.com>
//
// SPDX-License-Identifier: GPL-2.0-only

#undef TRACE_SYSTEM
#define TRACE_SYSTEM writeback

#if !defined(LTTNG_TRACE_WRITEBACK_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_WRITEBACK_H

#include <lttng/tracepoint-event.h>
#include <linux/tracepoint.h>
#include <linux/backing-dev.h>
#include <linux/writeback.h>
#include <lttng/kernel-version.h>

#ifndef _TRACE_WRITEBACK_DEF_
#define _TRACE_WRITEBACK_DEF_

/*
 * Use the GPL-exported bdi_dev_name introduced in kernel 5.7. Do not use
 * static inline bdi_dev_name in prior kernels because it uses the bdi_unknown_name
 * symbol which is not exported to GPL modules.
 */
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,7,0))
static inline const char *lttng_bdi_dev_name(struct backing_dev_info *bdi)
{
	return bdi_dev_name(bdi);
}
#else
static inline const char *lttng_bdi_dev_name(struct backing_dev_info *bdi)
{
	if (!bdi || !bdi->dev)
		return "(unknown)";
	return dev_name(bdi->dev);
}
#endif
#endif

/*
 * Keep this macro around to track changes in the valid values for `state`.
 * Eventually, we will have a enum to describe that field. If the macro is
 * changed by an upstream kernel, the fact that we have a redefinition here
 * will generate a compiler warning.
 */
#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,12,0))
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
		{I_REFERENCED,		"I_REFERENCED"},	\
		{I_LINKABLE,		"I_LINKABLE"},		\
		{I_WB_SWITCH,		"I_WB_SWITCH"},		\
		{I_OVL_INUSE,		"I_OVL_INUSE"},		\
		{I_CREATING,		"I_CREATING"},		\
		{I_DONTCACHE,		"I_DONTCACHE"},		\
		{I_SYNC_QUEUED,		"I_SYNC_QUEUED"},	\
		{I_PINNING_NETFS_WB,	"I_PINNING_NETFS_WB"},	\
		{I_LRU_ISOLATING,	"I_LRU_ISOLATING"}	\
	)
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,9,0) || \
	LTTNG_KERNEL_RANGE(4,14,218, 4,15,0) ||			\
	LTTNG_KERNEL_RANGE(4,19,172, 4,20,0) ||			\
	LTTNG_KERNEL_RANGE(5,4,94, 5,5,0) ||			\
	LTTNG_UBUNTU_KERNEL_RANGE(4,15,18,141, 4,16,0,0) ||	\
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,240,0,0, 4,19,0,0,0,0))
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
		{I_REFERENCED,		"I_REFERENCED"}		\
	)
#else
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
#endif /* #if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,9,0)) */

LTTNG_TRACEPOINT_EVENT(writeback_dirty_page,
	TP_PROTO(struct page *page, struct address_space *mapping),
	TP_ARGS(page, mapping),
	TP_FIELDS(
		ctf_string(name, lttng_bdi_dev_name(mapping ? inode_to_bdi(mapping->host) : NULL))
		ctf_integer(unsigned long, ino, mapping ? mapping->host->i_ino : 0)
		ctf_integer(pgoff_t, index, page->index)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(writeback_dirty_inode_template,
	TP_PROTO(struct inode *inode, int flags),
	TP_ARGS(inode, flags),
	TP_FIELDS(
		/* may be called for files on pseudo FSes w/ unregistered bdi */
		ctf_string(name, lttng_bdi_dev_name(inode_to_bdi(inode)))
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
		ctf_string(name, lttng_bdi_dev_name(inode_to_bdi(inode)))
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
		ctf_string(name, lttng_bdi_dev_name(wb->bdi))
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
		ctf_string(name, lttng_bdi_dev_name(wb->bdi))
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
		ctf_string(name, lttng_bdi_dev_name(bdi))
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
		ctf_string(name, lttng_bdi_dev_name(bdi))
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

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,9,0) || \
	LTTNG_KERNEL_RANGE(5,8,6, 5,9,0) || \
	LTTNG_KERNEL_RANGE(5,4,62, 5,5,0) || \
	LTTNG_KERNEL_RANGE(4,19,143, 4,20,0) || \
	LTTNG_KERNEL_RANGE(4,14,196, 4,15,0) || \
	LTTNG_KERNEL_RANGE(4,9,235, 4,10,0) || \
	LTTNG_KERNEL_RANGE(4,4,235, 4,5,0) || \
	LTTNG_UBUNTU_KERNEL_RANGE(4,15,18,119, 4,16,0,0) || \
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,240,0,0, 4,19,0,0,0,0))
LTTNG_TRACEPOINT_EVENT(writeback_queue_io,
	TP_PROTO(struct bdi_writeback *wb,
		 struct wb_writeback_work *work,
		 unsigned long dirtied_before,
		 int moved),
	TP_ARGS(wb, work, dirtied_before, moved),
	TP_FIELDS(
		ctf_string(name, lttng_bdi_dev_name(wb->bdi))
		ctf_integer(unsigned long, older, dirtied_before)
		ctf_integer(int, moved, moved)
	)
)
#else
LTTNG_TRACEPOINT_EVENT(writeback_queue_io,
	TP_PROTO(struct bdi_writeback *wb,
		 struct wb_writeback_work *work,
		 int moved),
	TP_ARGS(wb, work, moved),
	TP_FIELDS(
		ctf_string(name, lttng_bdi_dev_name(wb->bdi))
		ctf_integer(int, moved, moved)
	)
)
#endif

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(5,8,0) || \
	LTTNG_RHEL_KERNEL_RANGE(4,18,0,305,0,0, 4,19,0,0,0,0))
LTTNG_TRACEPOINT_EVENT_MAP(global_dirty_state,

	writeback_global_dirty_state,

	TP_PROTO(unsigned long background_thresh,
		 unsigned long dirty_thresh
	),

	TP_ARGS(background_thresh,
		dirty_thresh
	),

	TP_FIELDS(
		ctf_integer(unsigned long, nr_dirty, global_node_page_state(NR_FILE_DIRTY))
		ctf_integer(unsigned long, nr_writeback, global_node_page_state(NR_WRITEBACK))
		ctf_integer(unsigned long, nr_dirtied, global_node_page_state(NR_DIRTIED))
		ctf_integer(unsigned long, nr_written, global_node_page_state(NR_WRITTEN))
		ctf_integer(unsigned long, background_thresh, background_thresh)
		ctf_integer(unsigned long, dirty_thresh, dirty_thresh)
		ctf_integer(unsigned long, dirty_limit, global_dirty_limit)
	)
)
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(4,14,0))
LTTNG_TRACEPOINT_EVENT_MAP(global_dirty_state,

	writeback_global_dirty_state,

	TP_PROTO(unsigned long background_thresh,
		 unsigned long dirty_thresh
	),

	TP_ARGS(background_thresh,
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
		ctf_integer(unsigned long, dirty_limit, global_dirty_limit)
	)
)
#else
LTTNG_TRACEPOINT_EVENT_MAP(global_dirty_state,

	writeback_global_dirty_state,

	TP_PROTO(unsigned long background_thresh,
		 unsigned long dirty_thresh
	),

	TP_ARGS(background_thresh,
		dirty_thresh
	),

	TP_FIELDS(
		ctf_integer(unsigned long, nr_dirty, global_page_state(NR_FILE_DIRTY))
		ctf_integer(unsigned long, nr_writeback, global_page_state(NR_WRITEBACK))
		ctf_integer(unsigned long, nr_unstable, global_page_state(NR_UNSTABLE_NFS))
		ctf_integer(unsigned long, nr_dirtied, global_page_state(NR_DIRTIED))
		ctf_integer(unsigned long, nr_written, global_page_state(NR_WRITTEN))
		ctf_integer(unsigned long, background_thresh, background_thresh)
		ctf_integer(unsigned long, dirty_thresh, dirty_thresh)
		ctf_integer(unsigned long, dirty_limit, global_dirty_limit)
	)
)
#endif

#define KBps(x)			((x) << (PAGE_SHIFT - 10))

LTTNG_TRACEPOINT_EVENT_MAP(bdi_dirty_ratelimit,

	writeback_bdi_dirty_ratelimit,

	TP_PROTO(struct bdi_writeback *wb,
		 unsigned long dirty_rate,
		 unsigned long task_ratelimit),

	TP_ARGS(wb, dirty_rate, task_ratelimit),

	TP_FIELDS(
		ctf_string(bdi, lttng_bdi_dev_name(wb->bdi))
		ctf_integer(unsigned long, write_bw, KBps(wb->bdi->wb.write_bandwidth))
		ctf_integer(unsigned long, avg_write_bw, KBps(wb->bdi->wb.avg_write_bandwidth))
		ctf_integer(unsigned long, dirty_rate, KBps(dirty_rate))
		ctf_integer(unsigned long, dirty_ratelimit, KBps(wb->bdi->wb.dirty_ratelimit))
		ctf_integer(unsigned long, task_ratelimit, KBps(task_ratelimit))
		ctf_integer(unsigned long, balanced_dirty_ratelimit,
					KBps(wb->bdi->wb.balanced_dirty_ratelimit))
	)
)

#if (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,15,0))
LTTNG_TRACEPOINT_EVENT_MAP(balance_dirty_pages,

	writeback_balance_dirty_pages,

	TP_PROTO(struct bdi_writeback *wb,
		 struct dirty_throttle_control *dtc,
		 unsigned long dirty_ratelimit,
		 unsigned long task_ratelimit,
		 unsigned long dirtied,
		 unsigned long period,
		 long pause,
		 unsigned long start_time),

	TP_ARGS(wb, dtc, dirty_ratelimit, task_ratelimit,
		dirtied, period, pause, start_time
	),

	TP_FIELDS(
		ctf_string(bdi, lttng_bdi_dev_name(wb->bdi))
		ctf_integer(unsigned long, limit, dtc->limit)
		ctf_integer(unsigned long, setpoint,
                    (dtc->limit + (dtc->thresh + dtc->bg_thresh)) / 2)
		ctf_integer(unsigned long, dirty, dtc->dirty)
		ctf_integer(unsigned long, wb_setpoint,
                    ((dtc->limit + (dtc->thresh + dtc->bg_thresh)) / 2)
                    * (dtc->wb_thresh / (dtc->thresh + 1)))
		ctf_integer(unsigned long, wb_dirty, dtc->wb_dirty)
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
#ifdef CONFIG_CGROUP_WRITEBACK
		ctf_integer(ino_t, cgroup_ino, cgroup_ino(wb->memcg_css->cgroup))
#else
        ctf_integer(ino_t, cgroup_ino, 1)
#endif
	)
)
#elif (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,14,2))
LTTNG_TRACEPOINT_EVENT_MAP(balance_dirty_pages,

	writeback_balance_dirty_pages,

	TP_PROTO(struct bdi_writeback *wb,
		 struct dirty_throttle_control *dtc,
		 unsigned long dirty_ratelimit,
		 unsigned long task_ratelimit,
		 unsigned long dirtied,
		 unsigned long period,
		 long pause,
		 unsigned long start_time),

	TP_ARGS(wb, dtc, dirty_ratelimit, task_ratelimit,
		dirtied, period, pause, start_time
	),

	TP_FIELDS(
		ctf_string(bdi, lttng_bdi_dev_name(wb->bdi))
		ctf_integer(unsigned long, limit, dtc->limit)
		ctf_integer(unsigned long, setpoint,
                    (dtc->limit + (dtc->thresh + dtc->bg_thresh)) / 2)
		ctf_integer(unsigned long, dirty, dtc->dirty)
		ctf_integer(unsigned long, wb_setpoint,
                    ((dtc->limit + (dtc->thresh + dtc->bg_thresh)) / 2)
                    * (dtc->wb_thresh / (dtc->thresh + 1)))
		ctf_integer(unsigned long, wb_dirty, dtc->wb_dirty)
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
#else
LTTNG_TRACEPOINT_EVENT_MAP(balance_dirty_pages,

	writeback_balance_dirty_pages,

	TP_PROTO(struct bdi_writeback *wb,
		 unsigned long thresh,
		 unsigned long bg_thresh,
		 unsigned long dirty,
		 unsigned long bdi_thresh,
		 unsigned long bdi_dirty,
		 unsigned long dirty_ratelimit,
		 unsigned long task_ratelimit,
		 unsigned long dirtied,
		 unsigned long period,
		 long pause,
		 unsigned long start_time),

	TP_ARGS(wb, thresh, bg_thresh, dirty, bdi_thresh, bdi_dirty,
		dirty_ratelimit, task_ratelimit,
		dirtied, period, pause, start_time
	),

	TP_FIELDS(
		ctf_string(bdi, lttng_bdi_dev_name(wb->bdi))
		ctf_integer(unsigned long, limit, global_dirty_limit)
		ctf_integer(unsigned long, setpoint,
			(global_dirty_limit + (thresh + bg_thresh) / 2) / 2)
		ctf_integer(unsigned long, dirty, dirty)
		ctf_integer(unsigned long, bdi_setpoint,
			((global_dirty_limit + (thresh + bg_thresh) / 2) / 2) *
				bdi_thresh / (thresh + 1))
		ctf_integer(unsigned long, bdi_dirty, bdi_dirty)
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
#endif /* else (LTTNG_LINUX_VERSION_CODE >= LTTNG_KERNEL_VERSION(6,14,2)) */

LTTNG_TRACEPOINT_EVENT(writeback_sb_inodes_requeue,

	TP_PROTO(struct inode *inode),
	TP_ARGS(inode),

	TP_FIELDS(
		ctf_string(name, lttng_bdi_dev_name(inode_to_bdi(inode)))
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
		ctf_string(name, lttng_bdi_dev_name(inode_to_bdi(inode)))
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
#include <lttng/define_trace.h>
