/* SPDX-License-Identifier: GPL-2.0-only */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM btrfs

#if !defined(LTTNG_TRACE_BTRFS_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_BTRFS_H

#include <lttng/lttng-tracepoint-event.h>
#include <linux/writeback.h>

#ifndef _TRACE_BTRFS_DEF_
#define _TRACE_BTRFS_DEF_
struct btrfs_root;
struct btrfs_fs_info;
struct btrfs_inode;
struct extent_map;
struct btrfs_ordered_extent;
struct btrfs_delayed_ref_node;
struct btrfs_delayed_tree_ref;
struct btrfs_delayed_data_ref;
struct btrfs_delayed_ref_head;
struct btrfs_block_group;
struct btrfs_free_cluster;
struct map_lookup;
struct extent_buffer;
struct extent_state;
#endif

#define BTRFS_UUID_SIZE 16

#define lttng_fs_info_fsid fs_info->fs_devices->fsid

LTTNG_TRACEPOINT_EVENT(btrfs_transaction_commit,

	TP_PROTO(const struct btrfs_root *root),

	TP_ARGS(root),

	TP_FIELDS(
		ctf_integer(u64, generation, root->fs_info->generation)
		ctf_integer(u64, root_objectid, root->root_key.objectid)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs__inode,

	TP_PROTO(const struct inode *inode),

	TP_ARGS(inode),

	TP_FIELDS(
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(blkcnt_t, blocks, inode->i_blocks)
		ctf_integer(u64, disk_i_size, BTRFS_I(inode)->disk_i_size)
		ctf_integer(u64, generation, BTRFS_I(inode)->generation)
		ctf_integer(u64, last_trans, BTRFS_I(inode)->last_trans)
		ctf_integer(u64, logged_trans, BTRFS_I(inode)->logged_trans)
		ctf_integer(u64, root_objectid,
				BTRFS_I(inode)->root->root_key.objectid)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__inode, btrfs_inode_new,

	TP_PROTO(const struct inode *inode),

	TP_ARGS(inode)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__inode, btrfs_inode_request,

	TP_PROTO(const struct inode *inode),

	TP_ARGS(inode)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__inode, btrfs_inode_evict,

	TP_PROTO(const struct inode *inode),

	TP_ARGS(inode)
)

LTTNG_TRACEPOINT_EVENT(btrfs_get_extent,

	TP_PROTO(const struct btrfs_root *root, const struct btrfs_inode *inode,
		const struct extent_map *map),

	TP_ARGS(root, inode, map),

	TP_FIELDS(
		ctf_integer(u64, root_objectid, root->root_key.objectid)
		ctf_integer(u64, ino, btrfs_ino(inode))
		ctf_integer(u64, start, map->start)
		ctf_integer(u64, len, map->len)
		ctf_integer(u64, orig_start, map->orig_start)
		ctf_integer(u64, block_start, map->block_start)
		ctf_integer(u64, block_len, map->block_len)
		ctf_integer(unsigned long, flags, map->flags)
		ctf_integer(int, refs, refcount_read(&map->refs))
		ctf_integer(unsigned int, compress_type, map->compress_type)
	)
)

LTTNG_TRACEPOINT_EVENT(btrfs_handle_em_exist,

	TP_PROTO(const struct btrfs_fs_info *fs_info,
		const struct extent_map *existing, const struct extent_map *map,
		u64 start, u64 len),

	TP_ARGS(fs_info, existing, map, start, len),

	TP_FIELDS(
		ctf_array(u8, fsid, lttng_fs_info_fsid, BTRFS_UUID_SIZE)
		ctf_integer(u64, e_start, existing->start)
		ctf_integer(u64, e_len, existing->len)
		ctf_integer(u64, map_start, map->start)
		ctf_integer(u64, map_len, map->len)
		ctf_integer(u64, start, start)
		ctf_integer(u64, len, len)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs__ordered_extent,

	TP_PROTO(const struct inode *inode,
		 const struct btrfs_ordered_extent *ordered),

	TP_ARGS(inode, ordered),

	TP_FIELDS(
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(u64, file_offset, ordered->file_offset)
		ctf_integer(u64, start, ordered->disk_bytenr)
		ctf_integer(u64, len, ordered->num_bytes)
		ctf_integer(u64, disk_len, ordered->disk_num_bytes)
		ctf_integer(u64, bytes_left, ordered->bytes_left)
		ctf_integer(unsigned long, flags, ordered->flags)
		ctf_integer(int, compress_type, ordered->compress_type)
		ctf_integer(int, refs, refcount_read(&ordered->refs))
		ctf_integer(u64, root_objectid,
				BTRFS_I(inode)->root->root_key.objectid)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__ordered_extent, btrfs_ordered_extent_add,

	TP_PROTO(const struct inode *inode,
		 const struct btrfs_ordered_extent *ordered),

	TP_ARGS(inode, ordered)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__ordered_extent, btrfs_ordered_extent_remove,

	TP_PROTO(const struct inode *inode,
		 const struct btrfs_ordered_extent *ordered),

	TP_ARGS(inode, ordered)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__ordered_extent, btrfs_ordered_extent_start,

	TP_PROTO(const struct inode *inode,
		 const struct btrfs_ordered_extent *ordered),

	TP_ARGS(inode, ordered)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__ordered_extent, btrfs_ordered_extent_put,

	TP_PROTO(const struct inode *inode,
		 const struct btrfs_ordered_extent *ordered),

	TP_ARGS(inode, ordered)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs__writepage,

	TP_PROTO(const struct page *page, const struct inode *inode,
		 const struct writeback_control *wbc),

	TP_ARGS(page, inode, wbc),

	TP_FIELDS(
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(pgoff_t, index, page->index)
		ctf_integer(long, nr_to_write, wbc->nr_to_write)
		ctf_integer(long, pages_skipped, wbc->pages_skipped)
		ctf_integer(loff_t, range_start, wbc->range_start)
		ctf_integer(loff_t, range_end, wbc->range_end)
		ctf_integer(char, for_kupdate, wbc->for_kupdate)
		ctf_integer(char, for_reclaim, wbc->for_reclaim)
		ctf_integer(char, range_cyclic, wbc->range_cyclic)
		ctf_integer(pgoff_t, writeback_index,
				inode->i_mapping->writeback_index)
		ctf_integer(u64, root_objectid,
				BTRFS_I(inode)->root->root_key.objectid)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(btrfs__writepage,

	__extent_writepage,

	btrfs__extent_writepage,

	TP_PROTO(const struct page *page, const struct inode *inode,
		 const struct writeback_control *wbc),

	TP_ARGS(page, inode, wbc)
)

LTTNG_TRACEPOINT_EVENT(btrfs_writepage_end_io_hook,

	TP_PROTO(const struct page *page, u64 start, u64 end, int uptodate),

	TP_ARGS(page, start, end, uptodate),

	TP_FIELDS(
		ctf_integer(ino_t, ino, page->mapping->host->i_ino)
		ctf_integer(pgoff_t, index, page->index)
		ctf_integer(u64, start, start)
		ctf_integer(u64, end, end)
		ctf_integer(int, uptodate, uptodate)
		ctf_integer(u64, root_objectid,
			BTRFS_I(page->mapping->host)->root->root_key.objectid)
	)
)

LTTNG_TRACEPOINT_EVENT(btrfs_sync_file,

	TP_PROTO(const struct file *file, int datasync),

	TP_ARGS(file, datasync),

	TP_FIELDS(
		ctf_integer(ino_t, ino, file->f_path.dentry->d_inode->i_ino)
		ctf_integer(ino_t, parent, file->f_path.dentry->d_parent->d_inode->i_ino)
		ctf_integer(int, datasync, datasync)
		ctf_integer(u64, root_objectid,
			BTRFS_I(file->f_path.dentry->d_inode)->root->root_key.objectid)
	)
)

LTTNG_TRACEPOINT_EVENT(btrfs_sync_fs,

	TP_PROTO(const struct btrfs_fs_info *fs_info, int wait),

	TP_ARGS(fs_info, wait),

	TP_FIELDS(
		ctf_integer(int, wait, wait)
	)
)

LTTNG_TRACEPOINT_EVENT(btrfs_add_block_group,

	TP_PROTO(const struct btrfs_fs_info *fs_info,
		 const struct btrfs_block_group *block_group, int create),

	TP_ARGS(fs_info, block_group, create),

	TP_FIELDS(
		ctf_array(u8, fsid, lttng_fs_info_fsid, BTRFS_UUID_SIZE)
		ctf_integer(u64, offset, block_group->start)
		ctf_integer(u64, size, block_group->length)
		ctf_integer(u64, flags, block_group->flags)
		ctf_integer(u64, bytes_used, block_group->used)
		ctf_integer(u64, bytes_super, block_group->bytes_super)
		ctf_integer(int, create, create)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs_delayed_tree_ref,

	TP_PROTO(const struct btrfs_fs_info *fs_info,
		 const struct btrfs_delayed_ref_node *ref,
		 const struct btrfs_delayed_tree_ref *full_ref,
		 int action),

	TP_ARGS(fs_info, ref, full_ref, action),

	TP_FIELDS(
		ctf_array(u8, fsid, lttng_fs_info_fsid, BTRFS_UUID_SIZE)
		ctf_integer(u64, bytenr, ref->bytenr)
		ctf_integer(u64, num_bytes, ref->num_bytes)
		ctf_integer(int, action, action)
		ctf_integer(u64, parent, full_ref->parent)
		ctf_integer(u64, ref_root, full_ref->root)
		ctf_integer(int, level, full_ref->level)
		ctf_integer(int, type, ref->type)
		ctf_integer(u64, seq, ref->seq)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(btrfs_delayed_tree_ref,

	add_delayed_tree_ref,

	btrfs_add_delayed_tree_ref,

	TP_PROTO(const struct btrfs_fs_info *fs_info,
		 const struct btrfs_delayed_ref_node *ref,
		 const struct btrfs_delayed_tree_ref *full_ref,
		 int action),

	TP_ARGS(fs_info, ref, full_ref, action)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(btrfs_delayed_tree_ref,

	run_delayed_tree_ref,

	btrfs_run_delayed_tree_ref,

	TP_PROTO(const struct btrfs_fs_info *fs_info,
		 const struct btrfs_delayed_ref_node *ref,
		 const struct btrfs_delayed_tree_ref *full_ref,
		 int action),

	TP_ARGS(fs_info, ref, full_ref, action)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs_delayed_data_ref,

	TP_PROTO(const struct btrfs_fs_info *fs_info,
		 const struct btrfs_delayed_ref_node *ref,
		 const struct btrfs_delayed_data_ref *full_ref,
		 int action),

	TP_ARGS(fs_info, ref, full_ref, action),

	TP_FIELDS(
		ctf_array(u8, fsid, lttng_fs_info_fsid, BTRFS_UUID_SIZE)
		ctf_integer(u64, bytenr, ref->bytenr)
		ctf_integer(u64, num_bytes, ref->num_bytes)
		ctf_integer(int, action, action)
		ctf_integer(u64, parent, full_ref->parent)
		ctf_integer(u64, ref_root, full_ref->root)
		ctf_integer(u64, owner, full_ref->objectid)
		ctf_integer(u64, offset, full_ref->offset)
		ctf_integer(int, type, ref->type)
		ctf_integer(u64, seq, ref->seq)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(btrfs_delayed_data_ref,

	add_delayed_data_ref,

	btrfs_add_delayed_data_ref,

	TP_PROTO(const struct btrfs_fs_info *fs_info,
		 const struct btrfs_delayed_ref_node *ref,
		 const struct btrfs_delayed_data_ref *full_ref,
		 int action),

	TP_ARGS(fs_info, ref, full_ref, action)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(btrfs_delayed_data_ref,

	run_delayed_data_ref,

	btrfs_run_delayed_data_ref,

	TP_PROTO(const struct btrfs_fs_info *fs_info,
		 const struct btrfs_delayed_ref_node *ref,
		 const struct btrfs_delayed_data_ref *full_ref,
		 int action),

	TP_ARGS(fs_info, ref, full_ref, action)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs_delayed_ref_head,

	TP_PROTO(const struct btrfs_fs_info *fs_info,
		 const struct btrfs_delayed_ref_head *head_ref,
		 int action),

	TP_ARGS(fs_info, head_ref, action),

	TP_FIELDS(
		ctf_integer(u64, bytenr, head_ref->bytenr)
		ctf_integer(u64, num_bytes, head_ref->num_bytes)
		ctf_integer(int, action, action)
		ctf_integer(int, is_data, head_ref->is_data)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(btrfs_delayed_ref_head,

	add_delayed_ref_head,

	btrfs_add_delayed_ref_head,

	TP_PROTO(const struct btrfs_fs_info *fs_info,
		 const struct btrfs_delayed_ref_head *head_ref,
		 int action),

	TP_ARGS(fs_info, head_ref, action)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE_MAP(btrfs_delayed_ref_head,

	run_delayed_ref_head,

	btrfs_run_delayed_ref_head,

	TP_PROTO(const struct btrfs_fs_info *fs_info,
		 const struct btrfs_delayed_ref_head *head_ref,
		 int action),

	TP_ARGS(fs_info, head_ref, action)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs__chunk,

	TP_PROTO(const struct btrfs_fs_info *fs_info, const struct map_lookup *map,
		 u64 offset, u64 size),

	TP_ARGS(fs_info, map, offset, size),

	TP_FIELDS(
		ctf_integer(int, num_stripes, map->num_stripes)
		ctf_integer(u64, type, map->type)
		ctf_integer(int, sub_stripes, map->sub_stripes)
		ctf_integer(u64, offset, offset)
		ctf_integer(u64, size, size)
		ctf_integer(u64, root_objectid, fs_info->chunk_root->root_key.objectid)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__chunk,  btrfs_chunk_alloc,

	TP_PROTO(const struct btrfs_fs_info *fs_info, const struct map_lookup *map,
		 u64 offset, u64 size),

	TP_ARGS(fs_info, map, offset, size)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__chunk,  btrfs_chunk_free,

	TP_PROTO(const struct btrfs_fs_info *fs_info, const struct map_lookup *map,
		 u64 offset, u64 size),

	TP_ARGS(fs_info, map, offset, size)
)

LTTNG_TRACEPOINT_EVENT(btrfs_cow_block,

	TP_PROTO(const struct btrfs_root *root, const struct extent_buffer *buf,
		 const struct extent_buffer *cow),

	TP_ARGS(root, buf, cow),

	TP_FIELDS(
		ctf_integer(u64, root_objectid, root->root_key.objectid)
		ctf_integer(u64, buf_start, buf->start)
		ctf_integer(int, refs, atomic_read(&buf->refs))
		ctf_integer(u64, cow_start, cow->start)
		ctf_integer(int, buf_level, btrfs_header_level(buf))
		ctf_integer(int, cow_level, btrfs_header_level(cow))
	)
)

LTTNG_TRACEPOINT_EVENT(btrfs_space_reservation,

	TP_PROTO(const struct btrfs_fs_info *fs_info, const char *type, u64 val,
		 u64 bytes, int reserve),

	TP_ARGS(fs_info, type, val, bytes, reserve),

	TP_FIELDS(
		ctf_array(u8, fsid, lttng_fs_info_fsid, BTRFS_UUID_SIZE)
		ctf_string(type, type)
		ctf_integer(u64, val, val)
		ctf_integer(u64, bytes, bytes)
		ctf_integer(int, reserve, reserve)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs__reserved_extent,

	TP_PROTO(const struct btrfs_fs_info *fs_info, u64 start, u64 len),

	TP_ARGS(fs_info, start, len),

	TP_FIELDS(
		ctf_integer(u64, start, start)
		ctf_integer(u64, len, len)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__reserved_extent,  btrfs_reserved_extent_alloc,

	TP_PROTO(const struct btrfs_fs_info *fs_info, u64 start, u64 len),

	TP_ARGS(fs_info, start, len)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__reserved_extent,  btrfs_reserved_extent_free,

	TP_PROTO(const struct btrfs_fs_info *fs_info, u64 start, u64 len),

	TP_ARGS(fs_info, start, len)
)

LTTNG_TRACEPOINT_EVENT_MAP(find_free_extent,

	btrfs_find_free_extent,

	TP_PROTO(const struct btrfs_fs_info *fs_info, u64 num_bytes, u64 empty_size,
		 u64 data),

	TP_ARGS(fs_info, num_bytes, empty_size, data),

	TP_FIELDS(
		ctf_array(u8, fsid, lttng_fs_info_fsid, BTRFS_UUID_SIZE)
		ctf_integer(u64, num_bytes, num_bytes)
		ctf_integer(u64, empty_size, empty_size)
		ctf_integer(u64, data, data)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs__reserve_extent,

	TP_PROTO(const struct btrfs_block_group *block_group, u64 start,
		 u64 len),

	TP_ARGS(block_group, start, len),

	TP_FIELDS(
		ctf_array(u8, fsid, block_group->lttng_fs_info_fsid, BTRFS_UUID_SIZE)
		ctf_integer(u64, bg_objectid, block_group->start)
		ctf_integer(u64, flags, block_group->flags)
		ctf_integer(u64, start, start)
		ctf_integer(u64, len, len)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__reserve_extent, btrfs_reserve_extent,

	TP_PROTO(const struct btrfs_block_group *block_group, u64 start,
		 u64 len),

	TP_ARGS(block_group, start, len)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__reserve_extent, btrfs_reserve_extent_cluster,

	TP_PROTO(const struct btrfs_block_group *block_group, u64 start,
		 u64 len),

	TP_ARGS(block_group, start, len)
)

LTTNG_TRACEPOINT_EVENT(btrfs_find_cluster,

	TP_PROTO(const struct btrfs_block_group *block_group, u64 start,
		 u64 bytes, u64 empty_size, u64 min_bytes),

	TP_ARGS(block_group, start, bytes, empty_size, min_bytes),

	TP_FIELDS(
		ctf_integer(u64, bg_objectid, block_group->start)
		ctf_integer(u64, flags, block_group->flags)
		ctf_integer(u64, start, start)
		ctf_integer(u64, bytes, bytes)
		ctf_integer(u64, empty_size, empty_size)
		ctf_integer(u64, min_bytes, min_bytes)
	)
)

LTTNG_TRACEPOINT_EVENT(btrfs_failed_cluster_setup,

	TP_PROTO(const struct btrfs_block_group *block_group),

	TP_ARGS(block_group),

	TP_FIELDS(
		ctf_integer(u64, bg_objectid, block_group->start)
	)
)

LTTNG_TRACEPOINT_EVENT(btrfs_setup_cluster,

	TP_PROTO(const struct btrfs_block_group *block_group,
		 const struct btrfs_free_cluster *cluster, u64 size, int bitmap),

	TP_ARGS(block_group, cluster, size, bitmap),

	TP_FIELDS(
		ctf_integer(u64, bg_objectid, block_group->start)
		ctf_integer(u64, flags, block_group->flags)
		ctf_integer(u64, start, cluster->window_start)
		ctf_integer(u64, max_size, cluster->max_size)
		ctf_integer(u64, size, size)
		ctf_integer(int, bitmap, bitmap)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(alloc_extent_state,

	btrfs_alloc_extent_state,

	TP_PROTO(const struct extent_state *state, gfp_t mask, unsigned long IP),

	TP_ARGS(state, mask, IP),

	TP_FIELDS(
		ctf_integer_hex(const struct extent_state *, state, state)
		ctf_integer(gfp_t, mask, mask)
		ctf_integer_hex(unsigned long, ip, IP)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(free_extent_state,

	btrfs_free_extent_state,

	TP_PROTO(const struct extent_state *state, unsigned long IP),

	TP_ARGS(state, IP),

	TP_FIELDS(
		ctf_integer_hex(const struct extent_state *, state, state)
		ctf_integer_hex(unsigned long, ip, IP)
	)
)

#endif /* LTTNG_TRACE_BTRFS_H */

/* This part must be outside protection */
#include <lttng/define_trace.h>
