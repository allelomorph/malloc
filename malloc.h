#ifndef MALLOC_H
#define MALLOC_H

/* size_t */
#include <stddef.h>


/* task 0. Naive malloc */
/* naive_malloc.c */
void *naive_malloc(size_t size);

/* task 1. malloc */
/* malloc.c */
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
