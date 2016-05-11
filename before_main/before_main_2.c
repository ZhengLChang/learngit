#include <stdio.h>
#include <stdlib.h>

void init(void) __attribute__((constructor));
void init(void)
{
	printf("before enter main!\n");
}
void exit_func(void)
{
	printf("after leave main!\n");
}
int main()
{
	int *m = malloc(5 * sizeof(int));
	atexit(exit_func);
	printf("hello world!\n");
	return 0;
}
