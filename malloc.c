#include "malloc.h"


/* removes a block from the free list and adjusts the head accordingly */
void freeListRemove(block_t *blk)
{
	if (!blk)
		return;

	/* list not circular, for now */
	if (blk == first_free_blk)
		first_free_blk = blk->next;

        if (blk->prev)
                blk->prev->next = blk->next;

        if (blk->next)
                blk->next->prev = blk->prev;

	/* should be at least one block before current program break */
	next_blk = (block_t *)((unsigned char *)blk + blk->size);
	/* mark block as used by flipping first bit in next block size */
	next_blk->size |= 0x1;
}


/* adds a block to the free list keeping the list sorted by the block begin address, this helps when scanning for continuous blocks */
void freeListAdd(block_t *blk)
{
	block_t *temp, *next_blk;

	if (!blk)
	{
		fprintf(stderr, "freeListAdd: blk is NULL\n");
		retrun;
	}

        if (first_free_blk == NULL || first_free_blk > blk) /* new head of free list */
        {
		if (first_free_blk == NULL || blk == first_block)
		{
			/* unused block at original program break; mark previous "block" as in use to prevent reading before start of heap */
			blk->size |= 0x1;
			blk->prev_size = 0;
		}

                if (first_free_blk != NULL)
                        first_free_blk->prev = blk;
                blk->next = first_free_blk;
		/* not circular list, for now */
		blk->prev = NULL;
		first_free_blk = blk;
        }
        else
        {
		temp = first_free_blk;
                while (temp->next && temp->next < blk)
                        temp = temp->next;
                blk->next = temp->next;
		if (temp->next != NULL)
			temp->next->prev = blk;
		blk->prev = temp;
                temp->next = blk;
        }

	/* record new_free_blk size in following block */
	next_blk = (block_t *)((unsigned char *)blk + blk->size);
	next_blk->prev_size = blk->size;
	/* mark block as unused by flipping first bit in next block size */
	next_blk->size &= ~0x1;
}

/* incoming size should be block size, not payload size */
/* splits the block b by creating a new block after size bytes, new block is returned */
block_t *splitFreeBlock(block_t *free_blk, size_t size)
{
        block_t *new_free_blk, *next_blk;

	/* no way to check if incoming block is free or not? first bit in size is used for status of _previous_ block */

	if (!free_blk)
	{
		fprintf(stderr, "splitFreeBlock: free_blk is NULL\n");
		retrun (NULL);
	}

	if (size < sizeof(block_t) || size % ALIGN)
	{
		fprintf(stderr, "splitFreeBlock: size not aligned\n");
		retrun (NULL);
	}

        new_free_blk = (block_t *)((unsigned char *)free_blk + size);

	new_free_blk->next = free_blk->next;
	new_free_blk->prev = free_blk->prev;
        new_free_blk->size = free_blk->size - size;
        free_blk->size = size;

	/* record new_free_blk size in following block */
	next_blk = (block_t *)((unsigned char *)new_free_blk + new_free_blk->size);
	next_blk->prev_size = new_free_blk->size;

	/* mark previous block as allocated */
	new_free_blk->size |= 0x1;

        return new_free_blk;
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
        block_t *blk, *new_blk, *next_blk;
        size_t aligned_sz, unused_blk_sz, page_sz;

	/* presumes alignment of starting progam break and previous blocks */
	aligned_sz = size + (ALIGN - (size % ALIGN));

	for (blk = first_free_blk; blk; blk = blk->next)
	{
		/* first fit, later upgrade to best fit LIFO */
                if (blk->size >= BLK_SZ(aligned_sz))
                {
                        freeListRemove(blk);

			/* perfect sized block found, return it */
                        if (blk->size == BLK_SZ(aligned_sz))
			{
				/* should be at least one empty block before program break */
				next_blk = (block_t *)((unsigned char *)blk + blk->size);
				/* mark block as used by flipping first bit in next block size */
				next_blk->size |= 0x1;

                                return (BLK_PAYLOAD(blk));
			}

			/* block larger then request, split and add fragemnt to free list */
                        new_blk = splitFreeBlock(blk, BLK_SZ(aligned_sz));
                        freeListAdd(new_blk);

			return (BLK_PAYLOAD(blk));
                }
	}

	/* ensure new program break is page-aligned */
	page_sz = sysconf(_SC_PAGESIZE);
	if (page_sz == -1)
	{
		fprintf(stderr, "_malloc: sysconf failure\n");
		return (NULL);
	}

	for (unused_blk_sz = page_sz;
	     /* unused region should fit header of an empty free block at end, in case of splitting? */
	     unused_blk_sz < BLK_SZ(aligned_sz) + sizeof(block_t);
	     unused_blk_sz += page_sz)
	{}

	/* new unused block at end of heap virtual address space */
        blk = (block_t *)sbrk(unused_blk_sz);
	if ((void *)blk == (void *)-1)
	{
		perror("_malloc: sbrk");
		return (NULL);
	}

        blk->size = unused_blk_sz;
	freeListAdd(blk);

#ifdef ZZZ
	/* but, added to head of free list (LIFO) */
        blk->next = first_free_blk;
	if (first_free_blk == NULL) /* first call of _malloc */
	{
		/* new unused block at starting program break; mark previous "block" as in use to prevent reading before start of heap */
		aligned_sz |= 0x1;
		first_free_blk = blk;
	}
	else
		first_free_blk->prev = blk;
#endif
        if (unused_blk_sz > BLK_SZ(aligned_sz & ~0x1) + sizeof(block_t))
        {
		new_blk = splitFreeBlock(blk, BLK_SZ(aligned_sz & ~0x1));
		freeListAdd(new_blk);
        }

	return (BLK_PAYLOAD(blk));
}
