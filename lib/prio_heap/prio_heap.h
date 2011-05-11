/*
 * LICENSING: this file is copied from the Linux kernel. We should therefore
 * assume a GPLv2 license for the code that comes from the Linux mainline.
 */

#ifndef _LINUX_PRIO_HEAP_H
#define _LINUX_PRIO_HEAP_H

/*
 * Static-sized priority heap containing pointers. Based on CLR, chapter 7.
 */

#include <linux/gfp.h>

/**
 * struct ptr_heap - simple static-sized priority heap
 * @ptrs - pointer to data area
 * @max - max number of elements that can be stored in @ptrs
 * @size - current number of valid elements in @ptrs (in the range 0..@size-1
 * @gt: comparison operator, which should implement "greater than"
 */
struct ptr_heap {
	void **ptrs;
	int max;
	int size;
	int (*gt)(void *, void *);
};

/**
 * heap_maximum - return the largest element in the heap
 * @heap: the heap to be operated on
 *
 * Returns the largest element in the heap, without performing any modification
 * to the heap structure. Returns NULL if the heap is empty.
 */
static inline void *heap_maximum(const struct ptr_heap *heap)
{
	return heap->size ? heap->ptrs[0] : NULL;
}

/**
 * heap_init - initialize an empty heap with a given memory size
 * @heap: the heap structure to be initialized
 * @size: amount of memory to use in bytes
 * @gfp_mask: mask to pass to kmalloc()
 * @gt: comparison operator, which should implement "greater than"
 */
extern int heap_init(struct ptr_heap *heap, size_t size, gfp_t gfp_mask,
		     int (*gt)(void *, void *));

/**
 * heap_free - release a heap's storage
 * @heap: the heap structure whose data should be released
 */
void heap_free(struct ptr_heap *heap);

/**
 * heap_insert - insert a value into the heap and return any overflowed value
 * @heap: the heap to be operated on
 * @p: the pointer to be inserted
 *
 * Attempts to insert the given value into the priority heap. If the
 * heap is full prior to the insertion, then the resulting heap will
 * consist of the smallest @max elements of the original heap and the
 * new element; the greatest element will be removed from the heap and
 * returned. Note that the returned element will be the new element
 * (i.e. no change to the heap) if the new element is greater than all
 * elements currently in the heap.
 */
extern void *heap_insert(struct ptr_heap *heap, void *p);

/**
 * heap_remove - remove the largest element from the heap
 * @heap: the heap to be operated on
 *
 * Returns the largest element in the heap. It removes this element from the
 * heap. Returns NULL if the heap is empty.
 */
extern void *heap_remove(struct ptr_heap *heap);

/**
 * heap_cherrypick - remove a given element from the heap
 * @heap: the heap to be operated on
 * @p: the element
 *
 * Remove the given element from the heap. Return the element if present, else
 * return NULL. This algorithm has a complexity of O(n), which is higher than
 * O(log(n)) provided by the rest of this API.
 */
extern void *heap_cherrypick(struct ptr_heap *heap, void *p);

/**
 * heap_replace_max - replace the the largest element from the heap
 * @heap: the heap to be operated on
 * @p: the pointer to be inserted as topmost element replacement
 *
 * Returns the largest element in the heap. It removes this element from the
 * heap. The heap is rebalanced only once after the insertion. Returns NULL if
 * the heap is empty.
 *
 * This is the equivalent of calling heap_remove() and then heap_insert(), but
 * it only rebalances the heap once.
 */
extern void *heap_replace_max(struct ptr_heap *heap, void *p);

#endif /* _LINUX_PRIO_HEAP_H */
