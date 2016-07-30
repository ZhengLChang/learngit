#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <zlib.h>
/*copy from git source*/
enum object_type
{
	OBJ_EXT = 0,
	OBJ_COMMIT = 1,
	OBJ_TREE = 2,
	OBJ_BLOB = 3,
	OBJ_TAG = 4,
};
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

void *unpack_data(unsigned char *packed_data, unsigned long size, unsigned long left, void *stored_buf)
{
	z_stream stream;
	int st;
	if(packed_data == NULL || stored_buf == NULL)
	{
		return NULL;
	}
	memset(&stream, 0, sizeof(stream));
	stream.next_in = packed_data;
	stream.avail_in = left;
	stream.next_out = stored_buf;
	stream.avail_out = size;
	inflateInit(&stream);
	st = inflate(&stream, Z_FINISH);
	inflateEnd(&stream);
	if(st != Z_STREAM_END)
	{
		printf("st != Z_STREAM_END\n");
		return NULL;
	}
	else if(stream.total_out != size)
	{
		printf("stream.total_out != size\n");
		return NULL;
	}
	return stored_buf;
}

int main(int argc, char **argv)
{
	char packfile[1024] = {0};
	char fileHead[5] = {0}, versionNum[5] = {0};
	int objectNum = 0;
	void *memBuf = NULL;
	unsigned char *packData = NULL;
	struct stat statBuf;
	enum object_type obj_type;
	int fd = -1;
	int i = 0;
	unsigned char oneByte;
	unsigned long size;
	unsigned int shift;
	unsigned char *unpack_data_buf = NULL;
	for(i = 1; i < argc; i++)
	{
		if(!memcmp(argv[i] + strlen(argv[i]) - 5, ".pack", 5) &&
		  !memcmp(argv[i], "filename=", 9))
		{
			strncpy(packfile, argv[i] + strlen("filename="), sizeof(packfile));
			break;
		}
	}
	if(packfile[0] == '\0')
	{
		printf("usage: %s filename=[XXXXX.pack]\n", argv[0]);
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
	printf("will print git packfile head\n\t\t flag: %s version: %d object: %d\n", fileHead, ntohl(*(int *)versionNum), ntohl(objectNum));
	printf("the last 20 bytes which is sha1 checksum:\n\t\t%s\n", sha1_to_hex(memBuf + statBuf.st_size - 20));
	
	
	printf("**************will parse the first data(you must know we can't get all data without index)*************\n");
	packData = (unsigned char *)(memBuf + 12); /*point to the first data*/
	oneByte = *packData++;
	obj_type = (oneByte >> 4) & 7; /*if a byte format is 12345678, the 234 is the type of obj. so ....*/
	size = oneByte & 15;	
	shift = 4;
	while(oneByte & 0x80) /*test the first bit*/
	{
		oneByte = *packData++;
		size += (oneByte & 0x7F) << shift; /*remember the this is a variable-length integer*/
		shift += 7;
	}
	printf("obj size: %ld\n", size);
	unpack_data_buf = (unsigned char *)malloc(size + 1);
	unpack_data_buf[size] = '\0';
	if(unpack_data_buf == NULL)
	{
		printf("malloc error\n");
	}
	else
	{
		if(unpack_data(packData, size, statBuf.st_size - (packData - (unsigned char *)memBuf), unpack_data_buf))
		{
			printf("first data in the pack: \n ***will begin***\n%s\n***end***\ntotal: %d\n", 
					unpack_data_buf, strlen((char *)unpack_data_buf));
		}
		else
		{
			printf("pack file error\n");
		}
	}
	printf("will print the last 8 byte\n");
	for(i = 0; i < 8; i++)
	{
		printf("%#x\t", *(unsigned char *)(unpack_data_buf + size - 8 + i));
	}
	printf("\n");
	switch(obj_type)
	{
		case OBJ_EXT:
			printf("object type is OBJ_EXT\n");
			break;
		case OBJ_COMMIT:
			printf("object type is OBJ_COMMIT\n");
			break;
		case OBJ_TREE:
			printf("object type is OBJ_TREE\n");
			break;
		case OBJ_BLOB:
			printf("object type is OBJ_BLOB\n");
			break;
		case OBJ_TAG:
			printf("object type is OBJ_TAG\n");
			break;
		default:
			printf("something error\n");
			break;
	}
	munmap(memBuf, statBuf.st_size);
	return 0;
}






















