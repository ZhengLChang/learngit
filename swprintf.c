#include <stdio.h>
#include <wchar.h>

int main(int argc, char **argv)
{
	char str[] = "zhenghuijie";
	wchar_t wstr[128] = {0};
	swprintf(wstr, 128, L"%s", str);
	wprintf(L"%ls\n", wstr);
	return 0;
}
