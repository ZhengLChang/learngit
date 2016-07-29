#include <wchar.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	unsigned char buf[1024];
	FILE *fp = NULL;
	int len;
	int i = 0;
	unsigned char buf1[] = "中文";
	wchar_t str[] = L"中文";
	char filename[1024];
	if(argc != 2 || memcmp(argv[1], "filename=", strlen("filename=")))
	{
		printf("usage %s filename=[filename]\n", argv[0]);
		return -1;
	}
	strncpy(filename, argv[1] + strlen("filename="), sizeof(filename) - 1);
	printf("will read from %s\n", filename);
	fp = fopen(filename, "r");
	if(fp == NULL)
	{
		printf("open file error\n");
		return -1;
	}
	len = fread(buf, 1, sizeof(buf), fp);
	if(ferror(fp) || !len)
	{
		printf("read file error\n");
		return -1;
	}
	else
	{
		printf("read %d-bytes\n", len);
		printf("buf data %s\n", (char *)buf);
	}
	printf("***********read from file**************\n");
	for(i = 0; i < len; i++)
	{
		printf("%#x, ", buf[i]);
	}
	printf("\n");
	
	printf("normal string\n");
	len = strlen(buf1);
	for(i = 0; i < len; i++)
	{
		printf("%#x, ", buf1[i]);
	}
	printf("\n");
	printf("wide string\n");
	len = wcslen(str);
	for(i = 0; i < len; i++)
	{
		printf("%#x, ", (unsigned int)str[i]);
	}
	printf("\n");
	return 0;
}
