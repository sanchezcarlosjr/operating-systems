#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>


double t[0x02000000];

#define SIZE 1024


void segments()
{
	static int s = 42;
	void *p = malloc(SIZE);
	printf("stack\t%010p\nbrk\t%010p\nheap\t%010p\nstatic(BSS)\t%010p\nstatic(initialized)\t%010p\ntext\t%010p\n",
		&p, sbrk(0), p, t, &s, segments
	      );

}



int main() {
	segments();
	exit(0);
}
