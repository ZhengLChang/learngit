#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	char str[100] = "ZhengHuijie";
	memmove(str + 2, str, strlen(str) + 1);
	str[0] = 'O';
	str[1] = 'K';
	printf("str = %s\n", str);
	return 0;
}
