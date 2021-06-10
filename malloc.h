#ifndef MALLOC_H
#define MALLOC_H

/* size_t */
#include <stddef.h>
/* uint8_t */
#include <stdint.h>


/**
 * struct block_s - heap allocation block header
 *
 * @prev_size: size of previous block in bytes, if free
 * @size: size of this block in bytes, including overhead
 * @next: in free blocks, points to next member of free list
 * @prev: in free blocks, points to previous member of free list
 */
typedef struct block_s
{
	size_t prev_size;
	size_t size;
	/* payload begins after size in allocated blocks */
	struct block_s *next;
	struct block_s *prev;
/*
 *	struct block_s *next_size_class;
 *	struct block_s *prev_size_class;
 */
} block_t;


/* double word alignment as in stock malloc */
/* allows for mimimum free block size of sizeof(block_t) */
#define ALIGN (2 * sizeof(void *))

#define HEADER_SZ (sizeof(size_t) * 2)
#define BLK_SZ(size) (HEADER_SZ + size)
#define PAYLOAD_SZ(size) (size - HEADER_SZ)

#define BLK_PAYLOAD(ptr) ((void *)((uint8_t *)ptr + HEADER_SZ))
#define BLK_HEADER(ptr) ((block_t *)((uint8_t *)ptr - HEADER_SZ))


/* task 0. Naive malloc */
/* naive_malloc.c */
void *naive_malloc(size_t size);

/* free_list.c */
void freeListRemove(block_t *blk);
void freeListAdd(block_t *blk);
block_t *newFreeBlock(size_t algnd_pyld_sz);
block_t *splitFreeBlock(block_t *free_blk, size_t size);
void coalesceFreeBlocks(void);

/* task 1. malloc */
/* malloc.c */
void initFreeListMutex(void) __attribute__ ((constructor));
void destroyFreeListMutex(void) __attribute__ ((destructor));
void printFreeList(char *prefix);
void *_malloc(size_t size);

/* task 2. free */
/* free.c */
void _free(void *ptr);

/* task 3. calloc */
/* calloc.c */
void *_calloc(size_t nmemb, size_t size);

/* task 4. realloc */
/* realloc.c */
void *_realloc(void *ptr, size_t size);

/* task 5. Multithreading */


#endif /* MALLOC_H */
