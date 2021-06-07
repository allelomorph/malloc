#include "malloc.h"
/* freeListAdd freeListRemove splitFreeBlock */
#include "free_list.c"
/* printf fprintf perror */
#include <stdio.h>
/* sysconf */
#include <unistd.h>


/* stats prints some debug information regarding the
 * current program break and the blocks on the free list */
void printFreeList(char *prefix)
{
        block_t *blk;
        int i;

        printf("[%s] program break: %10p\n", prefix, (void *)sbrk(0));
        printf("[%s] free list:\n", prefix);
	for (blk = first_free_blk, i = 0; blk; blk = blk->next, i++)
                printf("\t(%i) @%10p size:%lu next:%10p prev:%10p\n",
		       i, (void *)blk, blk->size,
		       (void *)(blk->next), (void *)(blk->prev));
}


/**
 * _malloc - TBD
 *
 * @size: size of memory requested by user, in bytes
 * Return: pointer to first byte in a contiguous region of `size`
 *   bytes in the heap; aligned for any kind of variable
 */
void *_malloc(size_t size)
{
	block_t *blk, *new_blk;
	size_t algnd_pyld_sz, new_blk_sz;
	long page_sz;

	printf("\t_malloc: user requested %lu bytes\n", size);

	/* presumes alignment of starting progam break and previous blocks */
	algnd_pyld_sz = size + (ALIGN - (size % ALIGN));

	for (blk = first_free_blk; blk; blk = blk->next)
	{
		/* first fit, later upgrade to best fit LIFO */
		if (blk->size >= BLK_SZ(algnd_pyld_sz))
		{
			printf("\t_malloc: found free block of %lu bytes\n", blk->size);

			freeListRemove(blk);

			printf("\t_malloc: removed block %10p from free list:\n", (void *)blk);
			printFreeList("removed block");

			if (blk->size == BLK_SZ(algnd_pyld_sz))
				return (BLK_PAYLOAD(blk));

			new_blk = splitFreeBlock(blk, BLK_SZ(algnd_pyld_sz));
			freeListAdd(new_blk);

			printf("\t_malloc: split remainder block %10p from block %10p:\n", (void *)new_blk, (void *)blk);
			printFreeList("after split");

			return (BLK_PAYLOAD(blk));
		}
	}

	/* no block large enough found, need allocate new one */

	/* ensure new program break is page-aligned */
	page_sz = sysconf(_SC_PAGESIZE);
	if (page_sz == -1)
	{
		fprintf(stderr, "_malloc: sysconf failure\n");
		return (NULL);
	}

	for (new_blk_sz = page_sz;
	     /* should fit an empty free block at end, in case of splitting? */
	     new_blk_sz < BLK_SZ(algnd_pyld_sz) + sizeof(block_t);
	     new_blk_sz += page_sz)
	{}

	printf("\t_malloc: new_blk_sz:%lu >= BLK_SZ(algnd_pyld_sz):%lu + sizeof(block_t):%lu\n", new_blk_sz, BLK_SZ(algnd_pyld_sz), sizeof(block_t));

	/* new unused block at end of heap virtual address space */
	new_blk = sbrk(new_blk_sz);
	if (new_blk == (void *)-1)
	{
		perror("_malloc: sbrk");
		return (NULL);
	}

	new_blk = (block_t *)new_blk;
	new_blk->size = new_blk_sz;
	freeListAdd(new_blk);

	printf("\t_malloc: new free block %10p added by moving program break to %p:\n", (void *)new_blk, sbrk(0));
	printFreeList("new free block");

	if (new_blk_sz > BLK_SZ(algnd_pyld_sz) + sizeof(block_t))
	{
		blk = splitFreeBlock(new_blk, BLK_SZ(algnd_pyld_sz));
		freeListAdd(blk);
		freeListRemove(new_blk);

		printf("\t_malloc: split remainder block %10p from block %10p:\n", (void *)blk, (void *)new_blk);
		printFreeList("after split");

	}

	printf("\t_malloc: returning payload:%p from block %p\n", BLK_PAYLOAD(new_blk), (void *)new_blk);

	return (BLK_PAYLOAD(new_blk));
}
