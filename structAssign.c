#include <stdio.h>

struct assign
{
	int i;
	char str[20];
};
int main(void)
{
	struct assign test1 = {1, "zhenghuijie"};
	struct assign test2 = test1;
	printf("now test2 is: %d, %s\n", test2.i, test2.str);
	return 0;
}
