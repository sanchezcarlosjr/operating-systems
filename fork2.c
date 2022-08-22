#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void main() {
	for(int i=0; i<4; i++) {
		if(fork() == 0) {
			printf("PID %d\n", (int) getpid());
			sleep(10);
		}
	}
	sleep(10);
	exit(0);
}
