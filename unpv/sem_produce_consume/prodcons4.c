#include <unp.h>

#define NBUFF 10
#define MAXNTHREADS 100

int nitems;
int nproducers, nconsumers;
int stoped = 0;

struct
{
	int buff[NBUFF];
	int nput;
	int nputval;
	int nget;
	int ngetval;
	sem_t mutex, nempty, nstored;
}shared;

void *produce(void *), *consume(void *);
void intProc(int signo);
int mim(int first, int second);

int main(int argc, char **argv)
{
	int i, prodcount[MAXNTHREADS], conscount[MAXNTHREADS];
	pthread_t tid_produce[MAXNTHREADS], tid_consume[MAXNTHREADS];

	if(argc != 4)
	{
		printf("usage: prodcons3 <#items> <#producers> <#consumes>\n");
		return -1;
	}
	if(SIG_ERR == signal(SIGINT, intProc))
	{
		printf("install SIGINT error\n");
		return -1;
	}
	nitems = atoi(argv[1]);
	if(nitems <= 0)
		nitems = NBUFF;
	nproducers = mim(atoi(argv[2]), MAXNTHREADS);
	nconsumers = mim(atoi(argv[3]), MAXNTHREADS);
	printf("nproducers = %d\n", nproducers);
	printf("nconsumers = %d\n", nconsumers);
	if((sem_init(&shared.mutex, 0, 1)) == -1)
	{
		printf("sem_open error: %s\n", strerror(errno));
		goto NORMALRET;	
	}
	if((sem_init(&shared.nempty, 0, nitems)) == -1)
	{
		printf("sem_open error: %s\n", strerror(errno));
		goto SECERR;
	}
	if((sem_init(&shared.nstored, 0, 0)) == -1)
	{
		printf("sem_open error: %s\n", strerror(errno));
		goto LASTERR;
	}
	for(i = 0; i < nproducers; i++)
	{
		prodcount[i] = 0;
		if(pthread_create(&tid_produce[i], NULL, produce, &prodcount[i]) != 0)
		{
			printf("pthread create error\n");
			return -1;
		}
	}
	for(i = 0; i < nconsumers; i++)
	{
		conscount[i] = 0;
		if(pthread_create(&tid_consume[i], NULL, consume, &conscount[i]) != 0)
		{
			printf("pthread create error\n");
			return -1;
		}
	}
	for(i = 0; i < nproducers; i++)
	{
		pthread_join(tid_produce[i], NULL);
		printf("procount[%d] = %d\n", i, procount[i]);
	}
	for(i = 0; i < nconsumers; i++)
	{
		pthread_join(tid_consume[i], NULL);
		printf("consumer count[%d] = %d\n", i, conscount[i]);
	}

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
	int num_stored = 0, num_empty = 0;
	printf("thead produce is created\n");
	while(!stoped)
	{
		if(sem_wait(&shared.nempty) < 0)
		{
			printf("sem_wait error: %s\n", strerror(errno));
			return NULL;
		}
		if(sem_wait(&shared.mutex))
		{
			printf("sem_wait error: %s\n", strerror(errno));
			return NULL;
		}
		shared.buff[shared.nput % nitems] = shared.nputval;
		printf("produce create %d\n", shared.nputval);
		shared.nput = (shared.nput + 1) % nitems;
		shared.nputval++;
		sem_getvalue(&shared.nstored, &num_stored);
		sem_getvalue(&shared.nempty, &num_empty);
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
		sleep(1);
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
	while(!stoped)
	for(i = 0; i < nitems; i++)
	{
		if(sem_wait(&shared.nstored) < 0)
		{
			printf("sem_wait error: %s\n", strerror(errno));
			return NULL;
		}
		if(sem_wait(&shared.mutex))
		{
			printf("sem_wait error: %s\n", strerror(errno));
			return NULL;
		}
		printf("consume destory %d\n", i);
		sem_getvalue(&shared.nstored, &num_stored);
		sem_getvalue(&shared.nempty, &num_empty);
		if(sem_post(&shared.mutex))
		{
			printf("sem_post error: %s\n", strerror(errno));
			return NULL;
		}
		if(sem_post(&shared.nempty))
		{
			printf("sem_post error: %s\n", strerror(errno));
			return NULL;
		}
		sleep(3);
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





