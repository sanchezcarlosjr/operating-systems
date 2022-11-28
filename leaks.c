#include <stdio.h>
#include <stdlib.h>

/*
   gcc -Wall -pedantic -g leaks.c -o leaks.out
   valgrind --leak-check=full ./leaks.out
*/

int main(int argc, char** argv){
	int i;
	int *a;


	for (i=0; i < 10; i++){
		a = malloc(sizeof(int) * 100);
	}

	free(a);
	return 0;
}
