#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct _realloc_test
{
	int a;
	char *b;
}realloc_test;

int main(void)
{
	realloc_test  *mytest = NULL, *temp = NULL;
	if((mytest = realloc(mytest, sizeof(realloc_test))) == NULL)
	{
		printf("error\n");
		return -1;
	}
	temp = mytest;
	if((mytest = realloc(mytest, sizeof(realloc_test))) == NULL)
	{
		printf("error\n");
		return -1;
	}
	else
	{
		temp = mytest + 1;
		printf("%x %d %x\n", temp,temp->a, temp->b);
	}
	if((mytest = realloc(mytest, sizeof(realloc_test))) == NULL)
	{
		printf("error\n");
		return -1;
	}
	else
	{
		temp = mytest + 2;
		printf("%x %d %x\n", temp,temp->a, temp->b);
	}
	if((mytest = realloc(mytest, sizeof(realloc_test))) == NULL)
	{
		printf("error\n");
		return -1;
	}
	else
	{
		temp = mytest + 3;
		printf("%x %d %x\n", temp,temp->a, temp->b);
	}
		return 0;
}
