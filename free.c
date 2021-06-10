#include "malloc.h"
/* perror */
#include <stdio.h>
/* sysconf */
#include <unistd.h>
/* pthread_mutex_lock pthread_mutex_unlock */
#include <pthread.h>


/**
 * trimProgramBreak - searches for free block abutting current program break,
 *   releases back to system if found
 */
void trimProgramBreak(void)
{
	block_t *curr;
	void *pgm_brk;
	long page_sz;

	pgm_brk = sbrk(0);
	if (pgm_brk == (void *)-1)
	{
		perror("trimProgramBreak: sbrk");
		return;
	}
	page_sz = sysconf(_SC_PAGESIZE);
	if (page_sz == -1)
	{
		perror("trimProgramBreak: sysconf");
		return;
	}

	pthread_mutex_lock(&free_list_mutex);

	for (curr = free_list_head; curr; curr = curr->next)
	{
		if ((uint8_t *)curr + curr->size == pgm_brk &&
		    curr->size >= (size_t)page_sz)
		{
			pthread_mutex_unlock(&free_list_mutex);

			freeListRemove(curr);

			pthread_mutex_lock(&free_list_mutex);

			if (brk(curr) != 0)
				perror("coalesceFreeBlocks: brk");
			break;
		}
	}

	pthread_mutex_unlock(&free_list_mutex);
}


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
	trimProgramBreak();
}
