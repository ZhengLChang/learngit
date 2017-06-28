#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

int main()
{
	FILE *fp = NULL;
	char buf[1024] = {0};
	wchar_t ch[2];
	setlocale(LC_ALL, "zh_CN.UTF-8");
	fp = fopen("/disk4/AX/A1x/src/mutilanguge/chinese_font.c", "r");
	if(fp == NULL)
	{
		return -1;
	}
	memset(ch, 0, 2 * sizeof(wchar_t));
	while(fgets(buf, sizeof(buf), fp))
	{
		int i = 0;
		if(buf[0] == '\n' || buf[0] == '\r' || strncmp(buf, "static const unsigned char unicode_", strlen("static const unsigned char unicode_")) != 0)
			continue;
		ch[0] = 0;
		for(i = strlen("static const unsigned char unicode_"); 
				(buf[i] >= '0' && buf[i] <= '9') ||
				(buf[i] >= 'a' && buf[i] <= 'f') ||
				(buf[i] >= 'A' && buf[i] <= 'F'); i++)
		{
			ch[0] = ch[0] << 4;
			if(buf[i] >= '0' && buf[i] <= '9')
			{
				ch[0] += buf[i] - '0';
			}
			else if((buf[i] >= 'a' && buf[i] <= 'f'))
			{
				ch[0] += buf[i] - 'a' + 10;
			}
			else if(buf[i] >= 'A' && buf[i] <= 'F')
			{
				ch[0] += buf[i] - 'A' + 10;
			}
		}
			if(ch[0] >= 0x4e00 && ch[0] <= 0x9fd8)
			{
			}
			else
			{
				printf("ch[0] = %#x error\n", (int)((unsigned short)ch[0]));
				wprintf(L"%ls\n", ch);
			}

	}

	return 0;
}
























