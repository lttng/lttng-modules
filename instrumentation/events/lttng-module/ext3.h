#undef TRACE_SYSTEM
#define TRACE_SYSTEM ext3

#if !defined(LTTNG_TRACE_EXT3_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_EXT3_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/version.h>

LTTNG_TRACEPOINT_EVENT(ext3_free_inode,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(umode_t, mode, inode->i_mode)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,5,0))
		ctf_integer(uid_t, uid, i_uid_read(inode))
		ctf_integer(gid_t, gid, i_gid_read(inode))
#else
		ctf_integer(uid_t, uid, inode->i_uid)
		ctf_integer(gid_t, gid, inode->i_gid)
#endif
		ctf_integer(blkcnt_t, blocks, inode->i_blocks)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_request_inode,
	TP_PROTO(struct inode *dir, int mode),

	TP_ARGS(dir, mode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, dir->i_sb->s_dev)
		ctf_integer(ino_t, dir, dir->i_ino)
		ctf_integer(umode_t, mode, mode)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_allocate_inode,
	TP_PROTO(struct inode *inode, struct inode *dir, int mode),

	TP_ARGS(inode, dir, mode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ino_t, dir, dir->i_ino)
		ctf_integer(umode_t, mode, mode)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_evict_inode,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(int, nlink, inode->i_nlink)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_drop_inode,
	TP_PROTO(struct inode *inode, int drop),

	TP_ARGS(inode, drop),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(int, drop, drop)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_mark_inode_dirty,
	TP_PROTO(struct inode *inode, unsigned long IP),

	TP_ARGS(inode, IP),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(unsigned long, ip, IP)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_write_begin,
	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int flags),

	TP_ARGS(inode, pos, len, flags),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(loff_t, pos, pos)
		ctf_integer(unsigned int, len, len)
		ctf_integer(unsigned int, flags, flags)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext3__write_end,
	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
			unsigned int copied),

	TP_ARGS(inode, pos, len, copied),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(loff_t, pos, pos)
		ctf_integer(unsigned int, len, len)
		ctf_integer(unsigned int, copied, copied)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext3__write_end, ext3_ordered_write_end,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int copied),

	TP_ARGS(inode, pos, len, copied)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext3__write_end, ext3_writeback_write_end,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int copied),

	TP_ARGS(inode, pos, len, copied)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext3__write_end, ext3_journalled_write_end,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int copied),

	TP_ARGS(inode, pos, len, copied)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext3__page_op,
	TP_PROTO(struct page *page),

	TP_ARGS(page),

	TP_FIELDS(
		ctf_integer(dev_t, dev, page->mapping->host->i_sb->s_dev)
		ctf_integer(ino_t, ino, page->mapping->host->i_ino)
		ctf_integer(pgoff_t, index, page->index)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext3__page_op, ext3_ordered_writepage,

	TP_PROTO(struct page *page),

	TP_ARGS(page)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext3__page_op, ext3_writeback_writepage,

	TP_PROTO(struct page *page),

	TP_ARGS(page)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext3__page_op, ext3_journalled_writepage,

	TP_PROTO(struct page *page),

	TP_ARGS(page)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext3__page_op, ext3_readpage,

	TP_PROTO(struct page *page),

	TP_ARGS(page)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext3__page_op, ext3_releasepage,

	TP_PROTO(struct page *page),

	TP_ARGS(page)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0))

LTTNG_TRACEPOINT_EVENT(ext3_invalidatepage,
	TP_PROTO(struct page *page, unsigned int offset, unsigned int length),

	TP_ARGS(page, offset, length),

	TP_FIELDS(
		ctf_integer(pgoff_t, index, page->index)
		ctf_integer(unsigned int, offset, offset)
		ctf_integer(unsigned int, length, length)
		ctf_integer(ino_t, ino, page->mapping->host->i_ino)
		ctf_integer(dev_t, dev, page->mapping->host->i_sb->s_dev)
	)
)

#else

LTTNG_TRACEPOINT_EVENT(ext3_invalidatepage,
	TP_PROTO(struct page *page, unsigned long offset),

	TP_ARGS(page, offset),

	TP_FIELDS(
		ctf_integer(pgoff_t, index, page->index)
		ctf_integer(unsigned long, offset, offset)
		ctf_integer(ino_t, ino, page->mapping->host->i_ino)
		ctf_integer(dev_t, dev, page->mapping->host->i_sb->s_dev)
	)
)

#endif

LTTNG_TRACEPOINT_EVENT(ext3_discard_blocks,
	TP_PROTO(struct super_block *sb, unsigned long blk,
			unsigned long count),

	TP_ARGS(sb, blk, count),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer(unsigned long, blk, blk)
		ctf_integer(unsigned long, count, count)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_request_blocks,
	TP_PROTO(struct inode *inode, unsigned long goal,
		 unsigned long count),

	TP_ARGS(inode, goal, count),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(unsigned long, count, count)
		ctf_integer(unsigned long, goal, goal)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_allocate_blocks,
	TP_PROTO(struct inode *inode, unsigned long goal,
		 unsigned long count, unsigned long block),

	TP_ARGS(inode, goal, count, block),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(unsigned long, block, block)
		ctf_integer(unsigned long, count, count)
		ctf_integer(unsigned long, goal, goal)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_free_blocks,
	TP_PROTO(struct inode *inode, unsigned long block,
		 unsigned long count),

	TP_ARGS(inode, block, count),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(umode_t, mode, inode->i_mode)
		ctf_integer(unsigned long, block, block)
		ctf_integer(unsigned long, count, count)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_sync_file_enter,
	TP_PROTO(struct file *file, int datasync),

	TP_ARGS(file, datasync),

	TP_FIELDS(
		ctf_integer(dev_t, dev, file->f_path.dentry->d_inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, file->f_path.dentry->d_inode->i_ino)
		ctf_integer(ino_t, parent, file->f_path.dentry->d_parent->d_inode->i_ino)
		ctf_integer(int, datasync, datasync)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_sync_file_exit,
	TP_PROTO(struct inode *inode, int ret),

	TP_ARGS(inode, ret),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(int, ret, ret)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_sync_fs,
	TP_PROTO(struct super_block *sb, int wait),

	TP_ARGS(sb, wait),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer(int, wait, wait)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_rsv_window_add,
	TP_PROTO(struct super_block *sb,
		 struct ext3_reserve_window_node *rsv_node),

	TP_ARGS(sb, rsv_node),

	TP_FIELDS(
		ctf_integer(unsigned long, start, rsv_node->rsv_window._rsv_start)
		ctf_integer(unsigned long, end, rsv_node->rsv_window._rsv_end)
		ctf_integer(dev_t, dev, sb->s_dev)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_discard_reservation,
	TP_PROTO(struct inode *inode,
		 struct ext3_reserve_window_node *rsv_node),

	TP_ARGS(inode, rsv_node),

	TP_FIELDS(
		ctf_integer(unsigned long, start, rsv_node->rsv_window._rsv_start)
		ctf_integer(unsigned long, end, rsv_node->rsv_window._rsv_end)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_alloc_new_reservation,
	TP_PROTO(struct super_block *sb, unsigned long goal),

	TP_ARGS(sb, goal),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer(unsigned long, goal, goal)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_reserved,
	TP_PROTO(struct super_block *sb, unsigned long block,
		 struct ext3_reserve_window_node *rsv_node),

	TP_ARGS(sb, block, rsv_node),

	TP_FIELDS(
		ctf_integer(unsigned long, block, block)
		ctf_integer(unsigned long, start, rsv_node->rsv_window._rsv_start)
		ctf_integer(unsigned long, end, rsv_node->rsv_window._rsv_end)
		ctf_integer(dev_t, dev, sb->s_dev)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_forget,
	TP_PROTO(struct inode *inode, int is_metadata, unsigned long block),

	TP_ARGS(inode, is_metadata, block),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(umode_t, mode, inode->i_mode)
		ctf_integer(int, is_metadata, is_metadata)
		ctf_integer(unsigned long, block, block)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_read_block_bitmap,
	TP_PROTO(struct super_block *sb, unsigned int group),

	TP_ARGS(sb, group),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer(__u32, group, group)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_direct_IO_enter,
	TP_PROTO(struct inode *inode, loff_t offset, unsigned long len, int rw),

	TP_ARGS(inode, offset, len, rw),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(loff_t, pos, offset)
		ctf_integer(unsigned long, len, len)
		ctf_integer(int, rw, rw)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_direct_IO_exit,
	TP_PROTO(struct inode *inode, loff_t offset, unsigned long len,
		 int rw, int ret),

	TP_ARGS(inode, offset, len, rw, ret),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(loff_t, pos, offset)
		ctf_integer(unsigned long, len, len)
		ctf_integer(int, rw, rw)
		ctf_integer(int, ret, ret)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_unlink_enter,
	TP_PROTO(struct inode *parent, struct dentry *dentry),

	TP_ARGS(parent, dentry),

	TP_FIELDS(
		ctf_integer(ino_t, parent, parent->i_ino)
		ctf_integer(ino_t, ino, dentry->d_inode->i_ino)
		ctf_integer(loff_t, size, dentry->d_inode->i_size)
		ctf_integer(dev_t, dev, dentry->d_inode->i_sb->s_dev)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_unlink_exit,
	TP_PROTO(struct dentry *dentry, int ret),

	TP_ARGS(dentry, ret),

	TP_FIELDS(
		ctf_integer(dev_t, dev, dentry->d_inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, dentry->d_inode->i_ino)
		ctf_integer(int, ret, ret)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext3__truncate,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(blkcnt_t, blocks, inode->i_blocks)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext3__truncate, ext3_truncate_enter,

	TP_PROTO(struct inode *inode),

	TP_ARGS(inode)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext3__truncate, ext3_truncate_exit,

	TP_PROTO(struct inode *inode),

	TP_ARGS(inode)
)

LTTNG_TRACEPOINT_EVENT(ext3_get_blocks_enter,
	TP_PROTO(struct inode *inode, unsigned long lblk,
		 unsigned long len, int create),

	TP_ARGS(inode, lblk, len, create),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(unsigned long, lblk, lblk)
		ctf_integer(unsigned long, len, len)
		ctf_integer(int, create, create)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_get_blocks_exit,
	TP_PROTO(struct inode *inode, unsigned long lblk,
		 unsigned long pblk, unsigned long len, int ret),

	TP_ARGS(inode, lblk, pblk, len, ret),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(unsigned long, lblk, lblk)
		ctf_integer(unsigned long, pblk, pblk)
		ctf_integer(unsigned long, len, len)
		ctf_integer(int, ret, ret)
	)
)

LTTNG_TRACEPOINT_EVENT(ext3_load_inode,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
	)
)

#endif /* LTTNG_TRACE_EXT3_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
