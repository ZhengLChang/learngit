#include <unp.h>

int main(int argc, char **argv)
{
	int fd1, fd2, *ptr1, *ptr2;
	pid_t childpid;
	struct stat stat;

	if(argc != 2)
	{
		printf("usage: %s <name>\n", argv[0]);
		return -1;
	}
	shm_unlink(argv[1]);
	if((fd1 = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL,
				0666)) < 0)
	{
		printf("shm_open error: %s\n", strerror(errno));
		return -1;
	}
	ftruncate(fd1, sizeof(int));
	if((fd2 = open("./motd", O_RDONLY)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	if(fstat(fd2, &stat) < 0)
	{
		printf("stat error: %s\n", strerror(errno));
		return -1;
	}
	if((childpid = fork()) == 0)
	{
		if((ptr1 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0)) == MAP_FAILED)
		{
			printf("%d, mmap error: %s\n", __LINE__, strerror(errno));
			return -1;
		}
		if((ptr2 = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd2, 0)) == MAP_FAILED)
		{
			printf("%d,mmap error: %s\n", __LINE__, strerror(errno));
			return -1;
		}
		printf("child: shm ptr = %p, motd ptr = %p\n", ptr1, ptr2);
		sleep(5);
		printf("shared memory integer = %d\n", *ptr1);
		exit(0);
	}
		if((ptr1 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0)) == MAP_FAILED)
		{
			printf("%d, mmap error: %s\n", __LINE__, strerror(errno));
			return -1;
		}
		if((ptr2 = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd2, 0)) == MAP_FAILED)
		{
			printf("%d, mmap error: %s\n", __LINE__, strerror(errno));
			return -1;
		}
		printf("parent: shm ptr = %p, motd ptr = %p\n", ptr1, ptr2);
		*ptr1 = 777;
		waitpid(childpid, NULL, 0);
		exit(0);
	
}














