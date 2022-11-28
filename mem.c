#include <stdio.h> 
#include <stdlib.h> 

/*
   free
   gcc mem.c -o mem.out
   free
*/

int main()

{ 

	int *ptr; unsigned long i,n; 

	printf("Enter number of int(4 byte) you want to allocate:"); 

	scanf("%lu",&n); 

	printf("Allocating %lu bytes......\n",n*sizeof(int)); 

	ptr=(int*)malloc(n*sizeof(int)); 

	if (ptr==NULL){ printf("ERROR!Memory not allocated!"); 

		exit(0); 

	} 


	printf("Filling int into memory.....\n"); 

	for (i = 0; i < n; i++){ 

		ptr[i] = 1; 

	} 

	//printf("Sleep 10 seconds......\n"); 

	//sleep(10); 

	printf("Free memory.\n"); 

	free(ptr); 

	return 0; 

}
