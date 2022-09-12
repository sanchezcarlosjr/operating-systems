#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

int executeChildProcess(const char* pathname, int i) {
	if (fork() != 0) {
		printf("Parent again! (pid:%d), (ppid:%d)\n", (int) getpid(), (int) getppid());	
		return 0;
	}
	unsigned int fileDescriptor;
	char c;
	int j;
	bool isError = (fileDescriptor = open(pathname, O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0;
	if(isError) {
		write(2, "Error\n", 6);
		return 1;
	}
	printf("\t Child (pid:%d) (ppid:%d)\n", (int) getpid(), getppid());	
	for (j=0; j < 100; j++) {
		c='A' + i - 1;
		write(fileDescriptor, &c, 1);
	}
	printf("\t I've done. Child (pid:%d) (ppid:%d)\n", (int) getpid(), getppid());	
	return 0;
}

int main(int argc, char* argv[]) {
	for (int i=1; i<argc; i++) {
		printf("%d,", i);
		printf("Parent (pid:%d) (ppid:%d)\n", (int) getpid(), getppid());	
		executeChildProcess(argv[i], i);
	}
	printf("Process (pid:%d) (ppid:%d)\n", (int) getpid(), getppid());	
	exit(0);
}
