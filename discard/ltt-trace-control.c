/*
 * LTT trace control module over debugfs.
 *
 * Copyright 2008 - Zhaolei <zhaolei@cn.fujitsu.com>
 *
 * Copyright 2009 - Gui Jianfeng <guijianfeng@cn.fujitsu.com>
 *                  Make mark-control work in debugfs
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

/*
 * Todo:
 *   Impl read operations for control file to read attributes
 *   Create a README file in ltt control dir, for display help info
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/notifier.h>
#include <linux/jiffies.h>
#include <linux/marker.h>

#include "ltt-tracer.h"

#define LTT_CONTROL_DIR "control"
#define MARKERS_CONTROL_DIR "markers"
#define LTT_SETUP_TRACE_FILE "setup_trace"
#define LTT_DESTROY_TRACE_FILE "destroy_trace"

#define LTT_WRITE_MAXLEN	(128)

struct dentry *ltt_control_dir, *ltt_setup_trace_file, *ltt_destroy_trace_file,
	*markers_control_dir;

/*
 * the traces_lock nests inside control_lock.
 * control_lock protects the consistency of directories presented in ltt
 * directory.
 */
static DEFINE_MUTEX(control_lock);

/*
 * big note about locking for marker control files :
 * If a marker control file is added/removed manually racing with module
 * load/unload, there may be warning messages appearing, but those two
 * operations should be able to execute concurrently without any lock
 * synchronizing their operation one wrt another.
 * Locking the marker mutex, module mutex and also keeping a mutex here
 * from mkdir/rmdir _and_ from the notifier called from module load/unload makes
 * life miserable and just asks for deadlocks.
 */

/*
 * lookup a file/dir in parent dir.
 * only designed to work well for debugfs.
 * (although it maybe ok for other fs)
 *
 * return:
 *	file/dir's dentry on success
 *	NULL on failure
 */
static struct dentry *dir_lookup(struct dentry *parent, const char *name)
{
	struct qstr q;
	struct dentry *d;

	q.name = name;
	q.len = strlen(name);
	q.hash = full_name_hash(q.name, q.len);

	d = d_lookup(parent, &q);
	if (d)
		dput(d);

	return d;
}


static ssize_t alloc_write(struct file *file, const char __user *user_buf,
			   size_t count, loff_t *ppos)
{
	int err = 0;
	int buf_size;
	char *buf = (char *)__get_free_page(GFP_KERNEL);
	char *cmd = (char *)__get_free_page(GFP_KERNEL);

	buf_size = min_t(size_t, count, PAGE_SIZE - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto err_copy_from_user;
	buf[buf_size] = 0;

	if (sscanf(buf, "%s", cmd) != 1) {
		err = -EPERM;
		goto err_get_cmd;
	}

	if ((cmd[0] != 'Y' && cmd[0] != 'y' && cmd[0] != '1') || cmd[1]) {
		err = -EPERM;
		goto err_bad_cmd;
	}

	err = ltt_trace_alloc(file->f_dentry->d_parent->d_name.name);
	if (IS_ERR_VALUE(err)) {
		printk(KERN_ERR "alloc_write: ltt_trace_alloc failed: %d\n",
			err);
		goto err_alloc_trace;
	}

	free_page((unsigned long)buf);
	free_page((unsigned long)cmd);
	return count;

err_alloc_trace:
err_bad_cmd:
err_get_cmd:
err_copy_from_user:
	free_page((unsigned long)buf);
	free_page((unsigned long)cmd);
	return err;
}

static const struct file_operations ltt_alloc_operations = {
	.write = alloc_write,
};


static ssize_t enabled_write(struct file *file, const char __user *user_buf,
			     size_t count, loff_t *ppos)
{
	int err = 0;
	int buf_size;
	char *buf = (char *)__get_free_page(GFP_KERNEL);
	char *cmd = (char *)__get_free_page(GFP_KERNEL);

	buf_size = min_t(size_t, count, PAGE_SIZE - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto err_copy_from_user;
	buf[buf_size] = 0;

	if (sscanf(buf, "%s", cmd) != 1) {
		err = -EPERM;
		goto err_get_cmd;
	}

	if (cmd[1]) {
		err = -EPERM;
		goto err_bad_cmd;
	}

	switch (cmd[0]) {
	case 'Y':
	case 'y':
	case '1':
		err = ltt_trace_start(file->f_dentry->d_parent->d_name.name);
		if (IS_ERR_VALUE(err)) {
			printk(KERN_ERR
			       "enabled_write: ltt_trace_start failed: %d\n",
			       err);
			err = -EPERM;
			goto err_start_trace;
		}
		break;
	case 'N':
	case 'n':
	case '0':
		err = ltt_trace_stop(file->f_dentry->d_parent->d_name.name);
		if (IS_ERR_VALUE(err)) {
			printk(KERN_ERR
			       "enabled_write: ltt_trace_stop failed: %d\n",
			       err);
			err = -EPERM;
			goto err_stop_trace;
		}
		break;
	default:
		err = -EPERM;
		goto err_bad_cmd;
	}

	free_page((unsigned long)buf);
	free_page((unsigned long)cmd);
	return count;

err_stop_trace:
err_start_trace:
err_bad_cmd:
err_get_cmd:
err_copy_from_user:
	free_page((unsigned long)buf);
	free_page((unsigned long)cmd);
	return err;
}

static const struct file_operations ltt_enabled_operations = {
	.write = enabled_write,
};


static ssize_t trans_write(struct file *file, const char __user *user_buf,
			   size_t count, loff_t *ppos)
{
	char *buf = (char *)__get_free_page(GFP_KERNEL);
	char *trans_name = (char *)__get_free_page(GFP_KERNEL);
	int err = 0;
	int buf_size;

	buf_size = min_t(size_t, count, PAGE_SIZE - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto err_copy_from_user;
	buf[buf_size] = 0;

	if (sscanf(buf, "%s", trans_name) != 1) {
		err = -EPERM;
		goto err_get_transname;
	}

	err = ltt_trace_set_type(file->f_dentry->d_parent->d_name.name,
				 trans_name);
	if (IS_ERR_VALUE(err)) {
		printk(KERN_ERR "trans_write: ltt_trace_set_type failed: %d\n",
		       err);
		goto err_set_trans;
	}

	free_page((unsigned long)buf);
	free_page((unsigned long)trans_name);
	return count;

err_set_trans:
err_get_transname:
err_copy_from_user:
	free_page((unsigned long)buf);
	free_page((unsigned long)trans_name);
	return err;
}

static const struct file_operations ltt_trans_operations = {
	.write = trans_write,
};


static ssize_t channel_subbuf_num_write(struct file *file,
		const char __user *user_buf, size_t count, loff_t *ppos)
{
	int err = 0;
	int buf_size;
	unsigned int num;
	const char *channel_name;
	const char *trace_name;
	char *buf = (char *)__get_free_page(GFP_KERNEL);

	buf_size = min_t(size_t, count, PAGE_SIZE - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto err_copy_from_user;
	buf[buf_size] = 0;

	if (sscanf(buf, "%u", &num) != 1) {
		err = -EPERM;
		goto err_get_number;
	}

	channel_name = file->f_dentry->d_parent->d_name.name;
	trace_name = file->f_dentry->d_parent->d_parent->d_parent->d_name.name;

	err = ltt_trace_set_channel_subbufcount(trace_name, channel_name, num);
	if (IS_ERR_VALUE(err)) {
		printk(KERN_ERR "channel_subbuf_num_write: "
		       "ltt_trace_set_channel_subbufcount failed: %d\n", err);
		goto err_set_subbufcount;
	}

	free_page((unsigned long)buf);
	return count;

err_set_subbufcount:
err_get_number:
err_copy_from_user:
	free_page((unsigned long)buf);
	return err;
}

static const struct file_operations ltt_channel_subbuf_num_operations = {
	.write = channel_subbuf_num_write,
};


static
ssize_t channel_subbuf_size_write(struct file *file,
				  const char __user *user_buf,
				  size_t count, loff_t *ppos)
{
	int err = 0;
	int buf_size;
	unsigned int num;
	const char *channel_name;
	const char *trace_name;
	char *buf = (char *)__get_free_page(GFP_KERNEL);

	buf_size = min_t(size_t, count, PAGE_SIZE - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto err_copy_from_user;
	buf[buf_size] = 0;

	if (sscanf(buf, "%u", &num) != 1) {
		err = -EPERM;
		goto err_get_number;
	}

	channel_name = file->f_dentry->d_parent->d_name.name;
	trace_name = file->f_dentry->d_parent->d_parent->d_parent->d_name.name;

	err = ltt_trace_set_channel_subbufsize(trace_name, channel_name, num);
	if (IS_ERR_VALUE(err)) {
		printk(KERN_ERR "channel_subbuf_size_write: "
		       "ltt_trace_set_channel_subbufsize failed: %d\n", err);
		goto err_set_subbufsize;
	}

	free_page((unsigned long)buf);
	return count;

err_set_subbufsize:
err_get_number:
err_copy_from_user:
	free_page((unsigned long)buf);
	return err;
}

static const struct file_operations ltt_channel_subbuf_size_operations = {
	.write = channel_subbuf_size_write,
};

static
ssize_t channel_switch_timer_write(struct file *file,
				   const char __user *user_buf,
				   size_t count, loff_t *ppos)
{
	int err = 0;
	int buf_size;
	unsigned long num;
	const char *channel_name;
	const char *trace_name;
	char *buf = (char *)__get_free_page(GFP_KERNEL);

	buf_size = min_t(size_t, count, PAGE_SIZE - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto err_copy_from_user;
	buf[buf_size] = 0;

	if (sscanf(buf, "%lu", &num) != 1) {
		err = -EPERM;
		goto err_get_number;
	}

	channel_name = file->f_dentry->d_parent->d_name.name;
	trace_name = file->f_dentry->d_parent->d_parent->d_parent->d_name.name;

	/* Convert from ms to us */
	num *= 1000;

	err = ltt_trace_set_channel_switch_timer(trace_name, channel_name, num);
	if (IS_ERR_VALUE(err)) {
		printk(KERN_ERR "channel_switch_timer_write: "
		       "ltt_trace_set_channel_switch_timer failed: %d\n", err);
		goto err_set_switch_timer;
	}

	free_page((unsigned long)buf);
	return count;

err_set_switch_timer:
err_get_number:
err_copy_from_user:
	free_page((unsigned long)buf);
	return err;
}

static struct file_operations ltt_channel_switch_timer_operations = {
	.write = channel_switch_timer_write,
};

static
ssize_t channel_overwrite_write(struct file *file,
				const char __user *user_buf, size_t count,
				loff_t *ppos)
{
	int err = 0;
	int buf_size;
	const char *channel_name;
	const char *trace_name;
	char *buf = (char *)__get_free_page(GFP_KERNEL);
	char *cmd = (char *)__get_free_page(GFP_KERNEL);

	buf_size = min_t(size_t, count, PAGE_SIZE - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto err_copy_from_user;
	buf[buf_size] = 0;

	if (sscanf(buf, "%s", cmd) != 1) {
		err = -EPERM;
		goto err_get_cmd;
	}

	if (cmd[1]) {
		err = -EPERM;
		goto err_bad_cmd;
	}

	channel_name = file->f_dentry->d_parent->d_name.name;
	trace_name = file->f_dentry->d_parent->d_parent->d_parent->d_name.name;

	switch (cmd[0]) {
	case 'Y':
	case 'y':
	case '1':
		err = ltt_trace_set_channel_overwrite(trace_name, channel_name,
						      1);
		if (IS_ERR_VALUE(err)) {
			printk(KERN_ERR "channel_overwrite_write: "
			       "ltt_trace_set_channel_overwrite failed: %d\n",
			       err);
			goto err_set_subbufsize;
		}
		break;
	case 'N':
	case 'n':
	case '0':
		err = ltt_trace_set_channel_overwrite(trace_name, channel_name,
						      0);
		if (IS_ERR_VALUE(err)) {
			printk(KERN_ERR "channel_overwrite_write: "
			       "ltt_trace_set_channel_overwrite failed: %d\n",
			       err);
			goto err_set_subbufsize;
		}
		break;
	default:
		err = -EPERM;
		goto err_bad_cmd;
	}

	free_page((unsigned long)buf);
	free_page((unsigned long)cmd);
	return count;

err_set_subbufsize:
err_bad_cmd:
err_get_cmd:
err_copy_from_user:
	free_page((unsigned long)buf);
	free_page((unsigned long)cmd);
	return err;
}

static const struct file_operations ltt_channel_overwrite_operations = {
	.write = channel_overwrite_write,
};


static
ssize_t channel_enable_write(struct file *file,
			     const char __user *user_buf, size_t count,
			     loff_t *ppos)
{
	int err = 0;
	int buf_size;
	const char *channel_name;
	const char *trace_name;
	char *buf = (char *)__get_free_page(GFP_KERNEL);
	char *cmd = (char *)__get_free_page(GFP_KERNEL);

	buf_size = min_t(size_t, count, PAGE_SIZE - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto err_copy_from_user;
	buf[buf_size] = 0;

	if (sscanf(buf, "%s", cmd) != 1) {
		err = -EPERM;
		goto err_get_cmd;
	}

	if (cmd[1]) {
		err = -EPERM;
		goto err_bad_cmd;
	}

	channel_name = file->f_dentry->d_parent->d_name.name;
	trace_name = file->f_dentry->d_parent->d_parent->d_parent->d_name.name;

	switch (cmd[0]) {
	case 'Y':
	case 'y':
	case '1':
		err = ltt_trace_set_channel_enable(trace_name, channel_name,
						   1);
		if (IS_ERR_VALUE(err)) {
			printk(KERN_ERR "channel_enable_write: "
			       "ltt_trace_set_channel_enable failed: %d\n",
			       err);
			goto err_set_subbufsize;
		}
		break;
	case 'N':
	case 'n':
	case '0':
		err = ltt_trace_set_channel_enable(trace_name, channel_name,
						   0);
		if (IS_ERR_VALUE(err)) {
			printk(KERN_ERR "channel_enable_write: "
			       "ltt_trace_set_channel_enable failed: %d\n",
			       err);
			goto err_set_subbufsize;
		}
		break;
	default:
		err = -EPERM;
		goto err_bad_cmd;
	}

	free_page((unsigned long)buf);
	free_page((unsigned long)cmd);
	return count;

err_set_subbufsize:
err_bad_cmd:
err_get_cmd:
err_copy_from_user:
	free_page((unsigned long)buf);
	free_page((unsigned long)cmd);
	return err;
}

static const struct file_operations ltt_channel_enable_operations = {
	.write = channel_enable_write,
};


static int _create_trace_control_dir(const char *trace_name,
				     struct ltt_trace *trace)
{
	int err;
	struct dentry *trace_root, *channel_root;
	struct dentry *tmp_den;
	int i;

	/* debugfs/control/trace_name */
	trace_root = debugfs_create_dir(trace_name, ltt_control_dir);
	if (IS_ERR(trace_root) || !trace_root) {
		printk(KERN_ERR "_create_trace_control_dir: "
		       "create control root dir of %s failed\n", trace_name);
		err = -ENOMEM;
		goto err_create_trace_root;
	}

	/* debugfs/control/trace_name/alloc */
	tmp_den = debugfs_create_file("alloc", S_IWUSR, trace_root, NULL,
				      &ltt_alloc_operations);
	if (IS_ERR(tmp_den) || !tmp_den) {
		printk(KERN_ERR "_create_trace_control_dir: "
		       "create file of alloc failed\n");
		err = -ENOMEM;
		goto err_create_subdir;
	}

	/* debugfs/control/trace_name/trans */
	tmp_den = debugfs_create_file("trans", S_IWUSR, trace_root, NULL,
				      &ltt_trans_operations);
	if (IS_ERR(tmp_den) || !tmp_den) {
		printk(KERN_ERR "_create_trace_control_dir: "
		       "create file of trans failed\n");
		err = -ENOMEM;
		goto err_create_subdir;
	}

	/* debugfs/control/trace_name/enabled */
	tmp_den = debugfs_create_file("enabled", S_IWUSR, trace_root, NULL,
				      &ltt_enabled_operations);
	if (IS_ERR(tmp_den) || !tmp_den) {
		printk(KERN_ERR "_create_trace_control_dir: "
		       "create file of enabled failed\n");
		err = -ENOMEM;
		goto err_create_subdir;
	}

	/* debugfs/control/trace_name/channel/ */
	channel_root = debugfs_create_dir("channel", trace_root);
	if (IS_ERR(channel_root) || !channel_root) {
		printk(KERN_ERR "_create_trace_control_dir: "
		       "create dir of channel failed\n");
		err = -ENOMEM;
		goto err_create_subdir;
	}

	/*
	 * Create dir and files in debugfs/ltt/control/trace_name/channel/
	 * Following things(without <>) will be created:
	 * `-- <control>
	 *     `-- <trace_name>
	 *         `-- <channel>
	 *             |-- <channel_name>
	 *             |   |-- enable
	 *             |   |-- overwrite
	 *             |   |-- subbuf_num
	 *             |   |-- subbuf_size
	 *             |   `-- switch_timer
	 *             `-- ...
	 */

	for (i = 0; i < trace->nr_channels; i++) {
		struct dentry *channel_den;
		struct ltt_chan *chan;

		chan = &trace->channels[i];
		if (!chan->active)
			continue;
		channel_den = debugfs_create_dir(chan->a.filename,
						 channel_root);
		if (IS_ERR(channel_den) || !channel_den) {
			printk(KERN_ERR "_create_trace_control_dir: "
			       "create channel dir of %s failed\n",
			       chan->a.filename);
			err = -ENOMEM;
			goto err_create_subdir;
		}

		tmp_den = debugfs_create_file("subbuf_num", S_IWUSR,
					      channel_den, NULL,
					      &ltt_channel_subbuf_num_operations);
		if (IS_ERR(tmp_den) || !tmp_den) {
			printk(KERN_ERR "_create_trace_control_dir: "
			       "create subbuf_num in %s failed\n",
			       chan->a.filename);
			err = -ENOMEM;
			goto err_create_subdir;
		}

		tmp_den = debugfs_create_file("subbuf_size", S_IWUSR,
					      channel_den, NULL,
					      &ltt_channel_subbuf_size_operations);
		if (IS_ERR(tmp_den) || !tmp_den) {
			printk(KERN_ERR "_create_trace_control_dir: "
			       "create subbuf_size in %s failed\n",
			       chan->a.filename);
			err = -ENOMEM;
			goto err_create_subdir;
		}

		tmp_den = debugfs_create_file("enable", S_IWUSR, channel_den,
					      NULL,
					      &ltt_channel_enable_operations);
		if (IS_ERR(tmp_den) || !tmp_den) {
			printk(KERN_ERR "_create_trace_control_dir: "
			       "create enable in %s failed\n",
			       chan->a.filename);
			err = -ENOMEM;
			goto err_create_subdir;
		}

		tmp_den = debugfs_create_file("overwrite", S_IWUSR, channel_den,
					      NULL,
					      &ltt_channel_overwrite_operations);
		if (IS_ERR(tmp_den) || !tmp_den) {
			printk(KERN_ERR "_create_trace_control_dir: "
			       "create overwrite in %s failed\n",
			       chan->a.filename);
			err = -ENOMEM;
			goto err_create_subdir;
		}

		tmp_den = debugfs_create_file("switch_timer", S_IWUSR,
					      channel_den, NULL,
					      &ltt_channel_switch_timer_operations);
		if (IS_ERR(tmp_den) || !tmp_den) {
			printk(KERN_ERR "_create_trace_control_dir: "
			       "create switch_timer in %s failed\n",
			       chan->a.filename);
			err = -ENOMEM;
			goto err_create_subdir;
		}
	}

	return 0;

err_create_subdir:
	debugfs_remove_recursive(trace_root);
err_create_trace_root:
	return err;
}

static
ssize_t setup_trace_write(struct file *file, const char __user *user_buf,
			  size_t count, loff_t *ppos)
{
	int err = 0;
	int buf_size;
	struct ltt_trace *trace;
	char *buf = (char *)__get_free_page(GFP_KERNEL);
	char *trace_name = (char *)__get_free_page(GFP_KERNEL);

	buf_size = min_t(size_t, count, PAGE_SIZE - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto err_copy_from_user;
	buf[buf_size] = 0;

	if (sscanf(buf, "%s", trace_name) != 1) {
		err = -EPERM;
		goto err_get_tracename;
	}

	mutex_lock(&control_lock);
	ltt_lock_traces();

	err = _ltt_trace_setup(trace_name);
	if (IS_ERR_VALUE(err)) {
		printk(KERN_ERR
		       "setup_trace_write: ltt_trace_setup failed: %d\n", err);
		goto err_setup_trace;
	}
	trace = _ltt_trace_find_setup(trace_name);
	BUG_ON(!trace);
	err = _create_trace_control_dir(trace_name, trace);
	if (IS_ERR_VALUE(err)) {
		printk(KERN_ERR "setup_trace_write: "
		       "_create_trace_control_dir failed: %d\n", err);
		goto err_create_trace_control_dir;
	}

	ltt_unlock_traces();
	mutex_unlock(&control_lock);

	free_page((unsigned long)buf);
	free_page((unsigned long)trace_name);
	return count;

err_create_trace_control_dir:
	ltt_trace_destroy(trace_name);
err_setup_trace:
	ltt_unlock_traces();
	mutex_unlock(&control_lock);
err_get_tracename:
err_copy_from_user:
	free_page((unsigned long)buf);
	free_page((unsigned long)trace_name);
	return err;
}

static const struct file_operations ltt_setup_trace_operations = {
	.write = setup_trace_write,
};

static
ssize_t destroy_trace_write(struct file *file, const char __user *user_buf,
			    size_t count, loff_t *ppos)
{
	struct dentry *trace_den;
	int buf_size;
	int err = 0;
	char *buf = (char *)__get_free_page(GFP_KERNEL);
	char *trace_name = (char *)__get_free_page(GFP_KERNEL);

	buf_size = min_t(size_t, count, PAGE_SIZE - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto err_copy_from_user;
	buf[buf_size] = 0;

	if (sscanf(buf, "%s", trace_name) != 1) {
		err = -EPERM;
		goto err_get_tracename;
	}

	mutex_lock(&control_lock);

	err = ltt_trace_destroy(trace_name);
	if (IS_ERR_VALUE(err)) {
		printk(KERN_ERR
		       "destroy_trace_write: ltt_trace_destroy failed: %d\n",
		       err);
		err = -EPERM;
		goto err_destroy_trace;
	}

	trace_den = dir_lookup(ltt_control_dir, trace_name);
	if (!trace_den) {
		printk(KERN_ERR
		       "destroy_trace_write: lookup for %s's dentry failed\n",
		       trace_name);
		err = -ENOENT;
		goto err_get_dentry;
	}

	debugfs_remove_recursive(trace_den);

	mutex_unlock(&control_lock);

	free_page((unsigned long)buf);
	free_page((unsigned long)trace_name);
	return count;

err_get_dentry:
err_destroy_trace:
	mutex_unlock(&control_lock);
err_get_tracename:
err_copy_from_user:
	free_page((unsigned long)buf);
	free_page((unsigned long)trace_name);
	return err;
}

static const struct file_operations ltt_destroy_trace_operations = {
	.write = destroy_trace_write,
};

static void init_marker_dir(struct dentry *dentry,
			    const struct inode_operations *opt)
{
	dentry->d_inode->i_op = opt;
}

static
ssize_t marker_enable_read(struct file *filp, char __user *ubuf,
			   size_t cnt, loff_t *ppos)
{
	char *buf;
	const char *channel, *marker;
	int len, enabled, present;

	marker = filp->f_dentry->d_parent->d_name.name;
	channel = filp->f_dentry->d_parent->d_parent->d_name.name;

	len = 0;
	buf = (char *)__get_free_page(GFP_KERNEL);

	/*
	 * Note: we cannot take the marker lock to make these two checks
	 * atomic, because the marker mutex nests inside the module mutex, taken
	 * inside the marker present check.
	 */
	enabled = is_marker_enabled(channel, marker);
	present = is_marker_present(channel, marker);

	if (enabled && present)
		len = snprintf(buf, PAGE_SIZE, "%d\n", 1);
	else if (enabled && !present)
		len = snprintf(buf, PAGE_SIZE, "%d\n", 2);
	else
		len = snprintf(buf, PAGE_SIZE, "%d\n", 0);


	if (len >= PAGE_SIZE) {
		len = PAGE_SIZE;
		buf[PAGE_SIZE] = '\0';
	}
	len = simple_read_from_buffer(ubuf, cnt, ppos, buf, len);
	free_page((unsigned long)buf);

	return len;
}

static
ssize_t marker_enable_write(struct file *filp, const char __user *ubuf,
			    size_t cnt, loff_t *ppos)
{
	char *buf = (char *)__get_free_page(GFP_KERNEL);
	int buf_size;
	ssize_t ret = 0;
	const char *channel, *marker;

	marker = filp->f_dentry->d_parent->d_name.name;
	channel = filp->f_dentry->d_parent->d_parent->d_name.name;

	buf_size = min_t(size_t, cnt, PAGE_SIZE - 1);
	ret = copy_from_user(buf, ubuf, buf_size);
	if (ret)
		goto end;

	buf[buf_size] = 0;

	switch (buf[0]) {
	case 'Y':
	case 'y':
	case '1':
		ret = ltt_marker_connect(channel, marker, "default");
		if (ret)
			goto end;
		break;
	case 'N':
	case 'n':
	case '0':
		ret = ltt_marker_disconnect(channel, marker, "default");
		if (ret)
			goto end;
		break;
	default:
		ret = -EPERM;
		goto end;
	}
	ret = cnt;
end:
	free_page((unsigned long)buf);
	return ret;
}

static const struct file_operations enable_fops = {
	.read = marker_enable_read,
	.write = marker_enable_write,
};

/*
 * In practice, the output size should never be larger than 4096 kB. If it
 * ever happens, the output will simply be truncated.
 */
static
ssize_t marker_info_read(struct file *filp, char __user *ubuf,
			 size_t cnt, loff_t *ppos)
{
	char *buf;
	const char *channel, *marker;
	int len;
	struct marker_iter iter;

	marker = filp->f_dentry->d_parent->d_name.name;
	channel = filp->f_dentry->d_parent->d_parent->d_name.name;

	len = 0;
	buf = (char *)__get_free_page(GFP_KERNEL);

	if (is_marker_enabled(channel, marker) &&
	    !is_marker_present(channel, marker)) {
		len += snprintf(buf + len, PAGE_SIZE - len,
				"Marker Pre-enabled\n");
		goto out;
	}

	marker_iter_reset(&iter);
	marker_iter_start(&iter);
	for (; iter.marker != NULL; marker_iter_next(&iter)) {
		if (!strcmp(iter.marker->channel, channel) &&
		    !strcmp(iter.marker->name, marker))
			len += snprintf(buf + len, PAGE_SIZE - len,
				       "Location: %s\n"
				       "format: \"%s\"\nstate: %d\n"
				       "event_id: %hu\n"
				       "call: 0x%p\n"
				       "probe %s : 0x%p\n\n",
#ifdef CONFIG_MODULES
				       iter.module ? iter.module->name :
#endif
				       "Core Kernel",
				       iter.marker->format,
				       _imv_read(iter.marker->state),
				       iter.marker->event_id,
				       iter.marker->call,
				       iter.marker->ptype ?
				       "multi" : "single", iter.marker->ptype ?
				       (void *)iter.marker->multi :
				       (void *)iter.marker->single.func);
			if (len >= PAGE_SIZE)
				break;
	}
	marker_iter_stop(&iter);

out:
	if (len >= PAGE_SIZE) {
		len = PAGE_SIZE;
		buf[PAGE_SIZE] = '\0';
	}

	len = simple_read_from_buffer(ubuf, cnt, ppos, buf, len);
	free_page((unsigned long)buf);

	return len;
}

static const struct file_operations info_fops = {
	.read = marker_info_read,
};

static int marker_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	struct dentry *marker_d, *enable_d, *info_d, *channel_d;
	int ret;

	ret = 0;
	channel_d = (struct dentry *)dir->i_private;
	mutex_unlock(&dir->i_mutex);

	marker_d = debugfs_create_dir(dentry->d_name.name,
				      channel_d);
	if (IS_ERR(marker_d)) {
		ret = PTR_ERR(marker_d);
		goto out;
	}

	enable_d = debugfs_create_file("enable", 0644, marker_d,
				       NULL, &enable_fops);
	if (IS_ERR(enable_d) || !enable_d) {
		printk(KERN_ERR
		       "%s: create file of %s failed\n",
		       __func__, "enable");
		ret = -ENOMEM;
		goto remove_marker_dir;
	}

	info_d = debugfs_create_file("info", 0644, marker_d,
				     NULL, &info_fops);
	if (IS_ERR(info_d) || !info_d) {
		printk(KERN_ERR
		       "%s: create file of %s failed\n",
		       __func__, "info");
		ret = -ENOMEM;
		goto remove_enable_dir;
	}

	goto out;

remove_enable_dir:
	debugfs_remove(enable_d);
remove_marker_dir:
	debugfs_remove(marker_d);
out:
	mutex_lock_nested(&dir->i_mutex, I_MUTEX_PARENT);
	return ret;
}

static int marker_rmdir(struct inode *dir, struct dentry *dentry)
{
	struct dentry *marker_d, *channel_d;
	const char *channel, *name;
	int ret, enabled, present;

	ret = 0;

	channel_d = (struct dentry *)dir->i_private;
	channel = channel_d->d_name.name;

	marker_d = dir_lookup(channel_d, dentry->d_name.name);

	if (!marker_d) {
		ret = -ENOENT;
		goto out;
	}

	name = marker_d->d_name.name;

	enabled = is_marker_enabled(channel, name);
	present = is_marker_present(channel, name);

	if (present || (!present && enabled)) {
		ret = -EPERM;
		goto out;
	}

	mutex_unlock(&dir->i_mutex);
	mutex_unlock(&dentry->d_inode->i_mutex);
	debugfs_remove_recursive(marker_d);
	mutex_lock_nested(&dir->i_mutex, I_MUTEX_PARENT);
	mutex_lock(&dentry->d_inode->i_mutex);
out:
	return ret;
}

const struct inode_operations channel_dir_opt = {
	.lookup = simple_lookup,
	.mkdir = marker_mkdir,
	.rmdir = marker_rmdir,
};

static int channel_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	struct dentry *channel_d;
	int ret;

	ret = 0;
	mutex_unlock(&dir->i_mutex);

	channel_d = debugfs_create_dir(dentry->d_name.name,
				       markers_control_dir);
	if (IS_ERR(channel_d)) {
		ret = PTR_ERR(channel_d);
		goto out;
	}

	channel_d->d_inode->i_private = (void *)channel_d;
	init_marker_dir(channel_d, &channel_dir_opt);
out:
	mutex_lock_nested(&dir->i_mutex, I_MUTEX_PARENT);
	return ret;
}

static int channel_rmdir(struct inode *dir, struct dentry *dentry)
{
	struct dentry *channel_d;
	int ret;

	ret = 0;

	channel_d = dir_lookup(markers_control_dir, dentry->d_name.name);
	if (!channel_d) {
		ret = -ENOENT;
		goto out;
	}

	if (list_empty(&channel_d->d_subdirs)) {
		mutex_unlock(&dir->i_mutex);
		mutex_unlock(&dentry->d_inode->i_mutex);
		debugfs_remove(channel_d);
		mutex_lock_nested(&dir->i_mutex, I_MUTEX_PARENT);
		mutex_lock(&dentry->d_inode->i_mutex);
	} else
		ret = -EPERM;

out:
	return ret;
}

const struct inode_operations root_dir_opt = {
	.lookup = simple_lookup,
	.mkdir = channel_mkdir,
	.rmdir = channel_rmdir
};

static int build_marker_file(struct marker *marker)
{
	struct dentry *channel_d, *marker_d, *enable_d, *info_d;
	int err;

	channel_d = dir_lookup(markers_control_dir, marker->channel);
	if (!channel_d) {
		channel_d = debugfs_create_dir(marker->channel,
					       markers_control_dir);
		if (IS_ERR(channel_d) || !channel_d) {
			printk(KERN_ERR
			       "%s: build channel dir of %s failed\n",
			       __func__, marker->channel);
			err = -ENOMEM;
			goto err_build_fail;
		}
		channel_d->d_inode->i_private = (void *)channel_d;
		init_marker_dir(channel_d, &channel_dir_opt);
	}

	marker_d  = dir_lookup(channel_d, marker->name);
	if (!marker_d) {
		marker_d = debugfs_create_dir(marker->name, channel_d);
		if (IS_ERR(marker_d) || !marker_d) {
			printk(KERN_ERR
			       "%s: marker dir of %s failed\n",
			       __func__, marker->name);
			err = -ENOMEM;
			goto err_build_fail;
		}
	}

	enable_d = dir_lookup(marker_d, "enable");
	if (!enable_d) {
		enable_d = debugfs_create_file("enable", 0644, marker_d,
						NULL, &enable_fops);
		if (IS_ERR(enable_d) || !enable_d) {
			printk(KERN_ERR
			       "%s: create file of %s failed\n",
			       __func__, "enable");
			err = -ENOMEM;
			goto err_build_fail;
		}
	}

	info_d = dir_lookup(marker_d, "info");
	if (!info_d) {
		info_d = debugfs_create_file("info", 0444, marker_d,
						NULL, &info_fops);
		if (IS_ERR(info_d) || !info_d) {
			printk(KERN_ERR
			       "%s: create file of %s failed\n",
			       __func__, "enable");
			err = -ENOMEM;
			goto err_build_fail;
		}
	}

	return 0;

err_build_fail:
	return err;
}

static int build_marker_control_files(void)
{
	struct marker_iter iter;
	int err;

	err = 0;
	if (!markers_control_dir)
		return -EEXIST;

	marker_iter_reset(&iter);
	marker_iter_start(&iter);
	for (; iter.marker != NULL; marker_iter_next(&iter)) {
		err = build_marker_file(iter.marker);
		if (err)
			goto out;
	}
	marker_iter_stop(&iter);

out:
	return err;
}

#ifdef CONFIG_MODULES
static int remove_marker_control_dir(struct module *mod, struct marker *marker)
{
	struct dentry *channel_d, *marker_d;
	const char *channel, *name;
	int count;
	struct marker_iter iter;

	count = 0;

	channel_d = dir_lookup(markers_control_dir, marker->channel);
	if (!channel_d)
		return -ENOENT;
	channel = channel_d->d_name.name;

	marker_d = dir_lookup(channel_d, marker->name);
	if (!marker_d)
		return -ENOENT;
	name = marker_d->d_name.name;

	marker_iter_reset(&iter);
	marker_iter_start(&iter);
	for (; iter.marker != NULL; marker_iter_next(&iter)) {
		if (!strcmp(iter.marker->channel, channel) &&
		    !strcmp(iter.marker->name, name) && mod != iter.module)
			count++;
	}

	if (count > 0)
		goto end;

	debugfs_remove_recursive(marker_d);
	if (list_empty(&channel_d->d_subdirs))
		debugfs_remove(channel_d);

end:
	marker_iter_stop(&iter);
	return 0;
}

static void cleanup_control_dir(struct module *mod, struct marker *begin,
				struct marker *end)
{
	struct marker *iter;

	if (!markers_control_dir)
		return;

	for (iter = begin; iter < end; iter++)
		remove_marker_control_dir(mod, iter);

	return;
}

static void build_control_dir(struct module *mod, struct marker *begin,
			      struct marker *end)
{
	struct marker *iter;
	int err;

	err = 0;
	if (!markers_control_dir)
		return;

	for (iter = begin; iter < end; iter++) {
		err = build_marker_file(iter);
		if (err)
			goto err_build_fail;
	}

	return;
err_build_fail:
	cleanup_control_dir(mod, begin, end);
}

static int module_notify(struct notifier_block *self,
		  unsigned long val, void *data)
{
	struct module *mod = data;

	switch (val) {
	case MODULE_STATE_COMING:
		build_control_dir(mod, mod->markers,
				  mod->markers + mod->num_markers);
		break;
	case MODULE_STATE_GOING:
		cleanup_control_dir(mod, mod->markers,
				    mod->markers + mod->num_markers);
		break;
	}
	return NOTIFY_DONE;
}
#else
static inline int module_notify(struct notifier_block *self,
		unsigned long val, void *data)
{
	return 0;
}
#endif

static struct notifier_block module_nb = {
	.notifier_call = module_notify,
};

static int __init ltt_trace_control_init(void)
{
	int err = 0;
	struct dentry *ltt_root_dentry;

	ltt_root_dentry = get_ltt_root();
	if (!ltt_root_dentry) {
		err = -ENOENT;
		goto err_no_root;
	}

	ltt_control_dir = debugfs_create_dir(LTT_CONTROL_DIR, ltt_root_dentry);
	if (IS_ERR(ltt_control_dir) || !ltt_control_dir) {
		printk(KERN_ERR
		       "ltt_channel_control_init: create dir of %s failed\n",
		       LTT_CONTROL_DIR);
		err = -ENOMEM;
		goto err_create_control_dir;
	}

	ltt_setup_trace_file = debugfs_create_file(LTT_SETUP_TRACE_FILE,
						   S_IWUSR, ltt_root_dentry,
						   NULL,
						   &ltt_setup_trace_operations);
	if (IS_ERR(ltt_setup_trace_file) || !ltt_setup_trace_file) {
		printk(KERN_ERR
		       "ltt_channel_control_init: create file of %s failed\n",
		       LTT_SETUP_TRACE_FILE);
		err = -ENOMEM;
		goto err_create_setup_trace_file;
	}

	ltt_destroy_trace_file = debugfs_create_file(LTT_DESTROY_TRACE_FILE,
						     S_IWUSR, ltt_root_dentry,
						     NULL,
						     &ltt_destroy_trace_operations);
	if (IS_ERR(ltt_destroy_trace_file) || !ltt_destroy_trace_file) {
		printk(KERN_ERR
		       "ltt_channel_control_init: create file of %s failed\n",
		       LTT_DESTROY_TRACE_FILE);
		err = -ENOMEM;
		goto err_create_destroy_trace_file;
	}

	markers_control_dir = debugfs_create_dir(MARKERS_CONTROL_DIR,
						 ltt_root_dentry);
	if (IS_ERR(markers_control_dir) || !markers_control_dir) {
		printk(KERN_ERR
		       "ltt_channel_control_init: create dir of %s failed\n",
		       MARKERS_CONTROL_DIR);
		err = -ENOMEM;
		goto err_create_marker_control_dir;
	}

	init_marker_dir(markers_control_dir, &root_dir_opt);

	if (build_marker_control_files())
		goto err_build_fail;

	if (!register_module_notifier(&module_nb))
		return 0;

err_build_fail:
	debugfs_remove_recursive(markers_control_dir);
	markers_control_dir = NULL;
err_create_marker_control_dir:
	debugfs_remove(ltt_destroy_trace_file);
err_create_destroy_trace_file:
	debugfs_remove(ltt_setup_trace_file);
err_create_setup_trace_file:
	debugfs_remove(ltt_control_dir);
err_create_control_dir:
err_no_root:
	return err;
}

static void __exit ltt_trace_control_exit(void)
{
	struct dentry *trace_dir;

	/* destory all traces */
	list_for_each_entry(trace_dir, &ltt_control_dir->d_subdirs,
			    d_u.d_child) {
		ltt_trace_stop(trace_dir->d_name.name);
		ltt_trace_destroy(trace_dir->d_name.name);
	}

	/* clean dirs in debugfs */
	debugfs_remove(ltt_setup_trace_file);
	debugfs_remove(ltt_destroy_trace_file);
	debugfs_remove_recursive(ltt_control_dir);
	debugfs_remove_recursive(markers_control_dir);
	unregister_module_notifier(&module_nb);
	put_ltt_root();
}

module_init(ltt_trace_control_init);
module_exit(ltt_trace_control_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Zhao Lei <zhaolei@cn.fujitsu.com>");
MODULE_DESCRIPTION("Linux Trace Toolkit Trace Controller");
