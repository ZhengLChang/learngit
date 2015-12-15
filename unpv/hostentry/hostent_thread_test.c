#include <unp.h>

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

void *mythread(void *);
int main(int argc, char **argv)
{
	pthread_t thread;
	char *ptr = NULL;
	while(--argc > 0)
	{
		ptr = *++argv;
		if(0 != pthread_create(&thread, NULL, mythread, (void *)ptr))		
		{
			printf("pthread_create error\n");
			return -1;
		}
		pthread_detach(thread);
	}
	sleep(12);
	exit(0);
}
void *mythread(void *hostname)
{	
	char *ptr = (char *)hostname;
	char **pptr;
	struct hostent *hptr;
	char str[INET_ADDRSTRLEN];
	printf("thread is create: %ld\n", pthread_self());
	if(0 == pthread_mutex_lock(&mymutex))	
	{
		if((hptr = gethostbyname(ptr)) == NULL)
		{
			err_msg("gethostbyname error for host: %s: %s", 
					ptr, hstrerror(h_errno));
			return NULL;
		}
		sleep(5);
		printf("official hostname: %s\n", hptr->h_name);

		for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
			printf("\talias: %s\n", *pptr);
		switch(hptr->h_addrtype)
		{
			case AF_INET:
				pptr = hptr->h_addr_list;
				for(; *pptr != NULL; pptr++)
					printf("\t%d address: %s\n", 
							((struct in_addr *)(*pptr))->s_addr,
							inet_ntop(AF_INET, *pptr, str, INET_ADDRSTRLEN));
				break;
			default:
				err_ret("unknown address type");
				break;
		}
	}
	else
	{
		printf("get lock error\n");
		return NULL;
	}
	if(0 != pthread_mutex_unlock(&mymutex))
	{
		printf("unlock error\n");
		return NULL;
	}
	else
	{
		printf("thread lose lock: %ld\n", pthread_self());
	}
	return NULL;
}
