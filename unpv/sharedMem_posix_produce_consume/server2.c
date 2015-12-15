#include "cliserv2.h"

int main(int argc, char **argv)
{
	int fd, index, lastnoverflow, temp;
	long offset;
	struct shmstruct *ptr;

	if(argc != 2)
	{
		printf("usage: server2 <name>\n");
		return -1;
	}
	shm_unlink(argv[1]);
	if((fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL,
					0666)) < 0)
	{
		printf("shm_open error: %s\n", strerror(errno));
		return -1;
	}
	ftruncate(fd, sizeof(struct shmstruct));
	if((ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE,
					MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		printf("shm_open error: %s\n", strerror(errno));
		return -1;
	}
	close(fd);
	for(index = 0; index < NMESG; index++)
	{
		ptr->msgoff[index] = index * MESGSIZE;
	}
	if(sem_init(&ptr->mutex, 1, 1) < 0 || 
	   sem_init(&ptr->nempty, 1, NMESG) < 0 ||
	   sem_init(&ptr->nstored, 1, 0) < 0 ||
	   sem_init(&ptr->noverflowmutex, 1, 1) < 0)
	{
		printf("sem_init error: %s\n", strerror(errno));
		return -1;
	}
	index = 0;
	lastnoverflow = 0;
	for(; ;)
	{
		sem_wait(&ptr->nstored);
		sem_wait(&ptr->mutex);
		offset = ptr->msgoff[index];
		printf("index = %d: %s\n", index, &ptr->msgdata[offset]);
		if(++index >= NMESG)
		{
			index = 0;
		}
		sem_post(&ptr->mutex);
		sem_post(&ptr->nempty);
		sem_wait(&ptr->noverflowmutex);
		temp = ptr->noverflow;
		sem_post(&ptr->noverflowmutex);
		if(temp != lastnoverflow)
		{
			printf("noverflow = %d\n", temp);
			lastnoverflow = temp;
		}
	}
	return 0;
}
























