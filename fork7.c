#include <stdio.h>
#include <unistd.h>
int level = 0;

int main() {
	printf("%*c[START] Process (pid=%d) (ppid=%d)\n", level*5, '+', getpid(), getppid());
	for(int i=0; i<2; i++) {
		if(fork() == 0) {
			level++;
			printf("%*c[START] Child Process (pid=%d) (ppid=%d)\n", level*5, '+', getpid(), getppid());
		} else {
			printf("%*c[START] Parent Process (pid=%d) (ppid=%d)\n", level*5, '+', getpid(), getppid());
		}
	}
	printf("%*c[DONE] Process (pid=%d) (ppid=%d)\n", level*5, '-', getpid(), getppid());
	return 0;
}
