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

#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/byteorder/generic.h>

#include <lttng-events.h>
#include <lttng-tracer.h>
#include <wrapper/tracepoint.h>
#include <wrapper/kstrtox.h>

#define TP_MODULE_NOAUTOLOAD
#define LTTNG_PACKAGE_BUILD
#define CREATE_TRACE_POINTS
#define TRACE_INCLUDE_PATH instrumentation/events/lttng-module
#define TRACE_INCLUDE_FILE lttng-test
#define LTTNG_INSTRUMENTATION
#include <instrumentation/events/lttng-module/lttng-test.h>

DEFINE_TRACE(lttng_test_filter_event);
DEFINE_TRACE(lttng_test_event_a);
DEFINE_TRACE(lttng_test_event_b);

#define LTTNG_TEST_FILTER_EVENT_FILE	"lttng-test-filter-event"
#define LTTNG_TEST_PROCFS_DIR		"lttng-test"
#define LTTNG_TEST_GENERATE_EVENTS	"gen-kernel-events"

#define LTTNG_WRITE_COUNT_MAX	64
#define LTTNG_TEST_SYM_LEN	40

/* Max number of events to generate in one call to avoid DoS */
#define LTTNG_TEST_EVENT_MAX	100

/* The max command length is 3 symbols and 2 spaces */
#define LTTNG_TEST_CMD_LEN	((LTTNG_TEST_SYM_LEN * 3) + 2)

static struct proc_dir_entry *lttng_test_filter_event_dentry;
static struct proc_dir_entry *lttng_test_dentry;
static struct proc_dir_entry *lttng_test_gen_events_dentry;

/**
 * lttng_test_method: defines a method, its argument and a run callback.
 * method_name: Name of the method
 * arg_name: Name of the argument (for usage message)
 * (*run): Function to call
 */
struct lttng_test_method {
	char method_name[LTTNG_TEST_SYM_LEN];
	char arg_name[LTTNG_TEST_SYM_LEN];
	int (*run)(char *arg, int id);
};

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

/**
 * event_a: triggers $arg lttng_test_event_a events
 * @arg: string containing the number of event to generate
 * @id: Unique identifier of the caller
 *
 * Returns 0 on success and a negative value on parsing error
 */
static int event_a(char *arg, int id)
{
	int i, ret;
	unsigned long iter;

	ret = kstrtoul(arg, 10, &iter);
	if (ret) {
		printk(KERN_ERR "Failed to parse %s arg", __func__);
		goto error;
	}

	if (iter > LTTNG_TEST_EVENT_MAX) {
		printk(KERN_ERR "Number of iteration (%lu) exceeding maximum ", iter);
		ret = -EDOM;
		goto error;
	}

	ret = 0;
	for (i = 0; i < iter; i++) {
		trace_lttng_test_event_a(id);
	}

error:
	return ret;
}

/**
 * event_b: triggers $arg lttng_test_event_b events
 * @arg: string containing the number of event to generate
 * @id: Unique identifier of the caller
 *
 * Returns 0 on success and a negative value on parsing error
 */
static int event_b(char *arg, int id)
{
	int i, ret;
	unsigned long iter;

	ret = kstrtoul(arg, 10, &iter);
	if (ret) {
		printk(KERN_ERR "Failed to parse %s arg", __func__);
		goto error;
	}

	if (iter > LTTNG_TEST_EVENT_MAX) {
		printk(KERN_ERR "Number of iteration (%lu) exceeding maximum ", iter);
		ret = -EDOM;
		goto error;
	}

	ret = 0;
	for (i = 0; i < iter; i++) {
		trace_lttng_test_event_b(id);
	}

error:
	return ret;
}

/**
 * Array of method descriptions.
 */
static struct lttng_test_method test_methods[] = {
	{"event_a", "count", &event_a},
	{"event_b", "count", &event_b},
};

/*
 * Returns the number of space-delimited words in a given string. Returns -1, if
 * non-ascii byte is found.
 */
static int count_word_in_str(char *str, size_t len)
{
	int i, count, prev_is_space;

	count = 0;
	prev_is_space = 1;
	for (i = 0; i < len && str[i] != '\0'; i++) {
		if (!isascii(str[i])) {
			count = -1;
			break;
		}

		switch (str[i]) {
		case ' ':
			prev_is_space = 1;
			break;
		default:
			if (prev_is_space) {
				count++;
				prev_is_space = 0;
			}
			break;
		}
	}
	return count;
}

/**
 * lttng_test_gen_events_write: run the speficied method with the specified
 * argument. The accepted syntax is:`<method_name> <arg> <unique-id>`
 * @file: file pointer
 * @user_buf: user string
 * @count: length to copy
 * @ppos: current position in the file
 *
 * Return -1 on error, with EFAULT errno. Returns count on success to report a
 * successful write to the caller.
 *
 */
static
ssize_t lttng_test_gen_events_write(struct file *file, const char __user *user_buf,
		    size_t count, loff_t *ppos)
{
	char *buf, *ptr;
	char *user_method, *user_arg, *user_id;
	int i, ret;
	size_t buf_len;
	unsigned long id;

	if (count > LTTNG_TEST_CMD_LEN) {
		ret = -EINVAL;
		goto kmalloc_err;
	}
	buf_len = count;

	buf = kmalloc(sizeof(char) * count, GFP_KERNEL);
	if (!buf) {
		ret = -ENOMEM;
		goto kmalloc_err;
	}
	ret = copy_from_user(buf, user_buf, count);
	if (ret) {
		printk(KERN_ERR "copy from user error\n");
		ret = -EFAULT;
		goto error;
	}

	buf[count-1] = '\0';

	/* Check that the string is of the right format */
	if (count_word_in_str(buf, count) != 3) {
		printk(KERN_ERR "lttng_gen_kernel_event wrong input format");
		ret = -EINVAL;
		goto error;
	}

	/* Extract the three fields for the user request */
	ptr = buf;
	user_method = strsep(&ptr, " ");
	if (!user_method) {
		printk(KERN_ERR "strsep returned NULL");
		ret = -EINVAL;
		goto error;
	}

	if (strlen(user_method) >= LTTNG_TEST_SYM_LEN) {
		ret = -EINVAL;
		goto error;
	}

	user_arg = strsep(&ptr, " ");
	if (!user_arg) {
		printk(KERN_ERR "strsep returned NULL");
		ret = -EINVAL;
		goto error;
	}

	if (strlen(user_arg) >= LTTNG_TEST_SYM_LEN) {
		ret = -EINVAL;
		goto error;
	}

	user_id = ptr;
	if (strlen(user_id) >= (LTTNG_TEST_SYM_LEN)) {
		ret = -EINVAL;
		goto error;
	}
	/* Convert the uuid from string to a int */
	ret = kstrtoul(user_id, 10, &id);
	if (ret) {
		printk(KERN_ERR "Failed to parse method uuid");
		ret = -EINVAL;
		goto error;
	}

	/*
	 * Iterate over all the methods and compare the name of the
	 * method and the string from the user
	 */
	ret = -EINVAL;
	for (i = 0; i < ARRAY_SIZE(test_methods); i++) {
		if (strcmp(user_method, test_methods[i].method_name) != 0) {
			continue;
		}

		/*
		 * Once a match is found, the method is called
		 * and the function returns
		 */
		ret = test_methods[i].run(user_arg, id);

		/*
		 * If the call is successful, return the number
		 * of count passed to write to return success to
		 * the caller.
		 */
		if (!ret) {
			ret = count;
		}
		break;
	}
error:
	kfree(buf);
kmalloc_err:
	return ret;
}

static int lttng_test_gen_events_show(struct seq_file *m, void *v)
{
	int i;
	seq_printf(m, "Available methods:\n");
	for (i = 0; i <  ARRAY_SIZE(test_methods); i++) {
		seq_printf(m, "%s %s unique-id\n", test_methods[i].method_name,
				test_methods[i].arg_name);
	}
	return 0;
}

static int lttng_test_gen_events_open(struct inode *inode, struct file *file)
{
	return single_open(file, lttng_test_gen_events_show, NULL);
}

static const struct file_operations lttng_test_gen_events_operations = {
	.write = lttng_test_gen_events_write,
	.open = lttng_test_gen_events_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static
int __init lttng_test_init(void)
{
	int ret = 0;

	(void) wrapper_lttng_fixup_sig(THIS_MODULE);
	wrapper_vmalloc_sync_all();
	/*
	 * The lttng_test_filter_event file is left in `/proc`
	 * for backward compatibility reason.
	 */
	lttng_test_filter_event_dentry =
			proc_create_data(LTTNG_TEST_FILTER_EVENT_FILE,
				S_IRUGO | S_IWUGO, NULL,
				&lttng_test_filter_event_operations, NULL);
	if (!lttng_test_filter_event_dentry) {
		printk(KERN_ERR "Error creating LTTng test filter file\n");
		ret = -ENOMEM;
		goto error;
	}
	/* Create lttng-test proc directory */
	lttng_test_dentry = proc_mkdir(LTTNG_TEST_PROCFS_DIR, NULL);
	if (!lttng_test_dentry) {
		printk(KERN_ERR "Error creating LTTng test directory\n");
		ret = -ENOMEM;
		goto error_proc_mkdir;
	}

	lttng_test_gen_events_dentry =
		proc_create_data(LTTNG_TEST_GENERATE_EVENTS,
				S_IRUGO | S_IWUGO, lttng_test_dentry,
				&lttng_test_gen_events_operations, NULL);
	if (!lttng_test_gen_events_dentry) {
		printk(KERN_ERR "Error creating LTTng gen-events file\n");
		ret = -ENOMEM;
		goto error_proc_gen_events;
	}

	ret = __lttng_events_init__lttng_test();
	if (ret)
		goto error_events;
	return ret;

error_events:
	remove_proc_entry(LTTNG_TEST_GENERATE_EVENTS, lttng_test_dentry);
error_proc_gen_events:
	remove_proc_entry(LTTNG_TEST_PROCFS_DIR, NULL);
error_proc_mkdir:
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
	if (lttng_test_gen_events_dentry && lttng_test_dentry)
		remove_proc_entry(LTTNG_TEST_GENERATE_EVENTS, lttng_test_dentry);
	if (lttng_test_dentry)
		remove_proc_entry(LTTNG_TEST_PROCFS_DIR, NULL);
}

module_exit(lttng_test_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers <mathieu.desnoyers@efficios.com>");
MODULE_DESCRIPTION("LTTng Test");
MODULE_VERSION(__stringify(LTTNG_MODULES_MAJOR_VERSION) "."
	__stringify(LTTNG_MODULES_MINOR_VERSION) "."
	__stringify(LTTNG_MODULES_PATCHLEVEL_VERSION)
	LTTNG_MODULES_EXTRAVERSION);
