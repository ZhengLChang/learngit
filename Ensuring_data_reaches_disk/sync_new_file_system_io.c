#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "sync-samples.h"

const char *message = "This is very important data!\n";

int main(int argc, char **argv)
{
	int ret;
	size_t message_len;
	int fd, dir_fd;
	mode_t old_mode;
	char *containing_dir;
	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", basename(argv[0]));
		exit(USER_ERR);
	}
	old_mode = umask((mode_t)0);
	fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd < 0)
	{
		perror("open");
		exit(SYS_ERR);
	}
	umask(old_mode);
	message_len = strlen(message);
	ret = full_write(fd, message, message_len);
	if(ret != (int)message_len)
	{
		if(ret < 0)
		{
			perror("write");
			exit(SYS_ERR);
		}
		if(unlink(argv[1]) < 0)
			perror("unlink");
		exit(SYS_ERR);
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
		perror("close dir_fd");
		exit(SYS_ERR);
	}
	if(close(fd) < 0)
	{
		perror("close fd");
		exit(SYS_ERR);
	}
	exit(0);
}
















