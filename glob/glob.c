#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_PATH "../*"

int main(int argc, char **argv)
{
	int ret, i;
	glob_t pglob;
	
	if((ret = glob(TEST_PATH, 0, NULL, &pglob)) != 0)
	{
		printf("error: glob()\n");
		return -1;
	}
	for(i = 0; i < pglob.gl_pathc; i++)
	{
		if(i != 0 && i % 3 == 0)
			printf("\n");
		printf("%s\t", pglob.gl_pathv[i]);
	}
	printf("\n");
	printf("the count of match file: %d\n", i);
	globfree(&pglob);
	return 0;
}








