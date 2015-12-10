#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("format error\n");
		return -1;
	}
	wchar_t *str_p = NULL;
	wchar_t *storeStr = (wchar_t *)calloc(1, sizeof(wchar_t) * 128);
	str_p = storeStr;
	swprintf(str_p, 128 * sizeof(wchar_t), L"%s", argv[1]);
	wprintf(L"%s\n", str_p);
//	strncpy(storeStr, argv[1], strlen(argv[1]));
	while(*str_p != L'\0')
	{
		printf("0x%x, ", (unsigned)*str_p);
		str_p++;
	}
	printf("0x0\n");
	free(storeStr);
	return 0;
}
