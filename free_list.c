#include "malloc.h"
/* fprintf perror */
#include <stdio.h>
/* sysconf */
#include <unistd.h>


/* static block_t *first_blk; */
static block_t *first_free_blk;


/**
 * freeListRemove - removes a block from the free list, to be allocated to the
 *   user if an exact size match, or split if larger
 *
 * @blk: block to remove from free list
 */
void freeListRemove(block_t *blk)
{
	if (blk == NULL)
	{
		fprintf(stderr, "freeListRemove: blk is NULL\n");
		return;
	}

	/* list not circular, for now */
	if (blk == first_free_blk)
		first_free_blk = blk->next;

	if (blk->prev)
		blk->prev->next = blk->next;

	if (blk->next)
		blk->next->prev = blk->prev;
}


/**
 * freeListAdd - adds a block to the free list, deallocating it from heap
 *   memory available to the user
 *
 * @blk: block to add to free list
 */
void freeListAdd(block_t *blk)
{
	block_t *temp;

	if (!blk)
	{
		fprintf(stderr, "freeListAdd: blk is NULL\n");
		return;
	}

	/* new head of free list */
	if (first_free_blk == NULL || first_free_blk > blk)
	{
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
}


/* no way to check if incoming block is free or not? */
/**
 * splitFreeBlock - splits free block into allocated block and remainder,
 *   remainder becomes new free block
 *
 * @free_blk: unused block from which to carve out allocation
 * @size: block size in bytes, including header, to allocate from free block
 * Return: pointer to first byte in a contiguous region of `size`
 *   bytes in the heap; aligned for any kind of variable
 */
block_t *splitFreeBlock(block_t *free_blk, size_t size)
{
	block_t *new_free_blk;

	if (!free_blk)
	{
		fprintf(stderr, "splitFreeBlock: free_blk is NULL\n");
		return (NULL);
	}

	if (size < sizeof(block_t) || size % ALIGN)
	{
		fprintf(stderr, "splitFreeBlock: size not aligned\n");
		return (NULL);
	}

	new_free_blk = (block_t *)((uint8_t *)free_blk + size);

	new_free_blk->next = free_blk->next;
	new_free_blk->prev = free_blk->prev;
	new_free_blk->size = free_blk->size - size;
	free_blk->size = size;

	return (new_free_blk);
}


/**
 * coalesceFreeBlocks - TBD
 */
void coalesceFreeBlocks(void)
{
        block_t *curr;
        void *pgm_brk;
	long page_sz;

        pgm_brk = sbrk(0);
        if (pgm_brk == (void *)-1)
        {
                perror("colaesceFreeBlocks: sbrk");
                return;
        }

	page_sz = sysconf(_SC_PAGESIZE);
	if (page_sz == -1)
	{
		perror("colaesceFreeBlocks: sysconf");
		return;
	}

	/* if free block is contiguous to the next in the list, merge them */
	for (curr = first_free_blk; curr; curr = curr->next)
        {
                if ((uint8_t *)curr + curr->size == (uint8_t *)curr->next)
                {
                        curr->size += curr->next->size;
                        curr->next = curr->next->next;
                        if (curr->next)
                                curr->next->prev = curr;
                }
        }

        printFreeList("after coalesce");

	/* check free list to see if there is a large unused block at the end of the heap */
	for (curr = first_free_blk; curr; curr = curr->next)
        {
		if ((uint8_t *)curr + curr->size == pgm_brk &&
		    curr->size >= (size_t)page_sz)
		{
			freeListRemove(curr);
			if (brk(curr) != 0)
				perror("coalesceFreeBlocks: brk");
			break;
		}
	}
        printFreeList("after release of large block");
}
