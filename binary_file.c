#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv)
{
	struct stat buf_stat;
	off_t file_size;
	int fd, n_readed;
	char *buf = NULL;
	if(argc != 2)
	{
		printf("usage: %s filename\n", argv[0]);
		return -1;
	}
	if((fd = open(argv[1], O_RDONLY)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	if(fstat(fd, &buf_stat) < 0)
	{
		printf("fstat error: %s\n", strerror(errno));
		return -1;
	}
	file_size = buf_stat.st_size;
	if(file_size <= 0)
	{
		printf("file is too small\n");
		return -1;
	}
	if((buf = malloc(file_size * sizeof(char))) == NULL)
	{
		printf("malloc error: %s\n", strerror(errno));
		return -1;
	}
	if((n_readed = read(fd, buf, file_size)) != file_size)
	{
		printf("read error: %s\n", strerror(errno));
		return -1;
	}
	if(memchr(buf, 0, n_readed) != 0)
	{
		printf("%s is binary file\n", argv[1]);
		return 0;
	}
	else
	{
		printf("%s is text file\n", argv[1]);
		return 0;
	}
}

















