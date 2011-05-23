/*
 * prio_heap.c
 *
 * Static-sized priority heap containing pointers. Based on CLRS,
 * chapter 6.
 *
 * Copyright 2011 - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 */

#include <linux/slab.h>
#include <linux/prio_heap.h>

/*
 * TODO implement heap_init, heap_free, heap_insert.
 */

static void heapify(struct ptr_heap *heap, int pos)
{
	void **ptrs = heap->ptrs;
	void *p = ptrs[pos];

	while (1) {
		int left = 2 * pos + 1;
		int right = 2 * pos + 2;
		int largest = pos;
		if (left < heap->size && heap->gt(ptrs[left], p))
			largest = left;
		if (right < heap->size && heap->gt(ptrs[right], ptrs[largest]))
			largest = right;
		if (largest == pos)
			break;
		/* Push p down the heap one level and bump one up */
		ptrs[pos] = ptrs[largest];
		ptrs[largest] = p;
		pos = largest;
	}
}

void *heap_replace_max(struct ptr_heap *heap, void *p)
{
	void *res;
	void **ptrs = heap->ptrs;

	if (!heap->size) {
		ptrs[0] = p;
		heap->size = 1;
		return NULL;
	}

	/* Replace the current max and heapify */
	res = ptrs[0];
	ptrs[0] = p;
	heapify(heap, 0);
	return res;
}

void *heap_remove(struct ptr_heap *heap)
{
	void **ptrs = heap->ptrs;

	switch (heap->size) {
	case 0:
		return NULL;
	case 1:
		heap->size = 0;
		return ptrs[0];
	}

	/* Shrink, replace the current max by previous last entry and heapify */
	return heap_replace_max(heap, ptrs[--heap->size]);
}

void *heap_cherrypick(struct ptr_heap *heap, void *p)
{
	void **ptrs = heap->ptrs;
	size_t pos, size = heap->size;

	for (pos = 0; pos < size; pos++)
		if (ptrs[pos] == p)
			goto found;
	return NULL;
found:
	if (heap->size == 1) {
		heap->size = 0;
		return ptrs[0];
	}
	/*
	 * Replace p with previous last entry and heapify.
	 */
	ptrs[pos] = ptrs[--heap->size];
	heapify(heap, pos);
	return p;
}
