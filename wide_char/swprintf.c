#include <wchar.h>
#include <stdio.h>

int main(void)
{
	wchar_t wch[64], wstr[128];
	swprintf(wch, sizeof(wch) / sizeof(wch[0]), 
				L"%s", "zhj");
	swprintf(wstr, sizeof(wstr) / sizeof(wstr[0]), 
			L"%ls%s", wch, " is here");
	wprintf(L"%ls\n", wstr);
	return 0;
}
