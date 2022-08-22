#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	int i, j, pid, salida;
	char c;
	for (i=1; i<argc; i++) {
		printf("%d,", i);
		printf("hello, I am parent (pid:%d)?\n", (int) getpid());	
		if ((pid=fork())==0) {
			if ((salida = open(argv[i], O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0) {
				write(2, "Error\n", 6);
				exit(1);
			}
			printf("hello, I am child (pid:%d)\n", (int) getpid());	
			for (j=0; j < 99; j++) {
				c= 'A' + i - 1;
				write(salida, &c, 1);
			}
			sleep(10);
			exit(0);
		}
	}
	exit(0);
}
