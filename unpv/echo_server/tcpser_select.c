#include <unp.h>
void sig_chld(int signo);

int main(int argc, char **argv)
{
	int listenfd, connfd;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	char dst[225];
	fd_set rset, allset;
	int maxfd, maxi, client[FD_SETSIZE], nready, i, n;
	char buf[MAXLINE];

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
	
	maxfd = listenfd;
	maxi = -1;
	for(i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	for(;;)
	{
		rset = allset;
		if((nready = select(maxfd + 1, &rset, NULL, NULL, NULL)) <= 0)
		{
			printf("select error: %s\n", strerror(errno));
			return -1;
		}
		if(FD_ISSET(listenfd, &rset))
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
			for(i = 0; i < FD_SETSIZE; i++)
			{
				if(client[i] < 0)
				{
					client[i] = connfd;
					break;
				}
			}
			if(i == FD_SETSIZE)
				err_quit("too many clients");
			FD_SET(connfd, &allset);
			if(connfd > maxfd)
				maxfd = connfd;
			if(i > maxi)
				maxi = i;
			if(--nready <= 0)
				continue;
		}
		for(i = 0; i <= maxi; i++)
		{
			if(client[i] < 0)
				continue;
			if(FD_ISSET(client[i], &rset))
			{
				if((n = read(client[i], buf, MAXLINE)) == 0)
				{
					close(client[i]);
					FD_CLR(client[i], &allset);
					client[i] = -1;
				}
				else
				{
					Writen(client[i], buf, n);
				}
				if(--nready <= 0)
					break;
			}
		}
	}
#if 0
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
	}
#endif
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





