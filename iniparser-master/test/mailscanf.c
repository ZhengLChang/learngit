#include <stdio.h>

int main(void)
{
	char szTmp[10] = {10};
	scanf("%*[^@]@%[^.]", szTmp);
	printf("%s\n", szTmp);
	return 0;
}
