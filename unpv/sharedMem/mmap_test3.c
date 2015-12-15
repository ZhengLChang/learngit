#include <unp.h>

int main(int argc, char **argv)
{
	int fd, i;
	char *ptr = NULL;
	size_t mmapsize, pagesize;

	if(argc != 3)
	{
		printf("usage: test1 <pathname> <mmapsize>\n");
		return -1;
	}
	mmapsize = atoi(argv[2]);

	if((fd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	if((ptr = mmap(NULL, mmapsize, PROT_READ|PROT_WRITE, MAP_SHARED,
					fd, 0)) == MAP_FAILED)
	{
		printf("map error: %s\n", strerror(errno));
		return -1;
	}
	if((pagesize = sysconf(_SC_PAGESIZE)) < 0)
	{
		printf("pagesize error: %s\n", strerror(errno));
		return -1;
	}
	printf("PAGESIZE = %ld\n", (long)pagesize);
	for(i = 4096; i < mmapsize; i += pagesize)
	{
		printf("setting the size to %d\n", i);
		if(ftruncate(fd, i) < 0)
		{
			printf("ftruncate error: %s\n", strerror(errno));
			return -1;
		}
		printf("ptr[%d] = %d\n", i - 1, ptr[i - 1]);
	}
	return 0;
}












