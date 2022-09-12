#include <unistd.h>
#include <stdio.h>

int value = 1;
int f() {
	int t = 1;
	printf("f() PID=%d\n", getpid());
	if(fork() == 0) {
	        printf("fork1() PID=%d, PPID=%d\n", getpid(), getppid());
		t++;
	        // value = 3;
		printf("CHILD t=%d\n", t);
		printf("CHILD v=%d\n", value);
		return 0;
	}
	if(fork() == 0) {
	        printf("fork2() PID=%d, PPID=%d\n", getpid(), getppid());
		t++;
	        // value = 3;
		printf("CHILD t=%d\n", t);
		printf("CHILD v=%d\n", value);
		return 0;
	}
	value = 2;
	printf("PARENT t=%d\n", t);
	printf("PARENT v=%d\n", value);
	return 0;
}

void g() {
	int j = 0;
	j++;
	j++;
}

int main() {
	printf("START OFF PID=%d, PPID=%d\n", getpid(), getppid());
	int i = 0;
	i++;
	i++;
	i++;
	i++;
	g();
	i++;
	i++;
	f();
	printf("v=%d\n", value);
	printf("i=%d\n", i);
	return 0;
}
