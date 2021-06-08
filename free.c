#include "malloc.h"


/**
 * _free - deallocates a block of memory and returns to free list, where it is
 *   merged with any contiguous free blocks
 *
 * @ptr: pointer to first byte of region of heap memory address space which can
 *   be released back to pool
 */
void _free(void *ptr)
{
	if (!ptr)
		return;

	freeListAdd(BLK_HEADER(ptr));
	coalesceFreeBlocks();
}
