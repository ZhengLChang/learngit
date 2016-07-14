#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>
#include "sync-samples.h"

const char *message = "This is very important data!\n";

int main(int argc, char **argv)
{
	int ret;
	size_t message_len;
	FILE *fp = NULL;
	int fd, dir_fd;
	char *containing_dir;

	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s <filename\n>");
		exit(USER_ERR);
	}
	fp = fopen(argv[1], "w");
	if(!fp)
	{
		perror("fopen");
		exit(LIB_ERR);
	}
	message_len = strlen(message);
	ret = fwrite(message, message_len, 1, fp);
	if(ret != 1)
	{
		fprintf(stderr, "fwrite failed: %d\n", ferror(fp));
		exit(LIB_ERR);
	}
	if(fflush(fp) != 0)
	{
		perror("fflush");
		if(errno == EBADF)
		{
			exit(LIB_ERR);
		}
		else
		{
			exit(SYS_ERR);
		}
	}
	fd = fileno(fp);
	if(fd == -1)
	{
		perror("fileno");
		exit(LIB_ERR);
	}
	if(fsync(fd) < 0)
	{
		perror("fsync");
		exit(SYS_ERR);
	}
	containing_dir = dirname(argv[1]);
	dir_fd = open(containing_dir, O_RDONLY);
	if(dir_fd < 0)
	{
		perror("open");
		exit(SYS_ERR);
	}
	if(fsync(dir_fd) < 0)
	{
		perror("fsync2");
		exit(SYS_ERR);
	}
	if(close(dir_fd) < 0)
	{
		perror("close");
		exit(SYS_ERR);
	}
	if(fclose(fd) < 0)
	{
		perror("fclose");
		exit(SYS_ERR);
	}
	exit(0);
}




















