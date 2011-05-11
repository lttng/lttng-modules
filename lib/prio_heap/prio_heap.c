/*
 * LICENSING: this file is copied from the Linux kernel. We should therefore
 * assume a GPLv2 license for the code that comes from the Linux mainline.
 */

/*
 * Static-sized priority heap containing pointers. Based on CLR, chapter 7.
 */

#include <linux/slab.h>
#include <linux/prio_heap.h>

int heap_init(struct ptr_heap *heap, size_t size, gfp_t gfp_mask,
	      int (*gt)(void *, void *))
{
	heap->ptrs = kmalloc(size, gfp_mask);
	if (!heap->ptrs)
		return -ENOMEM;
	heap->size = 0;
	heap->max = size / sizeof(void *);
	heap->gt = gt;
	return 0;
}

void heap_free(struct ptr_heap *heap)
{
	kfree(heap->ptrs);
}

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

void *heap_insert(struct ptr_heap *heap, void *p)
{
	void **ptrs = heap->ptrs;
	int pos;

	if (heap->size < heap->max) {
		/* Heap insertion */
		pos = heap->size++;
		while (pos > 0 && heap->gt(p, ptrs[(pos-1)/2])) {
			ptrs[pos] = ptrs[(pos-1)/2];
			pos = (pos-1)/2;
		}
		ptrs[pos] = p;
		return NULL;
	}

	/* The heap is full, so something will have to be dropped */

	/* If the new pointer is greater than the current max, drop it */
	if (heap->gt(p, ptrs[0]))
		return p;

	/* Replace the current max and heapify */
	return heap_replace_max(heap, p);
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
