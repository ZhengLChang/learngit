#include <wchar.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	wchar_t str[] = L"，";
	size_t len = wcslen(str);
	int i = 0;
//	printf("wchar_t size: %d\n", sizeof(wchar_t));
//	printf("size: %d\n", sizeof(str)/sizeof(wchar_t));
	for(i = 0; i < len; i++)
	{
		printf("%#x, ", (unsigned int)str[i]);
	}
	printf("0x0\n");
#if 0
	for(i = 0; i < sizeof(str); i++)
	{
		printf("%#x, ", *((unsigned char *)str + i));
	}
	printf("\n");
#endif
	return 0;
}
