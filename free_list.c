#include "malloc.h"
/* fprintf perror */
#include <stdio.h>
/* sysconf */
#include <unistd.h>
/* pthread_mutex_lock pthread_mutex_unlock */
#include <pthread.h>


static block_t *free_list_head;
static pthread_mutex_t free_list_mutex;


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

	pthread_mutex_lock(&free_list_mutex);

	if (blk == free_list_head)
		free_list_head = blk->next;

	if (blk->prev)
		blk->prev->next = blk->next;

	if (blk->next)
		blk->next->prev = blk->prev;

	pthread_mutex_unlock(&free_list_mutex);
}


/**
 * freeListAdd - adds a block to the free list, deallocating it from heap
 *   memory available to the user
 *
 * @blk: block to add to free list
 */
void freeListAdd(block_t *blk)
{
	if (!blk)
	{
		fprintf(stderr, "freeListAdd: blk is NULL\n");
		return;
	}

	pthread_mutex_lock(&free_list_mutex);

	/* LIFO free list, always add to head */
	if (free_list_head != NULL)
		free_list_head->prev = blk;
	blk->next = free_list_head;
	blk->prev = NULL;
	free_list_head = blk;

	pthread_mutex_unlock(&free_list_mutex);
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

	pthread_mutex_lock(&free_list_mutex);

	/* new unused block at end of heap virtual address space */
	new_blk = sbrk(new_blk_sz);
	if (new_blk == (void *)-1)
	{
		perror("newFreeBlock: sbrk");
		return (NULL);
	}
	new_blk->size = new_blk_sz;

	pthread_mutex_unlock(&free_list_mutex);

	freeListAdd(new_blk);
	return (new_blk);
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

	pthread_mutex_lock(&free_list_mutex);

	new_free_blk = (block_t *)((uint8_t *)free_blk + size);

	new_free_blk->next = free_blk->next;
	new_free_blk->prev = free_blk->prev;
	new_free_blk->size = free_blk->size - size;
	free_blk->size = size;

	pthread_mutex_unlock(&free_list_mutex);

	return (new_free_blk);
}


/**
 * coalesceFreeBlocks - linear traversal of free list, merging any block with
 *   any other free block that is next in the list and also in an contiguous
 *   address space of heap
 */
void coalesceFreeBlocks(void)
{
	block_t *curr;

	pthread_mutex_lock(&free_list_mutex);

	for (curr = free_list_head; curr; curr = curr->next)
	{
		if ((uint8_t *)curr + curr->size == (uint8_t *)curr->next)
		{
			curr->size += curr->next->size;
			curr->next = curr->next->next;
			if (curr->next)
				curr->next->prev = curr;
		}
	}

	pthread_mutex_unlock(&free_list_mutex);
}
