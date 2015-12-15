#include <unp.h>

int main(int argc, char **argv)
{
	int c, fd, flags;
	char *ptr;
	off_t length;

	flags = O_RDWR | O_CREAT;
	while((c = getopt(argc, argv, "e")) != -1)
	{
		switch(c)
		{
			case 'e':
				flags |= O_EXCL;
				break;
		}
	}
	if(optind != argc - 2)
	{
		printf("usage: shmcreate [-e] <name> <length>");
		return -1;
	}
	length = atol(argv[optind + 1]);
	if((fd = shm_open(argv[optind], flags, 0666)) < 0)
	{
		printf("shm_open error: %s\n", strerror(errno));
		return -1;
	}
	if(ftruncate(fd, length) < 0)
	{
		printf("ftruncate error: %s\n", strerror(errno));
		return -1;
	}
	if((ptr = mmap(NULL, length, PROT_READ|PROT_WRITE,
				MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		printf("mmap error: %s\n", strerror(errno));
		return -1;
	}
	return  0;
}




 
 








