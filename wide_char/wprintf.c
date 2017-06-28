#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

int main(void)
{
	int n = 0, i;
	wchar_t str_Contacts[] = {0x50, 0x65, 0x142, 0x6e, 0x79, 0x20, 0x72, 0x65, 0x73, 0x65, 0x74, 0x0};
#if 0
	wchar_t str_Contacts[2];
#endif
	setlocale(LC_ALL, "zh_CN.UTF-8");
#if 0
	memset(str_Contacts, 0, sizeof(str_Contacts));
	*((unsigned char *)str_Contacts + 0) = 0xdc;
	*((unsigned char *)str_Contacts + 1) = 0x95;
#endif
#if 1
	for(i = 0; i < sizeof(str_Contacts) / sizeof(str_Contacts[0]); i++)
	{
		wprintf(L"%#x\t", (int)str_Contacts[i]);
		if((i + 1) % 8 == 0)
			printf("\n");
	}
	wprintf(L"\n");
#endif
	n = wprintf(L"%ls\n", str_Contacts);
	wprintf(L"********************\n");
#if 0
	wprintf(L"%d\n", n);
#endif
	wprintf(L"********************\n");
	return 0;
}
