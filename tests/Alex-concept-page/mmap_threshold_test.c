#include <stdio.h>
#include <stdlib.h>

/* 0x1FFE8 is highest size on (first) call to malloc that does not use mmap() */

int main(void)
{
	void *ptr;

	ptr = malloc(0x1ffe8);
	free(ptr);

	return 0;
}
