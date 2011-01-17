/*
 * (C) Copyright	2009 -
 * 		Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * LTTng kprobes integration module.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/marker.h>
#include <linux/mutex.h>
#include <linux/jhash.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <linux/kallsyms.h>

#include "ltt-type-serializer.h"
#include "ltt-tracer.h"

#define LTT_KPROBES_DIR 	"kprobes"
#define LTT_KPROBES_ENABLE	"enable"
#define LTT_KPROBES_DISABLE	"disable"
#define LTT_KPROBES_LIST	"list"

/* Active LTTng kprobes hash table */
static DEFINE_MUTEX(ltt_kprobes_mutex);

#define LTT_KPROBE_HASH_BITS	6
#define LTT_KPROBE_TABLE_SIZE	(1 << LTT_KPROBE_HASH_BITS)
static struct hlist_head ltt_kprobe_table[LTT_KPROBE_TABLE_SIZE];

struct kprobe_entry {
	struct hlist_node hlist;
	struct kprobe kp;
	char key[0];
};

static struct dentry *ltt_kprobes_dir,
		     *ltt_kprobes_enable_dentry,
		     *ltt_kprobes_disable_dentry,
		     *ltt_kprobes_list_dentry;

static int module_exit;


static void trace_kprobe_table_entry(void *call_data, struct kprobe_entry *e)
{
	unsigned long addr;
	char *namebuf = (char *)__get_free_page(GFP_KERNEL);

	if (e->kp.addr) {
		sprint_symbol(namebuf, (unsigned long)e->kp.addr);
		addr = (unsigned long)e->kp.addr;
	} else {
		strncpy(namebuf, e->kp.symbol_name, PAGE_SIZE - 1);
		/* TODO : add offset */
		addr = kallsyms_lookup_name(namebuf);
	}
	if (addr)
		__trace_mark(0, kprobe_state, kprobe_table, call_data,
			     "ip 0x%lX symbol %s", addr, namebuf);
	free_page((unsigned long)namebuf);
}

DEFINE_MARKER(kernel, kprobe, "ip %lX");

static int ltt_kprobe_handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	struct marker *marker;
	unsigned long data;

	data = (unsigned long)p->addr;
	marker = &GET_MARKER(kernel, kprobe);
	ltt_specialized_trace(marker, marker->single.probe_private,
			      &data, sizeof(data), sizeof(data));
	return 0;
}

static int ltt_register_kprobe(const char *key)
{
	struct hlist_head *head;
	struct hlist_node *node;
	struct kprobe_entry *e = NULL;
	char *symbol_name = NULL;
	unsigned long addr;
	unsigned int offset = 0;
	u32 hash;
	size_t key_len = strlen(key) + 1;
	int ret;

	if (key_len == 1)
		return -ENOENT;	/* only \0 */

	if (sscanf(key, "%li", &addr) != 1)
		addr = 0;

	if (!addr) {
		const char *symbol_end = NULL;
		unsigned int symbol_len;	/* includes final \0 */

		symbol_end = strchr(key, ' ');
		if (symbol_end)
			symbol_len = symbol_end - key + 1;
		else
			symbol_len = key_len;
		symbol_name = kmalloc(symbol_len, GFP_KERNEL);
		if (!symbol_name) {
			ret = -ENOMEM;
			goto error;
		}
		memcpy(symbol_name, key, symbol_len - 1);
		symbol_name[symbol_len-1] = '\0';
		if (symbol_end) {
			symbol_end++;	/* start of offset */
			if (sscanf(symbol_end, "%i", &offset) != 1)
				offset = 0;
		}
	}

	hash = jhash(key, key_len-1, 0);
	head = &ltt_kprobe_table[hash & ((1 << LTT_KPROBE_HASH_BITS)-1)];
	hlist_for_each_entry(e, node, head, hlist) {
		if (!strcmp(key, e->key)) {
			printk(KERN_NOTICE "Kprobe %s busy\n", key);
			ret = -EBUSY;
			goto error;
		}
	}
	/*
	 * Using kzalloc here to allocate a variable length element. Could
	 * cause some memory fragmentation if overused.
	 */
	e = kzalloc(sizeof(struct kprobe_entry) + key_len, GFP_KERNEL);
	if (!e) {
		ret = -ENOMEM;
		goto error;
	}
	memcpy(e->key, key, key_len);
	hlist_add_head(&e->hlist, head);
	e->kp.pre_handler = ltt_kprobe_handler_pre;
	e->kp.symbol_name = symbol_name;
	e->kp.offset = offset;
	e->kp.addr = (void *)addr;
	ret = register_kprobe(&e->kp);
	if (ret < 0)
		goto error_list_del;
	trace_kprobe_table_entry(NULL, e);
	return 0;

error_list_del:
	hlist_del(&e->hlist);
error:
	kfree(symbol_name);
	kfree(e);
	return ret;
}

static int ltt_unregister_kprobe(const char *key)
{
	struct hlist_head *head;
	struct hlist_node *node;
	struct kprobe_entry *e;
	int found = 0;
	size_t key_len = strlen(key) + 1;
	u32 hash;

	hash = jhash(key, key_len-1, 0);
	head = &ltt_kprobe_table[hash & ((1 << LTT_KPROBE_HASH_BITS)-1)];
	hlist_for_each_entry(e, node, head, hlist) {
		if (!strcmp(key, e->key)) {
			found = 1;
			break;
		}
	}
	if (!found)
		return -ENOENT;
	hlist_del(&e->hlist);
	unregister_kprobe(&e->kp);
	kfree(e->kp.symbol_name);
	kfree(e);
	return 0;
}

static void ltt_unregister_all_kprobes(void)
{
	struct kprobe_entry *e;
	struct hlist_head *head;
	struct hlist_node *node, *tmp;
	unsigned int i;

	for (i = 0; i < LTT_KPROBE_TABLE_SIZE; i++) {
		head = &ltt_kprobe_table[i];
		hlist_for_each_entry_safe(e, node, tmp, head, hlist) {
			hlist_del(&e->hlist);
			unregister_kprobe(&e->kp);
			kfree(e->kp.symbol_name);
			kfree(e);
		}
	}
}

/*
 * Allows to specify either
 * - symbol
 * - symbol offset
 * - address
 */
static ssize_t enable_op_write(struct file *file,
	const char __user *user_buf, size_t count, loff_t *ppos)
{
	int err, buf_size;
	char *end;
	char *buf = (char *)__get_free_page(GFP_KERNEL);

	mutex_lock(&ltt_kprobes_mutex);
	if (module_exit) {
		err = -EPERM;
		goto error;
	}

	buf_size = min_t(size_t, count, PAGE_SIZE - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto error;
	buf[buf_size] = '\0';
	end = strchr(buf, '\n');
	if (end)
		*end = '\0';
	err = ltt_register_kprobe(buf);
	if (err)
		goto error;

	mutex_unlock(&ltt_kprobes_mutex);
	free_page((unsigned long)buf);
	return count;
error:
	mutex_unlock(&ltt_kprobes_mutex);
	free_page((unsigned long)buf);
	return err;
}

static const struct file_operations ltt_kprobes_enable = {
	.write = enable_op_write,
};

static ssize_t disable_op_write(struct file *file,
	const char __user *user_buf, size_t count, loff_t *ppos)
{
	int err, buf_size;
	char *end;
	char *buf = (char *)__get_free_page(GFP_KERNEL);

	mutex_lock(&ltt_kprobes_mutex);
	if (module_exit)
		goto end;

	buf_size = min_t(size_t, count, PAGE_SIZE - 1);
	err = copy_from_user(buf, user_buf, buf_size);
	if (err)
		goto error;
	buf[buf_size] = '\0';
	end = strchr(buf, '\n');
	if (end)
		*end = '\0';
	err = ltt_unregister_kprobe(buf);
	if (err)
		goto error;
end:
	mutex_unlock(&ltt_kprobes_mutex);
	free_page((unsigned long)buf);
	return count;
error:
	mutex_unlock(&ltt_kprobes_mutex);
	free_page((unsigned long)buf);
	return err;
}

static const struct file_operations ltt_kprobes_disable = {
	.write = disable_op_write,
};

/*
 * This seqfile read is not perfectly safe, as a kprobe could be removed from
 * the hash table between two reads. This will result in an incomplete output.
 */
static struct kprobe_entry *ltt_find_next_kprobe(struct kprobe_entry *prev)
{
	struct kprobe_entry *e;
	struct hlist_head *head;
	struct hlist_node *node;
	unsigned int i;
	int found = 0;

	if (prev == (void *)-1UL)
		return NULL;

	if (!prev)
		found = 1;

	for (i = 0; i < LTT_KPROBE_TABLE_SIZE; i++) {
		head = &ltt_kprobe_table[i];
		hlist_for_each_entry(e, node, head, hlist) {
			if (found)
				return e;
			if (e == prev)
				found = 1;
		}
	}
	return NULL;
}

static void *lk_next(struct seq_file *m, void *p, loff_t *pos)
{
	m->private = ltt_find_next_kprobe(m->private);
	if (!m->private) {
		m->private = (void *)-1UL;
		return NULL;
	}
	return m->private;
}

static void *lk_start(struct seq_file *m, loff_t *pos)
{
	mutex_lock(&ltt_kprobes_mutex);
	if (!*pos)
		m->private = NULL;
	m->private = ltt_find_next_kprobe(m->private);
	if (!m->private) {
		m->private = (void *)-1UL;
		return NULL;
	}
	return m->private;
}

static void lk_stop(struct seq_file *m, void *p)
{
	mutex_unlock(&ltt_kprobes_mutex);
}

static int lk_show(struct seq_file *m, void *p)
{
	struct kprobe_entry *e = m->private;
	seq_printf(m, "%s\n", e->key);
	return 0;
}

static const struct seq_operations ltt_kprobes_list_op = {
	.start = lk_start,
	.next = lk_next,
	.stop = lk_stop,
	.show = lk_show,
};

static int ltt_kprobes_list_open(struct inode *inode, struct file *file)
{
	int ret;

	ret = seq_open(file, &ltt_kprobes_list_op);
	if (ret == 0)
		((struct seq_file *)file->private_data)->private = NULL;
	return ret;
}

static int ltt_kprobes_list_release(struct inode *inode, struct file *file)
{
	struct seq_file *seq = file->private_data;

	seq->private = NULL;
	return seq_release(inode, file);
}

static const struct file_operations ltt_kprobes_list = {
	.open = ltt_kprobes_list_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = ltt_kprobes_list_release,
};

/*
 * kprobes table dump. Callback invoked by ltt-statedump. ltt-statedump must
 * take a reference to this module before calling this callback.
 */
void ltt_dump_kprobes_table(void *call_data)
{
	struct kprobe_entry *e;
	struct hlist_head *head;
	struct hlist_node *node;
	unsigned int i;

	for (i = 0; i < LTT_KPROBE_TABLE_SIZE; i++) {
		head = &ltt_kprobe_table[i];
		hlist_for_each_entry(e, node, head, hlist)
			trace_kprobe_table_entry(call_data, e);
	}
}
EXPORT_SYMBOL_GPL(ltt_dump_kprobes_table);

static int __init ltt_kprobes_init(void)
{
	struct dentry *ltt_root_dentry;
	int ret = 0;

	printk(KERN_INFO "LTT : ltt-kprobes init\n");
	mutex_lock(&ltt_kprobes_mutex);

	ltt_root_dentry = get_ltt_root();
	if (!ltt_root_dentry) {
		ret = -ENOENT;
		goto err_no_root;
	}

	ltt_kprobes_dir = debugfs_create_dir(LTT_KPROBES_DIR, ltt_root_dentry);
	if (!ltt_kprobes_dir) {
		printk(KERN_ERR
		       "ltt_kprobes_init: failed to create dir %s\n",
			LTT_KPROBES_DIR);
		ret = -ENOMEM;
		goto err_no_dir;
	}

	ltt_kprobes_enable_dentry = debugfs_create_file(LTT_KPROBES_ENABLE,
							S_IWUSR,
							ltt_kprobes_dir, NULL,
							&ltt_kprobes_enable);
	if (IS_ERR(ltt_kprobes_enable_dentry) || !ltt_kprobes_enable_dentry) {
		printk(KERN_ERR
		       "ltt_kprobes_init: failed to create file %s\n",
			LTT_KPROBES_ENABLE);
		ret = -ENOMEM;
		goto err_no_enable;
	}

	ltt_kprobes_disable_dentry = debugfs_create_file(LTT_KPROBES_DISABLE,
							 S_IWUSR,
							 ltt_kprobes_dir, NULL,
							 &ltt_kprobes_disable);
	if (IS_ERR(ltt_kprobes_disable_dentry) || !ltt_kprobes_disable_dentry) {
		printk(KERN_ERR
		       "ltt_kprobes_init: failed to create file %s\n",
			LTT_KPROBES_DISABLE);
		ret = -ENOMEM;
		goto err_no_disable;
	}

	ltt_kprobes_list_dentry = debugfs_create_file(LTT_KPROBES_LIST,
						      S_IWUSR, ltt_kprobes_dir,
						      NULL, &ltt_kprobes_list);
	if (IS_ERR(ltt_kprobes_list_dentry) || !ltt_kprobes_list_dentry) {
		printk(KERN_ERR
		       "ltt_kprobes_init: failed to create file %s\n",
			LTT_KPROBES_LIST);
		ret = -ENOMEM;
		goto err_no_list;
	}
	ltt_statedump_register_kprobes_dump(ltt_dump_kprobes_table);

	mutex_unlock(&ltt_kprobes_mutex);
	return ret;

err_no_list:
	debugfs_remove(ltt_kprobes_disable_dentry);
err_no_disable:
	debugfs_remove(ltt_kprobes_enable_dentry);
err_no_enable:
	debugfs_remove(ltt_kprobes_dir);
err_no_dir:
err_no_root:
	mutex_unlock(&ltt_kprobes_mutex);
	return ret;
}
module_init(ltt_kprobes_init);

static void __exit ltt_kprobes_exit(void)
{
	printk(KERN_INFO "LTT : ltt-kprobes exit\n");
	mutex_lock(&ltt_kprobes_mutex);
	module_exit = 1;
	ltt_statedump_unregister_kprobes_dump(ltt_dump_kprobes_table);
	debugfs_remove(ltt_kprobes_list_dentry);
	debugfs_remove(ltt_kprobes_disable_dentry);
	debugfs_remove(ltt_kprobes_enable_dentry);
	debugfs_remove(ltt_kprobes_dir);
	ltt_unregister_all_kprobes();
	mutex_unlock(&ltt_kprobes_mutex);
}
module_exit(ltt_kprobes_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Linux Trace Toolkit Kprobes Support");
