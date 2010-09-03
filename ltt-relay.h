 /*
 * include/linux/ltt-relay.h
 *
 * Copyright (C) 2008,2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 *
 * Dual LGPL v2.1/GPL v2 license.
 *
 * Credits to Steven Rostedt for proposing to use an extra-subbuffer owned by
 * the reader in flight recorder mode.
 */

#ifndef _LINUX_LTT_RELAY_H
#define _LINUX_LTT_RELAY_H

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/kref.h>
#include <linux/mm.h>
#include <linux/ltt-channels.h>

#include "ltt-tracer-core.h"

/* Use lowest pointer bit to show the sub-buffer has no reference. */
#define RCHAN_NOREF_FLAG	0x1UL

#define RCHAN_SB_IS_NOREF(x)	((unsigned long)(x) & RCHAN_NOREF_FLAG)
#define RCHAN_SB_SET_NOREF(x)	\
	(x = (struct chanbuf_page *)((unsigned long)(x) | RCHAN_NOREF_FLAG))
#define RCHAN_SB_CLEAR_NOREF(x)	\
	(x = (struct chanbuf_page *)((unsigned long)(x) & ~RCHAN_NOREF_FLAG))

struct ltt_trace;

struct chanbuf_page {
	void *virt;			/* page virtual address (cached) */
	struct page *page;		/* pointer to page structure */
};

struct chanbuf_sb {
	struct chanbuf_page *pages;	/* Pointer to rchan pages for subbuf */
};

struct ltt_chanbuf_alloc {
	struct chanbuf_sb *buf_wsb;	/* Array of rchan_sb for writer */
	struct chanbuf_sb buf_rsb;	/* chanbuf_sb for reader */
	void **_virt;			/* Array of pointers to page addr */
	struct page **_pages;		/* Array of pointers to pages */
	struct dentry *dentry;		/* Associated file dentry */
	unsigned int nr_pages;		/* Number pages in buffer */

	struct ltt_chan_alloc *chan;	/* Associated channel */
	unsigned int cpu;		/* This buffer's cpu */
	unsigned int allocated:1;	/* Bool: is buffer allocated ? */
};

int ltt_chanbuf_alloc_create(struct ltt_chanbuf_alloc *buf,
			     struct ltt_chan_alloc *chan, int cpu);
void ltt_chanbuf_alloc_free(struct ltt_chanbuf_alloc *buf);
int ltt_chan_alloc_init(struct ltt_chan_alloc *chan, struct ltt_trace *trace,
			const char *base_filename,
			struct dentry *parent, size_t sb_size,
			size_t n_sb, int extra_reader_sb, int overwrite);
void ltt_chan_alloc_free(struct ltt_chan_alloc *chan);
void ltt_chan_alloc_remove_files(struct ltt_chan_alloc *chan);
int ltt_chanbuf_create_file(const char *filename, struct dentry *parent,
			    int mode, struct ltt_chanbuf *buf);
int ltt_chanbuf_remove_file(struct ltt_chanbuf *buf);

void ltt_chan_for_each_channel(void (*cb) (struct ltt_chanbuf *buf), int cpu);

extern void _ltt_relay_write(struct ltt_chanbuf_alloc *bufa,
			     size_t offset, const void *src, size_t len,
			     ssize_t pagecpy);

extern void _ltt_relay_strncpy(struct ltt_chanbuf_alloc *bufa,
			       size_t offset, const void *src, size_t len,
			       ssize_t pagecpy);

extern void _ltt_relay_strncpy_fixup(struct ltt_chanbuf_alloc *bufa,
				     size_t offset, size_t len, size_t copied,
				     int terminated);

extern int ltt_relay_read(struct ltt_chanbuf_alloc *bufa,
			  size_t offset, void *dest, size_t len);

extern int ltt_relay_read_cstr(struct ltt_chanbuf_alloc *bufa,
			       size_t offset, void *dest, size_t len);

extern struct page *ltt_relay_read_get_page(struct ltt_chanbuf_alloc *bufa,
					    size_t offset);

/*
 * Return the address where a given offset is located.
 * Should be used to get the current subbuffer header pointer. Given we know
 * it's never on a page boundary, it's safe to write directly to this address,
 * as long as the write is never bigger than a page size.
 */
extern void *ltt_relay_offset_address(struct ltt_chanbuf_alloc *bufa,
				      size_t offset);
extern void *ltt_relay_read_offset_address(struct ltt_chanbuf_alloc *bufa,
					   size_t offset);

#ifdef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
static __inline__
void ltt_relay_do_copy(void *dest, const void *src, size_t len)
{
	switch (len) {
	case 0:
		break;
	case 1:
		*(u8 *)dest = *(const u8 *)src;
		break;
	case 2:
		*(u16 *)dest = *(const u16 *)src;
		break;
	case 4:
		*(u32 *)dest = *(const u32 *)src;
		break;
	case 8:
		*(u64 *)dest = *(const u64 *)src;
		break;
	default:
		/*
		 * What we really want here is an __inline__ memcpy, but we don't
		 * have constants, so gcc generally uses a function call.
		 */
		for (; len > 0; len--)
			*(u8 *)dest++ = *(const u8 *)src++;
	}
}
#else
/*
 * Returns whether the dest and src addresses are aligned on
 * min(sizeof(void *), len). Call this with statically known len for efficiency.
 */
static __inline__
int addr_aligned(const void *dest, const void *src, size_t len)
{
	if (ltt_align((size_t)dest, len))
		return 0;
	if (ltt_align((size_t)src, len))
		return 0;
	return 1;
}

static __inline__
void ltt_relay_do_copy(void *dest, const void *src, size_t len)
{
	switch (len) {
	case 0:
		break;
	case 1:
		*(u8 *)dest = *(const u8 *)src;
		break;
	case 2:
		if (unlikely(!addr_aligned(dest, src, 2)))
			goto memcpy_fallback;
		*(u16 *)dest = *(const u16 *)src;
		break;
	case 4:
		if (unlikely(!addr_aligned(dest, src, 4)))
			goto memcpy_fallback;
		*(u32 *)dest = *(const u32 *)src;
		break;
	case 8:
		if (unlikely(!addr_aligned(dest, src, 8)))
			goto memcpy_fallback;
		*(u64 *)dest = *(const u64 *)src;
		break;
	default:
		goto memcpy_fallback;
	}
	return;

memcpy_fallback:
	/*
	 * What we really want here is an inline memcpy, but we don't
	 * have constants, so gcc generally uses a function call.
	 */
	for (; len > 0; len--)
		*(u8 *)dest++ = *(const u8 *)src++;
}
#endif

/*
 * ltt_relay_do_memset - write character into dest.
 * @dest: destination
 * @src: source character
 * @len: length to write
 */
static __inline__
void ltt_relay_do_memset(void *dest, char src, size_t len)
{
	/*
	 * What we really want here is an __inline__ memset, but we
	 * don't have constants, so gcc generally uses a function call.
	 */
	for (; len > 0; len--)
		*(u8 *)dest++ = src;
}


/*
 * ltt_relay_do_strncpy - copy a string up to a certain number of bytes
 * @dest: destination
 * @src: source
 * @len: max. length to copy
 * @terminated: output string ends with \0 (output)
 *
 * returns the number of bytes copied. Does not finalize with \0 if len is
 * reached.
 */
static __inline__
size_t ltt_relay_do_strncpy(void *dest, const void *src, size_t len,
			    int *terminated)
{
	size_t orig_len = len;

	*terminated = 0;
	/*
	 * What we really want here is an __inline__ strncpy, but we
	 * don't have constants, so gcc generally uses a function call.
	 */
	for (; len > 0; len--) {
		*(u8 *)dest = ACCESS_ONCE(*(const u8 *)src);
		/* Check with dest, because src may be modified concurrently */
		if (*(const u8 *)dest == '\0') {
			len--;
			*terminated = 1;
			break;
		}
		dest++;
		src++;
	}
	return orig_len - len;
}

static __inline__
int ltt_relay_write(struct ltt_chanbuf_alloc *bufa,
		    struct ltt_chan_alloc *chana, size_t offset,
		    const void *src, size_t len)
{
	size_t sbidx, index;
	ssize_t pagecpy;
	struct chanbuf_page *rpages;

	offset &= chana->buf_size - 1;
	sbidx = offset >> chana->sb_size_order;
	index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;
	pagecpy = min_t(size_t, len, (- offset) & ~PAGE_MASK);
	rpages = bufa->buf_wsb[sbidx].pages;
	WARN_ON_ONCE(RCHAN_SB_IS_NOREF(rpages));
	ltt_relay_do_copy(rpages[index].virt + (offset & ~PAGE_MASK),
			  src, pagecpy);

	if (unlikely(len != pagecpy))
		_ltt_relay_write(bufa, offset, src, len, pagecpy);
	return len;
}

static __inline__
int ltt_relay_strncpy(struct ltt_chanbuf_alloc *bufa,
		      struct ltt_chan_alloc *chana, size_t offset,
		      const void *src, size_t len)
{
	size_t sbidx, index;
	ssize_t pagecpy, copied;
	struct chanbuf_page *rpages;
	int terminated;

	offset &= chana->buf_size - 1;
	sbidx = offset >> chana->sb_size_order;
	index = (offset & (chana->sb_size - 1)) >> PAGE_SHIFT;
	pagecpy = min_t(size_t, len, (- offset) & ~PAGE_MASK);
	rpages = bufa->buf_wsb[sbidx].pages;
	WARN_ON_ONCE(RCHAN_SB_IS_NOREF(rpages));
	copied = ltt_relay_do_strncpy(rpages[index].virt
				      + (offset & ~PAGE_MASK),
				      src, pagecpy, &terminated);
	if (unlikely(copied < pagecpy || ((len == pagecpy) && !terminated)))
		_ltt_relay_strncpy_fixup(bufa, offset, len, copied,
					 terminated);
	else {
		if (unlikely(len != pagecpy))
			_ltt_relay_strncpy(bufa, offset, src, len, pagecpy);
	}
	return len;
}

/**
 * ltt_clear_noref_flag - Clear the noref subbuffer flag, for writer.
 */
static __inline__
void ltt_clear_noref_flag(struct ltt_chanbuf_alloc *bufa, long idx)
{
	struct chanbuf_page *sb_pages, *new_sb_pages;

	sb_pages = bufa->buf_wsb[idx].pages;
	for (;;) {
		if (!RCHAN_SB_IS_NOREF(sb_pages))
			return;	/* Already writing to this buffer */
		new_sb_pages = sb_pages;
		RCHAN_SB_CLEAR_NOREF(new_sb_pages);
		new_sb_pages = cmpxchg(&bufa->buf_wsb[idx].pages,
			sb_pages, new_sb_pages);
		if (likely(new_sb_pages == sb_pages))
			break;
		sb_pages = new_sb_pages;
	}
}

/**
 * ltt_set_noref_flag - Set the noref subbuffer flag, for writer.
 */
static __inline__
void ltt_set_noref_flag(struct ltt_chanbuf_alloc *bufa, long idx)
{
	struct chanbuf_page *sb_pages, *new_sb_pages;

	sb_pages = bufa->buf_wsb[idx].pages;
	for (;;) {
		if (RCHAN_SB_IS_NOREF(sb_pages))
			return;	/* Already set */
		new_sb_pages = sb_pages;
		RCHAN_SB_SET_NOREF(new_sb_pages);
		new_sb_pages = cmpxchg(&bufa->buf_wsb[idx].pages,
			sb_pages, new_sb_pages);
		if (likely(new_sb_pages == sb_pages))
			break;
		sb_pages = new_sb_pages;
	}
}

/**
 * update_read_sb_index - Read-side subbuffer index update.
 */
static __inline__
int update_read_sb_index(struct ltt_chanbuf_alloc *bufa,
			 struct ltt_chan_alloc *chana,
			 long consumed_idx)
{
	struct chanbuf_page *old_wpage, *new_wpage;

	if (unlikely(chana->extra_reader_sb)) {
		/*
		 * Exchange the target writer subbuffer with our own unused
		 * subbuffer.
		 */
		old_wpage = bufa->buf_wsb[consumed_idx].pages;
		if (unlikely(!RCHAN_SB_IS_NOREF(old_wpage)))
			return -EAGAIN;
		WARN_ON_ONCE(!RCHAN_SB_IS_NOREF(bufa->buf_rsb.pages));
		new_wpage = cmpxchg(&bufa->buf_wsb[consumed_idx].pages,
				old_wpage,
				bufa->buf_rsb.pages);
		if (unlikely(old_wpage != new_wpage))
			return -EAGAIN;
		bufa->buf_rsb.pages = new_wpage;
		RCHAN_SB_CLEAR_NOREF(bufa->buf_rsb.pages);
	} else {
		/* No page exchange, use the writer page directly */
		bufa->buf_rsb.pages = bufa->buf_wsb[consumed_idx].pages;
		RCHAN_SB_CLEAR_NOREF(bufa->buf_rsb.pages);
	}
	return 0;
}

ssize_t ltt_relay_file_splice_read(struct file *in, loff_t *ppos,
				   struct pipe_inode_info *pipe, size_t len,
				   unsigned int flags);
loff_t ltt_relay_no_llseek(struct file *file, loff_t offset, int origin);

#endif /* _LINUX_LTT_RELAY_H */
