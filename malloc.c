#include "malloc.h"
/* freeListAdd freeListRemove splitFreeBlock */
#include "free_list.c"
/* printf fprintf perror */
#include <stdio.h>
/* sysconf */
#include <unistd.h>


/**
 * initFreeListMutex - initializes global mutex for free block list before
 *   main
 */
void initFreeListMutex(void)
{
	pthread_mutex_init(&free_list_mutex, NULL);
}


/**
 * destroyFreeListMutex - destroys global mutex for free block list
 *   after main
 */
void destroyFreeListMutex(void)
{
	pthread_mutex_destroy(&free_list_mutex);
}

/*
		pthread_mutex_lock(&task_status_mutex);
		pthread_mutex_unlock(&task_status_mutex);
*/

/**
 * printFreeList - test print of current program break and free list
 *
 * @prefix: label string for output, intended to give troubleshooting context
 */
void printFreeList(char *prefix)
{
	void *pgm_brk;
	block_t *blk;
	int i;

	pgm_brk = sbrk(0);
	if (pgm_brk == (void *)-1)
	{
		perror("printFreeList: sbrk");
		return;
	}

	printf("[%s] program break: %10p\n", prefix, pgm_brk);
	printf("[%s] free list:\n", prefix);
	for (blk = free_list_head, i = 0; blk; blk = blk->next, i++)
                printf("\t(%i) @%10p size:%lu next:%10p prev:%10p\n",
		       i, (void *)blk, blk->size,
		       (void *)(blk->next), (void *)(blk->prev));
}


/**
 * _malloc - allocates space in the heap of a running process for user data
 *
 * @size: size of memory requested by user, in bytes
 * Return: pointer to first byte in a contiguous region of `size`
 *   bytes in the heap, aligned for any kind of variable, or NULL on failure
 */
void *_malloc(size_t size)
{
	block_t *blk, *remainder_blk;
	size_t algnd_pyld_sz;

	/* presumes alignment of starting progam break and previous blocks */
	algnd_pyld_sz = size + (ALIGN - (size % ALIGN));

	for (blk = free_list_head; blk; blk = blk->next)
	{
		/* first fit linear search, LIFO population of list */
		if (blk->size >= BLK_SZ(algnd_pyld_sz))
			break;
	}

	/* no adequate block found, need create new one by extending break */
	if (!blk)
		blk = newFreeBlock(algnd_pyld_sz);
	if (!blk)
		return (NULL);

	freeListRemove(blk);

	if (blk->size == BLK_SZ(algnd_pyld_sz))
		return (BLK_PAYLOAD(blk));

	remainder_blk = splitFreeBlock(blk, BLK_SZ(algnd_pyld_sz));
	if (!remainder_blk)
		return (NULL);

	freeListAdd(remainder_blk);

	return (BLK_PAYLOAD(blk));
}
