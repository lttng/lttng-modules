/*
 * ltt/probes/jbd2-trace.c
 *
 * JBD2 tracepoint probes.
 *
 * (C) Copyright 2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/mutex.h>
#include <linux/rcupdate.h>
#include <trace/events/jbd2.h>

#include "../ltt-tracer.h"

static struct dentry *jbd2_filter_dentry, *jbd2_filter_dev_dentry;
static DEFINE_MUTEX(jbd2_filter_mutex);
/* Make sure we don't race between module exit and file write */
static int module_exits;

struct rcu_dev_filter {
	struct rcu_head rcu;
	char devname[NAME_MAX];
};

static struct rcu_dev_filter *dev_filter;

/*
 * Probes are executed in rcu_sched read-side critical section.
 */
static int do_filter(const char *dev)
{
	struct rcu_dev_filter *ldev_filter = rcu_dereference(dev_filter);

	if (unlikely(ldev_filter))
		if (unlikely(strcmp(ldev_filter->devname, dev)))
			return 0;
	return 1;
}

void probe_jbd2_checkpoint(void *data, journal_t *journal, int result)
{
	if (unlikely(!do_filter(journal->j_devname)))
		return;
	trace_mark_tp(jbd2, checkpoint, jbd2_checkpoint,
		probe_jbd2_checkpoint, "dev %s need_checkpoint %d",
		journal->j_devname, result);
}

void probe_jbd2_start_commit(void *data, journal_t *journal,
			     transaction_t *commit_transaction)
{
	if (unlikely(!do_filter(journal->j_devname)))
		return;
	trace_mark_tp(jbd2, start_commit, jbd2_start_commit,
		probe_jbd2_start_commit, "dev %s transaction %d",
		journal->j_devname, commit_transaction->t_tid);
}

void probe_jbd2_end_commit(void *data, journal_t *journal,
			   transaction_t *commit_transaction)
{
	if (unlikely(!do_filter(journal->j_devname)))
		return;
	trace_mark_tp(jbd2, end_commit, jbd2_end_commit,
		probe_jbd2_end_commit, "dev %s transaction %d head %d",
		journal->j_devname, commit_transaction->t_tid,
		journal->j_tail_sequence);
}

static void free_dev_filter(struct rcu_head *head)
{
	kfree(container_of(head, struct rcu_dev_filter, rcu));
}

static ssize_t filter_op_write(struct file *file,
	const char __user *user_buf, size_t count, loff_t *ppos)
{
	int err = 0;
	char buf[NAME_MAX];
	int buf_size;
	char name[NAME_MAX];
	struct rcu_dev_filter *new, *old;

	mutex_lock(&jbd2_filter_mutex);
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

	mutex_unlock(&jbd2_filter_mutex);
	return count;

error:
	mutex_unlock(&jbd2_filter_mutex);
	return err;
}

static ssize_t filter_op_read(struct file *filp, char __user *buffer,
	size_t count, loff_t *ppos)
{
	ssize_t bcount;
	const char *devname;

	mutex_lock(&jbd2_filter_mutex);
	if (!dev_filter)
		devname = "*";
	else
		devname = dev_filter->devname;
	bcount = simple_read_from_buffer(buffer, count, ppos,
			devname, strlen(devname));
	mutex_unlock(&jbd2_filter_mutex);
	return bcount;
}

static struct file_operations jbd2_file_operations = {
	.write = filter_op_write,
	.read = filter_op_read,
};

static void release_filter_dev(void)
{
	struct rcu_dev_filter *old;

	mutex_lock(&jbd2_filter_mutex);
	module_exits = 1;
	old = dev_filter;
	rcu_assign_pointer(dev_filter, NULL);
	if (old)
		call_rcu_sched(&old->rcu, free_dev_filter);
	mutex_unlock(&jbd2_filter_mutex);
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

	jbd2_filter_dentry = debugfs_create_dir("jbd2", filter_root_dentry);

	if (IS_ERR(jbd2_filter_dentry) || !jbd2_filter_dentry) {
		printk(KERN_ERR "Failed to create jbd2 filter file\n");
		err = -ENOMEM;
		goto end;
	}

	jbd2_filter_dev_dentry = debugfs_create_file("dev", S_IWUSR,
			jbd2_filter_dentry, NULL, &jbd2_file_operations);
	if (IS_ERR(jbd2_filter_dentry) || !jbd2_filter_dentry) {
		printk(KERN_ERR "Failed to create jbd2 filter file\n");
		err = -ENOMEM;
		goto release_filter_dentry;
	}

	goto end;

release_filter_dentry:
	debugfs_remove(jbd2_filter_dentry);
	release_filter_dev();
end:
	return err;
}

static void __exit filter_exit(void)
{
	debugfs_remove(jbd2_filter_dev_dentry);
	debugfs_remove(jbd2_filter_dentry);
	release_filter_dev();
}

module_init(filter_init);
module_exit(filter_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("JBD2 Tracepoint Probes");
