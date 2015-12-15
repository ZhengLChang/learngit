#include <unp.h>

#define SEM_NAME "mysem"

int main(int argc, char **argv)
{
	int i, nloop, fd, zero = 0;
	int *ptr;
	sem_t *mutex;
	if(argc != 3)
	{
		printf("usage: incr2 <pathname> <#loops>\n");
		return -1;
	}
	nloop = atoi(argv[2]);
	if((fd = open(argv[1], O_RDWR | O_CREAT, 0666)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	if(write(fd, &zero, sizeof(int)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	if((ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED,
					fd, 0)) == MAP_FAILED)
	{
		printf("mmap error: %s\n", strerror(errno));
		return -1;
	}
	if((mutex = sem_open(SEM_NAME, O_CREAT|O_EXCL, 0666, 1)) == SEM_FAILED)
	{
		printf("sem_open error: %s\n", strerror(errno));
		return -1;
	}

	sem_unlink(SEM_NAME);
	setbuf(stdout, NULL);
	if(fork() == 0)
	{
		for(i = 0; i < nloop; i++)
		{
			sem_wait(mutex);
			printf("child: %d\n", (*ptr)++);
			sem_post(mutex);
		}
		exit(0);
	}
	for(i = 0; i < nloop; i++)
	{
		sem_wait(mutex);
		printf("parent: %d\n", (*ptr)++);
		sem_post(mutex);
	}
	exit(0);
}














