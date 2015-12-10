#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "iniparser.h"
#include "dictionary.h"

int main(int argc, char **argv)
{
	dictionary *ini;
	char *ini_name;
	char **val, **key;
      	unsigned *hash;
	int i = 0;
	char tem[1024];
	FILE *fd = NULL;
	if(argc < 2)
	{
		ini_name = "twisted.ini";
	}
	else
	{
		ini_name = argv[1];
	}
	if((ini = iniparser_load(ini_name)) == NULL)
	{
		printf("iniparser_load error\n");
		return -1;
	}
	val = ini->val;
	key = ini->key;
	hash = ini->hash;
	for(; *key != NULL; i++)
	{
		if(*key != NULL)
		{
			snprintf(tem, sizeof(tem) - 1, "ztj", *key);
			if(iniparser_set(ini, *key, tem) < 0)
			{
				printf("rewrite key %s, value %s error\n", *key, tem);
			}
			else
			{
				printf("rewrite key %s, value %s success\n", *key, tem);
			}
			val ++;
			key ++;
		}
		else
		{
			break;
		}
//		if(i > 10)
//		break;
	}
	if(NULL == (fd = fopen(ini_name, "w")))
	{
		printf("open file error\n");
		return -1;
	}
	iniparser_dump_ini(ini, stdout);
	iniparser_dump_ini(ini, fd);
	fclose(fd);
	iniparser_freedict(ini);
	return 0;
}










