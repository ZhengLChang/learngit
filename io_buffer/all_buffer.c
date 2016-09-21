#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char **argv)
{
	int i = 0;
	for(i = 0; i < 1025; i++)
	{
		fprintf(stdout, "a");
	}
	while(1);
	return 0;
}
