#ifndef FREE_LIST_H
#define FREE_LIST_H

/* size_t block_t */
#include "malloc.h"
/* pthread_mutex_t */
#include <pthread.h>


static block_t *free_list_head;
static pthread_mutex_t free_list_mutex;


/* free_list.c */
void freeListRemove(block_t *blk);
void freeListAdd(block_t *blk);
block_t *newFreeBlock(size_t algnd_pyld_sz);
block_t *splitFreeBlock(block_t *free_blk, size_t size);
void coalesceFreeBlocks(void);

#endif /* FREE_LIST_H */
