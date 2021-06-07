#include "malloc.h"


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

        freeListAdd(BLK_HEADER(ptr));
        stats("before coalesce");
        coalesceFreeBlocks();
}
