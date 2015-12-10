#include <stdio.h>
extern int *array;
int main()
{
	printf("%p\n", array);
//	array[0] = 2;
	return 0;
}
