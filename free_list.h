#ifndef FREE_LIST_H
#define FREE_LIST_H

/* block_t */
#include "malloc.h"

/* static block_t *first_blk; */
static block_t *first_free_blk;

/* free_list.c */
void freeListRemove(block_t *blk);
void freeListAdd(block_t *blk);
block_t *splitFreeBlock(block_t *free_blk, size_t size);
void coalesceFreeBlocks(void);

#endif /* FREE_LIST_H */
