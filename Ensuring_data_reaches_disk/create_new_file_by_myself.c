#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include <fcntl.h>
char *message = "zhengTianjie";
int write_to_file(char *filename, char *data)
{
       	FILE *fp;
	char path[128], tmp_path[128], *containing_dir;
	int dir_fd, tmp_fd;
	int ret;
		       
	if(data == NULL || filename == NULL || data[0] == '\0')
		return -1;
	strncpy(path, filename, strlen(filename));
	containing_dir = dirname(path);
	if((dir_fd = open(containing_dir, O_RDONLY)) < 0)
	{
		return -1;
	}
	ret = snprintf(tmp_path, 128, "%s/tmp_XXXXXX",
			containing_dir);
	if(ret > sizeof(tmp_path))
		return -1;
	tmp_fd = mkstemp(tmp_path);
	if(tmp_fd == -1)
		return -1;
	fp = fdopen(tmp_fd, "w");
	if ( fp == NULL ) {
		return -1;
	}
	if(1 != fwrite(data, strlen(data), 1, fp))
		return -1;
	if(fflush(fp) != 0)
		return -1;
	if(fsync(tmp_fd) < 0)
		return -1;
	/* A common error is not to check file close return */
	if ( fclose( fp ) != 0 )
		return -1;
	if(rename(tmp_path, filename) < 0)
		return -1;
	if(fsync(dir_fd) < 0)
		return -1;
	if(close(dir_fd) < 0)
		return -1; 
	return 0;
}

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		printf("Usage %s <filename>\n", basename(argv[0]));
		return -1;
	}

	if(write_to_file(argv[1], message) != 0)
	{
		printf("error occor!\n");
	}
	else
	{
		printf("success!\n");
	}
	return 0;
}












