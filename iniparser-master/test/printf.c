#include <stdio.h>

int main(void)
{
	char str[100] = "zheng\nhui\njie\n";
	printf("%[^\\n]", str);
	return 0;
}
