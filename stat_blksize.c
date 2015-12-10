#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	struct stat buf;
	if(argc != 2)
	{
		printf("usage: %s filename\n", argv[0]);
		return -1;
	}
	if(stat(argv[1], &buf) < 0)
	{
		printf("stat error: %s\n", strerror(errno));
		return -1;
	}
	printf("file size %ld\n", buf.st_size);
	printf("blksize %ld\n", buf.st_blksize);
	printf("blocks %ld\n", buf.st_blocks);
	return 0;
}




