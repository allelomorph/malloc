#include "malloc.h"
/* also includes free_list.h */
#include "free_list.c"
/* printf fprintf perror */
#include <stdio.h>
/* sysconf */
#include <unistd.h>



void _cleanup()
{
        printf("cleaning memory up\n");
        if (first_free_blk)
                if (brk(first_free_blk) != 0)
                        perror("_cleanup: brk");

        first_free_blk = NULL;
        stats("_cleanup end");
}


/* stats prints some debug information regarding the
 * current program break and the blocks on the free list */
void stats(char *prefix)
{
        block_t *blk;
        int i;

        printf("[%s] program break: %10p\n", prefix, (void *)sbrk(0));
        printf("[%s] free list:\n", prefix);
	for (blk = first_free_blk, i = 0; blk; blk = blk->next, i++)
                printf("(%i) <%10p> (size: %lu)\n", i, (void *)blk, blk->size);
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
	size_t aligned_sz, new_blk_sz;
	long page_sz;

	/* presumes alignment of starting progam break and previous blocks */
	aligned_sz = size + (ALIGN - (size % ALIGN));

	for (blk = first_free_blk; blk; blk = blk->next)
	{
		/* first fit, later upgrade to best fit LIFO */
		if (blk->size >= BLK_SZ(aligned_sz))
		{
			freeListRemove(blk);

			if (blk->size == BLK_SZ(aligned_sz))
				return (BLK_PAYLOAD(blk));

			new_blk = splitFreeBlock(blk, BLK_SZ(aligned_sz));
			freeListAdd(new_blk);

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
	     new_blk_sz < BLK_SZ(aligned_sz) + sizeof(block_t);
	     new_blk_sz += page_sz)
	{}

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

	if (new_blk_sz > BLK_SZ(aligned_sz) + sizeof(block_t))
	{
		blk = splitFreeBlock(new_blk, BLK_SZ(aligned_sz));
		freeListAdd(blk);
	}
	else
		blk = new_blk;

	return (BLK_PAYLOAD(blk));
}
