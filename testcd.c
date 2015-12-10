#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
	system("cd ..;");
	FILE *fp = fopen("testcd.txt", "w");
	if(fp == NULL)
	{
		printf("error\n");
		return -1;
	}
	return 0;
}
