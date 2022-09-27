#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
 
void *printAMessageAsync(void *vargp)
{
    printf("\rMessage from thread.\n");
    return NULL;
}
  
int main()
{
    pthread_t thread_id;
    printf("\rMessage 1\n");
    pthread_create(&thread_id, NULL, printAMessageAsync, NULL);
    printf("\rMessage 2\n");
    sleep(1);
    return 0;
}
