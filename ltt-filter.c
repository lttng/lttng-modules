/*
 * Copyright (C) 2008 Mathieu Desnoyers
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/mutex.h>

#include "ltt-tracer.h"

#define LTT_FILTER_DIR	"filter"

/*
 * Protects the ltt_filter_dir allocation.
 */
static DEFINE_MUTEX(ltt_filter_mutex);

static struct dentry *ltt_filter_dir;

struct dentry *get_filter_root(void)
{
	struct dentry *ltt_root_dentry;

	mutex_lock(&ltt_filter_mutex);
	if (!ltt_filter_dir) {
		ltt_root_dentry = get_ltt_root();
		if (!ltt_root_dentry)
			goto err_no_root;

		ltt_filter_dir = debugfs_create_dir(LTT_FILTER_DIR,
						    ltt_root_dentry);
		if (!ltt_filter_dir)
			printk(KERN_ERR
			       "ltt_filter_init: failed to create dir %s\n",
			       LTT_FILTER_DIR);
	}
err_no_root:
	mutex_unlock(&ltt_filter_mutex);
	return ltt_filter_dir;
}
EXPORT_SYMBOL_GPL(get_filter_root);

static void __exit ltt_filter_exit(void)
{
	debugfs_remove(ltt_filter_dir);
}

module_exit(ltt_filter_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>");
MODULE_DESCRIPTION("Linux Trace Toolkit Filter");
