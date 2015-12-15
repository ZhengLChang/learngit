#include <unp.h>

int main(int argc, char **argv)
{
	int i, fd;
	struct stat stat;
	unsigned char c, *ptr;

	if(argc != 2)
	{
		printf("usage: shmread <name>");
		return -1;
	}
	if((fd  = shm_open(argv[1], O_RDONLY, 0)) < 0)
	{
		printf("shm_open error: %s\n", strerror(errno));
		return -1;
	}
	if(fstat(fd, &stat) < 0)
	{
		printf("fstat error: %s\n", strerror(errno));
		return -1;
	}
	if((ptr = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		printf("mmap error: %s\n", strerror(errno));
		return -1;
	}
	close(fd);
	for(i = 0; i < stat.st_size; i++)
	{
		if((c = *ptr++) != (i % 256))
		{
		}
		printf("ptr[%d] = %d\n", i, c);
	}
	return 0;
}









