#include <stdio.h>
#include <wchar.h>
#include <locale.h>

int main(void)
{
	//wchar_t str_Search[] = {0x53, 0x65, 0x61, 0x72, 0x63, 0x68, 0x0};
	wchar_t str_Search[] = {0x44, 0x69, 0x61, 0x6c, 0x65, 0x64, 0x20, 0x43, 0x61, 0x6c, 0x6c, 0x73, 0x0};
	//wchar_t ws[] =	{0x42, 0x6c, 0x6f, 0x6b, 0x75, 0x6a, 0x20, 0x61, 0x6e, 0x6f, 0x6e, 0x69, 0x6d, 0x6f, 0x77, 0x65, 0x20, 0x70, 0x6f, 0x142, 0x2e, 0x0};
	setlocale(LC_ALL, "zh_CN.UTF-8");
	wprintf(L"%ls\n", str_Search);
	return 0;
}
