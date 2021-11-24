#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/**
 * pmem - print mem
 * @p: memory address to start printing from
 * @bytes: number of bytes to print
 *
 * Return: nothing
 */
void pmem(void *p, unsigned int bytes)
{
	unsigned char *ptr;
	unsigned int i;

	ptr = (unsigned char *)p;
	for (i = 0; i < bytes; i++)
	{
		if (i != 0)
			printf(" ");
		printf("%02x", *(ptr + i));
	}

	printf("\n");
}


#define ARR_SZ 8


/**
 * main - confirm the source code
 *
 * Return: EXIT_FAILURE if something failed. Otherwise EXIT_SUCCESS
 */
int main(void)
{
	void *p;
	int i, arr[ARR_SZ] = {0, 1, 24, 25, 40, 41, 56, 57};
	size_t size_of_the_chunk;
	size_t size_of_the_previous_chunk;
	void *chunks[ARR_SZ];
	char prev_used;

	for (i = 0; i < ARR_SZ; i++)
	{
		p = malloc(arr[i]);
		chunks[i] = (void *)((char *)p - 0x10);
		printf("%p\n", p);
	}

	free((char *)(chunks[3]) + 0x10);
	free((char *)(chunks[5]) + 0x10);

	for (i = 0; i < ARR_SZ; i++)
	{
		p = chunks[i];
		printf("chunks[%d]: ", i);
		pmem(p, 0x10);
		size_of_the_chunk = *((size_t *)((char *)p + 8));
		prev_used = size_of_the_chunk & 1;
		size_of_the_chunk -= prev_used;
		size_of_the_previous_chunk = *((size_t *)((char *)p));
		printf("chunks[%d]: %p, requested:%i, size = %li, prev (%s) = %li\n",
			   i, p, arr[i], size_of_the_chunk,
			   prev_used ? "allocated" : "unallocated",
			   size_of_the_previous_chunk);
	}

	return (EXIT_SUCCESS);
}
