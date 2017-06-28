#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

int main(void)
{
	int n = 0, i;
	wchar_t str_Contacts[] = {0xFF00, 0x0};
	setlocale(LC_ALL, "zh_CN.UTF-8");
#if 1
	for(i = 0; i < (0xFFEE - 0xFF00 + 1); i++)
	{
		wprintf(L"%#x = ", (int)str_Contacts[0]);
		n = wprintf(L"%ls\n", str_Contacts);
		if((i + 1) % 8 == 0)
			printf("\n");
		str_Contacts[0] = ((unsigned)str_Contacts[0]) + 1;
	}
	wprintf(L"\n");
#endif
//	n = wprintf(L"%ls\n", str_Contacts);
	wprintf(L"********************\n");
#if 0
	wprintf(L"%d\n", n);
#endif
	wprintf(L"********************\n");
	return 0;
}
