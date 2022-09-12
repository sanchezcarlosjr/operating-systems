#include <stdio.h>
#include <unistd.h>

bool x(int y) {
	printf("ABC,");
	return y;
}

int main()
{
   fork();
   x(fork()) && x(fork()) || x(fork());
   fork();
   return 0;
}
