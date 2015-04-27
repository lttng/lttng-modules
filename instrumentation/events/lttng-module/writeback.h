#undef TRACE_SYSTEM
#define TRACE_SYSTEM writeback

#if !defined(LTTNG_TRACE_WRITEBACK_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_WRITEBACK_H

#include "../../../probes/lttng-tracepoint-event.h"
#include <linux/tracepoint.h>
#include <linux/backing-dev.h>
#include <linux/writeback.h>
#include <linux/version.h>

#ifndef _TRACE_WRITEBACK_DEF_
#define _TRACE_WRITEBACK_DEF_

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0))
static inline struct backing_dev_info *inode_to_bdi(struct inode *inode)
{
	struct super_block *sb = inode->i_sb;

	if (strcmp(sb->s_type->name, "bdev") == 0)
		return inode->i_mapping->backing_dev_info;

	return sb->s_bdi;
}
#endif /* #if (LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)) */

#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
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
#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0)) */
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
		{I_REFERENCED,		"I_REFERENCED"}		\
	)
#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0)) */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))

LTTNG_TRACEPOINT_EVENT(writeback_dirty_page,
	TP_PROTO(struct page *page, struct address_space *mapping),
	TP_ARGS(page, mapping),
	TP_FIELDS(
		ctf_array_text(char, name,
			mapping ? dev_name(inode_to_bdi(mapping->host)->dev) : "(unknown)", 32)
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
			inode_to_bdi(inode)->dev ?
				dev_name(inode_to_bdi(inode)->dev) : "(unknown)", 32)
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
			dev_name(inode_to_bdi(inode)->dev), 32)
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

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))

LTTNG_TRACEPOINT_EVENT(writeback_dirty_page,
	TP_PROTO(struct page *page, struct address_space *mapping),
	TP_ARGS(page, mapping),
	TP_FIELDS(
		ctf_array_text(char, name,
			mapping ? dev_name(mapping->backing_dev_info->dev) : "(unknown)", 32)
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
			inode->i_mapping->backing_dev_info->dev ?
				dev_name(inode->i_mapping->backing_dev_info->dev)
				: "(unknown)", 32)
		ctf_integer(unsigned long, ino, inode->i_ino)
		ctf_integer(unsigned long, flags, flags)
	)
)
#define LTTNG_TRACEPOINT_EVENT_WRITEBACK_DIRTY_INODE_TEMPLATE(name) \
LTTNG_TRACEPOINT_EVENT_INSTANCE(writeback_dirty_inode_template, name, \
	TP_PROTO(struct inode *inode, int flags), \
	TP_ARGS(inode, flags))
LTTNG_TRACEPOINT_EVENT_WRITEBACK_DIRTY_INODE_TEMPLATE(writeback_dirty_inode_start)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_DIRTY_INODE_TEMPLATE(writeback_dirty_inode)

LTTNG_TRACEPOINT_EVENT_CLASS(writeback_write_inode_template,
	TP_PROTO(struct inode *inode, struct writeback_control *wbc),
	TP_ARGS(inode, wbc),
	TP_FIELDS(
		ctf_array_text(char, name,
			dev_name(inode->i_mapping->backing_dev_info->dev), 32)
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

#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0)) */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))

LTTNG_TRACEPOINT_EVENT_CLASS(writeback_work_class,
	TP_PROTO(struct backing_dev_info *bdi, struct wb_writeback_work *work),
	TP_ARGS(bdi, work),
	TP_FIELDS(
		ctf_array_text(char, name, bdi->dev ? dev_name(bdi->dev) :
				"(unknown)", 32)
	)
)

#else /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0)) */

LTTNG_TRACEPOINT_EVENT_CLASS(writeback_work_class,
	TP_PROTO(struct backing_dev_info *bdi, struct wb_writeback_work *work),
	TP_ARGS(bdi, work),
	TP_FIELDS(
		ctf_array_text(char, name,
			dev_name(bdi->dev ? bdi->dev :
				default_backing_dev_info.dev), 32)
	)
)

#endif /* #else #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0)) */

#define LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(name) \
LTTNG_TRACEPOINT_EVENT_INSTANCE(writeback_work_class, name, \
	TP_PROTO(struct backing_dev_info *bdi, struct wb_writeback_work *work), \
	TP_ARGS(bdi, work))
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(writeback_nothread)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(writeback_queue)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(writeback_exec)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0))
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(writeback_start)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(writeback_written)
LTTNG_TRACEPOINT_EVENT_WRITEBACK_WORK_INSTANCE(writeback_wait)
#endif

LTTNG_TRACEPOINT_EVENT(writeback_pages_written,
	TP_PROTO(long pages_written),
	TP_ARGS(pages_written),
	TP_FIELDS(
		ctf_integer(long, pages, pages_written)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(writeback_class,
	TP_PROTO(struct backing_dev_info *bdi),
	TP_ARGS(bdi),
	TP_FIELDS(
		ctf_array_text(char, name,
			dev_name(bdi->dev), 32)
	)
)
#undef DEFINE_WRITEBACK_EVENT
#define DEFINE_WRITEBACK_EVENT(name) \
LTTNG_TRACEPOINT_EVENT_INSTANCE(writeback_class, name, \
	TP_PROTO(struct backing_dev_info *bdi), \
	TP_ARGS(bdi))

#define DEFINE_WRITEBACK_EVENT_MAP(name, map) \
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(writeback_class, name, map, \
	TP_PROTO(struct backing_dev_info *bdi), \
	TP_ARGS(bdi))

DEFINE_WRITEBACK_EVENT(writeback_nowork)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
DEFINE_WRITEBACK_EVENT(writeback_wake_background)
#endif
DEFINE_WRITEBACK_EVENT(writeback_wake_thread)
DEFINE_WRITEBACK_EVENT(writeback_wake_forker_thread)
DEFINE_WRITEBACK_EVENT(writeback_bdi_register)
DEFINE_WRITEBACK_EVENT(writeback_bdi_unregister)
DEFINE_WRITEBACK_EVENT(writeback_thread_start)
DEFINE_WRITEBACK_EVENT(writeback_thread_stop)
#if (LTTNG_KERNEL_RANGE(3,1,0, 3,2,0))
DEFINE_WRITEBACK_EVENT_MAP(balance_dirty_start, writeback_balance_dirty_start)
DEFINE_WRITEBACK_EVENT_MAP(balance_dirty_wait, writeback_balance_dirty_wait)

LTTNG_TRACEPOINT_EVENT_MAP(balance_dirty_written,

	writeback_balance_dirty_written,

	TP_PROTO(struct backing_dev_info *bdi, int written),

	TP_ARGS(bdi, written),

	TP_FIELDS(
		ctf_array_text(char, name, dev_name(bdi->dev), 32)
		ctf_integer(int, written, written)
	)
)
#endif

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
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,1,0))
		ctf_integer(int, more_io, wbc->more_io)
		ctf_integer(unsigned long, older_than_this,
			wbc->older_than_this ? *wbc->older_than_this : 0)
#endif
		ctf_integer(long, range_start, (long) wbc->range_start)
		ctf_integer(long, range_end, (long) wbc->range_end)
	)
)

#undef DEFINE_WBC_EVENT
#define LTTNG_TRACEPOINT_EVENT_WBC_INSTANCE(name, map) \
LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(writeback_wbc_class, name, map, \
	TP_PROTO(struct writeback_control *wbc, struct backing_dev_info *bdi), \
	TP_ARGS(wbc, bdi))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,1,0))
LTTNG_TRACEPOINT_EVENT_WBC_INSTANCE(wbc_writeback_start, writeback_wbc_writeback_start)
LTTNG_TRACEPOINT_EVENT_WBC_INSTANCE(wbc_writeback_written, writeback_wbc_writeback_written)
LTTNG_TRACEPOINT_EVENT_WBC_INSTANCE(wbc_writeback_wait, writeback_wbc_writeback_wait)
LTTNG_TRACEPOINT_EVENT_WBC_INSTANCE(wbc_balance_dirty_start, writeback_wbc_balance_dirty_start)
LTTNG_TRACEPOINT_EVENT_WBC_INSTANCE(wbc_balance_dirty_written, writeback_wbc_balance_dirty_written)
LTTNG_TRACEPOINT_EVENT_WBC_INSTANCE(wbc_balance_dirty_wait, writeback_wbc_balance_dirty_wait)
#endif
LTTNG_TRACEPOINT_EVENT_WBC_INSTANCE(wbc_writepage, writeback_wbc_writepage)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0))
LTTNG_TRACEPOINT_EVENT(writeback_queue_io,
	TP_PROTO(struct bdi_writeback *wb,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
		 struct wb_writeback_work *work,
#else
		 unsigned long *older_than_this,
#endif
		 int moved),
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
	TP_ARGS(wb, work, moved),
#else
	TP_ARGS(wb, older_than_this, moved),
#endif
	TP_FIELDS(
		ctf_array_text(char, name, dev_name(wb->bdi->dev), 32)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
#else
		ctf_integer(unsigned long, older,
			older_than_this ? *older_than_this : 0)
		ctf_integer(long, age,
			older_than_this ?
				(jiffies - *older_than_this) * 1000 / HZ
				: -1)
#endif
		ctf_integer(int, moved, moved)
	)
)

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

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))

#define KBps(x)			((x) << (PAGE_SHIFT - 10))

LTTNG_TRACEPOINT_EVENT_MAP(bdi_dirty_ratelimit,

	writeback_bdi_dirty_ratelimit,

	TP_PROTO(struct backing_dev_info *bdi,
		 unsigned long dirty_rate,
		 unsigned long task_ratelimit),

	TP_ARGS(bdi, dirty_rate, task_ratelimit),

	TP_FIELDS(
		ctf_array_text(char, bdi, dev_name(bdi->dev), 32)
		ctf_integer(unsigned long, write_bw, KBps(bdi->write_bandwidth))
		ctf_integer(unsigned long, avg_write_bw, KBps(bdi->avg_write_bandwidth))
		ctf_integer(unsigned long, dirty_rate, KBps(dirty_rate))
		ctf_integer(unsigned long, dirty_ratelimit, KBps(bdi->dirty_ratelimit))
		ctf_integer(unsigned long, task_ratelimit, KBps(task_ratelimit))
		ctf_integer(unsigned long, balanced_dirty_ratelimit,
					KBps(bdi->balanced_dirty_ratelimit))
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(balance_dirty_pages,

	writeback_balance_dirty_pages,

	TP_PROTO(struct backing_dev_info *bdi,
		 unsigned long thresh,
		 unsigned long bg_thresh,
		 unsigned long dirty,
		 unsigned long bdi_thresh,
		 unsigned long bdi_dirty,
		 unsigned long dirty_ratelimit,
		 unsigned long task_ratelimit,
		 unsigned long dirtied,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
		 unsigned long period,
#endif
		 long pause,
		 unsigned long start_time),

	TP_ARGS(bdi, thresh, bg_thresh, dirty, bdi_thresh, bdi_dirty,
		dirty_ratelimit, task_ratelimit,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
		dirtied, period, pause, start_time
#else
		dirtied, pause, start_time
#endif
	),

	TP_FIELDS(
		ctf_array_text(char, bdi, dev_name(bdi->dev), 32)
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
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
		ctf_integer(unsigned long, period,
			period * 1000 / HZ)
		ctf_integer(long, think,
			current->dirty_paused_when == 0 ? 0 :
				(long)(jiffies - current->dirty_paused_when) * 1000/HZ)
#endif
	)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,5,0))
LTTNG_TRACEPOINT_EVENT(writeback_sb_inodes_requeue,

	TP_PROTO(struct inode *inode),
	TP_ARGS(inode),

	TP_FIELDS(
		ctf_array_text(char, name,
			dev_name(inode_to_bdi(inode)->dev), 32)
		ctf_integer(unsigned long, ino, inode->i_ino)
		ctf_integer(unsigned long, state, inode->i_state)
		ctf_integer(unsigned long, dirtied_when, inode->dirtied_when)
	)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
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
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0))
LTTNG_TRACEPOINT_EVENT_CLASS(writeback_single_inode_template,

	TP_PROTO(struct inode *inode,
		 struct writeback_control *wbc,
		 unsigned long nr_to_write
	),

	TP_ARGS(inode, wbc, nr_to_write),

	TP_FIELDS(
		ctf_array_text(char, name,
			dev_name(inode_to_bdi(inode)->dev), 32)
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

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))
LTTNG_TRACEPOINT_EVENT_INSTANCE(writeback_single_inode_template, writeback_single_inode_requeue,
	TP_PROTO(struct inode *inode,
		struct writeback_control *wbc,
		unsigned long nr_to_write),
	TP_ARGS(inode, wbc, nr_to_write)
)
#endif

LTTNG_TRACEPOINT_EVENT_INSTANCE(writeback_single_inode_template, writeback_single_inode,
	TP_PROTO(struct inode *inode,
		 struct writeback_control *wbc,
		 unsigned long nr_to_write),
	TP_ARGS(inode, wbc, nr_to_write)
)
#endif

#endif /* LTTNG_TRACE_WRITEBACK_H */

/* This part must be outside protection */
#include "../../../probes/define_trace.h"
