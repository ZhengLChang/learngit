#include <wchar.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	wchar_t str[] = L"转移:";
	size_t len = wcslen(str);
	int i = 0;
	for(; i < len; i++)
	{
		printf("%#x, ", (unsigned int)str[i]);
	}
	printf("\n");
	return 0;
}
