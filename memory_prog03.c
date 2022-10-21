//$ gcc memory_prog03.c -o prog03.out
//$ size prog03.out

#include <stdio.h>


int global; /*variable sin inicializar almacenada en BSS */


int main(void)

{

static int i; /*variable sin inicializar estática almacenada en BSS */

return 0;

}
