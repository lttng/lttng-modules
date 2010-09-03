/*
 * ltt/probes/mm-trace.c
 *
 * MM tracepoint probes.
 *
 * (C) Copyright 2009 - Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/mm.h>
#include <linux/swap.h>

#include <asm/pgtable.h>
#include <asm/tlbflush.h>
#include <linux/swapops.h>
#include <trace/page_alloc.h>
#include <trace/filemap.h>
#include <trace/swap.h>
#include <trace/hugetlb.h>

#include "../ltt-type-serializer.h"

void probe_wait_on_page_start(void *_data, struct page *page, int bit_nr)
{
	trace_mark_tp(mm, wait_on_page_start, wait_on_page_start,
		probe_wait_on_page_start, "pfn %lu bit_nr %d",
		page_to_pfn(page), bit_nr);
}

void probe_wait_on_page_end(void *_data, struct page *page, int bit_nr)
{
	trace_mark_tp(mm, wait_on_page_end, wait_on_page_end,
		probe_wait_on_page_end, "pfn %lu bit_nr %d",
		page_to_pfn(page), bit_nr);
}

void probe_hugetlb_page_free(void *_data, struct page *page)
{
	trace_mark_tp(mm, huge_page_free, hugetlb_page_free,
		probe_hugetlb_page_free, "pfn %lu", page_to_pfn(page));
}

void probe_hugetlb_page_alloc(void *_data, struct page *page)
{
	if (page)
		trace_mark_tp(mm, huge_page_alloc, hugetlb_page_alloc,
			probe_hugetlb_page_alloc, "pfn %lu", page_to_pfn(page));
}

/* mm_page_free specialized tracepoint probe */

void probe_page_free(void *_data, struct page *page, unsigned int order);

DEFINE_MARKER_TP(mm, page_free, page_free, probe_page_free,
	"pfn %lu order %u");

notrace void probe_page_free(void *_data, struct page *page, unsigned int order)
{
	struct marker *marker;
	struct serialize_long_int data;

	data.f1 = page_to_pfn(page);
	data.f2 = order;

	marker = &GET_MARKER(mm, page_free);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(long));
}

/* mm_page_alloc specialized tracepoint probe */

void probe_page_alloc(void *_data, struct page *page, unsigned int order);

DEFINE_MARKER_TP(mm, page_alloc, page_alloc, probe_page_alloc,
	"pfn %lu order %u");

notrace void probe_page_alloc(void *_data, struct page *page, unsigned int order)
{
	struct marker *marker;
	struct serialize_long_int data;

	if (unlikely(!page))
		return;

	data.f1 = page_to_pfn(page);
	data.f2 = order;

	marker = &GET_MARKER(mm, page_alloc);
	ltt_specialized_trace(marker, marker->single.probe_private,
		&data, serialize_sizeof(data), sizeof(long));
}

#ifdef CONFIG_SWAP
void probe_swap_in(void *_data, struct page *page, swp_entry_t entry)
{
	trace_mark_tp(mm, swap_in, swap_in, probe_swap_in,
		"pfn %lu filp %p offset %lu",
		page_to_pfn(page),
		get_swap_info_struct(swp_type(entry))->swap_file,
		swp_offset(entry));
}

void probe_swap_out(void *_data, struct page *page)
{
	trace_mark_tp(mm, swap_out, swap_out, probe_swap_out,
		"pfn %lu filp %p offset %lu",
		page_to_pfn(page),
		get_swap_info_struct(swp_type(
			page_swp_entry(page)))->swap_file,
		swp_offset(page_swp_entry(page)));
}

void probe_swap_file_close(void *_data, struct file *file)
{
	trace_mark_tp(mm, swap_file_close, swap_file_close,
		probe_swap_file_close, "filp %p", file);
}

void probe_swap_file_open(void *_data, struct file *file, char *filename)
{
	trace_mark_tp(mm, swap_file_open, swap_file_open,
		probe_swap_file_open, "filp %p filename %s",
		file, filename);
}
#endif

void probe_add_to_page_cache(void *_data, struct address_space *mapping, pgoff_t offset)
{
	trace_mark_tp(mm, add_to_page_cache, add_to_page_cache,
		probe_add_to_page_cache,
		"inode %lu sdev %u",
		mapping->host->i_ino, mapping->host->i_sb->s_dev);
}

void probe_remove_from_page_cache(void *_data, struct address_space *mapping)
{
	trace_mark_tp(mm, remove_from_page_cache, remove_from_page_cache,
		probe_remove_from_page_cache,
		"inode %lu sdev %u",
		mapping->host->i_ino, mapping->host->i_sb->s_dev);
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("MM Tracepoint Probes");
