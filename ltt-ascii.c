/*
 * LTT ascii binary buffer to ascii converter.
 *
 * Copyright       2008 - 2009   Lai Jiangshan (laijs@cn.fujitsu.com)
 * Copyright       2009 -        Mathieu Desnoyers mathieu.desnoyers@polymtl.ca
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

/*
 * TODO
 *
 * Move to new switch behavior: Wait for data for the duration of the
 * timer interval + safety, if none is coming, consider that no activity occured
 * in the buffer.
 *
 * Fix case when having a text file open and destroying trace.
 *
 * - Automate periodical switch:
 *
 * The debugfs file "switch_timer" receives a timer period as parameter
 * (e.g. echo 100 > switch_timer) to activate the timer per channel. This can
 * also be accessed through the internal API _before the trace session starts_.
 * This timer will insure that we periodically have subbuffers to read, and
 * therefore that the merge-sort does not wait endlessly for a subbuffer.
 *
 * - If a channel is switched and read without data, make sure it is still
 * considered afterward (not removed from the queue).
 *
 * - Create a ascii/tracename/ALL file to merge-sort all active channels.
 * - Create a ascii/tracename/README file to contain the text output legend.
 * - Remove leading zeroes from timestamps.
 * - Enhance pretty-printing to make sure all types used for addesses output in
 * the form 0xAB00000000 (not decimal). This is true for %p and 0x%...X.
 * - Hotplug support
 */




#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/cpu.h>
#include <linux/fs.h>

#include "ltt-tracer.h"
#include "ltt-relay.h"
#include "ltt-relay-lockless.h"

#if 0
#define DEBUGP printk
#else
#define DEBUGP(fmt , a...)
#endif

struct dentry *ltt_ascii_dir_dentry;
EXPORT_SYMBOL_GPL(ltt_ascii_dir_dentry);

struct ltt_relay_iter;

struct ltt_relay_cpu_iter {
	/* cpu buffer information */
	struct ltt_chanbuf *buf;
	struct ltt_relay_iter *iter;
	int sb_ref;		/* holding a reference to a subbuffer */
	long read_sb_offset;	/* offset of the subbuffer read */

	/* current event information */
	struct ltt_subbuffer_header *header;
	long hdr_offset;	/* event header offset */
	long payload_offset;	/* event payload offset */
	u64 tsc;	/* full 64-bits timestamp value */
	u32 data_size;
	u16 chID;	/* channel ID, const */
	u16 eID;
};

struct ltt_relay_iter {
	struct ltt_relay_cpu_iter iter_cpu[NR_CPUS];
	struct ltt_chan *chan;
	loff_t pos;
	int cpu;
	int nr_refs;
};

/*
 * offset of 0 in subbuffer means "subbuf size" (filled subbuffer).
 */
static int is_subbuffer_offset_end(struct ltt_relay_cpu_iter *citer,
				   long offset)
{
	struct ltt_chan *chan = container_of(citer->buf->a.chan,
					     struct ltt_chan, a);
	long sub_offset = SUBBUF_OFFSET(offset - 1, chan) + 1;

	return (sub_offset <= citer->header->data_size);
}

static u64 calculate_tsc(u64 pre_tsc, u64 read_tsc, unsigned int rflags)
{
	u64 new_tsc = read_tsc;

	if (rflags != LTT_RFLAG_ID_SIZE_TSC) {
		BUG_ON(read_tsc >> LTT_TSC_BITS);

		new_tsc = (pre_tsc & ~LTT_TSC_MASK) + read_tsc;
		if (read_tsc < (pre_tsc & LTT_TSC_MASK))
			new_tsc += 1UL << LTT_TSC_BITS;
	}

	return new_tsc;
}

/*
 * calculate payload offset */
static inline long calculate_payload_offset(long offset, u16 chID, u16 eID)
{
	const char *fmt;

	if (!ltt_get_alignment())
		return offset;

	fmt = marker_get_fmt_from_id(chID, eID);
	BUG_ON(!fmt);

	return offset + ltt_fmt_largest_align(offset, fmt);
}

static void update_new_event(struct ltt_relay_cpu_iter *citer, long hdr_offset)
{
	u64 read_tsc;
	unsigned int rflags;
	long tmp_offset;

	WARN_ON_ONCE(hdr_offset != citer->hdr_offset);

	tmp_offset = ltt_read_event_header(&citer->buf->a, hdr_offset,
					   &read_tsc, &citer->data_size,
					   &citer->eID, &rflags);
	citer->payload_offset = calculate_payload_offset(tmp_offset,
							 citer->chID,
							 citer->eID);

	citer->tsc = calculate_tsc(citer->tsc, read_tsc, rflags);
}

static void update_event_size(struct ltt_relay_cpu_iter *citer, long hdr_offset)
{
	char output[1];
	const char *fmt;
	size_t data_size;

	if (citer->data_size != INT_MAX)
		return;

	fmt = marker_get_fmt_from_id(citer->chID, citer->eID);
	BUG_ON(!fmt);
	ltt_serialize_printf(citer->buf, citer->payload_offset,
			     &data_size, output, 0, fmt);
	citer->data_size = data_size;
}

static void update_cpu_iter(struct ltt_relay_cpu_iter *citer, long hdr_offset)
{
	if (unlikely((!citer->sb_ref)
		     || is_subbuffer_offset_end(citer, hdr_offset))) {
		citer->header = NULL;
		return;
	}
	update_new_event(citer, hdr_offset);
	update_event_size(citer, hdr_offset);
}

/*
 * returns 0 if we get a subbuffer reference.
 * else, the buffer has not available data, try again later.
 */
static int subbuffer_start(struct ltt_relay_cpu_iter *citer, long *offset)
{
	int ret;
	struct ltt_relay_iter *iter = citer->iter;

	ret = ltt_chanbuf_get_subbuf(citer->buf, offset);
	if (!ret) {
		citer->header = ltt_relay_read_offset_address(&citer->buf->a,
							      *offset);
		citer->hdr_offset = (*offset) + ltt_sb_header_size();
		citer->tsc = citer->header->cycle_count_begin;
		iter->nr_refs++;
		citer->sb_ref = 1;
		return 0;
	} else {
		if (ltt_chanbuf_is_finalized(citer->buf))
			return -ENODATA;
		else
			return -EAGAIN;
	}
}

static void subbuffer_stop(struct ltt_relay_cpu_iter *citer,
			   long offset)
{
	int ret;
	struct ltt_relay_iter *iter = citer->iter;

	WARN_ON_ONCE(!citer->sb_ref);
	ret = ltt_chanbuf_put_subbuf(citer->buf, offset);
	WARN_ON_ONCE(ret);
	citer->sb_ref = 0;
	iter->nr_refs--;
}

static void ltt_relay_advance_cpu_iter(struct ltt_relay_cpu_iter *citer)
{
	long old_offset = citer->payload_offset;
	long new_offset = citer->payload_offset;
	int ret;

	/* find that whether we read all data in this subbuffer */
	if (unlikely(is_subbuffer_offset_end(citer,
					     old_offset + citer->data_size))) {
		DEBUGP(KERN_DEBUG "LTT ASCII stop cpu %d offset %lX\n",
		       citer->buf->a.cpu, citer->read_sb_offset);
		subbuffer_stop(citer, citer->read_sb_offset);
		for (;;) {
			ret = subbuffer_start(citer, &citer->read_sb_offset);
			DEBUGP(KERN_DEBUG
			       "LTT ASCII start cpu %d ret %d offset %lX\n",
			       citer->buf->a.cpu, ret, citer->read_sb_offset);
			if (!ret || ret == -ENODATA) {
				break;	/* got data, or finalized */
			} else {	/* -EAGAIN */
				if (signal_pending(current))
					break;
				schedule_timeout_interruptible(1);
				//TODO: check for no-data delay. take ref. break
			}
		}
	} else {
		new_offset += citer->data_size;
		citer->hdr_offset = new_offset + ltt_align(new_offset, sizeof(struct ltt_event_header));
		DEBUGP(KERN_DEBUG
		       "LTT ASCII old_offset %lX new_offset %lX cpu %d\n",
		       old_offset, new_offset, citer->buf->a.cpu);
	}

	update_cpu_iter(citer, citer->hdr_offset);
}

static int cpu_iter_eof(struct ltt_relay_cpu_iter *citer)
{
	return !citer->sb_ref;
}

static int ltt_relay_iter_eof(struct ltt_relay_iter *iter)
{
	return iter->nr_refs == 0;
}

static void ltt_relay_advance_iter(struct ltt_relay_iter *iter)
{
	int i;
	struct ltt_relay_cpu_iter *curr, *min = NULL;
	iter->cpu = -1;

	/*
	 * find the event with the minimum tsc.
	 * TODO: use min-heep for 4096CPUS
	 */
	for_each_possible_cpu(i) {
		curr = &iter->iter_cpu[i];

		if (!curr->buf->a.allocated || !curr->header)
			continue;

		if (cpu_iter_eof(curr))
			continue;

		if (!min || curr->tsc < min->tsc) {
			min = curr;
			iter->cpu = i;
		}
	}

	/* update cpu_iter for next ltt_relay_advance_iter() */
	if (min)
		ltt_relay_advance_cpu_iter(min);
}

static void *ascii_next(struct seq_file *m, void *v, loff_t *ppos)
{
	struct ltt_relay_iter *iter = m->private;

	WARN_ON_ONCE(!iter->nr_refs);
	BUG_ON(v != iter);

	ltt_relay_advance_iter(iter);
	return (ltt_relay_iter_eof(iter) || signal_pending(current))
		? NULL : iter;
}

static void *ascii_start(struct seq_file *m, loff_t *ppos)
{
	struct ltt_relay_iter *iter = m->private;

	ltt_relay_advance_iter(iter);
	return (ltt_relay_iter_eof(iter) || signal_pending(current))
		? NULL : iter;
}

static void ascii_stop(struct seq_file *m, void *v)
{
}

static
int seq_serialize(struct seq_file *m, struct ltt_chanbuf *buf,
		  size_t buf_offset, const char *fmt, size_t *data_size)
{
	int len;

	if (m->count < m->size) {
		len = ltt_serialize_printf(buf, buf_offset, data_size,
					   m->buf + m->count,
					   m->size - m->count, fmt);
		if (m->count + len < m->size) {
			m->count += len;
			return 0;
		}
	}

	m->count = m->size;
	return -1;
}

static int ascii_show(struct seq_file *m, void *v)
{
	struct ltt_relay_iter *iter = v;
	struct ltt_relay_cpu_iter *citer;
	const char *name;
	const char *fmt;
	unsigned long long tsc;
	size_t data_size;

	if (iter->cpu == -1)
		return 0;

	citer = &iter->iter_cpu[iter->cpu];
	WARN_ON_ONCE(!citer->sb_ref);
	/*
	 * Nothing to show, we are at the end of the last subbuffer currently
	 * having data.
	 */
	if (!citer->header)
		return 0;

	tsc = citer->tsc;
	name = marker_get_name_from_id(citer->chID, citer->eID);
	fmt = marker_get_fmt_from_id(citer->chID, citer->eID);

	if (!name || !fmt)
		return 0;

	seq_printf(m, "event:%16.16s: cpu:%2d time:%20.20llu ",
		   name, iter->cpu, tsc);
	seq_serialize(m, citer->buf, citer->payload_offset, fmt, &data_size);
	seq_puts(m, "\n");
	if (citer->data_size == INT_MAX)
		citer->data_size = data_size;

	return 0;
}

static struct seq_operations ascii_seq_ops = {
	.start		= ascii_start,
	.next		= ascii_next,
	.stop		= ascii_stop,
	.show		= ascii_show,
};

/* FIXME : cpu hotplug support */
static int ltt_relay_iter_open_channel(struct ltt_relay_iter *iter,
				       struct ltt_chan *chan)
{
	int i, ret;
	u16 chID = ltt_channels_get_index_from_name(chan->a.filename);

	/* we don't need lock relay_channels_mutex */
	for_each_possible_cpu(i) {
		struct ltt_relay_cpu_iter *citer = &iter->iter_cpu[i];

		citer->buf = per_cpu_ptr(chan->a.buf, i);
		if (!citer->buf->a.allocated)
			continue;

		citer->iter = iter;	/* easy lazy parent info */
		citer->chID = chID;

		ret = ltt_chanbuf_open_read(citer->buf);
		if (ret) {
			/* Failed to open a percpu buffer, close everything. */
			citer->buf = NULL;
			goto error;
		}

		for (;;) {
			ret = subbuffer_start(citer,
					      &citer->read_sb_offset);
			DEBUGP(KERN_DEBUG
				"LTT ASCII open start "
				"cpu %d ret %d offset %lX\n",
				citer->buf->a.cpu, ret, citer->read_sb_offset);
			if (!ret || ret == -ENODATA) {
				break;	/* got data, or finalized */
			} else {	/* -EAGAIN */
				if (signal_pending(current))
					break;
				schedule_timeout_interruptible(1);
			}
		}
		update_cpu_iter(citer, citer->hdr_offset);
	}
	if (!iter->nr_refs) {
		ret = -ENODATA;
		goto error;
	}

	return 0;

error:
	for_each_possible_cpu(i) {
		struct ltt_relay_cpu_iter *citer = &iter->iter_cpu[i];

		if (!citer->buf)
			break;

		if (citer->buf->a.allocated)
			ltt_chanbuf_release_read(citer->buf);
	}
	return ret;
}

/* FIXME : cpu hotplug support */
static int ltt_relay_iter_release_channel(struct ltt_relay_iter *iter)
{
	int i;

	for_each_possible_cpu(i) {
		struct ltt_relay_cpu_iter *citer = &iter->iter_cpu[i];

		if (citer->sb_ref) {
			WARN_ON_ONCE(!citer->buf->a.allocated);
			DEBUGP(KERN_DEBUG
				"LTT ASCII release stop cpu %d offset %lX\n",
				citer->buf->a.cpu, citer->read_sb_offset);
			subbuffer_stop(&iter->iter_cpu[i],
				       citer->read_sb_offset);
		}
		if (citer->buf->a.allocated)
			ltt_chanbuf_release_read(citer->buf);
	}
	WARN_ON_ONCE(iter->nr_refs);
	return 0;
}

static int ltt_relay_ascii_open(struct inode *inode, struct file *file)
{
	int ret;
	struct ltt_chan *chan = inode->i_private;
	struct ltt_relay_iter *iter = kzalloc(sizeof(*iter), GFP_KERNEL);
	if (!iter)
		return -ENOMEM;

	iter->chan = chan;
	ret = ltt_relay_iter_open_channel(iter, chan);
	if (ret)
		goto error_free_alloc;

	ret = seq_open(file, &ascii_seq_ops);
	if (ret)
		goto error_release_channel;
	((struct seq_file *)file->private_data)->private = iter;
	return 0;

error_release_channel:
	ltt_relay_iter_release_channel(iter);
error_free_alloc:
	kfree(iter);
	return ret;
}

static int ltt_relay_ascii_release(struct inode *inode, struct file *file)
{
	struct seq_file *seq = file->private_data;
	struct ltt_relay_iter *iter = seq->private;

	ltt_relay_iter_release_channel(iter);
	kfree(iter);
	return 0;
}

static struct file_operations ltt_ascii_fops =
{
	.read = seq_read,
	.open = ltt_relay_ascii_open,
	.release = ltt_relay_ascii_release,
	.llseek = no_llseek,
	.owner = THIS_MODULE,
};

int ltt_ascii_create(struct ltt_chan *chan)
{
	struct dentry *dentry;

	dentry = debugfs_create_file(chan->a.filename,
				     S_IRUSR | S_IRGRP,
				     chan->a.trace->dentry.ascii_root,
				     chan, &ltt_ascii_fops);
	if (IS_ERR(dentry))
		return PTR_ERR(dentry);

	if (!dentry)
		return -EEXIST;

	chan->a.ascii_dentry = dentry;
	dentry->d_inode->i_private = chan;
	return 0;
}
EXPORT_SYMBOL_GPL(ltt_ascii_create);

void ltt_ascii_remove(struct ltt_chan *chan)
{
	struct dentry *dentry;

	dentry = dget(chan->a.ascii_dentry);
	debugfs_remove(dentry);
	/* TODO: wait / wakeup instead */
	/*
	 * Wait for every reference to the dentry to be gone,
	 * except us.
	 */
	while (atomic_read(&dentry->d_count) != 1)
		msleep(100);
	dput(dentry);
}
EXPORT_SYMBOL_GPL(ltt_ascii_remove);

int ltt_ascii_create_dir(struct ltt_trace *new_trace)
{
	new_trace->dentry.ascii_root = debugfs_create_dir(new_trace->trace_name,
							  ltt_ascii_dir_dentry);
	if (!new_trace->dentry.ascii_root)
		return -EEXIST;
	return 0;
}
EXPORT_SYMBOL_GPL(ltt_ascii_create_dir);

void ltt_ascii_remove_dir(struct ltt_trace *trace)
{
	debugfs_remove(trace->dentry.ascii_root);
}
EXPORT_SYMBOL_GPL(ltt_ascii_remove_dir);

__init int ltt_ascii_init(void)
{
	ltt_ascii_dir_dentry = debugfs_create_dir(LTT_ASCII, get_ltt_root());

	return ltt_ascii_dir_dentry ? 0 : -EFAULT;
}

__exit void ltt_ascii_exit(void)
{
	debugfs_remove(ltt_ascii_dir_dentry);
	put_ltt_root();
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Lai Jiangshan@FNST and Mathieu Desnoyers");
MODULE_DESCRIPTION("Linux Trace Toolkit Next Generation Ascii Converter");
