#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

void *naive_malloc_extend(size_t size);

void *naive_malloc_extend(size_t size)
{
	static void *blk_0_addr;
	static size_t used_blk_ct;
		long int page_sz;

		void *brk_pt, *payload_addr, *blk_addr, *unused_blk_addr;
	size_t i, blk_sz, unused_blk_sz;

	/* first call */
	if (blk_0_addr == NULL && used_blk_ct == 0)
	{
		page_sz = sysconf(_SC_PAGESIZE);
		if (page_sz == -1)
		{
			fprintf(stderr, "naive_malloc_page: sysconf failure\n");
			return (NULL);
		}

		blk_0_addr = sbrk((intptr_t)page_sz);
			if (blk_0_addr == (void *)-1)
		{
			perror("naive_malloc_page: sbrk");
			return (NULL);
		}
	}

	brk_pt = sbrk(0);
	if (brk_pt == (void *)-1)
	{
		perror("naive_malloc_page: sbrk");
		return (NULL);
	}

	/* find next unused block */
	for (blk_addr = blk_0_addr, i = 0;
		 blk_addr < brk_pt && i < used_blk_ct; i++)
	{
		blk_sz = *((size_t *)blk_addr);
		blk_addr = (unsigned char *)blk_addr + blk_sz;
	}
	unused_blk_addr = blk_addr;
	unused_blk_sz = used_blk_ct ? *((size_t *)unused_blk_addr) : (size_t)page_sz;

	/* extend unused region if too small for new block + unused header */
	while (unused_blk_sz < (sizeof(size_t) * 2) + size)
	{
		page_sz = sysconf(_SC_PAGESIZE);
		if (page_sz == -1)
		{
			fprintf(stderr, "naive_malloc_page: sysconf failure\n");
			return (NULL);
		}

		if (sbrk((intptr_t)page_sz) == (void *)-1)
		{
			perror("naive_malloc_page: sbrk");
			return (NULL);
		}

		unused_blk_sz += page_sz;
		*((size_t *)unused_blk_addr) = unused_blk_sz;
	}

	/* set new block for use at start of previously unused block */
		blk_sz = sizeof(size_t) + size;
	*((size_t *)blk_addr) = blk_sz;
	used_blk_ct++;

	/* set new start of unused block after new block */
	unused_blk_sz -= blk_sz;
	unused_blk_addr = (unsigned char *)unused_blk_addr + blk_sz;
	*((size_t *)unused_blk_addr) = unused_blk_sz;

		payload_addr = (unsigned char *)blk_addr + sizeof(size_t);
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

	printf("PID:%i\n", getpid());
	printf("Starting break is %p\n", sbrk(0));

	for (i = 0; i < 10; i++)
	{
		void *chunk;

		str = naive_malloc_extend(1016);
		strncpy(str, "Holberton", 10);
		printf("%p: %s, ", (void *)str, str);
		chunk = (void *)(str - sizeof(size_t));
		printf("chunk addr: %p, ", (void *)chunk);
		printf("size: %lu, ", *((size_t *)chunk));
		printf("break: %p\n", sbrk(0));
	}

	printf("Final break is %p\n", sbrk(0));
	getchar();
	return (EXIT_SUCCESS);
}
