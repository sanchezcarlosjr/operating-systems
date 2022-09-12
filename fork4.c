#include <stdio.h>
#include <unistd.h>

int main()
{
	int a=5;
	for(int i=0; i<5; i++) {
		if (fork() == 0)
		{
			a = a + 5;
			printf("%d, %d \n", a, &a);
		}
		else
		{
			a = a - 5;
			printf ("%d, %d \n", a,& a);
		}}
	return 0;
}
