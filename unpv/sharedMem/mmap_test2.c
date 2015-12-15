#include <unp.h>

int main(int argc, char **argv)
{
	int fd, i;
	char *ptr = NULL;
	size_t filesize, mmapsize, pagesize;

	if(argc != 4)
	{
		printf("usage: test1 <pathname> <filesize> <mmapsize>\n");
		return -1;
	}
	filesize = atoi(argv[2]);
	mmapsize = atoi(argv[3]);

	if((fd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	if(lseek(fd, filesize - 1, SEEK_SET) < 0)
	{
		printf("lseek error: %s\n", strerror(errno));
		return -1;
	}
	if(write(fd, "", 1) < 0)
	{
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	if((ptr = mmap(NULL, mmapsize, PROT_READ|PROT_WRITE, MAP_SHARED,
					fd, 0)) == MAP_FAILED)
	{
		printf("map error: %s\n", strerror(errno));
		return -1;
	}
	close(fd);
	if((pagesize = sysconf(_SC_PAGESIZE)) < 0)
	{
		printf("pagesize error: %s\n", strerror(errno));
		return -1;
	}
	printf("PAGESIZE = %ld\n", (long)pagesize);
	for(i = 0; i < max(filesize, mmapsize); i += pagesize)
	{
		printf("ptr[%d] = %d\n", i, ptr[i]);
		ptr[i] = 1;
		printf("ptr[%d] = %d\n", i + pagesize - 1, ptr[i + pagesize - 1]);
		ptr[i + pagesize - 1] = 1;
	}
	printf("ptr[%d] = %d\n", i, ptr[i]);
#if 0
	printf("ptr[%d] = %d\n", i + pagesize - 1, ptr[i + pagesize - 1]);
	i += pagesize;
	printf("ptr[%d] = %d\n", i, ptr[i]);
	printf("ptr[%d] = %d\n", i + pagesize - 1, ptr[i + pagesize - 1]);
#endif
	return 0;
}












