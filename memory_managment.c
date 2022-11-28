#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>


#define TROZO 50*1024*1024 // 50 MiB
#define PUNTO (5*1024*1024) // 5 MiB, 1 Mib = 1024**2 bytes


void accede (char * p, unsigned long long tam)
{

	unsigned long long i;

	for (i=0; i< tam; i++){
		p[i]='a';
		if ((i%PUNTO)==0)
			write (1,".",1); /* imprime un punto por cada 10 MiB accedidos*/

	}

}


int main (int argc,char*argv[])
{

	char *p;
	unsigned long long total=0, cantidad=TROZO;
	unsigned long long maximo=ULLONG_MAX;


	if (argv[1]!=NULL){
		maximo=strtoull(argv[1],NULL,10);
		if (argv[2]!=NULL)
			cantidad=strtoull(argv[2],NULL,10);
	}


	while (total<maximo && (p=malloc(cantidad))!=NULL){
		total+=cantidad;
		printf ("asignados nuevos %llu bytes=%llu MiB (total %llu bytes=%llu MiB) en %p\n", cantidad,cantidad/(1024*1024), total, total/(1024*1024), p); 
		accede (p,cantidad);
		getchar(); // pause program
	}


	printf ("Total asignacion: %llu\n",total);

	return 0;
}
