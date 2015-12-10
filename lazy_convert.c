#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("format error\n");
		return -1;
	}
	char *str_p = NULL;
	char *storeStr = (char *)calloc(1, strlen(argv[1]) + 1);
	strncpy(storeStr, argv[1], strlen(argv[1]));
	str_p = strtok(storeStr, ";&#");
	while(str_p)
	{
		printf("0x%x, ", atoi(str_p));
		str_p = strtok(NULL, ";&#");
	}
	printf("0x0\n");
	return 0;
}
