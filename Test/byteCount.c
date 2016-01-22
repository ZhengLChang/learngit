#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		return -1;
	}
	printf("strlen(%s) = %d\n", argv[1], strlen(argv[1]));
	return 0;
}
