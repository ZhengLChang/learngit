#include <stdio.h>

int main(void)
{
	char buf1[10] = {0};
	char buf2[10] = {0};
	scanf("%[^=]=%[^;#]", buf1, buf2);
	printf("%s, %s\n", buf1, buf2);
	return 0;
}
