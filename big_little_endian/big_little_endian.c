#include <stdio.h>

int main(void)
{
	short int x = 0x1122;
	char x1, x2;
	x1 = ((char *)&x)[0];
	x2 = ((char *)&x)[1];
	if(x1 == 0x22 && x2 == 0x11)
	{
		printf("little endian\n");
	}
	else
	{
		printf("big endian\n");
	}
	return 0;
}
