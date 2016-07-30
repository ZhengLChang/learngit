#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	char packfile[1024] = {0};
	char fileHead[5] = {0}, versionNum[5] = {0};
	int objectNum = 0;
	void *memBuf = NULL;
	struct stat statBuf;
	int fd = -1;
	int i = 0;
	for(i = 1; i < argc; i++)
	{
		if(!memcmp(argv[i] + strlen(argv[i]) - 5, ".pack", 5))
		{
			strncpy(packfile, argv[i] + strlen("filename="), sizeof(packfile));
			break;
		}
	}
	if(packfile[0] == '\0')
	{
		printf("usage: %s [XXXXX.pack]\n", argv[0]);
		return -1;
	}
	if((fd = open(packfile, O_RDWR)) < 0)
	{
		printf("open file error\n");
		return -1;
	}
	if(fstat(fd, &statBuf) == -1)
	{
		close(fd);
		printf("stat error\n");
		return -1;
	}
	memBuf = mmap(NULL, statBuf.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	memcpy((void *)fileHead, memBuf, 4);
	memcpy((void *)versionNum, memBuf + 4, 4);
	memcpy((void *)&objectNum, memBuf + 8, 4);
	fileHead[4] = '\0';
	versionNum[4] = '\0';
	printf("will print git packfile head\n\t flag: %s version: %d object: %d\n", fileHead, ntohl(*(int *)versionNum), ntohl(objectNum));
	munmap(memBuf, statBuf.st_size);
	return 0;
}






















