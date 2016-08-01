#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int i = 0, fd = -1;
	unsigned char head[5] = {0};
	int version;
	char packfile[1024] = {0};
	void *memBuf = NULL;
	struct stat stat_buf;
	for(i = 1; i < argc; i++)
	{
		if(!memcmp(argv[i] + strlen(argv[i]) - 4, ".idx", 4) &&
		  !memcmp(argv[i], "filename=", 9))
		{
			strncpy(packfile, argv[i] + strlen("filename="), sizeof(packfile));
			break;
		}
	}
	if(packfile[0] == '\0')
	{
		printf("usage: %s filename=[XXXXX.idx]\n", argv[0]);
		return -1;
	}
	if((fd = open(packfile, O_RDWR)) < 0)
	{
		printf("open file error: %s\n", strerror(errno));
		return -1;
	}
	if(fstat(fd, &stat_buf) < 0)
	{
		printf("fstat error: %s\n", strerror(errno));
		return -1;
	}
	memBuf = mmap(NULL, stat_buf.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	close(fd);
	if(MAP_FAILED == memBuf)
	{
		printf("mmap error: %s\n", strerror(errno));	
		return -1;
	}
	memcpy(head, memBuf, 4);
	memcpy(&version, memBuf + 4, 4);
	head[4] = '\0';
	printf("will print git indexfile head\n\t\t flag: %d %d %d %d version: %d\n", 
					head[0], head[1], head[2], head[3], ntohl(version));
	munmap(memBuf, stat_buf.st_size);
	return 0;
}



















