#include <unp.h>

int main(int argc, char **argv)
{
	sem_t *sem = NULL;
	int val;

	if(argc != 2)
	{
		printf("usage: sempost <name>\n");
		return -1;
	}
	if((sem = sem_open(argv[1], 0)) == SEM_FAILED)
	{
		printf("sem_open error: %s\n", strerror(errno));
		return -1;
	}
	if(sem_getvalue(sem, &val) < 0)
	{
		printf("sem_getvalue error: %s\n", strerror(errno));
		return -1;
	}
	else
	{
		printf("before post, value =  %d\n", val);
	}
	if(sem_post(sem) < 0)
	{
		printf("sem_wait error: %s\n", strerror(errno));
		return -1;
	}
	if(sem_getvalue(sem, &val) < 0)
	{
		printf("sem_getvalue error: %s\n", strerror(errno));
		return -1;
	}
	else
	{
		printf("after post, value =  %d\n", val);
	}
	return 0;
}
