#include "malloc.h"

#include <stdio.h>


/**
 * _free - TBD
 *
 * @ptr: pointer to first byte of a contigous region of heap memory which can
 *   be released back to pool
 */
void _free(void *ptr)
{
	if (!ptr)
		return;

	printf("\tuser freed %p\n", ptr);

        freeListAdd(BLK_HEADER(ptr));
	printFreeList("before coalesce");
        coalesceFreeBlocks();
}
