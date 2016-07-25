#include <stdio.h>

int main(void)
{
	char str[] = "Hello, stupid world";
	char buf[5];
	snprintf(buf, 4, "%.*s", 2, str);
	printf("%s\n", buf);
	return 0;
}
