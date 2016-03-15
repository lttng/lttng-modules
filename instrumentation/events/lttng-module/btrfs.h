#undef TRACE_SYSTEM
#define TRACE_SYSTEM btrfs

#if !defined(LTTNG_TRACE_BTRFS_H) || defined(TRACE_HEADER_MULTI_READ)
#define LTTNG_TRACE_BTRFS_H

#include <probes/lttng-tracepoint-event.h>
#include <linux/writeback.h>
#include <trace/events/gfpflags.h>
#include <linux/version.h>

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
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
struct btrfs_block_group_cache;
struct btrfs_free_cluster;
#endif
struct map_lookup;
struct extent_buffer;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
struct extent_state;
#endif
#endif

#define BTRFS_UUID_SIZE 16

LTTNG_TRACEPOINT_EVENT(btrfs_transaction_commit,

	TP_PROTO(struct btrfs_root *root),

	TP_ARGS(root),

	TP_FIELDS(
		ctf_integer(u64, generation, root->fs_info->generation)
		ctf_integer(u64, root_objectid, root->root_key.objectid)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs__inode,

	TP_PROTO(struct inode *inode),

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

	TP_PROTO(struct inode *inode),

	TP_ARGS(inode)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__inode, btrfs_inode_request,

	TP_PROTO(struct inode *inode),

	TP_ARGS(inode)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__inode, btrfs_inode_evict,

	TP_PROTO(struct inode *inode),

	TP_ARGS(inode)
)

LTTNG_TRACEPOINT_EVENT(btrfs_get_extent,

	TP_PROTO(struct btrfs_root *root, struct extent_map *map),

	TP_ARGS(root, map),

	TP_FIELDS(
		ctf_integer(u64, root_objectid, root->root_key.objectid)
		ctf_integer(u64, start, map->start)
		ctf_integer(u64, len, map->len)
		ctf_integer(u64, orig_start, map->orig_start)
		ctf_integer(u64, block_start, map->block_start)
		ctf_integer(u64, block_len, map->block_len)
		ctf_integer(unsigned long, flags, map->flags)
		ctf_integer(int, refs, atomic_read(&map->refs))
		ctf_integer(unsigned int, compress_type, map->compress_type)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs__ordered_extent,

	TP_PROTO(struct inode *inode, struct btrfs_ordered_extent *ordered),

	TP_ARGS(inode, ordered),

	TP_FIELDS(
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(u64, file_offset, ordered->file_offset)
		ctf_integer(u64, start, ordered->start)
		ctf_integer(u64, len, ordered->len)
		ctf_integer(u64, disk_len, ordered->disk_len)
		ctf_integer(u64, bytes_left, ordered->bytes_left)
		ctf_integer(unsigned long, flags, ordered->flags)
		ctf_integer(int, compress_type, ordered->compress_type)
		ctf_integer(int, refs, atomic_read(&ordered->refs))
		ctf_integer(u64, root_objectid,
				BTRFS_I(inode)->root->root_key.objectid)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__ordered_extent, btrfs_ordered_extent_add,

	TP_PROTO(struct inode *inode, struct btrfs_ordered_extent *ordered),

	TP_ARGS(inode, ordered)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__ordered_extent, btrfs_ordered_extent_remove,

	TP_PROTO(struct inode *inode, struct btrfs_ordered_extent *ordered),

	TP_ARGS(inode, ordered)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__ordered_extent, btrfs_ordered_extent_start,

	TP_PROTO(struct inode *inode, struct btrfs_ordered_extent *ordered),

	TP_ARGS(inode, ordered)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__ordered_extent, btrfs_ordered_extent_put,

	TP_PROTO(struct inode *inode, struct btrfs_ordered_extent *ordered),

	TP_ARGS(inode, ordered)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs__writepage,

	TP_PROTO(struct page *page, struct inode *inode,
		 struct writeback_control *wbc),

	TP_ARGS(page, inode, wbc),

	TP_FIELDS(
		ctf_integer(ino_t, ino, inode->i_ino)
		ctf_integer(pgoff_t, index, page->index)
		ctf_integer(long, nr_to_write, wbc->nr_to_write)
		ctf_integer(long, pages_skipped, wbc->pages_skipped)
		ctf_integer(loff_t, range_start, wbc->range_start)
		ctf_integer(loff_t, range_end, wbc->range_end)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,1,0))
		ctf_integer(char, nonblocking, wbc->nonblocking)
#endif
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

	TP_PROTO(struct page *page, struct inode *inode,
		 struct writeback_control *wbc),

	TP_ARGS(page, inode, wbc)
)

LTTNG_TRACEPOINT_EVENT(btrfs_writepage_end_io_hook,

	TP_PROTO(struct page *page, u64 start, u64 end, int uptodate),

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

	TP_PROTO(struct file *file, int datasync),

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

	TP_PROTO(int wait),

	TP_ARGS(wait),

	TP_FIELDS(
		ctf_integer(int, wait, wait)
	)
)

LTTNG_TRACEPOINT_EVENT(btrfs_delayed_tree_ref,

	TP_PROTO(struct btrfs_delayed_ref_node *ref,
		 struct btrfs_delayed_tree_ref *full_ref,
		 int action),

	TP_ARGS(ref, full_ref, action),

	TP_FIELDS(
		ctf_integer(u64, bytenr, ref->bytenr)
		ctf_integer(u64, num_bytes, ref->num_bytes)
		ctf_integer(int, action, action)
		ctf_integer(u64, parent, full_ref->parent)
		ctf_integer(u64, ref_root, full_ref->root)
		ctf_integer(int, level, full_ref->level)
		ctf_integer(int, type, ref->type)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0))
		ctf_integer(u64, seq, ref->seq)
#endif
	)
)

LTTNG_TRACEPOINT_EVENT(btrfs_delayed_data_ref,

	TP_PROTO(struct btrfs_delayed_ref_node *ref,
		 struct btrfs_delayed_data_ref *full_ref,
		 int action),

	TP_ARGS(ref, full_ref, action),

	TP_FIELDS(
		ctf_integer(u64, bytenr, ref->bytenr)
		ctf_integer(u64, num_bytes, ref->num_bytes)
		ctf_integer(int, action, action)
		ctf_integer(u64, parent, full_ref->parent)
		ctf_integer(u64, ref_root, full_ref->root)
		ctf_integer(u64, owner, full_ref->objectid)
		ctf_integer(u64, offset, full_ref->offset)
		ctf_integer(int, type, ref->type)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0))
		ctf_integer(u64, seq, ref->seq)
#endif
	)
)

LTTNG_TRACEPOINT_EVENT(btrfs_delayed_ref_head,

	TP_PROTO(struct btrfs_delayed_ref_node *ref,
		 struct btrfs_delayed_ref_head *head_ref,
		 int action),

	TP_ARGS(ref, head_ref, action),

	TP_FIELDS(
		ctf_integer(u64, bytenr, ref->bytenr)
		ctf_integer(u64, num_bytes, ref->num_bytes)
		ctf_integer(int, action, action)
		ctf_integer(int, is_data, head_ref->is_data)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs__chunk,

	TP_PROTO(struct btrfs_root *root, struct map_lookup *map,
		 u64 offset, u64 size),

	TP_ARGS(root, map, offset, size),

	TP_FIELDS(
		ctf_integer(int, num_stripes, map->num_stripes)
		ctf_integer(u64, type, map->type)
		ctf_integer(int, sub_stripes, map->sub_stripes)
		ctf_integer(u64, offset, offset)
		ctf_integer(u64, size, size)
		ctf_integer(u64, root_objectid, root->root_key.objectid)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__chunk,  btrfs_chunk_alloc,

	TP_PROTO(struct btrfs_root *root, struct map_lookup *map,
		 u64 offset, u64 size),

	TP_ARGS(root, map, offset, size)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__chunk,  btrfs_chunk_free,

	TP_PROTO(struct btrfs_root *root, struct map_lookup *map,
		 u64 offset, u64 size),

	TP_ARGS(root, map, offset, size)
)

LTTNG_TRACEPOINT_EVENT(btrfs_cow_block,

	TP_PROTO(struct btrfs_root *root, struct extent_buffer *buf,
		 struct extent_buffer *cow),

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

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
LTTNG_TRACEPOINT_EVENT(btrfs_space_reservation,

	TP_PROTO(struct btrfs_fs_info *fs_info, char *type, u64 val,
		 u64 bytes, int reserve),

	TP_ARGS(fs_info, type, val, bytes, reserve),

	TP_FIELDS(
		ctf_array(u8, fsid, fs_info->fsid, BTRFS_UUID_SIZE)
		ctf_string(type, type)
		ctf_integer(u64, val, val)
		ctf_integer(u64, bytes, bytes)
		ctf_integer(int, reserve, reserve)
	)
)
#endif

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs__reserved_extent,

	TP_PROTO(struct btrfs_root *root, u64 start, u64 len),

	TP_ARGS(root, start, len),

	TP_FIELDS(
		ctf_integer(u64, root_objectid, root->root_key.objectid)
		ctf_integer(u64, start, start)
		ctf_integer(u64, len, len)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__reserved_extent,  btrfs_reserved_extent_alloc,

	TP_PROTO(struct btrfs_root *root, u64 start, u64 len),

	TP_ARGS(root, start, len)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__reserved_extent,  btrfs_reserved_extent_free,

	TP_PROTO(struct btrfs_root *root, u64 start, u64 len),

	TP_ARGS(root, start, len)
)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
LTTNG_TRACEPOINT_EVENT_MAP(find_free_extent,

	btrfs_find_free_extent,

	TP_PROTO(struct btrfs_root *root, u64 num_bytes, u64 empty_size,
		 u64 data),

	TP_ARGS(root, num_bytes, empty_size, data),

	TP_FIELDS(
		ctf_integer(u64, root_objectid, root->root_key.objectid)
		ctf_integer(u64, num_bytes, num_bytes)
		ctf_integer(u64, empty_size, empty_size)
		ctf_integer(u64, data, data)
	)
)

LTTNG_TRACEPOINT_EVENT_CLASS(btrfs__reserve_extent,

	TP_PROTO(struct btrfs_root *root,
		 struct btrfs_block_group_cache *block_group, u64 start,
		 u64 len),

	TP_ARGS(root, block_group, start, len),

	TP_FIELDS(
		ctf_integer(u64, root_objectid, root->root_key.objectid)
		ctf_integer(u64, bg_objectid, block_group->key.objectid)
		ctf_integer(u64, flags, block_group->flags)
		ctf_integer(u64, start, start)
		ctf_integer(u64, len, len)
	)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__reserve_extent, btrfs_reserve_extent,

	TP_PROTO(struct btrfs_root *root,
		 struct btrfs_block_group_cache *block_group, u64 start,
		 u64 len),

	TP_ARGS(root, block_group, start, len)
)

LTTNG_TRACEPOINT_EVENT_INSTANCE(btrfs__reserve_extent, btrfs_reserve_extent_cluster,

	TP_PROTO(struct btrfs_root *root,
		 struct btrfs_block_group_cache *block_group, u64 start,
		 u64 len),

	TP_ARGS(root, block_group, start, len)
)

LTTNG_TRACEPOINT_EVENT(btrfs_find_cluster,

	TP_PROTO(struct btrfs_block_group_cache *block_group, u64 start,
		 u64 bytes, u64 empty_size, u64 min_bytes),

	TP_ARGS(block_group, start, bytes, empty_size, min_bytes),

	TP_FIELDS(
		ctf_integer(u64, bg_objectid, block_group->key.objectid)
		ctf_integer(u64, flags, block_group->flags)
		ctf_integer(u64, start, start)
		ctf_integer(u64, bytes, bytes)
		ctf_integer(u64, empty_size, empty_size)
		ctf_integer(u64, min_bytes, min_bytes)
	)
)

LTTNG_TRACEPOINT_EVENT(btrfs_failed_cluster_setup,

	TP_PROTO(struct btrfs_block_group_cache *block_group),

	TP_ARGS(block_group),

	TP_FIELDS(
		ctf_integer(u64, bg_objectid, block_group->key.objectid)
	)
)

LTTNG_TRACEPOINT_EVENT(btrfs_setup_cluster,

	TP_PROTO(struct btrfs_block_group_cache *block_group,
		 struct btrfs_free_cluster *cluster, u64 size, int bitmap),

	TP_ARGS(block_group, cluster, size, bitmap),

	TP_FIELDS(
		ctf_integer(u64, bg_objectid, block_group->key.objectid)
		ctf_integer(u64, flags, block_group->flags)
		ctf_integer(u64, start, cluster->window_start)
		ctf_integer(u64, max_size, cluster->max_size)
		ctf_integer(u64, size, size)
		ctf_integer(int, bitmap, bitmap)
	)
)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
LTTNG_TRACEPOINT_EVENT_MAP(alloc_extent_state,

	btrfs_alloc_extent_state,

	TP_PROTO(struct extent_state *state, gfp_t mask, unsigned long IP),

	TP_ARGS(state, mask, IP),

	TP_FIELDS(
		ctf_integer_hex(struct extent_state *, state, state)
		ctf_integer(gfp_t, mask, mask)
		ctf_integer(unsigned long, ip, IP)
	)
)

LTTNG_TRACEPOINT_EVENT_MAP(free_extent_state,

	btrfs_free_extent_state,

	TP_PROTO(struct extent_state *state, unsigned long IP),

	TP_ARGS(state, IP),

	TP_FIELDS(
		ctf_integer_hex(struct extent_state *, state, state)
		ctf_integer(unsigned long, ip, IP)
	)
)
#endif

#endif /* LTTNG_TRACE_BTRFS_H */

/* This part must be outside protection */
#include <probes/define_trace.h>
