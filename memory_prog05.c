//$ gcc memory_prog05.c -o prog05.out
//$ size prog05.out
#include <stdio.h>

int global = 10; /* variable global inicializada almacenada en DS */


int main(void)

{

	static int i = 100; /* variable estática inicializada almacenada en DS */

	return 0;

}
