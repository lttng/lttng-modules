/*
 * lttng-test.c
 *
 * Linux Trace Toolkit Next Generation Test Module
 *
 * Copyright 2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/byteorder/generic.h>

#include "../lttng-events.h"
#include "../lttng-tracer.h"
#include "../wrapper/tracepoint.h"
#include "../wrapper/kstrtox.h"

#define TP_MODULE_NOAUTOLOAD
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH ../instrumentation/events/lttng-module
#define TRACE_INCLUDE_FILE lttng-test
#define LTTNG_INSTRUMENTATION
#include "../instrumentation/events/lttng-module/lttng-test.h"

DEFINE_TRACE(lttng_test_filter_event);

#define LTTNG_TEST_FILTER_EVENT_FILE	"lttng-test-filter-event"

#define LTTNG_WRITE_COUNT_MAX	64

static struct proc_dir_entry *lttng_test_filter_event_dentry;

static
void trace_test_event(unsigned int nr_iter)
{
	int i, netint;
	long values[] = { 1, 2, 3 };
	char text[10] = "test";
	char escape[10] = "\\*";

	for (i = 0; i < nr_iter; i++) {
		netint = htonl(i);
		trace_lttng_test_filter_event(i, netint, values, text, strlen(text), escape);
	}
}

/**
 * lttng_filter_event_write - trigger a lttng_test_filter_event
 * @file: file pointer
 * @user_buf: user string
 * @count: length to copy
 *
 * Return -1 on error, with EFAULT errno. Returns count on success.
 */
static
ssize_t lttng_test_filter_event_write(struct file *file, const char __user *user_buf,
		    size_t count, loff_t *ppos)
{
	unsigned int nr_iter;
	ssize_t written;
	int ret;

	/* Get the number of iterations */
	ret = lttng_kstrtouint_from_user(user_buf, count, 10, &nr_iter);
	if (ret) {
		written = ret;
		goto end;
	}
	/* Trace the event */
	trace_test_event(nr_iter);
	written = count;
	*ppos += written;
end:
	return written;
}

static const struct file_operations lttng_test_filter_event_operations = {
	.write = lttng_test_filter_event_write,
};

static
int __init lttng_test_init(void)
{
	int ret = 0;

	(void) wrapper_lttng_fixup_sig(THIS_MODULE);
	wrapper_vmalloc_sync_all();
	lttng_test_filter_event_dentry =
			proc_create_data(LTTNG_TEST_FILTER_EVENT_FILE,
				S_IRUGO | S_IWUGO, NULL,
				&lttng_test_filter_event_operations, NULL);
	if (!lttng_test_filter_event_dentry) {
		printk(KERN_ERR "Error creating LTTng test filter file\n");
		ret = -ENOMEM;
		goto error;
	}
	ret = __lttng_events_init__lttng_test();
	if (ret)
		goto error_events;
	return ret;

error_events:
	remove_proc_entry(LTTNG_TEST_FILTER_EVENT_FILE, NULL);
error:
	return ret;
}

module_init(lttng_test_init);

static
void __exit lttng_test_exit(void)
{
	__lttng_events_exit__lttng_test();
	if (lttng_test_filter_event_dentry)
		remove_proc_entry(LTTNG_TEST_FILTER_EVENT_FILE, NULL);
}

module_exit(lttng_test_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng Test");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
