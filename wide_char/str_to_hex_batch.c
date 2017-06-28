#include <wchar.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	wchar_t str[][150] = { 
		L"Połączenie IP",
		L"Połączenie IP", 
	};
	int i = 0, j = 0;
#if 0
	printf("wchar_t size: %d\n", sizeof(wchar_t));
	printf("size: %d\n", sizeof(str[j])/sizeof(wchar_t));
#endif
	for(j = 0; j < sizeof(str) / sizeof(str[0]); j++)
	{
		size_t len = wcslen(str[j]);
		printf("size: %d\n", len);
		for(i = 0; i < len; i++)
		{
			printf("%#x, ", (unsigned int)str[j][i]);
		}
		printf("0x0\n\n");
	}
#if 0
	for(i = 0; i < sizeof(str); i++)
	{
		printf("%#x, ", *((unsigned char *)str + i));
	}
	printf("\n");
#endif
	return 0;
}
