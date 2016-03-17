#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	wchar_t text[16];
	swprintf(text, sizeof(text), L"%d\n", 17);
	wprintf(text);
	return 0;
}
