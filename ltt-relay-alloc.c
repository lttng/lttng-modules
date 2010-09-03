/*
 * ltt-relay-alloc.c
 *
 * Copyright (C) 2008,2009 - Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/errno.h>
#include <linux/stddef.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/cpu.h>
#include <linux/bitops.h>
#include <linux/delay.h>

#include "ltt-relay.h"
#include "ltt-tracer.h"
#include "ltt-relay-lockless.h"	/* for cpu hotplug */

/**
 * ltt_chanbuf_allocate - allocate a channel buffer
 * @buf: the buffer struct
 * @size: total size of the buffer
 * @n_sb: number of subbuffers
 * @extra_reader_sb: need extra subbuffer for reader
 */
static
int ltt_chanbuf_allocate(struct ltt_chanbuf_alloc *buf, size_t size,
			 size_t n_sb, int extra_reader_sb)
{
	long i, j, n_pages, n_pages_per_sb, page_idx = 0;
	struct page **pages;
	void **virt;

	n_pages = size >> PAGE_SHIFT;
	n_pages_per_sb = n_pages >> get_count_order(n_sb);
	if (extra_reader_sb)
		n_pages += n_pages_per_sb;	/* Add pages for reader */

	pages = kmalloc_node(max_t(size_t, sizeof(*pages) * n_pages,
				   1 << INTERNODE_CACHE_SHIFT),
			GFP_KERNEL, cpu_to_node(buf->cpu));
	if (unlikely(!pages))
		goto pages_error;

	virt = kmalloc_node(ALIGN(sizeof(*virt) * n_pages,
				  1 << INTERNODE_CACHE_SHIFT),
			GFP_KERNEL, cpu_to_node(buf->cpu));
	if (unlikely(!virt))
		goto virt_error;

	for (i = 0; i < n_pages; i++) {
		pages[i] = alloc_pages_node(cpu_to_node(buf->cpu),
			GFP_KERNEL | __GFP_ZERO, 0);
		if (unlikely(!pages[i]))
			goto depopulate;
		virt[i] = page_address(pages[i]);
	}
	buf->nr_pages = n_pages;
	buf->_pages = pages;
	buf->_virt = virt;

	/* Allocate write-side page index */
	buf->buf_wsb = kzalloc_node(max_t(size_t,
				sizeof(struct chanbuf_sb) * n_sb,
				1 << INTERNODE_CACHE_SHIFT),
				GFP_KERNEL, cpu_to_node(buf->cpu));
	if (unlikely(!buf->buf_wsb))
		goto depopulate;

	for (i = 0; i < n_sb; i++) {
		buf->buf_wsb[i].pages =
			kzalloc_node(max_t(size_t,
				sizeof(struct chanbuf_page) * n_pages_per_sb,
				1 << INTERNODE_CACHE_SHIFT),
				GFP_KERNEL, cpu_to_node(buf->cpu));
		if (!buf->buf_wsb[i].pages)
			goto free_buf_wsb;
	}

	if (extra_reader_sb) {
		/* Allocate read-side page index */
		buf->buf_rsb.pages =
			kzalloc_node(max_t(size_t,
				sizeof(struct chanbuf_page) * n_pages_per_sb,
				1 << INTERNODE_CACHE_SHIFT),
				GFP_KERNEL, cpu_to_node(buf->cpu));
		if (unlikely(!buf->buf_rsb.pages))
			goto free_buf_wsb;
	} else {
		buf->buf_rsb.pages = buf->buf_wsb[0].pages;
	}

	/* Assign pages to write-side page index */
	for (i = 0; i < n_sb; i++) {
		for (j = 0; j < n_pages_per_sb; j++) {
			WARN_ON(page_idx > n_pages);
			buf->buf_wsb[i].pages[j].virt = virt[page_idx];
			buf->buf_wsb[i].pages[j].page = pages[page_idx];
			page_idx++;
		}
		RCHAN_SB_SET_NOREF(buf->buf_wsb[i].pages);
	}

	if (extra_reader_sb) {
		for (j = 0; j < n_pages_per_sb; j++) {
			WARN_ON(page_idx > n_pages);
			buf->buf_rsb.pages[j].virt = virt[page_idx];
			buf->buf_rsb.pages[j].page = pages[page_idx];
			page_idx++;
		}
		RCHAN_SB_SET_NOREF(buf->buf_rsb.pages);
	}

	/*
	 * If kmalloc ever uses vmalloc underneath, make sure the buffer pages
	 * will not fault.
	 */
	vmalloc_sync_all();
	return 0;

free_buf_wsb:
	for (i = 0; i < n_sb; i++) {
		RCHAN_SB_CLEAR_NOREF(buf->buf_wsb[i].pages);
		kfree(buf->buf_wsb[i].pages);
	}
	kfree(buf->buf_wsb);
depopulate:
	/*
	 * Free all pages from [ i - 1 down to 0 ].
	 * If i = 0, don't free anything.
	 */
	for (i--; i >= 0; i--)
		__free_page(pages[i]);
	kfree(virt);
virt_error:
	kfree(pages);
pages_error:
	return -ENOMEM;
}

int ltt_chanbuf_alloc_create(struct ltt_chanbuf_alloc *buf,
			     struct ltt_chan_alloc *chan, int cpu)
{
	int ret = 0;

	ret = ltt_chanbuf_allocate(buf, chan->buf_size, chan->n_sb,
				   chan->extra_reader_sb);
	if (ret)
		goto end;

	buf->chan = chan;
	buf->cpu = cpu;
end:
	return ret;
}

void ltt_chanbuf_alloc_free(struct ltt_chanbuf_alloc *buf)
{
	struct ltt_chan_alloc *chan = buf->chan;
	struct page **pages;
	long i;

	/* Destroy index */
	if (chan->extra_reader_sb) {
		RCHAN_SB_CLEAR_NOREF(buf->buf_rsb.pages);
		kfree(buf->buf_rsb.pages);
	}
	for (i = 0; i < chan->n_sb; i++) {
		RCHAN_SB_CLEAR_NOREF(buf->buf_wsb[i].pages);
		kfree(buf->buf_wsb[i].pages);
	}
	kfree(buf->buf_wsb);

	/* Destroy pages */
	pages = buf->_pages;
	for (i = 0; i < buf->nr_pages; i++)
		__free_page(pages[i]);
	kfree(buf->_pages);
	kfree(buf->_virt);
	buf->allocated = 0;
}

/**
 *	ltt_relay_hotcpu_callback - CPU hotplug callback
 *	@nb: notifier block
 *	@action: hotplug action to take
 *	@hcpu: CPU number
 *
 *	Returns the success/failure of the operation. (%NOTIFY_OK, %NOTIFY_BAD)
 */
static
int __cpuinit ltt_relay_hotcpu_callback(struct notifier_block *nb,
					unsigned long action,
					void *hcpu)
{
	unsigned int cpu = (unsigned long)hcpu;
	struct ltt_trace *trace;
	struct ltt_chan *chan;
	struct ltt_chanbuf *buf;
	int ret, i;

	switch (action) {
	case CPU_UP_PREPARE:
	case CPU_UP_PREPARE_FROZEN:
		/*
		 * CPU hotplug lock protects trace lock from this callback.
		 */
		__list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
			for (i = 0; i < trace->nr_channels; i++) {
				chan = &trace->channels[i];
				buf = per_cpu_ptr(chan->a.buf, cpu);
				ret = ltt_chanbuf_create(buf, &chan->a, cpu);
				if (ret) {
					printk(KERN_ERR
					  "ltt_relay_hotcpu_callback: cpu %d "
					  "buffer creation failed\n", cpu);
					return NOTIFY_BAD;
				}

			}
		}
		break;
	case CPU_DEAD:
	case CPU_DEAD_FROZEN:
		/* No need to do a buffer switch here, because it will happen
		 * when tracing is stopped, or will be done by switch timer CPU
		 * DEAD callback. */
		break;
	}
	return NOTIFY_OK;
}

/*
 * Must be called with either trace lock or rcu read lock sched held.
 */
void ltt_chan_for_each_channel(void (*cb) (struct ltt_chanbuf *buf), int cpu)
{
	struct ltt_trace *trace;
	struct ltt_chan *chan;
	struct ltt_chanbuf *buf;
	int i;

	__list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		for (i = 0; i < trace->nr_channels; i++) {
			chan = &trace->channels[i];
			if (!chan->active)
				continue;
			buf = per_cpu_ptr(chan->a.buf, cpu);
			cb(buf);
		}
	}
}

/**
 * ltt_chan_create - create a new relay channel
 * @chan: channel
 * @trace: trace
 * @base_filename: base name of files to create
 * @parent: dentry of parent directory, %NULL for root directory
 * @sb_size: size of sub-buffers (> PAGE_SIZE, power of 2)
 * @n_sb: number of sub-buffers (power of 2)
 * @extra_reader_sb: allocate an extra subbuffer for the reader
 * @overwrite: channel is in overwrite mode
 *
 * Returns channel pointer if successful, %NULL otherwise.
 *
 * Creates per-cpu channel buffers using the sizes and attributes
 * specified.  The created channel buffer files will be named
 * base_filename_0...base_filename_N-1.  File permissions will
 * be %S_IRUSR.
 */
int ltt_chan_alloc_init(struct ltt_chan_alloc *chan, struct ltt_trace *trace,
			const char *base_filename,
			struct dentry *parent, size_t sb_size,
			size_t n_sb, int extra_reader_sb, int overwrite)
{
	unsigned int i;
	int ret;

	if (!base_filename)
		return -EPERM;

	if (!(sb_size && n_sb))
		return -EPERM;

	/* Check that the subbuffer size is larger than a page. */
	WARN_ON_ONCE(sb_size < PAGE_SIZE);

	/*
	 * Make sure the number of subbuffers and subbuffer size are power of 2.
	 */
	WARN_ON_ONCE(hweight32(sb_size) != 1);
	WARN_ON(hweight32(n_sb) != 1);

	chan->trace = trace;
	chan->buf_size = n_sb * sb_size;
	chan->sb_size = sb_size;
	chan->sb_size_order = get_count_order(sb_size);
	chan->n_sb_order = get_count_order(n_sb);
	chan->extra_reader_sb = extra_reader_sb;
	chan->n_sb = n_sb;
	chan->parent = parent;
	strlcpy(chan->filename, base_filename, NAME_MAX);
	kref_init(&chan->kref);
	kref_get(&chan->trace->kref);

	/* Allocating the child structure */
	chan->buf = alloc_percpu(struct ltt_chanbuf);
	if (!chan->buf)
		goto free_chan;

	for_each_online_cpu(i) {
		ret = ltt_chanbuf_create(per_cpu_ptr(chan->buf, i), chan, i);
		if (ret)
			goto free_bufs;
	}

	return 0;

free_bufs:
	for_each_possible_cpu(i) {
		struct ltt_chanbuf *buf = per_cpu_ptr(chan->buf, i);

		if (!buf->a.allocated)
			continue;
		ltt_chanbuf_remove_file(buf);
		ltt_chanbuf_free(buf);
	}
	free_percpu(chan->buf);
free_chan:
	kref_put(&chan->kref, ltt_chan_free);
	return -ENOMEM;
}

/**
 * ltt_chan_alloc_remove_files - remove channel files.
 * @chan: the channel
 *
 * Remove all channel files and wait for dentry use counts to become zero.
 */
void ltt_chan_alloc_remove_files(struct ltt_chan_alloc *chan)
{
	unsigned int i;
	struct dentry *dentry;

	for_each_possible_cpu(i) {
		struct ltt_chanbuf *buf = per_cpu_ptr(chan->buf, i);

		if (!buf->a.allocated)
			continue;
		dentry = dget(buf->a.dentry);
		ltt_chanbuf_remove_file(buf);
		/* TODO: wait / wakeup instead */
		/*
		 * Wait for every reference to the dentry to be gone,
		 * except us.
		 */
		while (atomic_read(&dentry->d_count) != 1)
			msleep(100);
		dput(dentry);
	}
}

/**
 * ltt_chan_alloc_free - destroy the channel
 * @chan: the channel
 *
 * Destroy all channel buffers and frees the channel.
 */
void ltt_chan_alloc_free(struct ltt_chan_alloc *chan)
{
	unsigned int i;

	for_each_possible_cpu(i) {
		struct ltt_chanbuf *buf = per_cpu_ptr(chan->buf, i);

		if (!buf->a.allocated)
			continue;
		ltt_chanbuf_free(buf);
	}
	free_percpu(chan->buf);
	kref_put(&chan->trace->kref, ltt_release_trace);
	wake_up_interruptible(&chan->trace->kref_wq);
}

/**
 * _ltt_relay_write - write data to a ltt_relay buffer.
 * @bufa : buffer
 * @offset : offset within the buffer
 * @src : source address
 * @len : length to write
 * @pagecpy : page size copied so far
 */
void _ltt_relay_write(struct ltt_chanbuf_alloc *bufa, size_t offset,
		      const void *src, size_t len, ssize_t pagecpy)
{
	struct ltt_chan_alloc *chana = bufa->chan;
	size_t sbidx, index;
	struct chanbuf_page *rpages;

	do {
		len -= pagecpy;
		src += pagecpy;
		offset += pagecpy;
		sbidx = offset >> chana->sb_size_order;
		index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;

		/*
		 * Underlying layer should never ask for writes across
		 * subbuffers.
		 */
		WARN_ON(offset >= chana->buf_size);

		pagecpy = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		rpages = bufa->buf_wsb[sbidx].pages;
		WARN_ON_ONCE(RCHAN_SB_IS_NOREF(rpages));
		ltt_relay_do_copy(rpages[index].virt + (offset & ~PAGE_MASK),
				  src, pagecpy);
	} while (unlikely(len != pagecpy));
}
EXPORT_SYMBOL_GPL(_ltt_relay_write);

/**
 * _ltt_relay_strncpy_fixup - Fix an incomplete string in a ltt_relay buffer.
 * @bufa : buffer
 * @offset : offset within the buffer
 * @len : length to write
 * @copied: string actually copied
 * @terminated: does string end with \0
 *
 * Fills string with "X" if incomplete.
 */
void _ltt_relay_strncpy_fixup(struct ltt_chanbuf_alloc *bufa, size_t offset,
			      size_t len, size_t copied, int terminated)
{
	struct ltt_chan_alloc *chana = bufa->chan;
	size_t sbidx, index;
	ssize_t pagecpy;
	struct chanbuf_page *rpages;

	if (copied == len) {
		/*
		 * Deal with non-terminated string.
		 */
		WARN_ON_ONCE(terminated);
		offset += copied - 1;
		sbidx = offset >> chana->sb_size_order;
		index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;
		/*
		 * Underlying layer should never ask for writes across
		 * subbuffers.
		 */
		WARN_ON(offset >= chana->buf_size);
		rpages = bufa->buf_wsb[sbidx].pages;
		WARN_ON_ONCE(RCHAN_SB_IS_NOREF(rpages));
		ltt_relay_do_memset(rpages[index].virt + (offset & ~PAGE_MASK),
				    '\0', 1);
		return;
	}

	/*
	 * Deal with incomplete string.
	 * Overwrite string's \0 with X too.
	 */
	pagecpy = copied - 1;
	do {
		WARN_ON_ONCE(!terminated);
		len -= pagecpy;
		offset += pagecpy;
		sbidx = offset >> chana->sb_size_order;
		index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;

		/*
		 * Underlying layer should never ask for writes across
		 * subbuffers.
		 */
		WARN_ON(offset >= chana->buf_size);

		pagecpy = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		rpages = bufa->buf_wsb[sbidx].pages;
		WARN_ON_ONCE(RCHAN_SB_IS_NOREF(rpages));
		ltt_relay_do_memset(rpages[index].virt + (offset & ~PAGE_MASK),
				    'X', pagecpy);
	} while (unlikely(len != pagecpy));
	/*
	 * Overwrite last 'X' with '\0'.
	 */
	offset += pagecpy - 1;
	sbidx = offset >> chana->sb_size_order;
	index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;
	/*
	 * Underlying layer should never ask for writes across subbuffers.
	 */
	WARN_ON(offset >= chana->buf_size);
	rpages = bufa->buf_wsb[sbidx].pages;
	WARN_ON_ONCE(RCHAN_SB_IS_NOREF(rpages));
	ltt_relay_do_memset(rpages[index].virt + (offset & ~PAGE_MASK),
			    '\0', 1);
}
EXPORT_SYMBOL_GPL(_ltt_relay_strncpy_fixup);

/**
 * _ltt_relay_strncpy - copy a string to a ltt_relay buffer.
 * @bufa : buffer
 * @offset : offset within the buffer
 * @src : source address
 * @len : length to write
 * @pagecpy : page size copied so far
 */
void _ltt_relay_strncpy(struct ltt_chanbuf_alloc *bufa, size_t offset,
		        const void *src, size_t len, ssize_t pagecpy)
{
	struct ltt_chan_alloc *chana = bufa->chan;
	size_t sbidx, index, copied;
	struct chanbuf_page *rpages;
	int terminated;

	do {
		len -= pagecpy;
		src += pagecpy;
		offset += pagecpy;
		sbidx = offset >> chana->sb_size_order;
		index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;

		/*
		 * Underlying layer should never ask for writes across
		 * subbuffers.
		 */
		WARN_ON(offset >= chana->buf_size);

		pagecpy = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		rpages = bufa->buf_wsb[sbidx].pages;
		WARN_ON_ONCE(RCHAN_SB_IS_NOREF(rpages));
		copied = ltt_relay_do_strncpy(rpages[index].virt
					      + (offset & ~PAGE_MASK),
					      src, pagecpy, &terminated);
		if (copied < pagecpy || ((len == pagecpy) && !terminated)) {
			_ltt_relay_strncpy_fixup(bufa, offset, len, copied,
						 terminated);
			break;
		}
	} while (unlikely(len != pagecpy));
}
EXPORT_SYMBOL_GPL(_ltt_relay_strncpy);

/**
 * ltt_relay_read - read data from ltt_relay_buffer.
 * @bufa : buffer
 * @offset : offset within the buffer
 * @dest : destination address
 * @len : length to write
 *
 * Should be protected by get_subbuf/put_subbuf.
 */
int ltt_relay_read(struct ltt_chanbuf_alloc *bufa, size_t offset, void *dest,
		   size_t len)
{
	struct ltt_chan_alloc *chana = bufa->chan;
	size_t index;
	ssize_t pagecpy, orig_len;
	struct chanbuf_page *rpages;

	orig_len = len;
	offset &= chana->buf_size - 1;
	index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;
	if (unlikely(!len))
		return 0;
	for (;;) {
		pagecpy = min_t(size_t, len, PAGE_SIZE - (offset & ~PAGE_MASK));
		rpages = bufa->buf_rsb.pages;
		WARN_ON_ONCE(RCHAN_SB_IS_NOREF(rpages));
		memcpy(dest, rpages[index].virt + (offset & ~PAGE_MASK),
		       pagecpy);
		len -= pagecpy;
		if (likely(!len))
			break;
		dest += pagecpy;
		offset += pagecpy;
		index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;
		/*
		 * Underlying layer should never ask for reads across
		 * subbuffers.
		 */
		WARN_ON(offset >= chana->buf_size);
	}
	return orig_len;
}
EXPORT_SYMBOL_GPL(ltt_relay_read);

/**
 * ltt_relay_read_cstr - read a C-style string from ltt_relay_buffer.
 * @bufa : buffer
 * @offset : offset within the buffer
 * @dest : destination address
 * @len : destination's length
 *
 * return string's length
 * Should be protected by get_subbuf/put_subbuf.
 */
int ltt_relay_read_cstr(struct ltt_chanbuf_alloc *bufa, size_t offset,
			void *dest, size_t len)
{
	struct ltt_chan_alloc *chana = bufa->chan;
	size_t index;
	ssize_t pagecpy, pagelen, strpagelen, orig_offset;
	char *str;
	struct chanbuf_page *rpages;

	offset &= chana->buf_size - 1;
	index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;
	orig_offset = offset;
	for (;;) {
		rpages = bufa->buf_rsb.pages;
		WARN_ON_ONCE(RCHAN_SB_IS_NOREF(rpages));
		str = (char *)rpages[index].virt + (offset & ~PAGE_MASK);
		pagelen = PAGE_SIZE - (offset & ~PAGE_MASK);
		strpagelen = strnlen(str, pagelen);
		if (len) {
			pagecpy = min_t(size_t, len, strpagelen);
			if (dest) {
				memcpy(dest, str, pagecpy);
				dest += pagecpy;
			}
			len -= pagecpy;
		}
		offset += strpagelen;
		index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;
		if (strpagelen < pagelen)
			break;
		/*
		 * Underlying layer should never ask for reads across
		 * subbuffers.
		 */
		WARN_ON(offset >= chana->buf_size);
	}
	if (dest && len)
		((char *)dest)[0] = 0;
	return offset - orig_offset;
}
EXPORT_SYMBOL_GPL(ltt_relay_read_cstr);

/**
 * ltt_relay_read_get_page - Get a whole page to read from
 * @bufa : buffer
 * @offset : offset within the buffer
 *
 * Should be protected by get_subbuf/put_subbuf.
 */
struct page *ltt_relay_read_get_page(struct ltt_chanbuf_alloc *bufa,
				     size_t offset)
{
	size_t index;
	struct chanbuf_page *rpages;
	struct ltt_chan_alloc *chana = bufa->chan;

	offset &= chana->buf_size - 1;
	index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;
	rpages = bufa->buf_rsb.pages;
	WARN_ON_ONCE(RCHAN_SB_IS_NOREF(rpages));
	return rpages[index].page;
}
EXPORT_SYMBOL_GPL(ltt_relay_read_get_page);

/**
 * ltt_relay_read_offset_address - get address of a location within the buffer
 * @bufa : buffer
 * @offset : offset within the buffer.
 *
 * Return the address where a given offset is located (for read).
 * Should be used to get the current subbuffer header pointer. Given we know
 * it's never on a page boundary, it's safe to write directly to this address,
 * as long as the write is never bigger than a page size.
 */
void *ltt_relay_read_offset_address(struct ltt_chanbuf_alloc *bufa,
				    size_t offset)
{
	size_t index;
	struct chanbuf_page *rpages;
	struct ltt_chan_alloc *chana = bufa->chan;

	offset &= chana->buf_size - 1;
	index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;
	rpages = bufa->buf_rsb.pages;
	WARN_ON_ONCE(RCHAN_SB_IS_NOREF(rpages));
	return rpages[index].virt + (offset & ~PAGE_MASK);
}
EXPORT_SYMBOL_GPL(ltt_relay_read_offset_address);

/**
 * ltt_relay_offset_address - get address of a location within the buffer
 * @bufa : buffer
 * @offset : offset within the buffer.
 *
 * Return the address where a given offset is located.
 * Should be used to get the current subbuffer header pointer. Given we know
 * it's never on a page boundary, it's safe to write directly to this address,
 * as long as the write is never bigger than a page size.
 */
void *ltt_relay_offset_address(struct ltt_chanbuf_alloc *bufa, size_t offset)
{
	size_t sbidx, index;
	struct chanbuf_page *rpages;
	struct ltt_chan_alloc *chana = bufa->chan;

	offset &= chana->buf_size - 1;
	sbidx = offset >> chana->sb_size_order;
	index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;
	rpages = bufa->buf_wsb[sbidx].pages;
	WARN_ON_ONCE(RCHAN_SB_IS_NOREF(rpages));
	return rpages[index].virt + (offset & ~PAGE_MASK);
}
EXPORT_SYMBOL_GPL(ltt_relay_offset_address);

static __init int ltt_relay_alloc_init(void)
{
	hotcpu_notifier(ltt_relay_hotcpu_callback, 5);
	ltt_relay_init();
	ltt_ascii_init();
	return 0;
}

static void __exit ltt_relay_alloc_exit(void)
{
	ltt_ascii_exit();
	ltt_relay_exit();
}

module_init(ltt_relay_alloc_init);
module_exit(ltt_relay_alloc_exit);
