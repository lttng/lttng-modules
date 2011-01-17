/*
 * ltt/probes/ext4-trace.c
 *
 * ext4 tracepoint probes.
 *
 * (C) Copyright 2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/writeback.h>
#include <linux/debugfs.h>
#include <linux/mutex.h>
#include <linux/rcupdate.h>
#include <trace/events/ext4.h>

#include "../ltt-tracer.h"
#include "../../fs/ext4/mballoc.h"

static struct dentry *ext4_filter_dentry, *ext4_filter_dev_dentry,
	*ext4_filter_inode_dentry;
static DEFINE_MUTEX(ext4_filter_mutex);
/* Make sure we don't race between module exit and file write */
static int module_exits;

struct rcu_dev_filter {
	struct rcu_head rcu;
	char devname[NAME_MAX];
};

static struct rcu_dev_filter *dev_filter;
/* ~0UL inode_filter enables all inodes */
static unsigned long inode_filter = ~0UL;

/*
 * Probes are executed in rcu_sched read-side critical section.
 */

static int do_dev_filter(const char *dev)
{
	struct rcu_dev_filter *ldev_filter = rcu_dereference(dev_filter);

	if (unlikely(ldev_filter))
		if (unlikely(strcmp(ldev_filter->devname, dev)))
			return 0;
	return 1;
}

static int do_inode_filter(unsigned long ino)
{
	if (unlikely(inode_filter != ~0UL))
		if (unlikely(inode_filter != ino))
			return 0;
	return 1;
}

/*
 * Logical AND between dev and inode filter.
 */
static int do_filter(const char *dev, unsigned long ino)
{
	if (unlikely(!do_dev_filter(dev)))
		return 0;
	if (unlikely(!do_inode_filter(ino)))
		return 0;
	return 1;
}


void probe_ext4_free_inode(void *data, struct inode *inode)
{
	if (unlikely(!do_filter(inode->i_sb->s_id, inode->i_ino)))
		return;
	trace_mark_tp(ext4, free_inode, ext4_free_inode,
		probe_ext4_free_inode,
		"dev %s ino %lu mode %d uid %lu gid %lu blocks %llu",
		inode->i_sb->s_id, inode->i_ino, inode->i_mode,
		(unsigned long) inode->i_uid, (unsigned long) inode->i_gid,
		(unsigned long long) inode->i_blocks);
}

void probe_ext4_request_inode(void *data, struct inode *dir, int mode)
{
	if (unlikely(!do_filter(dir->i_sb->s_id, dir->i_ino)))
		return;
	trace_mark_tp(ext4, request_inode, ext4_request_inode,
		probe_ext4_request_inode,
		"dev %s dir %lu mode %d",
		dir->i_sb->s_id, dir->i_ino, mode);
}

void probe_ext4_allocate_inode(void *data, struct inode *inode, struct inode *dir, int mode)
{
	if (unlikely(!do_filter(inode->i_sb->s_id, inode->i_ino)
		     && !do_filter(dir->i_sb->s_id, dir->i_ino)))
		return;
	trace_mark_tp(ext4, allocate_inode, ext4_allocate_inode,
		probe_ext4_allocate_inode,
		"dev %s ino %lu dir %lu mode %d",
		dir->i_sb->s_id, inode->i_ino, dir->i_ino, mode);
}

void probe_ext4_write_begin(void *data, struct inode *inode, loff_t pos, unsigned int len,
			    unsigned int flags)
{
	if (unlikely(!do_filter(inode->i_sb->s_id, inode->i_ino)))
		return;
	trace_mark_tp(ext4, write_begin, ext4_write_begin,
		probe_ext4_write_begin,
		"dev %s ino %lu pos %llu len %u flags %u",
		inode->i_sb->s_id, inode->i_ino,
		(unsigned long long) pos, len, flags);
}

void probe_ext4_ordered_write_end(void *data, struct inode *inode, loff_t pos,
			    unsigned int len, unsigned int copied)
{
	if (unlikely(!do_filter(inode->i_sb->s_id, inode->i_ino)))
		return;
	trace_mark_tp(ext4, ordered_write_end, ext4_ordered_write_end,
		probe_ext4_ordered_write_end,
		"dev %s ino %lu pos %llu len %u copied %u",
		inode->i_sb->s_id, inode->i_ino,
		(unsigned long long) pos, len, copied);
}

void probe_ext4_writeback_write_end(void *data, struct inode *inode, loff_t pos,
			    unsigned int len, unsigned int copied)
{
	if (unlikely(!do_filter(inode->i_sb->s_id, inode->i_ino)))
		return;
	trace_mark_tp(ext4, writeback_write_end, ext4_writeback_write_end,
		probe_ext4_writeback_write_end,
		"dev %s ino %lu pos %llu len %u copied %u",
		inode->i_sb->s_id, inode->i_ino,
		(unsigned long long) pos, len, copied);
}

void probe_ext4_journalled_write_end(void *data, struct inode *inode, loff_t pos,
			    unsigned int len, unsigned int copied)
{
	if (unlikely(!do_filter(inode->i_sb->s_id, inode->i_ino)))
		return;
	trace_mark_tp(ext4, journalled_write_end, ext4_journalled_write_end,
		probe_ext4_journalled_write_end,
		"dev %s ino %lu pos %llu len %u copied %u",
		inode->i_sb->s_id, inode->i_ino,
		(unsigned long long) pos, len, copied);
}

/*
 * note : wbc_flags will have to be decoded by userspace.
 * #1x uses a single byte in the trace. Limits to 8 bits.
 */
void probe_ext4_da_writepages(void *data, struct inode *inode,
			      struct writeback_control *wbc)
{
	if (unlikely(!do_filter(inode->i_sb->s_id, inode->i_ino)))
		return;
	trace_mark_tp(ext4, da_writepages, ext4_da_writepages,
		probe_ext4_da_writepages,
		"dev %s ino %lu nr_to_write %ld "
		"pages_skipped %ld range_start %llu range_end %llu "
		"wbc_flags(nonblocking,for_kupdate,"
		"for_reclaim,range_cyclic) #1x%u",
		inode->i_sb->s_id, inode->i_ino, wbc->nr_to_write,
		wbc->pages_skipped,
		(unsigned long long) wbc->range_start,
		(unsigned long long) wbc->range_end,
		  (wbc->nonblocking << 3)
		| (wbc->for_kupdate << 2)
		| (wbc->for_reclaim << 1)
		| wbc->range_cyclic);
}

/*
 * note : wbc_flags will have to be decoded by userspace.
 * #1x uses a single byte in the trace. Limits to 8 bits.
 */
void probe_ext4_da_writepages_result(void *data, struct inode *inode,
				     struct writeback_control *wbc,
				     int ret, int pages_written)
{
	if (unlikely(!do_filter(inode->i_sb->s_id, inode->i_ino)))
		return;
	trace_mark_tp(ext4, da_writepages_result, ext4_da_writepages_result,
		probe_ext4_da_writepages_result,
		"dev %s ino %lu ret %d pages_written %d "
		"pages_skipped %ld "
		"wbc_flags(encountered_congestion,"
		"more_io,no_nrwrite_index_update) #1x%u",
		inode->i_sb->s_id, inode->i_ino, ret, pages_written,
		wbc->pages_skipped,
		  (wbc->encountered_congestion << 2)
		| (wbc->more_io << 1)
		| wbc->no_nrwrite_index_update);
}

void probe_ext4_da_write_begin(void *data, struct inode *inode, loff_t pos,
			    unsigned int len, unsigned int flags)
{
	if (unlikely(!do_filter(inode->i_sb->s_id, inode->i_ino)))
		return;
	trace_mark_tp(ext4, da_write_begin, ext4_da_write_begin,
		probe_ext4_da_write_begin,
		"dev %s ino %lu pos %llu len %u flags %u",
		inode->i_sb->s_id, inode->i_ino,
		(unsigned long long) pos, len, flags);
}

void probe_ext4_da_write_end(void *data, struct inode *inode, loff_t pos,
			    unsigned int len, unsigned int copied)
{
	if (unlikely(!do_filter(inode->i_sb->s_id, inode->i_ino)))
		return;
	trace_mark_tp(ext4, da_write_end, ext4_da_write_end,
		probe_ext4_da_write_end,
		"dev %s ino %lu pos %llu len %u copied %u",
		inode->i_sb->s_id, inode->i_ino,
		(unsigned long long) pos, len, copied);
}

void probe_ext4_discard_blocks(void *data, struct super_block *sb, unsigned long long blk,
			       unsigned long long count)
{
	if (unlikely(!do_dev_filter(sb->s_id)))
		return;
	trace_mark_tp(ext4, discard_blocks, ext4_discard_blocks,
		probe_ext4_discard_blocks,
		"dev %s blk %llu count %llu",
		sb->s_id, blk, count);
}

void probe_ext4_mb_new_inode_pa(void *data, struct ext4_allocation_context *ac,
			        struct ext4_prealloc_space *pa)
{
	if (unlikely(!do_filter(ac->ac_sb->s_id, ac->ac_inode->i_ino)))
		return;
	trace_mark_tp(ext4, mb_new_inode_pa, ext4_mb_new_inode_pa,
		probe_ext4_mb_new_inode_pa,
		"dev %s ino %lu pstart %llu len %u lstart %u",
		ac->ac_sb->s_id, ac->ac_inode->i_ino, pa->pa_pstart,
		pa->pa_len, pa->pa_lstart);
}

void probe_ext4_mb_new_group_pa(void *data, struct ext4_allocation_context *ac,
			        struct ext4_prealloc_space *pa)
{
	if (unlikely(!do_dev_filter(ac->ac_sb->s_id)))
		return;
	trace_mark_tp(ext4, mb_new_group_pa, ext4_mb_new_group_pa,
		probe_ext4_mb_new_group_pa,
		"dev %s pstart %llu len %u lstart %u",
		ac->ac_sb->s_id, pa->pa_pstart,
		pa->pa_len, pa->pa_lstart);
}

void probe_ext4_mb_release_inode_pa(void *data, struct ext4_allocation_context *ac,
				    struct ext4_prealloc_space *pa,
				    unsigned long long block,
				    unsigned int count)
{
	if (unlikely(!do_filter(ac->ac_sb->s_id, ac->ac_inode->i_ino)))
		return;
	trace_mark_tp(ext4, mb_release_inode_pa, ext4_mb_release_inode_pa,
		probe_ext4_mb_release_inode_pa,
		"dev %s ino %lu block %llu count %u",
		ac->ac_sb->s_id, pa->pa_inode->i_ino, block, count);
}

void probe_ext4_mb_release_group_pa(void *data, struct ext4_allocation_context *ac,
				    struct ext4_prealloc_space *pa)
{
	if (unlikely(!do_dev_filter(ac->ac_sb->s_id)))
		return;
	trace_mark_tp(ext4, mb_release_group_pa, ext4_mb_release_group_pa,
		probe_ext4_mb_release_group_pa,
		"dev %s pstart %llu len %d",
		ac->ac_sb->s_id, pa->pa_pstart, pa->pa_len);
}

void probe_ext4_discard_preallocations(void *data, struct inode *inode)
{
	if (unlikely(!do_filter(inode->i_sb->s_id, inode->i_ino)))
		return;
	trace_mark_tp(ext4, discard_preallocations,
		ext4_discard_preallocations,
		probe_ext4_discard_preallocations,
		"dev %s ino %lu",
		inode->i_sb->s_id, inode->i_ino);
}

void probe_ext4_mb_discard_preallocations(void *data, struct super_block *sb, int needed)
{
	if (unlikely(!do_dev_filter(sb->s_id)))
		return;
	trace_mark_tp(ext4, mb_discard_preallocations,
		ext4_mb_discard_preallocations,
		probe_ext4_mb_discard_preallocations,
		"dev %s needed %d",
		sb->s_id, needed);
}

void probe_ext4_request_blocks(void *data, struct ext4_allocation_request *ar)
{
	if (ar->inode) {
		if (unlikely(!do_filter(ar->inode->i_sb->s_id,
					ar->inode->i_ino)))
			return;
	} else {
		if (unlikely(!do_dev_filter(ar->inode->i_sb->s_id)))
			return;
	}
	trace_mark_tp(ext4, request_blocks, ext4_request_blocks,
		probe_ext4_request_blocks,
		"dev %s flags %u len %u ino %lu "
		"lblk %llu goal %llu lleft %llu lright %llu "
		"pleft %llu pright %llu",
		ar->inode->i_sb->s_id, ar->flags, ar->len,
		ar->inode ? ar->inode->i_ino : 0,
		(unsigned long long) ar->logical,
		(unsigned long long) ar->goal,
		(unsigned long long) ar->lleft,
		(unsigned long long) ar->lright,
		(unsigned long long) ar->pleft,
		(unsigned long long) ar->pright);
}

void probe_ext4_allocate_blocks(void *data, struct ext4_allocation_request *ar,
				unsigned long long block)
{
	if (ar->inode) {
		if (unlikely(!do_filter(ar->inode->i_sb->s_id,
					ar->inode->i_ino)))
			return;
	} else {
		if (unlikely(!do_dev_filter(ar->inode->i_sb->s_id)))
			return;
	}
	trace_mark_tp(ext4, allocate_blocks, ext4_allocate_blocks,
		probe_ext4_allocate_blocks,
		"dev %s block %llu flags %u len %u ino %lu "
		"logical %llu goal %llu lleft %llu lright %llu "
		"pleft %llu pright %llu",
		ar->inode->i_sb->s_id, (unsigned long long) block,
		ar->flags, ar->len, ar->inode ? ar->inode->i_ino : 0,
		(unsigned long long) ar->logical,
		(unsigned long long) ar->goal,
		(unsigned long long) ar->lleft,
		(unsigned long long) ar->lright,
		(unsigned long long) ar->pleft,
		(unsigned long long) ar->pright);
}

void probe_ext4_free_blocks(void *data, struct inode *inode, __u64 block,
			    unsigned long count, int metadata)
{
	if (unlikely(!do_filter(inode->i_sb->s_id, inode->i_ino)))
		return;
	trace_mark_tp(ext4, free_blocks, ext4_free_blocks,
		probe_ext4_free_blocks,
		"dev %s block %llu count %lu metadata %d ino %lu",
		inode->i_sb->s_id, (unsigned long long)block,
		count, metadata, inode->i_ino);
}

void probe_ext4_sync_file(void *data, struct file *file, struct dentry *dentry,
			  int datasync)
{
	if (unlikely(!do_dev_filter(dentry->d_inode->i_sb->s_id)))
		return;
	if (unlikely(!do_inode_filter(dentry->d_inode->i_ino)
			&& !do_inode_filter(dentry->d_parent->d_inode->i_ino)))
		return;
	trace_mark_tp(ext4, sync_file, ext4_sync_file,
		probe_ext4_sync_file,
		"dev %s datasync %d ino %ld parent %ld",
		dentry->d_inode->i_sb->s_id, datasync, dentry->d_inode->i_ino,
		dentry->d_parent->d_inode->i_ino);
}

void probe_ext4_sync_fs(void *data, struct super_block *sb, int wait)
{
	if (unlikely(!do_dev_filter(sb->s_id)))
		return;
	trace_mark_tp(ext4, sync_fs, ext4_sync_fs,
		probe_ext4_sync_fs,
		"dev %s wait %d",
		sb->s_id, wait);
}

static void free_dev_filter(struct rcu_head *head)
{
	kfree(container_of(head, struct rcu_dev_filter, rcu));
}

static ssize_t dev_filter_op_write(struct file *file,
	const char __user *user_buf, size_t count, loff_t *ppos)
{
	int err = 0;
	char buf[NAME_MAX];
	int buf_size;
	char name[NAME_MAX];
	struct rcu_dev_filter *new, *old;

	mutex_lock(&ext4_filter_mutex);
	if (module_exits) {
		err = -EPERM;
		goto error;
	}
	buf_size = min(count, sizeof(buf) - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto error;
	buf[buf_size] = 0;

	if (sscanf(buf, "%s", name) != 1) {
		err = -EPERM;
		goto error;
	}

	old = dev_filter;

	/* Empty string or * means all active */
	if (name[0] == '\0' || (name[0] == '*' && name[1] == '\0')) {
		new = NULL;
	} else {
		new = kmalloc(sizeof(*new), GFP_KERNEL);
		strcpy(new->devname, name);
	}

	rcu_assign_pointer(dev_filter, new);
	if (old)
		call_rcu_sched(&old->rcu, free_dev_filter);

	mutex_unlock(&ext4_filter_mutex);
	return count;

error:
	mutex_unlock(&ext4_filter_mutex);
	return err;
}

static ssize_t dev_filter_op_read(struct file *filp, char __user *buffer,
	size_t count, loff_t *ppos)
{
	ssize_t bcount;
	const char *devname;

	mutex_lock(&ext4_filter_mutex);
	if (!dev_filter)
		devname = "*";
	else
		devname = dev_filter->devname;
	bcount = simple_read_from_buffer(buffer, count, ppos,
			devname, strlen(devname));
	mutex_unlock(&ext4_filter_mutex);
	return bcount;
}

static struct file_operations ext4_dev_file_operations = {
	.write = dev_filter_op_write,
	.read = dev_filter_op_read,
};

static ssize_t inode_filter_op_write(struct file *file,
	const char __user *user_buf, size_t count, loff_t *ppos)
{
	int err = 0;
	char buf[NAME_MAX];
	int buf_size;
	char name[NAME_MAX];
	unsigned long inode_num;

	mutex_lock(&ext4_filter_mutex);
	if (module_exits) {
		err = -EPERM;
		goto error;
	}
	buf_size = min(count, sizeof(buf) - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto error;
	buf[buf_size] = 0;

	if (sscanf(buf, "%s", name) != 1) {
		err = -EPERM;
		goto error;
	}

	/* Empty string or * means all active */
	if (name[0] == '\0' || (name[0] == '*' && name[1] == '\0')) {
		inode_filter = ~0UL;
	} else {
		if (sscanf(buf, "%lu", &inode_num) != 1) {
			err = -EPERM;
			goto error;
		}
		inode_filter = inode_num;
	}

	mutex_unlock(&ext4_filter_mutex);
	return count;

error:
	mutex_unlock(&ext4_filter_mutex);
	return err;
}

static ssize_t inode_filter_op_read(struct file *filp, char __user *buffer,
	size_t count, loff_t *ppos)
{
	ssize_t bcount;
	char inode_str[NAME_MAX];

	mutex_lock(&ext4_filter_mutex);
	if (inode_filter == ~0UL)
		strcpy(inode_str, "*");
	else {
		bcount = snprintf(inode_str, sizeof(inode_str), "%lu",
			       inode_filter);
		if (bcount == sizeof(inode_str))
			bcount = -ENOSPC;
		if (bcount < 0)
			goto end;
	}
	bcount = simple_read_from_buffer(buffer, count, ppos,
			inode_str, strlen(inode_str));
end:
	mutex_unlock(&ext4_filter_mutex);
	return bcount;
}

static struct file_operations ext4_inode_file_operations = {
	.write = inode_filter_op_write,
	.read = inode_filter_op_read,
};

static void release_filter_dev(void)
{
	struct rcu_dev_filter *old;

	mutex_lock(&ext4_filter_mutex);
	module_exits = 1;
	old = dev_filter;
	rcu_assign_pointer(dev_filter, NULL);
	if (old)
		call_rcu_sched(&old->rcu, free_dev_filter);
	mutex_unlock(&ext4_filter_mutex);
}

static int __init filter_init(void)
{
	struct dentry *filter_root_dentry;
	int err = 0;

	filter_root_dentry = get_filter_root();
	if (!filter_root_dentry) {
		err = -ENOENT;
		goto end;
	}

	ext4_filter_dentry = debugfs_create_dir("ext4", filter_root_dentry);

	if (IS_ERR(ext4_filter_dentry) || !ext4_filter_dentry) {
		printk(KERN_ERR "Failed to create ext4 filter file\n");
		err = -ENOMEM;
		goto end;
	}

	ext4_filter_dev_dentry = debugfs_create_file("dev", S_IWUSR,
			ext4_filter_dentry, NULL, &ext4_dev_file_operations);
	if (IS_ERR(ext4_filter_dev_dentry) || !ext4_filter_dev_dentry) {
		printk(KERN_ERR "Failed to create ext4 dev filter file\n");
		err = -ENOMEM;
		goto release_filter_dentry;
	}

	ext4_filter_inode_dentry = debugfs_create_file("inode", S_IWUSR,
			ext4_filter_dentry, NULL, &ext4_inode_file_operations);
	if (IS_ERR(ext4_filter_inode_dentry) || !ext4_filter_inode_dentry) {
		printk(KERN_ERR "Failed to create ext4 inode filter file\n");
		err = -ENOMEM;
		goto release_filter_dev_dentry;
	}

	goto end;

release_filter_dev_dentry:
	debugfs_remove(ext4_filter_dev_dentry);
release_filter_dentry:
	debugfs_remove(ext4_filter_dentry);
	release_filter_dev();
end:
	return err;
}

static void __exit filter_exit(void)
{
	debugfs_remove(ext4_filter_dev_dentry);
	debugfs_remove(ext4_filter_inode_dentry);
	debugfs_remove(ext4_filter_dentry);
	release_filter_dev();
}

module_init(filter_init);
module_exit(filter_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("ext4 Tracepoint Probes");
