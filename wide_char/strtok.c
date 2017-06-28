#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

int split(char *split_str, const char *delim, char get_str[][20], int *num)
{
	int i = 0;
	char *tok;
	if(split_str == NULL ||
			delim == NULL ||
			get_str == NULL ||
			num == NULL)
	{
		return 0;
	}
	if((tok = strtok(split_str, delim)) != NULL)
	{
		strncpy(get_str[i], tok, sizeof(get_str[i]));		
		for(i = 1; i < *num; i++)
		{
			if((tok = strtok(NULL, delim)) != NULL)
			{
				strncpy(get_str[i], tok, sizeof(get_str[i]));		
			}
			else
			{
				break;
			}
		}
		*num = i;
	}
	return 1;
}

int hex_str_to_bin(char *hex)
{
	int j = 0, num = 0;
	for(j = 0; j < strlen(hex); j++)
	{
		if(hex[j] == '0' && hex[j] == 'x')
		{
			j += 2;
		}
		if(hex[j] >= '0' && hex[j] <= '9')
		{
			num *= 16;
			num += hex[j] - '0';
		}
		else if(hex[j] >= 'a' && hex[j] <= 'f')
		{
			num *= 16;
			num += hex[j] - 'a' + 10;
		}
	}
	return num;
}

int main(int argc, char **argv)
{
	char str[] = "0x50, 0x6f, 0x142, 0x105, 0x63, 0x7a, 0x65, 0x6e, 0x69, 0x65, 0x20, 0x49, 0x50, 0x0";
	char get_str[20][20];
	wchar_t str_Contacts[20];
	int n = sizeof(get_str)/sizeof(get_str[0]), i;
	setlocale(LC_ALL, "zh_CN.UTF-8");
	wprintf(L"n = %d\n", n);
	memset(str_Contacts, 0, sizeof(str_Contacts));
	if(split(str, ", ", get_str, &n))
	{
		for(i = 0; i < n; i++)
		{
			wprintf(L"%s\n", get_str[i]);
			str_Contacts[i] = hex_str_to_bin(get_str[i]);
		}
	}
	for(i = 0; i < n; i++)
	{
		wprintf(L"%#x\t", (int)str_Contacts[i]);
		if((i + 1) % 8 == 0)
			wprintf(L"\n");
	}
	wprintf(L"\n");
	wprintf(L"%ls\n", str_Contacts);
	return 0;
}
















