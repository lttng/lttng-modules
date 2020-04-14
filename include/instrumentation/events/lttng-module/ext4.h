/* SPDX-License-Identifier: GPL-2.0-only */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM ext4

#if !defined(LTTNG_TRACE_EXT4_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_EXT4_H

#include <lttng/lttng-tracepoint-event.h>
#include <linux/writeback.h>

#ifndef _TRACE_EXT4_DEF_
#define _TRACE_EXT4_DEF_
struct ext4_allocation_context;
struct ext4_allocation_request;
struct ext4_prealloc_space;
struct ext4_inode_info;
struct mpage_da_data;
struct ext4_map_blocks;
struct ext4_extent;
#endif

#define EXT4_I(inode) (container_of(inode, struct ext4_inode_info, vfs_inode))
#define TP_MODE_T	__u16

LTTNG_TRACEPOINT_EVENT(ext4_free_inode,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(uid_t, uid, i_uid_read(inode))
		ctf_integer(gid_t, gid, i_gid_read(inode))
		ctf_integer(__u64, blocks, inode->i_blocks)
		ctf_integer(TP_MODE_T, mode, inode->i_mode)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_request_inode,
	TP_PROTO(struct inode *dir, int mode),

	TP_ARGS(dir, mode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, dir->i_sb->s_dev)
		ctf_integer(ino_t, dir, dir->i_ino)
		ctf_integer(TP_MODE_T, mode, mode)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_allocate_inode,
	TP_PROTO(struct inode *inode, struct inode *dir, int mode),

	TP_ARGS(inode, dir, mode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ino_t, dir, dir->i_ino)
		ctf_integer(TP_MODE_T, mode, mode)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_evict_inode,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(int, nlink, inode->i_nlink)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_drop_inode,
	TP_PROTO(struct inode *inode, int drop),

	TP_ARGS(inode, drop),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(int, drop, drop)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_mark_inode_dirty,
	TP_PROTO(struct inode *inode, unsigned long IP),

	TP_ARGS(inode, IP),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer_hex(unsigned long, ip, IP)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_begin_ordered_truncate,
	TP_PROTO(struct inode *inode, loff_t new_size),

	TP_ARGS(inode, new_size),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(loff_t, new_size, new_size)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext4__write_begin,

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

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__write_begin, ext4_write_begin,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int flags),

	TP_ARGS(inode, pos, len, flags)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__write_begin, ext4_da_write_begin,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int flags),

	TP_ARGS(inode, pos, len, flags)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext4__write_end,
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

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__write_end, ext4_ordered_write_end,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int copied),

	TP_ARGS(inode, pos, len, copied)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__write_end, ext4_writeback_write_end,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int copied),

	TP_ARGS(inode, pos, len, copied)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__write_end, ext4_journalled_write_end,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int copied),

	TP_ARGS(inode, pos, len, copied)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__write_end, ext4_da_write_end,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int copied),

	TP_ARGS(inode, pos, len, copied)
)

LTTNG_TRACEPOINT_EVENT(ext4_da_writepages,
	TP_PROTO(struct inode *inode, struct writeback_control *wbc),

	TP_ARGS(inode, wbc),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(long, nr_to_write, wbc->nr_to_write)
		ctf_integer(long, pages_skipped, wbc->pages_skipped)
		ctf_integer(loff_t, range_start, wbc->range_start)
		ctf_integer(loff_t, range_end, wbc->range_end)
		ctf_integer(pgoff_t, writeback_index, inode->i_mapping->writeback_index)
		ctf_integer(int, sync_mode, wbc->sync_mode)
		ctf_integer(char, for_kupdate, wbc->for_kupdate)
		ctf_integer(char, range_cyclic, wbc->range_cyclic)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_da_write_pages,
	TP_PROTO(struct inode *inode, pgoff_t first_page,
		 struct writeback_control *wbc),

	TP_ARGS(inode, first_page, wbc),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(pgoff_t, first_page, first_page)
		ctf_integer(long, nr_to_write, wbc->nr_to_write)
		ctf_integer(int, sync_mode, wbc->sync_mode)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_da_write_pages_extent,
	TP_PROTO(struct inode *inode, struct ext4_map_blocks *map),

	TP_ARGS(inode, map),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(__u64, lblk, map->m_lblk)
		ctf_integer(__u32, len, map->m_len)
		ctf_integer(__u32, flags, map->m_flags)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_da_writepages_result,
	TP_PROTO(struct inode *inode, struct writeback_control *wbc,
			int ret, int pages_written),

	TP_ARGS(inode, wbc, ret, pages_written),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(int, ret, ret)
		ctf_integer(int, pages_written, pages_written)
		ctf_integer(long, pages_skipped, wbc->pages_skipped)
		ctf_integer(pgoff_t, writeback_index, inode->i_mapping->writeback_index)
		ctf_integer(int, sync_mode, wbc->sync_mode)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext4__page_op,
	TP_PROTO(struct page *page),

	TP_ARGS(page),

	TP_FIELDS(
		ctf_integer(dev_t, dev, page->mapping->host->i_sb->s_dev)
		ctf_integer(ino_t, ino, page->mapping->host->i_ino)
		ctf_integer(pgoff_t, index, page->index)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__page_op, ext4_writepage,

	TP_PROTO(struct page *page),

	TP_ARGS(page)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__page_op, ext4_readpage,

	TP_PROTO(struct page *page),

	TP_ARGS(page)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__page_op, ext4_releasepage,

	TP_PROTO(struct page *page),

	TP_ARGS(page)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext4_invalidatepage_op,
	TP_PROTO(struct page *page, unsigned int offset, unsigned int length),

	TP_ARGS(page, offset, length),

	TP_FIELDS(
		ctf_integer(dev_t, dev, page->mapping->host->i_sb->s_dev)
		ctf_integer(ino_t, ino, page->mapping->host->i_ino)
		ctf_integer(pgoff_t, index, page->index)
		ctf_integer(unsigned int, offset, offset)
		ctf_integer(unsigned int, length, length)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4_invalidatepage_op, ext4_invalidatepage,
	TP_PROTO(struct page *page, unsigned int offset, unsigned int length),

	TP_ARGS(page, offset, length)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4_invalidatepage_op, ext4_journalled_invalidatepage,
	TP_PROTO(struct page *page, unsigned int offset, unsigned int length),

	TP_ARGS(page, offset, length)
)

LTTNG_TRACEPOINT_EVENT(ext4_discard_blocks,
	TP_PROTO(struct super_block *sb, unsigned long long blk,
			unsigned long long count),

	TP_ARGS(sb, blk, count),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer(__u64, blk, blk)
		ctf_integer(__u64, count, count)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext4__mb_new_pa,
	TP_PROTO(struct ext4_allocation_context *ac,
		 struct ext4_prealloc_space *pa),

	TP_ARGS(ac, pa),

	TP_FIELDS(
		ctf_integer(dev_t, dev, ac->ac_sb->s_dev)
		ctf_integer(ino_t, ino, ac->ac_inode->i_ino)
		ctf_integer(__u64, pa_pstart, pa->pa_pstart)
		ctf_integer(__u64, pa_lstart, pa->pa_lstart)
		ctf_integer(__u32, pa_len, pa->pa_len)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__mb_new_pa, ext4_mb_new_inode_pa,

	TP_PROTO(struct ext4_allocation_context *ac,
		 struct ext4_prealloc_space *pa),

	TP_ARGS(ac, pa)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__mb_new_pa, ext4_mb_new_group_pa,

	TP_PROTO(struct ext4_allocation_context *ac,
		 struct ext4_prealloc_space *pa),

	TP_ARGS(ac, pa)
)

LTTNG_TRACEPOINT_EVENT(ext4_mb_release_inode_pa,
	TP_PROTO(
		 struct ext4_prealloc_space *pa,
		 unsigned long long block, unsigned int count),

	TP_ARGS(pa, block, count),

	TP_FIELDS(
		ctf_integer(dev_t, dev, pa->pa_inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, pa->pa_inode->i_ino)
		ctf_integer(__u64, block, block)
		ctf_integer(__u32, count, count)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_mb_release_group_pa,

	TP_PROTO(struct super_block *sb, struct ext4_prealloc_space *pa),

	TP_ARGS(sb, pa),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer(__u64, pa_pstart, pa->pa_pstart)
		ctf_integer(__u32, pa_len, pa->pa_len)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_discard_preallocations,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_mb_discard_preallocations,
	TP_PROTO(struct super_block *sb, int needed),

	TP_ARGS(sb, needed),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer(int, needed, needed)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_request_blocks,
	TP_PROTO(struct ext4_allocation_request *ar),

	TP_ARGS(ar),

	TP_FIELDS(
		ctf_integer(dev_t, dev, ar->inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, ar->inode->i_ino)
		ctf_integer(unsigned int, len, ar->len)
		ctf_integer(__u32, logical, ar->logical)
		ctf_integer(__u32, lleft, ar->lleft)
		ctf_integer(__u32, lright, ar->lright)
		ctf_integer(__u64, goal, ar->goal)
		ctf_integer(__u64, pleft, ar->pleft)
		ctf_integer(__u64, pright, ar->pright)
		ctf_integer(unsigned int, flags, ar->flags)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_allocate_blocks,
	TP_PROTO(struct ext4_allocation_request *ar, unsigned long long block),

	TP_ARGS(ar, block),

	TP_FIELDS(
		ctf_integer(dev_t, dev, ar->inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, ar->inode->i_ino)
		ctf_integer(__u64, block, block)
		ctf_integer(unsigned int, len, ar->len)
		ctf_integer(__u32, logical, ar->logical)
		ctf_integer(__u32, lleft, ar->lleft)
		ctf_integer(__u32, lright, ar->lright)
		ctf_integer(__u64, goal, ar->goal)
		ctf_integer(__u64, pleft, ar->pleft)
		ctf_integer(__u64, pright, ar->pright)
		ctf_integer(unsigned int, flags, ar->flags)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_free_blocks,
	TP_PROTO(struct inode *inode, __u64 block, unsigned long count,
		 int flags),

	TP_ARGS(inode, block, count, flags),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(__u64, block, block)
		ctf_integer(unsigned long, count, count)
		ctf_integer(int, flags, flags)
		ctf_integer(TP_MODE_T, mode, inode->i_mode)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_sync_file_enter,
	TP_PROTO(struct file *file, int datasync),

	TP_ARGS(file, datasync),

	TP_FIELDS(
		ctf_integer(dev_t, dev, file->f_path.dentry->d_inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, file->f_path.dentry->d_inode->i_ino)
		ctf_integer(ino_t, parent, file->f_path.dentry->d_parent->d_inode->i_ino)
		ctf_integer(int, datasync, datasync)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_sync_file_exit,
	TP_PROTO(struct inode *inode, int ret),

	TP_ARGS(inode, ret),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(int, ret, ret)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_sync_fs,
	TP_PROTO(struct super_block *sb, int wait),

	TP_ARGS(sb, wait),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer(int, wait, wait)

	)
)

LTTNG_TRACEPOINT_EVENT(ext4_alloc_da_blocks,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(unsigned int, data_blocks, EXT4_I(inode)->i_reserved_data_blocks)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_mballoc_alloc,
	TP_PROTO(struct ext4_allocation_context *ac),

	TP_ARGS(ac),

	TP_FIELDS(
		ctf_integer(dev_t, dev, ac->ac_inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, ac->ac_inode->i_ino)
		ctf_integer(__u32, orig_logical, ac->ac_o_ex.fe_logical)
		ctf_integer(int, orig_start, ac->ac_o_ex.fe_start)
		ctf_integer(__u32, orig_group, ac->ac_o_ex.fe_group)
		ctf_integer(int, orig_len, ac->ac_o_ex.fe_len)
		ctf_integer(__u32, goal_logical, ac->ac_g_ex.fe_logical)
		ctf_integer(int, goal_start, ac->ac_g_ex.fe_start)
		ctf_integer(__u32, goal_group, ac->ac_g_ex.fe_group)
		ctf_integer(int, goal_len, ac->ac_g_ex.fe_len)
		ctf_integer(__u32, result_logical, ac->ac_f_ex.fe_logical)
		ctf_integer(int, result_start, ac->ac_f_ex.fe_start)
		ctf_integer(__u32, result_group, ac->ac_f_ex.fe_group)
		ctf_integer(int, result_len, ac->ac_f_ex.fe_len)
		ctf_integer(__u16, found, ac->ac_found)
		ctf_integer(__u16, groups, ac->ac_groups_scanned)
		ctf_integer(__u16, buddy, ac->ac_buddy)
		ctf_integer(__u16, flags, ac->ac_flags)
		ctf_integer(__u16, tail, ac->ac_tail)
		ctf_integer(__u8, cr, ac->ac_criteria)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_mballoc_prealloc,
	TP_PROTO(struct ext4_allocation_context *ac),

	TP_ARGS(ac),

	TP_FIELDS(
		ctf_integer(dev_t, dev, ac->ac_inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, ac->ac_inode->i_ino)
		ctf_integer(__u32, orig_logical, ac->ac_o_ex.fe_logical)
		ctf_integer(int, orig_start, ac->ac_o_ex.fe_start)
		ctf_integer(__u32, orig_group, ac->ac_o_ex.fe_group)
		ctf_integer(int, orig_len, ac->ac_o_ex.fe_len)
		ctf_integer(__u32, result_logical, ac->ac_b_ex.fe_logical)
		ctf_integer(int, result_start, ac->ac_b_ex.fe_start)
		ctf_integer(__u32, result_group, ac->ac_b_ex.fe_group)
		ctf_integer(int, result_len, ac->ac_b_ex.fe_len)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext4__mballoc,
	TP_PROTO(struct super_block *sb,
		 struct inode *inode,
		 ext4_group_t group,
		 ext4_grpblk_t start,
		 ext4_grpblk_t len),

	TP_ARGS(sb, inode, group, start, len),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer(ino_t, ino, inode ? inode->i_ino : 0)
		ctf_integer(int, result_start, start)
		ctf_integer(__u32, result_group, group)
		ctf_integer(int, result_len, len)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__mballoc, ext4_mballoc_discard,

	TP_PROTO(struct super_block *sb,
		 struct inode *inode,
		 ext4_group_t group,
		 ext4_grpblk_t start,
		 ext4_grpblk_t len),

	TP_ARGS(sb, inode, group, start, len)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__mballoc, ext4_mballoc_free,

	TP_PROTO(struct super_block *sb,
		 struct inode *inode,
		 ext4_group_t group,
		 ext4_grpblk_t start,
		 ext4_grpblk_t len),

	TP_ARGS(sb, inode, group, start, len)
)

LTTNG_TRACEPOINT_EVENT(ext4_forget,
	TP_PROTO(struct inode *inode, int is_metadata, __u64 block),

	TP_ARGS(inode, is_metadata, block),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(__u64, block, block)
		ctf_integer(int, is_metadata, is_metadata)
		ctf_integer(TP_MODE_T, mode, inode->i_mode)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_da_update_reserve_space,
	TP_PROTO(struct inode *inode, int used_blocks, int quota_claim),

	TP_ARGS(inode, used_blocks, quota_claim),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(__u64, i_blocks, inode->i_blocks)
		ctf_integer(int, used_blocks, used_blocks)
		ctf_integer(int, reserved_data_blocks,
				EXT4_I(inode)->i_reserved_data_blocks)
		ctf_integer(int, quota_claim, quota_claim)
		ctf_integer(TP_MODE_T, mode, inode->i_mode)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_da_reserve_space,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(__u64, i_blocks, inode->i_blocks)
		ctf_integer(int, reserved_data_blocks,
				EXT4_I(inode)->i_reserved_data_blocks)
		ctf_integer(TP_MODE_T, mode, inode->i_mode)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_da_release_space,
	TP_PROTO(struct inode *inode, int freed_blocks),

	TP_ARGS(inode, freed_blocks),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(__u64, i_blocks, inode->i_blocks)
		ctf_integer(int, freed_blocks, freed_blocks)
		ctf_integer(int, reserved_data_blocks,
				EXT4_I(inode)->i_reserved_data_blocks)
		ctf_integer(TP_MODE_T, mode, inode->i_mode)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext4__bitmap_load,
	TP_PROTO(struct super_block *sb, unsigned long group),

	TP_ARGS(sb, group),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer(__u32, group, group)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__bitmap_load, ext4_mb_bitmap_load,

	TP_PROTO(struct super_block *sb, unsigned long group),

	TP_ARGS(sb, group)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__bitmap_load, ext4_mb_buddy_bitmap_load,

	TP_PROTO(struct super_block *sb, unsigned long group),

	TP_ARGS(sb, group)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__bitmap_load, ext4_read_block_bitmap_load,

	TP_PROTO(struct super_block *sb, unsigned long group),

	TP_ARGS(sb, group)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__bitmap_load, ext4_load_inode_bitmap,

	TP_PROTO(struct super_block *sb, unsigned long group),

	TP_ARGS(sb, group)
)

LTTNG_TRACEPOINT_EVENT(ext4_direct_IO_enter,
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

LTTNG_TRACEPOINT_EVENT(ext4_direct_IO_exit,
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

LTTNG_TRACEPOINT_EVENT(ext4_fallocate_exit,
	TP_PROTO(struct inode *inode, loff_t offset,
		 unsigned int max_blocks, int ret),

	TP_ARGS(inode, offset, max_blocks, ret),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(loff_t, pos, offset)
		ctf_integer(unsigned int, blocks, max_blocks)
		ctf_integer(int, ret, ret)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext4__fallocate_mode,
	TP_PROTO(struct inode *inode, loff_t offset, loff_t len, int mode),

	TP_ARGS(inode, offset, len, mode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(loff_t, pos, offset)
		ctf_integer(loff_t, len, len)
		ctf_integer(int, mode, mode)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__fallocate_mode, ext4_fallocate_enter,

	TP_PROTO(struct inode *inode, loff_t offset, loff_t len, int mode),

	TP_ARGS(inode, offset, len, mode)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__fallocate_mode, ext4_punch_hole,

	TP_PROTO(struct inode *inode, loff_t offset, loff_t len, int mode),

	TP_ARGS(inode, offset, len, mode)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__fallocate_mode, ext4_zero_range,

	TP_PROTO(struct inode *inode, loff_t offset, loff_t len, int mode),

	TP_ARGS(inode, offset, len, mode)
)

LTTNG_TRACEPOINT_EVENT(ext4_unlink_enter,
	TP_PROTO(struct inode *parent, struct dentry *dentry),

	TP_ARGS(parent, dentry),

	TP_FIELDS(
		ctf_integer(dev_t, dev, dentry->d_inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, dentry->d_inode->i_ino)
		ctf_integer(ino_t, parent, parent->i_ino)
		ctf_integer(loff_t, size, dentry->d_inode->i_size)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_unlink_exit,
	TP_PROTO(struct dentry *dentry, int ret),

	TP_ARGS(dentry, ret),

	TP_FIELDS(
		ctf_integer(dev_t, dev, dentry->d_inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, dentry->d_inode->i_ino)
		ctf_integer(int, ret, ret)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext4__truncate,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(unsigned int, blocks, inode->i_blocks)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__truncate, ext4_truncate_enter,

	TP_PROTO(struct inode *inode),

	TP_ARGS(inode)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__truncate, ext4_truncate_exit,

	TP_PROTO(struct inode *inode),

	TP_ARGS(inode)
)

/* 'ux' is the uninitialized extent. */
LTTNG_TRACEPOINT_EVENT(ext4_ext_convert_to_initialized_enter,
	TP_PROTO(struct inode *inode, struct ext4_map_blocks *map,
		 struct ext4_extent *ux),

	TP_ARGS(inode, map, ux),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_lblk_t, m_lblk, map->m_lblk)
		ctf_integer(unsigned, m_len, map->m_len)
		ctf_integer(ext4_lblk_t, u_lblk, le32_to_cpu(ux->ee_block))
		ctf_integer(unsigned, u_len, ext4_ext_get_actual_len(ux))
		ctf_integer(ext4_fsblk_t, u_pblk, ext4_ext_pblock(ux))
	)
)

/*
 * 'ux' is the uninitialized extent.
 * 'ix' is the initialized extent to which blocks are transferred.
 */
LTTNG_TRACEPOINT_EVENT(ext4_ext_convert_to_initialized_fastpath,
	TP_PROTO(struct inode *inode, struct ext4_map_blocks *map,
		 struct ext4_extent *ux, struct ext4_extent *ix),

	TP_ARGS(inode, map, ux, ix),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_lblk_t, m_lblk, map->m_lblk)
		ctf_integer(unsigned, m_len, map->m_len)
		ctf_integer(ext4_lblk_t, u_lblk, le32_to_cpu(ux->ee_block))
		ctf_integer(unsigned, u_len, ext4_ext_get_actual_len(ux))
		ctf_integer(ext4_fsblk_t, u_pblk, ext4_ext_pblock(ux))
		ctf_integer(ext4_lblk_t, i_lblk, le32_to_cpu(ix->ee_block))
		ctf_integer(unsigned, i_len, ext4_ext_get_actual_len(ix))
		ctf_integer(ext4_fsblk_t, i_pblk, ext4_ext_pblock(ix))
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext4__map_blocks_enter,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk,
		 unsigned int len, unsigned int flags),

	TP_ARGS(inode, lblk, len, flags),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_lblk_t, lblk, lblk)
		ctf_integer(unsigned int, len, len)
		ctf_integer(unsigned int, flags, flags)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__map_blocks_enter, ext4_ext_map_blocks_enter,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk,
		 unsigned len, unsigned flags),

	TP_ARGS(inode, lblk, len, flags)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__map_blocks_enter, ext4_ind_map_blocks_enter,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk,
		 unsigned len, unsigned flags),

	TP_ARGS(inode, lblk, len, flags)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext4__map_blocks_exit,
	TP_PROTO(struct inode *inode, unsigned flags, struct ext4_map_blocks *map,
		 int ret),

	TP_ARGS(inode, flags, map, ret),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(unsigned int, flags, flags)
		ctf_integer(ext4_fsblk_t, pblk, map->m_pblk)
		ctf_integer(ext4_lblk_t, lblk, map->m_lblk)
		ctf_integer(unsigned int, len, map->m_len)
		ctf_integer(unsigned int, mflags, map->m_flags)
		ctf_integer(int, ret, ret)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__map_blocks_exit, ext4_ext_map_blocks_exit,
	TP_PROTO(struct inode *inode, unsigned flags,
		 struct ext4_map_blocks *map, int ret),

	TP_ARGS(inode, flags, map, ret)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__map_blocks_exit, ext4_ind_map_blocks_exit,
	TP_PROTO(struct inode *inode, unsigned flags,
		 struct ext4_map_blocks *map, int ret),

	TP_ARGS(inode, flags, map, ret)
)

LTTNG_TRACEPOINT_EVENT(ext4_ext_load_extent,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk, ext4_fsblk_t pblk),

	TP_ARGS(inode, lblk, pblk),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_fsblk_t, pblk, pblk)
		ctf_integer(ext4_lblk_t, lblk, lblk)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_load_inode,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_journal_start,
	TP_PROTO(struct super_block *sb, int blocks, int rsv_blocks,
		 int revoke_creds, unsigned long IP),

	TP_ARGS(sb, blocks, rsv_blocks, revoke_creds, IP),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer_hex(unsigned long, ip, IP)
		ctf_integer(int, blocks, blocks)
		ctf_integer(int, rsv_blocks, rsv_blocks)
		ctf_integer(int, revoke_creds, revoke_creds)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_journal_start_reserved,
	TP_PROTO(struct super_block *sb, int blocks, unsigned long IP),

	TP_ARGS(sb, blocks, IP),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer_hex(unsigned long, ip, IP)
		ctf_integer(int, blocks, blocks)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_ext_handle_uninitialized_extents,
	TP_PROTO(struct inode *inode, struct ext4_map_blocks *map, int flags,
		 unsigned int allocated, ext4_fsblk_t newblock),

	TP_ARGS(inode, map, flags, allocated, newblock),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(int, flags, flags)
		ctf_integer(ext4_lblk_t, lblk, map->m_lblk)
		ctf_integer(ext4_fsblk_t, pblk, map->m_pblk)
		ctf_integer(unsigned int, len, map->m_len)
		ctf_integer(unsigned int, allocated, allocated)
		ctf_integer(ext4_fsblk_t, newblk, newblock)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_get_implied_cluster_alloc_exit,
	TP_PROTO(struct super_block *sb, struct ext4_map_blocks *map, int ret),

	TP_ARGS(sb, map, ret),

	TP_FIELDS(
		ctf_integer(dev_t, dev, sb->s_dev)
		ctf_integer(unsigned int, flags, map->m_flags)
		ctf_integer(ext4_lblk_t, lblk, map->m_lblk)
		ctf_integer(ext4_fsblk_t, pblk, map->m_pblk)
		ctf_integer(unsigned int, len, map->m_len)
		ctf_integer(int, ret, ret)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_ext_put_in_cache,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk, unsigned int len,
		 ext4_fsblk_t start),

	TP_ARGS(inode, lblk, len, start),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_lblk_t, lblk, lblk)
		ctf_integer(unsigned int, len, len)
		ctf_integer(ext4_fsblk_t, start, start)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_ext_in_cache,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk, int ret),

	TP_ARGS(inode, lblk, ret),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_lblk_t, lblk, lblk)
		ctf_integer(int, ret, ret)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_find_delalloc_range,
	TP_PROTO(struct inode *inode, ext4_lblk_t from, ext4_lblk_t to,
		int reverse, int found, ext4_lblk_t found_blk),

	TP_ARGS(inode, from, to, reverse, found, found_blk),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_lblk_t, from, from)
		ctf_integer(ext4_lblk_t, to, to)
		ctf_integer(int, reverse, reverse)
		ctf_integer(int, found, found)
		ctf_integer(ext4_lblk_t, found_blk, found_blk)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_get_reserved_cluster_alloc,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk, unsigned int len),

	TP_ARGS(inode, lblk, len),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_lblk_t, lblk, lblk)
		ctf_integer(unsigned int, len, len)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_ext_show_extent,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk, ext4_fsblk_t pblk,
		 unsigned short len),

	TP_ARGS(inode, lblk, pblk, len),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_fsblk_t, pblk, pblk)
		ctf_integer(ext4_lblk_t, lblk, lblk)
		ctf_integer(unsigned short, len, len)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_remove_blocks,
	TP_PROTO(struct inode *inode, struct ext4_extent *ex,
		 ext4_lblk_t from, ext4_fsblk_t to,
		 struct partial_cluster *pc),

	TP_ARGS(inode, ex, from, to, pc),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_lblk_t, from, from)
		ctf_integer(ext4_lblk_t, to, to)
		ctf_integer(ext4_fsblk_t, ee_pblk, ext4_ext_pblock(ex))
		ctf_integer(ext4_lblk_t, ee_lblk, le32_to_cpu(ex->ee_block))
		ctf_integer(unsigned short, ee_len, ext4_ext_get_actual_len(ex))
		ctf_integer(ext4_fsblk_t, pc_pclu, pc->pclu)
		ctf_integer(ext4_lblk_t, pc_lblk, pc->lblk)
		ctf_integer(int, pc_state, pc->state)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_ext_rm_leaf,
	TP_PROTO(struct inode *inode, ext4_lblk_t start,
		 struct ext4_extent *ex,
		 struct partial_cluster *pc),

	TP_ARGS(inode, start, ex, pc),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_lblk_t, start, start)
		ctf_integer(ext4_lblk_t, ee_lblk, le32_to_cpu(ex->ee_block))
		ctf_integer(ext4_fsblk_t, ee_pblk, ext4_ext_pblock(ex))
		ctf_integer(short, ee_len, ext4_ext_get_actual_len(ex))
		ctf_integer(ext4_fsblk_t, pc_pclu, pc->pclu)
		ctf_integer(ext4_lblk_t, pc_lblk, pc->lblk)
		ctf_integer(int, pc_state, pc->state)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_ext_rm_idx,
	TP_PROTO(struct inode *inode, ext4_fsblk_t pblk),

	TP_ARGS(inode, pblk),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_fsblk_t, pblk, pblk)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_ext_remove_space,
	TP_PROTO(struct inode *inode, ext4_lblk_t start,
		 ext4_lblk_t end, int depth),

	TP_ARGS(inode, start, end, depth),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_lblk_t, start, start)
		ctf_integer(ext4_lblk_t, end, end)
		ctf_integer(int, depth, depth)
	)
)

LTTNG_TRACEPOINT_EVENT(ext4_ext_remove_space_done,
	TP_PROTO(struct inode *inode, ext4_lblk_t start, ext4_lblk_t end,
		 int depth, struct partial_cluster *pc, __le16 eh_entries),

	TP_ARGS(inode, start, end, depth, pc, eh_entries),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_lblk_t, start, start)
		ctf_integer(ext4_lblk_t, end, end)
		ctf_integer(int, depth, depth)
		ctf_integer(unsigned short, eh_entries, le16_to_cpu(eh_entries))
		ctf_integer(ext4_fsblk_t, pc_pclu, pc->pclu)
		ctf_integer(ext4_lblk_t, pc_lblk, pc->lblk)
		ctf_integer(int, pc_state, pc->state)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(ext4__es_extent,
	TP_PROTO(struct inode *inode, struct extent_status *es),

	TP_ARGS(inode, es),

	TP_FIELDS(
		ctf_integer(dev_t, dev, inode->i_sb->s_dev)
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(ext4_lblk_t, lblk, es->es_lblk)
		ctf_integer(ext4_lblk_t, len, es->es_len)
		ctf_integer(ext4_fsblk_t, pblk, ext4_es_pblock(es))
		ctf_integer(char, status, ext4_es_status(es))
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__es_extent, ext4_es_insert_extent,
	TP_PROTO(struct inode *inode, struct extent_status *es),

	TP_ARGS(inode, es)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(ext4__es_extent, ext4_es_cache_extent,
	TP_PROTO(struct inode *inode, struct extent_status *es),

	TP_ARGS(inode, es)
)

#endif /* LTTNG_TRACE_EXT4_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
