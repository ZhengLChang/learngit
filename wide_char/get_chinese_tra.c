#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wchar.h>

const char input[] = "./chinese_tra1.txt";
const char output[] = "./output.txt";
int main(void)
{
	int input_fd = open(input, O_RDONLY), size = sizeof("盤");
	FILE *output_fp = fopen(output, "w");
	struct stat stat;
	void *buf, *p;
	if(input_fd < 0 || output_fp == NULL)
	{
		printf("read error\n");
		return -1;
	}
	if(fstat(input_fd, &stat) < 0)
	{
		printf("fstat error\n");
		return -1;
	}
	buf = mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, input_fd, 0);	
	if(buf == NULL)
	{
		printf("mmap error\n");
		return -1;
	}
#if 0
	p = buf;
	printf("%#x\t", *(unsigned char *)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("%#x\t", *(unsigned char*)p);
	p += sizeof(unsigned char);
	printf("\n");
	return 0;
#endif
	write(fileno(output_fp), buf,  2);	
	for(p = buf; p < buf + stat.st_size; )
	{
		if(*(char *)p != '[')
		{
			p += 2;
			continue;
		}
		//write(fileno(output_fp), p,  stat.st_size - (p - buf));	
		//return 0;
		p += 2;

		while(*(char *)p != ']' && *(char *)p != ' ' && *(char *)p != '[' && *(char *)p != '\n')
		{
			write(fileno(output_fp), p,  2);	
			p += 2;
		}
		p += 2;
	}
	return 0;
}














