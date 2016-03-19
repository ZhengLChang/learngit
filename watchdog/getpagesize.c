#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int i = getpagesize();
	printf("page size = %d\n", i);
	return 0;
}
