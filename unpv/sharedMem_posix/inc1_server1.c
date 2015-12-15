#include <unp.h>

struct shmstruct
{
	int count;
};

sem_t *mutex;

int main(int argc, char **argv)
{
	int fd;
	struct shmstruct *ptr;
	if(argc != 3)
	{
		printf("usage: server1 <shmname> <semname>\n");
		return -1;
	}
	shm_unlink(argv[1]);
	if((fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL,
				0666)) < 0)
	{
		printf("shm_open error: %s\n", strerror(errno));
		return -1;
	}
	if(ftruncate(fd, sizeof(struct shmstruct)) < 0)
	{
		printf("ftruncate error: %s\n", strerror(errno));
		return -1;
	}
	if((ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE,
					MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		printf("mmap error: %s\n", strerror(errno));
		return -1;
	}
	close(fd);
	sem_unlink(argv[2]);
	if((mutex = sem_open(argv[2], O_CREAT|O_EXCL, 0666, 1)) < SEM_FAILED)
	{
		printf("sem_open error: %s\n", strerror(errno));
		return -1;
	}
	sem_close(mutex);
	exit(1);
}
















