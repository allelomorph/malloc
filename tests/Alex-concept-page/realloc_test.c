#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>


/*
	   The  realloc()  function  changes  the size of the memory block pointed
	   by ptr to size bytes.  The contents will be unchanged in the range from
	   the start of the region up to the minimum of the old and  new  sizes.

	   If the  new size is larger than the old size, the added memory will not
	   be initialized.

	   If ptr is NULL, then the call is equivalent to malloc(size), for all
	   values of size; if size is equal to zero, and  ptr  is  not  NULL, then
	   the  call is equivalent to free(ptr).

	   Unless ptr is NULL, it must have been returned by an earlier call to
	   malloc(), calloc() or realloc().  If the area pointed to was moved, a
	   free(ptr) is done.

	   The realloc() function returns a pointer to the newly allocated memory,
	   which is suitably aligned for any kind of  variable and may be different
	   from ptr, or NULL if the request fails.

	   If size was equal to 0, either NULL or a pointer suitable to be passed
	   to free() is returned.  If realloc()  fails  the  original  block  is
	   left untouched; it is not freed or moved.
*/


int main(void)
{
	void *ptr = NULL;

	printf("PID:%i\n", getpid());

	ptr = realloc(NULL, 10);
	printf("realloc(NULL, 10):%p\n", ptr);

	ptr = realloc(ptr, 16);
	printf("realloc(ptr, 16):%p\n", ptr);

	ptr = realloc(ptr, 30);
	printf("realloc(ptr, 30):%p\n", ptr);

	ptr = realloc(ptr, 0);
	printf("realloc(ptr, 0):%p\n", ptr);
/*
		free(ptr);
	printf("free(ptr):%p\n", ptr);
*/
	getchar();
	return (0);
}
