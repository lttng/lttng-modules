/*
 * lttng.c
 *
 * LTTng logger ABI
 *
 * Copyright (C) 2008-2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <linux/module.h>
#include <linux/tracepoint.h>
#include <linux/uaccess.h>
#include <linux/gfp.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include "../wrapper/vmalloc.h"
#include "../lttng-events.h"

#define TP_MODULE_NOAUTOLOAD
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH ../instrumentation/events/lttng-module
#define TRACE_INCLUDE_FILE lttng

#include "../instrumentation/events/lttng-module/lttng.h"

/* Events written through logger are truncated at 1024 bytes */
#define LTTNG_LOGGER_COUNT_MAX	1024
#define LTTNG_LOGGER_FILE	"lttng-logger"

static struct proc_dir_entry *lttng_logger_dentry;

/**
 * lttng_logger_write - write a userspace string into the trace system
 * @file: file pointer
 * @user_buf: user string
 * @count: length to copy
 * @ppos: file position
 *
 * Copy a userspace string into a trace event named "lttng:logger".
 * Copies at most @count bytes into the event "msg" dynamic array.
 * Truncates the count at LTTNG_LOGGER_COUNT_MAX. Returns the number of
 * bytes copied from the source.
 * Return -1 on error, with EFAULT errno.
 */
static
ssize_t lttng_logger_write(struct file *file, const char __user *user_buf,
		    size_t count, loff_t *ppos)
{
	unsigned int nr_pages = 1, i;
	unsigned long uaddr = (unsigned long) user_buf;
	struct page *pages[2];
	ssize_t written;
	int ret;

	/* Truncate count */
	if (unlikely(count > LTTNG_LOGGER_COUNT_MAX))
		count = LTTNG_LOGGER_COUNT_MAX;

	/* How many pages are we dealing with ? */
	if (unlikely((uaddr & PAGE_MASK) != ((uaddr + count) & PAGE_MASK)))
		nr_pages = 2;

	/* Pin userspace pages */
	ret = get_user_pages_fast(uaddr, nr_pages, 0, pages);
	if (unlikely(ret < nr_pages)) {
		if (ret > 0) {
			BUG_ON(ret != 1);
			put_page(pages[0]);
		}
		written = -EFAULT;
		goto end;
	}

	/* Trace the event */
	trace_lttng_logger(user_buf, count);
	written = count;
	*ppos += written;

	for (i = 0; i < nr_pages; i++)
		put_page(pages[i]);
end:
	return written;
}

static const struct file_operations lttng_logger_operations = {
	.write = lttng_logger_write,
};

int __init lttng_logger_init(void)
{
	int ret = 0;

	wrapper_vmalloc_sync_all();
	lttng_logger_dentry = proc_create_data(LTTNG_LOGGER_FILE,
				S_IRUGO | S_IWUGO, NULL,
				&lttng_logger_operations, NULL);
	if (!lttng_logger_dentry) {
		printk(KERN_ERR "Error creating LTTng logger file\n");
		ret = -ENOMEM;
		goto error;
	}
	ret = __lttng_events_init__lttng();
	if (ret)
		goto error_events;
	return ret;

error_events:
	remove_proc_entry("lttng-logger", NULL);
error:
	return ret;
}

void __exit lttng_logger_exit(void)
{
	__lttng_events_exit__lttng();
	if (lttng_logger_dentry)
		remove_proc_entry("lttng-logger", NULL);
}
