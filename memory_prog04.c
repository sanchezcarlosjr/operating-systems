//$ gcc memory_prog04.c -o prog04.out
//$ size prog04.out

#include <stdio.h>


int global; /*variable sin inicializar almacenada en BSS */


int main(void)

{

	static int i = 100; /* variable estática Inicializada almacenada en DS*/

	return 0;

}
