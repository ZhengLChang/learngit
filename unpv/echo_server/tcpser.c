#include <unp.h>
void sig_chld(int signo);

int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	char dst[225];

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error: %s\n", strerror(errno));
		return -1;
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr  = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVPORT);
	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("bind error: %s\n", strerror(errno));
		return -1;
	}
	if(listen(listenfd, 10) < 0)
	{
		printf("listen error: %s\n", strerror(errno));
		return -1;
	}
	if(SIG_ERR == signal(SIGCHLD, sig_chld))
	{
		printf("Signal error\n");
		return -1;
	}
	for(;;)
	{
		clilen = sizeof(cliaddr);
		if((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0)
		{
			if(errno == EINTR)
				continue;
			else
			{
				printf("accept error: %s\n", strerror(errno));
				return -1;
			}
		}
		printf("%s %d is connected\n", inet_ntop(AF_INET, &cliaddr.sin_addr,
					dst, sizeof(dst)), ntohs(cliaddr.sin_port));
		if((childpid = fork()) == 0)
		{
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
#if 0
		{
			if(system("sleep 1") < 0)
			{
				if(errno == EINTR)
					printf("read is interrupted\n");
				else
				{
					printf("read error: %s\n", strerror(errno));
				}
			}
		}
#endif
	}
	return 0;
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}





