#include <stdio.h>
#include <wchar.h>
#include <locale.h>

int main(void)
{
	wchar_t ws[] = L"中文";
	char str[] = "中文";
	setlocale(LC_ALL, "zh_CN.UTF-8");
	wprintf(L"%ls\n", ws);
	wprintf(L"%s", str);
	return 0;
}
