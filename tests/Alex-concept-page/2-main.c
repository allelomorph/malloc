#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


void *naive_malloc(size_t size)
{
	return (sbrk(size));
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
		str = naive_malloc(10);
		strncpy(str, "Holberton", 10);
		printf("%p: %s\n", (void *)str, str);
	}

	printf("Final break is %p\n", sbrk(0));
	getchar();
	return (EXIT_SUCCESS);
}
