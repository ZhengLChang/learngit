#include <unp.h>

int main(int argc, char **argv)
{
	int fd, i;
	char *ptr;
	size_t filesize, mmapsize, pagesize;
	if(argc != 4)
	{
		printf("usage: test1 <pathname> <filesize> <mmapsize>\n");
		return -1;
	}
	filesize = atoi(argv[2]);
	mmapsize = atoi(argv[3]);

	if((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	lseek(fd, filesize - 1, SEEK_SET);
	write(fd, "", 1);
	if((ptr = mmap(NULL, mmapsize, PROT_READ|PROT_WRITE, MAP_SHARED,
					fd, 0)) == MAP_FAILED)
	{
		printf("mmap error: %s\n", strerror(errno));
		return -1;
	}
	close(fd);
	pagesize = sysconf(_SC_PAGESIZE);
	printf("PAGESIZE = %ld\n", (long)pagesize);
	for(i = 0; i < max(filesize, mmapsize); i += pagesize)
	{
		printf("ptr[%d] = %d\n", i, ptr[i]);
		ptr[i] = 1;
		printf("ptr[%d] = %d\n", i + pagesize - 1, ptr[i + pagesize - 1]);
		ptr[i + pagesize - 1] = 1;
	}
	printf("ptr[%d] = %d\n", i, ptr[i]);
	printf("process will be over\n");
	exit(0);
}







