#include "malloc.h"
/* freeListAdd freeListRemove splitFreeBlock */
#include "free_list.c"
/* printf fprintf perror */
#include <stdio.h>
/* sysconf */
#include <unistd.h>


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
	for (blk = first_free_blk, i = 0; blk; blk = blk->next, i++)
                printf("\t(%i) @%10p size:%lu next:%10p prev:%10p\n",
		       i, (void *)blk, blk->size,
		       (void *)(blk->next), (void *)(blk->prev));
}


/**
 * newFreeBlock - creates virgin free block in heap by extending program break
 *
 * @algnd_pyld_sz: size of memory requested by user, in bytes, aligned by
 *   double word
 * Return: pointer to new block in free list, or NULL on failure
 */
block_t *newFreeBlock(size_t algnd_pyld_sz)
{
	block_t *new_blk;
	size_t new_blk_sz;
	long page_sz;

	/* ensure new program break is page-aligned */
	page_sz = sysconf(_SC_PAGESIZE);
	if (page_sz == -1)
	{
		fprintf(stderr, "newFreeBlock: sysconf failure\n");
		return (NULL);
	}

	for (new_blk_sz = (size_t)page_sz;
	     /* should fit an empty free block at end, in case of splitting */
	     new_blk_sz < BLK_SZ(algnd_pyld_sz) + sizeof(block_t);
	     new_blk_sz += page_sz)
	{}

	/* new unused block at end of heap virtual address space */
	new_blk = sbrk(new_blk_sz);
	if (new_blk == (void *)-1)
	{
		perror("newFreeBlock: sbrk");
		return (NULL);
	}

	new_blk->size = new_blk_sz;
	freeListAdd(new_blk);

	return (new_blk);
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

	for (blk = first_free_blk; blk; blk = blk->next)
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
