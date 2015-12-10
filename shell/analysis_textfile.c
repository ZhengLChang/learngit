#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
	int fd;
	struct stat buf;
	char *str;
	if(argc != 2)
	{
		printf("usage: %s filename\n", argv[0]);
		return -1;
	}
	if((fd = open(argv[1], O_RDONLY)) < 0)
	{
		printf("open file error: %s\n", strerror(errno));
		return -1;
	}
	if(fstat(fd, &buf) < 0)
	{
		printf("fstat error: %s\n", strerror(errno));
		return -1;
	}
	str = calloc(1, buf.st_size + 1);
	if(str == NULL)
	{
		printf("calloc error: %s\n", strerror(errno));
		return -1;
	}
	if(read(fd, str, buf.st_size) != buf.st_size)
	{
		printf("read error: %s\n", strerror(errno));
		return -1;
	}
	write(1, str, buf.st_size);
	return -1;
}










