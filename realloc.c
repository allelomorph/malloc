#include "malloc.h"
/* memcpy */
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))


/**
 * _realloc - changes the size of the memory block pointed by ptr to size
 *   bytes; contents will be unchanged in the range from the start of the
 *   region up to the minimum of the old and new sizes
 *
 * @ptr: pointer to the memory space to resize
 * @size: new size needed to be allocated for the user, in bytes
 * Return: pointer to first byte in a new contiguous region of `size`
 *   bytes in the heap, aligned for any kind of variable, or NULL on failure
 */
void *_realloc(void *ptr, size_t size)
{
	void *new_ptr;
	block_t *blk;

	if (ptr == NULL)
		return (_malloc(size));

	if (size == 0)
	{
		_free(ptr);
		return (NULL);
	}

	/* return block unchanged if requested size is same (once aligned) */
	blk = BLK_HEADER(ptr);
	if (PAYLOAD_SZ(blk->size) == size + (ALIGN - (size % ALIGN)))
		return (ptr);

	new_ptr = _malloc(size);
	if (!new_ptr)
		return (NULL);

	/* currently assuming that ptr returned by _malloc or glibc malloc */
	memcpy(new_ptr, ptr, MIN(blk->size, size));

	_free(ptr);
	return (new_ptr);
}
