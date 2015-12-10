#include <stdio.h>

int main(void)
{
	char a[] = "abcde1234";
	char *const c = a;

	c[0] = 'x';

	printf("c = %s\n", c);
	return 0;
}
