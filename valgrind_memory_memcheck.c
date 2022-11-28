#include <stdio.h>
#include <stdlib.h>

// If you are on Arch Linux, you would install https://wiki.archlinux.org/title/Debuginfod
/* 
   gcc -o test.o -g -O0 valgrind_memory_memcheck.c 
   valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./test.o
*/

int main() {

	char *p;

	// Allocation #1 of 19 bytes
	p = (char *) malloc(19);

	// Allocation #2 of 12 bytes
	p = (char *) malloc(12);

	// We free #2 block of 12 bytes
	free(p);

	// Allocation #3 of 16 bytes
	p = (char *) malloc(16);

	// #1 and #2 blocks are definitely lost since we don't free them.
	// these block could provoke leaks.

	return 0;
}

