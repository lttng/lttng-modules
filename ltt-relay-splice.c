/*
 * Copyright (C) 2002-2005 - Tom Zanussi (zanussi@us.ibm.com), IBM Corp
 * Copyright (C) 1999-2005 - Karim Yaghmour (karim@opersys.com)
 * Copyright (C) 2008-2009 - Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * Re-using content from kernel/relay.c
 *
 * This file is released under the GPL.
 */

#include <linux/errno.h>
#include <linux/stddef.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/cpu.h>
#include <linux/splice.h>
#include <linux/pipe_fs_i.h>
#include <linux/bitops.h>

#include "ltt-relay.h"
#include "ltt-relay-lockless.h"

loff_t ltt_relay_no_llseek(struct file *file, loff_t offset, int origin)
{
	return -ESPIPE;
}

static void ltt_relay_pipe_buf_release(struct pipe_inode_info *pipe,
				       struct pipe_buffer *pbuf)
{
}

static struct pipe_buf_operations ltt_relay_pipe_buf_ops = {
	.can_merge = 0,
	.map = generic_pipe_buf_map,
	.unmap = generic_pipe_buf_unmap,
	.confirm = generic_pipe_buf_confirm,
	.release = ltt_relay_pipe_buf_release,
	.steal = generic_pipe_buf_steal,
	.get = generic_pipe_buf_get,
};

static void ltt_relay_page_release(struct splice_pipe_desc *spd, unsigned int i)
{
}

/*
 *	subbuf_splice_actor - splice up to one subbuf's worth of data
 */
static int subbuf_splice_actor(struct file *in,
			       loff_t *ppos,
			       struct pipe_inode_info *pipe,
			       size_t len,
			       unsigned int flags)
{
	struct ltt_chanbuf *buf = in->private_data;
	struct ltt_chan *chan = container_of(buf->a.chan, struct ltt_chan, a);
	unsigned int poff, subbuf_pages, nr_pages;
	struct page *pages[PIPE_DEF_BUFFERS];
	struct partial_page partial[PIPE_DEF_BUFFERS];
	struct splice_pipe_desc spd = {
		.pages = pages,
		.nr_pages = 0,
		.partial = partial,
		.flags = flags,
		.ops = &ltt_relay_pipe_buf_ops,
		.spd_release = ltt_relay_page_release,
	};
	long consumed_old, consumed_idx, roffset;
	unsigned long bytes_avail;

	/*
	 * Check that a GET_SUBBUF ioctl has been done before.
	 */
	WARN_ON(atomic_long_read(&buf->active_readers) != 1);
	consumed_old = atomic_long_read(&buf->consumed);
	consumed_old += *ppos;
	consumed_idx = SUBBUF_INDEX(consumed_old, chan);

	/*
	 * Adjust read len, if longer than what is available.
	 * Max read size is 1 subbuffer due to get_subbuf/put_subbuf for
	 * protection.
	 */
	bytes_avail = chan->a.sb_size;
	WARN_ON(bytes_avail > chan->a.buf_size);
	len = min_t(size_t, len, bytes_avail);
	subbuf_pages = bytes_avail >> PAGE_SHIFT;
	nr_pages = min_t(unsigned int, subbuf_pages, PIPE_DEF_BUFFERS);
	roffset = consumed_old & PAGE_MASK;
	poff = consumed_old & ~PAGE_MASK;
	printk_dbg(KERN_DEBUG "SPLICE actor len %zu pos %zd write_pos %ld\n",
		   len, (ssize_t)*ppos, local_read(&buf->offset));

	for (; spd.nr_pages < nr_pages; spd.nr_pages++) {
		unsigned int this_len;
		struct page *page;

		if (!len)
			break;
		printk_dbg(KERN_DEBUG "SPLICE actor loop len %zu roffset %ld\n",
			   len, roffset);

		this_len = PAGE_SIZE - poff;
		page = ltt_relay_read_get_page(&buf->a, roffset);
		spd.pages[spd.nr_pages] = page;
		spd.partial[spd.nr_pages].offset = poff;
		spd.partial[spd.nr_pages].len = this_len;

		poff = 0;
		roffset += PAGE_SIZE;
		len -= this_len;
	}

	if (!spd.nr_pages)
		return 0;

	return splice_to_pipe(pipe, &spd);
}

ssize_t ltt_relay_file_splice_read(struct file *in, loff_t *ppos,
				   struct pipe_inode_info *pipe, size_t len,
				   unsigned int flags)
{
	ssize_t spliced;
	int ret;

	ret = 0;
	spliced = 0;

	printk_dbg(KERN_DEBUG "SPLICE read len %zu pos %zd\n", len,
		   (ssize_t)*ppos);
	while (len && !spliced) {
		ret = subbuf_splice_actor(in, ppos, pipe, len, flags);
		printk_dbg(KERN_DEBUG "SPLICE read loop ret %d\n", ret);
		if (ret < 0)
			break;
		else if (!ret) {
			if (flags & SPLICE_F_NONBLOCK)
				ret = -EAGAIN;
			break;
		}

		*ppos += ret;
		if (ret > len)
			len = 0;
		else
			len -= ret;
		spliced += ret;
	}

	if (spliced)
		return spliced;

	return ret;
}
