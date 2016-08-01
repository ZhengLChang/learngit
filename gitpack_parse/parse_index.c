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

char * sha1_to_hex(const unsigned char *sha1)
{
	static char buffer[50];
	static const char hex[] = "0123456789abcdef";
	char *buf = buffer;
	int i;

	for (i = 0; i < 20; i++) 
	{
		unsigned int val = *sha1++;
		*buf++ = hex[val >> 4];
		*buf++ = hex[val & 0xf];
		
	}
		return buffer;
}

int main(int argc, char **argv)
{
	int i = 0, j = 0, fd = -1;
	unsigned char head[5] = {0};
	int version;
	char packfile[1024] = {0};
	void *memBuf = NULL;
	int *entry_table_p = NULL, entry_table[256];
	unsigned char *cur_mem_location = NULL;
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

	/*will parse fanout table*/
	entry_table_p = (int *)(memBuf + 8);
	for(i = 0; i < 256; i++)
	{
		static  int frontValue = 0;
		if(frontValue > ntohl(entry_table_p[i]))
		{
			printf("non-monotonous\n");
			goto ERR;
		}
		entry_table[i] = ntohl(entry_table_p[i]) - frontValue;
		frontValue = ntohl(entry_table_p[i]);
		printf("%5d\t", ntohl(entry_table_p[i]));
		if((i + 1) % 8 == 0
			|| i == 255)
		{
			printf("\n");
		}
	}
	/*will parse second layer which is 20-byte object names which aready know in front layer*/
	cur_mem_location = (unsigned char *)(memBuf + 8 + 256 * 4);
	for(i = 0; i < 256; i++)
	{
		for(j = 0; j < entry_table[i]; j++)
		{
			if(cur_mem_location[0] != i)
			{
				printf("the third layer occur error\n");
				goto ERR;
			}
			cur_mem_location += 20;
		}
		if(j == entry_table[i])
		{
			printf("%#3x checkout OK\t count %d\n", i, j);
		}
		else 
		{
			printf("%#x checkout error!!!\n", i);
			goto ERR;
		}
	}
	/*The thired layer is cyclic redundancy checkout, we ignore it*/
	/**************************/
	/*the fourth layer is packfile offsers, we ignore it*/
	/*************************/
	/*the fifth layer is exist when packfiles larger than 2 GB, we ignore it*/
ERR:	
	munmap(memBuf, stat_buf.st_size);
	return 0;
}



















