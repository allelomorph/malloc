#include <stdlib.h>
#include <stdio.h>

/**
 * main - print locations of various elements
 *
 * Return: EXIT_FAILURE if something failed. Otherwise EXIT_SUCCESS
 */
int main(void)
{
	int a, i;
	void *p;
	int (*ptr_to_main)() = main;
	unsigned char *LE_ptr = (unsigned char *)&ptr_to_main;

	printf("Address of a: %p\n", (void *)&a);
	p = malloc(98);
	if (p == NULL)
	{
		fprintf(stderr, "Can't malloc\n");
		return (EXIT_FAILURE);
	}
	printf("Allocated space in the heap: %p\n", p);

	/*
	printf("Address of function main: %p\n", (void *)main);
	*/
	/*
	 * throws compiler error for trying to convert function pointer to
	 * object pointer if compiled with -pedantic; alternative here:
	 * https://stackoverflow.com/q/10932910/
	 */
	printf("Address of function main: 0x");

	/* little endian, print in reverse order */
	for (i = sizeof(ptr_to_main) - 1; i >= 0; i--)
		printf("%02x", LE_ptr[i]);
	putchar('\n');

	return (EXIT_SUCCESS);
}
