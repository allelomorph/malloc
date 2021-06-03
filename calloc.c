#include "malloc.h"
/* memset */
#include <string.h>


/**
 * _calloc - allocates memory for an array of nmemb elements of size bytes each
 *   and returns a pointer to the allocated memory, all bytes set to zero
 *
 * @nmemb: number of elements; multiplier for size
 * @size: size in bytes of element; multiplicand for nmemb
 * Return: pointer to first byte in a contiguous region of `nmemb` * `size`
 *   bytes in the heap, aligned for any kind of variable and all initalized to
 *   0, or NULL on failure
 */
void *_calloc(size_t nmemb, size_t size)
{
	void *ptr;

	ptr = _malloc(nmemb * size);

	if (ptr)
		memset(ptr, 0, nmemb * size);

	return (ptr);
}
