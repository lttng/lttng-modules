/*
 * Copyright (C) 2008 Mathieu Desnoyers
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/marker.h>
#include <linux/uaccess.h>
#include <linux/gfp.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/slab.h>

#include "ltt-type-serializer.h"

#define LTT_WRITE_EVENT_FILE	"write_event"

DEFINE_MARKER(userspace, event, "string %s");
static struct dentry *ltt_event_file;

/**
 * write_event - write a userspace string into the trace system
 * @file: file pointer
 * @user_buf: user string
 * @count: length to copy, including the final NULL
 * @ppos: unused
 *
 * Copy a string into a trace event, in channel "userspace", event "event".
 * Copies until either \n or \0 is reached.
 * On success, returns the number of bytes copied from the source, including the
 * \n or \0 character (if there was one in the count range). It cannot return
 * more than count.
 * Inspired from tracing_mark_write implementation from Steven Rostedt and
 * Ingo Molnar.
 */
static
ssize_t write_event(struct file *file, const char __user *user_buf,
		    size_t count, loff_t *ppos)
{
	struct marker *marker;
	char *buf, *end;
	long copycount;
	ssize_t ret;

	buf = kmalloc(count + 1, GFP_KERNEL);
	if (!buf) {
		ret = -ENOMEM;
		goto string_out;
	}
	copycount = strncpy_from_user(buf, user_buf, count);
	if (copycount < 0) {
		ret = -EFAULT;
		goto string_err;
	}
	/* Cut from the first nil or newline. */
	buf[copycount] = '\0';
	end = strchr(buf, '\n');
	if (end) {
		*end = '\0';
		copycount = end - buf;
	}
	/* Add final \0 to copycount */
	copycount++;
	marker = &GET_MARKER(userspace, event);
	ltt_specialized_trace(marker, marker->single.probe_private, buf,
			      copycount, sizeof(char));
	/* If there is no \0 nor \n in count, do not return a larger value */
	ret = min_t(size_t, copycount, count);
string_err:
	kfree(buf);
string_out:
	return ret;
}

static const struct file_operations ltt_userspace_operations = {
	.write = write_event,
};

static int __init ltt_userspace_init(void)
{
	struct dentry *ltt_root_dentry;
	int err = 0;

	ltt_root_dentry = get_ltt_root();
	if (!ltt_root_dentry) {
		err = -ENOENT;
		goto err_no_root;
	}

	ltt_event_file = debugfs_create_file(LTT_WRITE_EVENT_FILE,
					     S_IWUGO,
					     ltt_root_dentry,
					     NULL,
					     &ltt_userspace_operations);
	if (IS_ERR(ltt_event_file) || !ltt_event_file) {
		printk(KERN_ERR
			"ltt_userspace_init: failed to create file %s\n",
			LTT_WRITE_EVENT_FILE);
		err = -EPERM;
		goto err_no_file;
	}

	return err;
err_no_file:
	put_ltt_root();
err_no_root:
	return err;
}

static void __exit ltt_userspace_exit(void)
{
	debugfs_remove(ltt_event_file);
	put_ltt_root();
}

module_init(ltt_userspace_init);
module_exit(ltt_userspace_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>");
MODULE_DESCRIPTION("Linux Trace Toolkit Userspace Event");
