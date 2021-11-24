#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/**
 * main - Program entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	void *p1, *p2;

	p1 = sbrk(0);
	if (p1 == (void *)-1)
	{
		perror(NULL);
		return (EXIT_FAILURE);
	}
	printf("Starting break is %p\n", p1);

	p2 = sbrk(1);
	if (p2 == (void *)-1)
	{
		perror(NULL);
		return (EXIT_FAILURE);
	}
	p2 = sbrk(0);
	printf("Break after extension is %p\n", p2);

	*((char *)p1) = 'H';
	printf("Address of our char is %p\n", p1);
	printf("Value at this address is '%c'\n", *((char *)p1));

	return (EXIT_SUCCESS);
}
