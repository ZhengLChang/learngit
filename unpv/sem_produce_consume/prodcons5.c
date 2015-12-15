#include <unp.h>


#define NBUFF 10
#define MAXNTHREADS 100

int nproducers;
int stoped = 0;

struct
{
	struct
	{
		char data[BUFFSIZE];
		ssize_t n;
	}buff[NBUFF];
	int read_place;
	int write_place;
	sem_t mutex, nempty, nstored;
}shared;

int fd;

void *produce(void *), *consume(void *);
void intProc(int signo);
int mim(int first, int second);
int getArrPre(int now, int arrCount);

int main(int argc, char **argv)
{
	pthread_t tid_produce, tid_consume;
	bzero(&shared, sizeof(shared));

	if(argc != 2)
	{
		printf("usage: mycat2 <pathname>\n");
		return -1;
	}
	if((fd = open(argv[1], O_RDONLY)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	if(SIG_ERR == signal(SIGINT, intProc))
	{
		printf("install SIGINT error\n");
		return -1;
	}
	printf("nproducers = %d\n", nproducers);
	if((sem_init(&shared.mutex, 0, 1)) == -1)
	{
		printf("sem_open error: %s\n", strerror(errno));
		goto NORMALRET;	
	}
	if((sem_init(&shared.nempty, 0, NBUFF)) == -1)
	{
		printf("sem_open error: %s\n", strerror(errno));
		goto SECERR;
	}
	if((sem_init(&shared.nstored, 0, 0)) == -1)
	{
		printf("sem_open error: %s\n", strerror(errno));
		goto LASTERR;
	}
	if(pthread_create(&tid_produce, NULL, produce, NULL) != 0)
	{
		printf("pthread create error\n");
		return -1;
	}
	if(pthread_create(&tid_consume, NULL, consume, NULL) != 0)
	{
		printf("pthread create error\n");
		return -1;
	}
	pthread_join(tid_produce, NULL);
	pthread_join(tid_consume, NULL);

NORMALRET:	
	if(sem_destroy(&shared.mutex) < 0)
	{
		printf("sem_destroy mutex error: %s\n", strerror(errno));
		return -1;
	}
LASTERR:
	if(sem_destroy(&shared.nempty) < 0)
	{
		printf("sem_destroy nempty error: %s\n", strerror(errno));
		return -1;
	}
SECERR:
	if(sem_destroy(&shared.nstored) < 0)
	{
		printf("sem_destroy stored error: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

void *produce(void *arg)
{
	int i, num_stored = 0, num_empty = 0;
	printf("thead produce is created\n");
	for(i = 0; !stoped; )
	{
		if(sem_wait(&shared.nempty) < 0)
		{
			printf("sem_wait error: %s\n", strerror(errno));
			return NULL;
		}
		if(sem_wait(&shared.mutex) < 0)
		{
			printf("sem_wait error: %s\n", strerror(errno));
			return NULL;
		}
		printf("produce get lock, write_place = %d\n", shared.write_place);
		if(((shared.write_place + 1) % NBUFF) != shared.read_place)
		{
			shared.buff[shared.write_place].n = read(fd, shared.buff[shared.write_place].data, BUFFSIZE);
			sem_getvalue(&shared.nstored, &num_stored);
			sem_getvalue(&shared.nempty, &num_empty);
		}
		if(sem_post(&shared.mutex) < 0)
		{
			printf("sem_post error: %s\n", strerror(errno));
			return NULL;
		}
		if(sem_post(&shared.nstored) < 0)
		{
			printf("sem_post error: %s\n", strerror(errno));
			return NULL;
		}
		if(shared.buff[shared.write_place].n <= 0)
		{
			sleep(1);
			stoped = 1;
			shared.write_place = ((shared.write_place + 1) % NBUFF);
			break;
		}
		else
		{
			shared.write_place = ((shared.write_place + 1) % NBUFF);
		}
	}
	num_stored++;
	printf("thead produce is over, nstored = %d, nempty = %d\n", 
			num_stored, num_empty);
	return NULL;
}

void *consume(void *arg)
{
	int i;
	int num_stored = 0, num_empty = 0;
	printf("thead consume is created\n");
	for(i = 0; (i == 0) || (!stoped) || (shared.buff[getArrPre(shared.read_place, NBUFF)].n != 0);)
	{
		i = 1;
		if(sem_wait(&shared.nstored) < 0)
		{
			printf("sem_wait error: %s\n", strerror(errno));
			return NULL;
		}
		if(sem_wait(&shared.mutex) < 0)
		{
			printf("sem_wait error: %s\n", strerror(errno));
			return NULL;
		}
		printf("consume get lock, read_place is %d\n", shared.read_place);

		if(shared.read_place != shared.write_place && shared.buff[shared.read_place].n != 0)
		{
			write(STDOUT_FILENO, shared.buff[shared.read_place].data, shared.buff[shared.read_place].n);
			shared.read_place = ((shared.read_place + 1) % NBUFF);
		}
		else if(shared.read_place == shared.write_place)
		{
			printf("read_place equal write_place\n");
		}
		else if(shared.buff[shared.read_place].n == 0)
		{
			printf("n = 0\n");	
		}
		sem_getvalue(&shared.nempty, &num_empty);
		if(sem_post(&shared.mutex) < 0)
		{
			printf("sem_post error: %s\n", strerror(errno));
			return NULL;
		}
		if(sem_post(&shared.nempty) < 0)
		{
			printf("sem_post error: %s\n", strerror(errno));
			return NULL;
		}
		sem_getvalue(&shared.nstored, &num_stored);
	}
	num_empty++;
	printf("thead consume is over, nstored = %d, nempty = %d\n", 
			num_stored, num_empty);
	return NULL;
}

void intProc(int signo)
{
	stoped = 1;
	return ;
}


int mim(int first, int second)
{
	return (first < second ? first : second);
}

int getArrPre(int now, int arrCount)
{
	if(now < 0 || arrCount <= 0)
		return -1;
	if(now == 0)
	{
		return arrCount - 1;
	}
	else
	{
		return now - 1;
	}
}



