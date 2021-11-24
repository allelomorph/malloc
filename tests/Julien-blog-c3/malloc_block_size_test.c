#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

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


/* test of threshold sizes for malloc block allocation */

#define ARR_SZ 8

/**
 * main - the 0x10 lost bytes
 *
 * Return: EXIT_FAILURE if something failed. Otherwise EXIT_SUCCESS
 */
int main(void)
{
	void *p;
	int i, arr[ARR_SZ] = {0, 1, 24, 25, 40, 41, 56, 57};

	for (i = 0; i < ARR_SZ; i++)
	{
		p = malloc(arr[i]);
		printf("%i byte payload for user at @ %p\n", arr[i], p);
		printf("bytes at %p:\n", (void *)((char *)p - 0x10));
		pmem((char *)p - 0x10, 0x10);

		printf("testing data fill with 0x01 %i times\n", arr[i]);
		memset(p, 0x01, arr[i]);
/*
		free(p);
*/
	}

	return (EXIT_SUCCESS);
}
