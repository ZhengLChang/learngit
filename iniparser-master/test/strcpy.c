#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	char tem[20] = "zhenghuijie";
	strcpy(tem, tem + 1);
	printf("%s\n", tem);
	return 0;
}
