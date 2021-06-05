#ifndef MALLOC_H
#define MALLOC_H

/* size_t */
#include <stddef.h>


typedef struct block_s
{
	size_t prev_size;  /* Size of previous block (if free).  */
        size_t size;       /* Size in bytes, including overhead. */
	struct block_s *next;  /* used only if free. */
	struct block_s *prev;
/*
	struct block_s *next_size_class; * used only if free. *
	struct block_s *prev_size_class;
*/
};


/* double word alignment as in stock malloc, allows for mimimum free block size of sizeof(block_t) */
#define ALIGN (2 * sizeof(void *))

static block_t *first_blk;
static block_t *first_free_blk;

#define HEADER_SZ (sizeof(size_t) * 2)
#define BLK_SZ(size) (HEADER_SZ + size)

#define BLK_PAYLOAD(ptr) ((void *)((unit8_t *)ptr + HEADER_SZ))
#define BLK_HEADER(ptr) ((block_t *)((unit8_t *)ptr - HEADER_SZ))


/* task 0. Naive malloc */
/* naive_malloc.c */
void *naive_malloc(size_t size);

/* task 1. malloc */
/* malloc.c */
void freeListRemove(block_t *blk);
void freeListAdd(block_t *blk);
block_t *splitFreeBlock(block_t *free_blk, size_t size);
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
