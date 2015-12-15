#include <unp.h>

#define NBUFF 10

int nitems;
int stoped = 0;

struct
{
	int buff[NBUFF];
	sem_t mutex, nempty, nstored;
}shared;

void *produce(void *), *consume(void *);
void intProc(int signo);

int main(int argc, char **argv)
{
	pthread_t tid_produce, tid_consume;

	if(argc != 2)
	{
		printf("usage: prodcons1 <#items>\n");
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
	int i;
	printf("thead produce is created\n");
	while(!stoped)
	for(i = 0; i < nitems; i++)
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
		shared.buff[i % NBUFF] = i;
		printf("produce create %d\n", i);
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
	printf("thead produce is over\n");
	return NULL;
}

void *consume(void *arg)
{
	int i;
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
		if(shared.buff[i % NBUFF] != i)
		{
			printf("buff[%d] = %d\n", i, shared.buff[i % NBUFF]);
		}
		else
		{
			printf("consume destory %d\n", i);
		}
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
	printf("thead consume is over\n");
	return NULL;
}

void intProc(int signo)
{
	stoped = 1;
	return ;
}







