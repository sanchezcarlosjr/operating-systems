#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

void executeChildProcess(const char* pathname, int i) {
	if (fork() != 0) {
		printf("Parent again! (pid:%d)\n", (int) getpid());	
		return;
	}
	unsigned int fileDescriptor;
	char c;
	int j;
	bool isError = (fileDescriptor = open(pathname, O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0;
	if(isError) {
		write(2, "Error\n", 6);
		exit(1);
	}
	printf("\t Child (pid:%d)\n", (int) getpid());	
	for (j=0; j < 100000; j++) {
		c='A' + i - 1;
		write(fileDescriptor, &c, 1);
	}
	printf("\t I've finish. Child (pid:%d)\n", (int) getpid());	
	exit(0);
}

int main(int argc, char* argv[]) {
	sleep(5);
	for (int i=1; i<argc; i++) {
		printf("%d,", i);
		printf("Parent (pid:%d)\n", (int) getpid());	
		executeChildProcess(argv[i], i);
	}
	exit(0);
}
