#include <unp.h>

struct shmstruct
{
	int count;
};

sem_t *mutex;

int main(int argc, char **argv)
{
	int fd, i, nloop;
	pid_t pid;
	struct shmstruct *ptr;
	if(argc != 4)
	{
		printf("usage: client1 <shmname> <shmname> <#loops>\n");
		return -1;
	}
	nloop = atoi(argv[3]);
	if((fd = shm_open(argv[1], O_RDWR, 0)) < 0)
	{
		printf("shm_open error: %s\n", strerror(errno));
		return -1;
	}
	if((ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE,
					MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		printf("map error: %s\n", strerror(errno));
		return -1;
	}
	close(fd);
	if((mutex = sem_open(argv[2], 0)) == SEM_FAILED)
	{
		printf("sem_open error: %s\n", strerror(errno));
		return -1;
	}
	pid = getpid();
	for(i = 0; i < nloop; i++)
	{
		sem_wait(mutex);
		printf("pid %ld: %d\n", (long)pid, ptr->count++);
		sem_post(mutex);
	}
	return 0;
}




















