#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *mythread(void *mypara)
{
	int i = 0;
	while(i < 10)
	{
		printf("i = %d\n", i);
	}
}

int main(void)
{
	pthread_t thread;
	pthread_create(&thread, NULL, mythread, NULL);
	pthread_join(thread, NULL);
	return 0;
}
