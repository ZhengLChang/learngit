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

struct parse_index
{
	char sha1[21];
	int offset_in_packfile;
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

int main(int argc, char **argv)
{
	int i = 0, j = 0, index_fd = -1, pack_fd = -1;
	unsigned char head[5] = {0};
	int version;
	char indexfile[1024] = {0}, packfile[1024] = {0};
	void *indexBuf = NULL, *packBuf = NULL;
	int *entry_table_p = NULL, entry_table[256];
	unsigned char *cur_mem_location = NULL;
	struct stat stat_index_buf, stat_pack_buf;
	struct parse_index *packfile_data = NULL;
	for(i = 1; i < argc; i++)
	{
		if(!memcmp(argv[i] + strlen(argv[i]) - 4, ".idx", 4) &&
		  !memcmp(argv[i], "filename=", 9))
		{
			strncpy(indexfile, argv[i] + strlen("filename="), sizeof(indexfile));
			break;
		}

		if(!memcmp(argv[i] + strlen(argv[i]) - 5, ".pack", 5) &&
		  !memcmp(argv[i], "filename=", 9))
		{
			strncpy(packfile, argv[i] + strlen("filename="), sizeof(packfile));
			break;
		}
	}
	if(packfile[0] == '\0' && indexfile[0] == '\0')
	{
		printf("usage: %s filename=[XXXXX.idx]\n", argv[0]);
		return -1;
	}
	if((index_fd = open(indexfile, O_RDWR)) < 0)
	{
		printf("open file error: %s\n", strerror(errno));
		return -1;
	}
	if(fstat(index_fd, &stat_index_buf) < 0)
	{
		printf("fstat error: %s\n", strerror(errno));
		return -1;
	}
	indexBuf = mmap(NULL, stat_index_buf.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, index_fd, 0);
	close(index_fd);
	if(MAP_FAILED == indexBuf)
	{
		printf("mmap error: %s\n", strerror(errno));	
		return -1;
	}
	memcpy(head, indexBuf, 4);
	memcpy(&version, indexBuf + 4, 4);
	head[4] = '\0';

	/*will parse fanout table*/
	entry_table_p = (int *)(indexBuf + 8);
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
	}
	printf("packfile is including %d obj\n", ntohl(entry_table_p[255]));
	packfile_data =  (struct parse_index *)malloc(ntohl(entry_table_p[255]) * sizeof(struct parse_index));
	if(packfile_data == NULL)
	{
		printf("malloc error: %s\n", strerror(errno));
		goto ERR;
	}
	/*will parse second layer which is 20-byte object names which aready know in front layer*/
	cur_mem_location = (unsigned char *)(indexBuf + 8 + 256 * 4);
	for(i = 0; i < 256; i++)
	{
		for(j = 0; j < entry_table[i]; j++)
		{
			if(cur_mem_location[0] != i)
			{
				printf("the third layer occur error\n");
				goto ERR;
			}
			if(i > 0)
			{
			}
			else if(i == 0)
			{
				strncpy(packfile_data.sha1, 
			}
			cur_mem_location += 20;
		}
		if(j != entry_table[i])
		{
			printf("%#x checkout error!!!\n", i);
			goto ERR;
		}
	}
ERR:	
	munmap(indexBuf, stat_index_buf.st_size);
	return 0;
}



















