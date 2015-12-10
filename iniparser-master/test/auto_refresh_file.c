#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "iniparser.h"
#include "dictionary.h"

int main(int argc, char **argv)
{
	dictionary *refreshedIni, *resourceIni;
	char *refreshedFile, *resourceFile;
	char **refreshedVal, **refreshedKey;
	char **resourceVal, **resourceKey;
	FILE *fd;
	
	if(argc != 3)
	{
		printf("usage: %s refreshedFile resourceFile\n", argv[0]);
		return -1;
	}
	else
	{
		refreshedFile = argv[1];
		resourceFile = argv[2];
	}
	if((refreshedIni = iniparser_load(refreshedFile)) == NULL)
	{
		printf("iniparser_load error\n");
		return -1;
	}
	if((resourceIni = iniparser_load(resourceFile)) == NULL)
	{
		printf("iniparser_load error\n");
		return -1;
	}
	refreshedVal = refreshedIni->val;
      	refreshedKey = refreshedIni->key;
	resourceVal = resourceIni->val;
	resourceKey = resourceIni->key;
	for(; *resourceKey != NULL; )
	{
		if(*resourceKey != NULL);
		{
			if(*resourceVal != NULL)
			{
				iniparser_set(refreshedIni, *resourceKey,
						*resourceVal);
			}
			resourceKey++;
			resourceVal++;
		}
	}
	if(NULL == (fd = fopen(refreshedFile, "w")))
	{
		printf("fopen file error\n");
		return -1;
	}
	iniparser_dump_ini(refreshedIni, fd);
	fclose(fd);
	iniparser_freedict(refreshedIni);
	iniparser_freedict(resourceIni);

	return 0;
}

















