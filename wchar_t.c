#include <stdio.h>
#include <stdlib.h>

int main(void)
{
//	printf("sizeof(wchar_t) = %d\n", sizeof(wchar_t));
	wchar_t wStr[] = L"比比";
	char *str_p = (char *)wStr;
	printf("print wchar: %ls\n", wStr);
	int i = 0;
	for(i = 0; i < sizeof(wStr);)
	{
		printf("%08x ", *(str_p + i));
		i++;
		if(i % 4 == 0)
			putchar(10);
	}
	return 0;
}
