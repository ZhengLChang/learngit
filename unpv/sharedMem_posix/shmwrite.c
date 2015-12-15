#include <unp.h>

int main(int argc, char *argv[])
{
	int i, fd;
	struct stat stat;
	unsigned char *ptr;
	if(argc != 2)
	{
		printf("usage: shmwrite <name>\n");
		return -1;
	}
	if((fd = shm_open(argv[1], O_RDWR, 0)) < 0)
	{
		printf("shm_open error: %s\n", strerror(errno));
		return -1;
	}
	if(fstat(fd, &stat) < 0)
	{
		printf("fstat error: %s\n", strerror(errno));
		return -1;
	}
	if((ptr = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE, 
				MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		printf("mmap error: %s\n", strerror(errno));
		return -1;
	}
	close(fd);
	for(i = 0; i < stat.st_size; i++)
	{
		*ptr++ = i % 256;
	}
	return 0;
}













