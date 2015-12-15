#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
	if(argc != 2)
		return -1;
	printf("%s value: %s\n", argv[1], getenv(argv[1]));
	return 0;
}
