#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	wchar_t text[16];
	swprintf(text, sizeof(text), L"%d\n", 17);
	printf(text);
	wprintf(L"zheng %ls\n", text);
	wprintf(L"*********delete list's item***********\n");
	return 0;
}
