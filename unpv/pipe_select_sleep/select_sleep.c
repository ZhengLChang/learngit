#include <unp.h>

int pipefd[2];
void *mythread(void *);


int main(void)
{
	char str[1024];
	pthread_t thread;
	int n = 0;
	if(pipe(pipefd) < 0)
	{
		printf("pipe error: %s\n", strerror(errno));
		return -1;
	}
	if(pthread_create(&thread, NULL, mythread, NULL) != 0)
	{
		printf("create pthread error\n");
		exit(1);
	}	
	while((fgets(str, sizeof(str) - 1, stdin)) != NULL)
	{
		n = strlen(str) + 1;
		if(write(pipefd[1], str, n) != n)
		{
			printf("write error: %s\n", strerror(errno));
			exit(1);
		}
	}
	close(pipefd[1]);
	pthread_join(thread, NULL);
	return 0;
}

void *mythread(void *arg)
{
	fd_set rdfs;
	struct timeval timeout;
	int select_ret;
	char buf[1024];
	int n;
	for(;;)
	{
		FD_ZERO(&rdfs);
		FD_SET(pipefd[0], &rdfs);
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		if((select_ret = select(pipefd[1] + 1, &rdfs, NULL, NULL, &timeout)) < 0)
		{
			printf("select error: %s\n", strerror(errno));
			exit(1);
		}
		else if(select_ret == 0)
		{
			printf("timeout\n");
			continue;
		}
		else if(select_ret == 1)
		{
			if(FD_ISSET(pipefd[0], &rdfs))
			{
				if((n = read(pipefd[0], buf, sizeof(buf) - 1)) > 0)
				{
					printf("thread get data: %s", buf);
					continue;
				}
				else if(n == 0)
				{
					close(pipefd[0]);
					printf("read EOF, thread will be over\n");
					return NULL;
				}
				else
				{
					printf("read error\n");
				}
			}
		}
		else
		{
			printf("select unknow error\n");
		}
	}
}








