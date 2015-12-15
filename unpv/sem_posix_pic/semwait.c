#include <unp.h>

int main(int argc, char **argv)
{
	sem_t *sem;
	int val;

	if(argc != 2)
	{
		printf("usage: semwait <name>\n");
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
		printf("before wait, value =  %d\n", val);
	}
	if(sem_wait(sem) < 0)
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
		printf("after wait, [%ld] get sem value =  %d\n", (long)getpid(), val);
	}
	pause();
	return 0;
}




