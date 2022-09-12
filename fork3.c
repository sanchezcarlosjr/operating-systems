#include <stdio.h>
#include <unistd.h>

int main()
{
	int a=5;
	if (fork() == 0)
	{
		a = a + 5;
		printf("%d, %d \n", a, &a);
	}
	else
	{
		a = a - 5;
		printf ("%d, %d \n", a,& a);
	}
	return 0;
}
