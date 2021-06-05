#include "malloc.h"
/* fprintf perror */
#include <stdio.h>
/* sysconf */
#include <unistd.h>


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
