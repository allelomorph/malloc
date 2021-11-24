#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

void *naive_malloc_header(size_t size);

void *naive_malloc_header(size_t size)
{
		void *brk_pt, *payload_addr;
	size_t block_size;

	brk_pt = sbrk(0);
	if (brk_pt == (void *)-1)
		return (NULL);

	block_size = sizeof(size_t) + size;

	brk_pt = sbrk(block_size);
	if (brk_pt == (void *)-1)
		return (NULL);

	*((size_t *)brk_pt) = block_size;
		payload_addr = (unsigned char *)brk_pt + sizeof(size_t);

	return (payload_addr);
}


/**
 * main - Program entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	char *str;
	int i;
	void *chunk;

	printf("PID:%i\n", getpid());
	printf("Starting break is %p\n", sbrk(0));

	for (i = 0; i < 10; i++)
	{
		str = naive_malloc_header(10);
	strncpy(str, "Holberton", 10);
		printf("%p: %s, ", (void *)str, str);
		chunk = (void *)(str - sizeof(size_t));
		printf("chunk starting at address %p, ", (void *)chunk);
		printf("chunk size is %lu\n", *((size_t *)chunk));
	}

	printf("Final break is %p\n", sbrk(0));
	return (EXIT_SUCCESS);
}
