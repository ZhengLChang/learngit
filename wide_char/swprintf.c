#include <wchar.h>
#include <stdio.h>

int main(void)
{
	wchar_t wch[64];
	swprintf(wch, sizeof(wch) / sizeof(wch[0]), 
			L"%s", "zhj");
	wprintf(L"%ls\n", wch);
	return 0;
}
