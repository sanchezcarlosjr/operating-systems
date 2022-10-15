/*
$ gcc readersAndWriters.c -o readersAndWriters.out -lpthread
*/


#define _REENTRANT

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_THREADS 128

#define p(s) {printf s;fflush(stdout);sleep(1);}

/* por defecto 20 threads */
int numthreads = 20;

/* rutinas de los threads */
void *lector(void *);
void *escritor(void *);

/* vector de rutinas de lectores y escritores (ratio lect/escr = 2) */
void *(*rutina[3])(void *)={escritor,lector,lector};

pthread_t tid[MAX_THREADS];      /* array de IDs thread (identificadores) */


/* semaforos */
sem_t x,escr;

/* variables globales */
int contador; /* para llevar el orden de llegada de las peticiones */
int cont_lect;

int main(int argc, char *argv[])
{
  int i,n=0;


  if (argc>1) n=atoi(argv[1]);
  if ((n>0) && (n<=MAX_THREADS)) numthreads=n;

  contador=0;
  cont_lect=0;

  sem_init(&x, 0, 1);
  sem_init(&escr, 0, 1);

  /* crear aleatoriamente lectores y escritores */
  for (i=0;i<numthreads;i++)
     pthread_create(&tid[i], NULL, rutina[rand()%3], (void *)i);

  /* esperar a que terminen todos */
  for (i=0;i<numthreads;i++)
     pthread_join(tid[i],NULL);

  p(("Acabaron todos los threads ...\n"));
  exit(0);
}



void * lector(void *arg)
{
   int n=(int)arg; /* cardinal */

   p(("L%d quiere leer.\n",n));
   sem_wait(&x);
   cont_lect++;
   if (cont_lect==1) sem_wait(&escr);
   sem_post(&x);
   p(("L%d LEE !!!\n",n));
   sleep(rand()%3);
   p(("L%d termina de leer.\n",n));
   sem_wait(&x);
   cont_lect--;
   if (cont_lect==0) sem_post(&escr);
   sem_post(&x);
}

void * escritor(void *arg)
{
   int n=(int)arg; /* cardinal */

   p(("E%d quiere escribir.\n",n));
   sem_wait(&escr);
   p(("E%d ESCRIBE !!!\n",n));
   sleep(rand()%3);
   p(("E%d termina de escribir.\n",n));
   sem_post(&escr);
}
