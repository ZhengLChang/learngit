#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

int main(int argc, char **argv)
{
	char path[] = "/first/file";
	char *str = basename(path);
	printf("after basename: %s\n", path);
	printf("get string: %s\n", str);
	return 0;	
}
