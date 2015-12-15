#include <unp.h>

#define SEM_NAME "./mysem"

int count = 0;

int main(int argc, char **argv)
{
	int i, nloop;
	sem_t *mutex;
	if(argc != 2)
	{
		printf("usage: incr1 <#loops>\n");
		return -1;
	}
	nloop = atoi(argv[1]);
	if((mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL,
				0666, 1)) == SEM_FAILED)
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
			printf("child: %d\n", count++);
			sem_post(mutex);
		}
		exit(0);
	}
	for(i = 0; i < nloop; i++)
	{
		sem_wait(mutex);
		printf("parent: %d\n", count++);
		sem_post(mutex);
	}
	exit(0);
}














