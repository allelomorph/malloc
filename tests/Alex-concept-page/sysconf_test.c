#include <stdio.h>
#include <unistd.h>

/* https://stackoverflow.com/questions/37897645/page-size-undeclared-c */

int main(void)
{
#ifdef PAGESIZE
	printf("sysconf(PAGESIZE):%li\n", sysconf(PAGESIZE));
#endif

#ifdef PAGE_SIZE
	printf("sysconf(PAGE_SIZE):%li\n", sysconf(PAGE_SIZE));
#endif

#ifdef _SC_PAGESIZE
	printf("sysconf(_SC_PAGESIZE):%li\n", sysconf(_SC_PAGESIZE));
#endif

	/* getpagesize() is depreacted and not in POSIX.1, avoid for lack of portability */
	printf("getpagesize():%i\n", getpagesize());

	return 0;
}
