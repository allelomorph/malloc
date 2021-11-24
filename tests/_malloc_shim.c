#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include "../malloc.h"

/* compile with `gcc -Wall -Werror -Wextra -pedantic \
   -shared -fPIC -ldl tests/_malloc_shim.c malloc.h malloc.c free.c \
   calloc.c realloc.c -o tests/_malloc_shim.so` */

/* use shim with:
   `LD_PRELOAD=tests/_malloc_shim.so ./<executable to shim>` */

void *malloc(size_t size)
{
	return (_malloc(size));
}

void free(void *ptr)
{
        _free(ptr);
}

void *calloc(size_t nmemb, size_t size)
{
	return (_calloc(nmemb, size));
}

void *realloc(void *ptr, size_t size)
{
	return (_realloc(ptr, size));
}
