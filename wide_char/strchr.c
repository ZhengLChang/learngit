#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("usage: %s string\n", argv[0]);
		return -1;
	}
	printf("%d\n", strchr(argv[1], '.') == NULL);
	return 0;
}
