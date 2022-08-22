#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	int i, j, pid, salida, estado;
	char c;
	for (i=1; i<argc; i++) {
		if ((pid= fork())==0) {
			if ((salida = open(argv[i], O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0) {
				write(2, "Error\n", 6);
				exit(1);
			}
			for (j=0; j < 100000; j++) {
				c= 'A' + i - 1;
				write(salida, &c, 1);
			}
			exit(0);
		}
		printf("Arrancado el proceso %d\n", pid);
	}
	for (i=1; i<argc; i++) {
		pid = wait(&estado);
		printf("Termina el proceso %d\n", pid);
	}
	exit(0);
}
