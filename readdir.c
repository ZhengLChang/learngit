#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

char store[50][100];

int main(int argc, char *argv[])
{
	DIR *dir_p = NULL;
	struct dirent *dir_entry = NULL;
	int i = 0;
	const char *filename;
	if(argc != 2)
	{
		filename=".";		
	}
	else
	{
		filename=argv[1];
	}
	if(NULL == (dir_p = opendir(filename)))
	{
		printf("opendir error: %s\n", strerror(errno));
		return -1;
	}
	i = 0;
	while((dir_entry = readdir(dir_p)) != NULL)
	{
		strncpy(store[i], dir_entry->d_name, strlen(dir_entry->d_name) + 1);
		i++;
		printf("%s\t%d\n", dir_entry->d_name, dir_entry->d_type);
	}
	for(; i > 0; )
	{
		i--;
		printf("%s\n", store[i]);
	}
	return 0;
}
