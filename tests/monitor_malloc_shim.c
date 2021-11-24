#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include "../malloc.h"

/* compile with `gcc -Wall -Werror -Wextra -pedantic \
   -shared -fPIC -ldl tests/_malloc_shim.c malloc.h malloc.c free.c \
   calloc.c realloc.c -o tests/montitor_malloc_shim.so` */

/* use shim with:
   `LD_PRELOAD=tests/monitor_malloc_shim.so ./<executable to shim>` */

void unbuffered_str_print(char *str)
{
	int i;

	if (!str)
		return;

	for (i = 0; str[i]; i++)
		putchar(str[i]);
}


void unbuffered_n_print(size_t n, size_t base)
{
	size_t _n, pow_base;
	char *chars = "0123456789abcdefghijklmnopqrstuvwxyz";

	if (!base)
		return;

	if (!n)
	{
		putchar('0');
		return;
	}

	for (pow_base = 1; pow_base < n; pow_base *= base)
	{}

	if (pow_base != n)
		pow_base /= base;

	for (_n = n; pow_base; _n %= base, pow_base /= base)
		putchar(chars[(_n / pow_base)]);

}


void *malloc(size_t size)
{
	unbuffered_str_print("malloc:");
	unbuffered_n_print(size, 10);
	putchar('\n');
	fflush(stdout);

	return (_malloc(size));
}

void free(void *ptr)
{
	unbuffered_str_print("free:");
	putchar('0');
	putchar('x');
	unbuffered_n_print((size_t)ptr, 16);
	putchar('\n');
	fflush(stdout);

        _free(ptr);
}


void *calloc(size_t nmemb, size_t size)
{
	unbuffered_str_print("calloc:");
	unbuffered_n_print(nmemb, 10);
	putchar(',');
	putchar(' ');
	unbuffered_n_print(size, 10);
	putchar('\n');
	fflush(stdout);

	return (_calloc(nmemb, size));
}

void *realloc(void *ptr, size_t size)
{
	unbuffered_str_print("realloc:");
	putchar('0');
	putchar('x');
	unbuffered_n_print((size_t)ptr, 16);
	putchar(',');
	putchar(' ');
	unbuffered_n_print(size, 10);
	putchar('\n');
	fflush(stdout);

	return (_realloc(ptr, size));
}
