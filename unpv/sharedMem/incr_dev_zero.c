#include <unp.h>
struct shared
{
	sem_t mutex;
	int count;
}shared;

int main(int argc, char **argv)
{
	int fd, i, nloop;
	struct shared *ptr;
	if(argc != 2)
	{
		printf("usage: incr_dev_zero <#loops>\n");
		return -1;
	}
	nloop = atoi(argv[1]);
	if((fd = open("/dev/zero", O_RDWR | O_CREAT, 0666)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	if(write(fd, &shared, sizeof(struct shared)) < 0)
	{
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	if((ptr = mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE,
					MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		printf("mmap error: %s\n", strerror(errno));
		return -1;
	}
	close(fd);
	if(sem_init(&ptr->mutex, 1, 1) < 0)
	{
		printf("sem_init error: %s\n", strerror(errno));
		return -1;
	}
	setbuf(stdout, NULL);
	if(fork() == 0)
	{
		for(i = 0; i < nloop; i++)
		{
			sem_wait(&ptr->mutex);
			printf("child: %d\n", ptr->count++);
			sem_post(&ptr->mutex);
		}
		exit(0);
	}
	for(i = 0; i < nloop; i++)
	{
		sem_wait(&ptr->mutex);
		printf("parent: %d\n", ptr->count++);
		sem_post(&ptr->mutex);
	}
	exit(0);
}












