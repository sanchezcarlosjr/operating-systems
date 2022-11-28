#include <stdio.h>
#include <stdlib.h>


/*
   gcc -Wall -pedantic -g matrix-leak.c -o matrix.out
   valgrind ./matrix.out
*/

int main(int argc, char** argv){

	int i;

	int *a = malloc(sizeof(int) * 10);

	if (!a)  /* if malloc failed */
	    return -1;

	for (i = 0; i < 11; i++){
		a[i] = i;
	}

	free(a);

	return 0;

}
