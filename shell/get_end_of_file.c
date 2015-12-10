#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	FILE *fp = NULL;
	char str[1024] = "";
	int len = 0, line = 0;
	if(argc != 2)
	{
		printf("usage: %s filename\n", argv[0]);
		return -1;
	}
	fp = fopen(argv[1], "r");
	if(fp == NULL)
	{
		printf("open file error\n");
		return -1;
	}
	while(fgets(str, 1024, fp) != NULL)
	{
		len = strlen(str);
		line++;
		if(str[len - 1] == '\n')
		{
			printf("%d is end by '\\n'\n", line);
		}
		else
		{
			printf("%d is not end by '\\n'\n", line);
		}
	}
	return 0;
}












