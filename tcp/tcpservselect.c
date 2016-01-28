#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>

#define MAXLINE 128
#define SERV_PROT 8898 
//static void str_echo(int sockfd);

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		printf("child %d terminated\n", pid);
	}
	return;
}

int main(int argc, char **argv)
{
	int i, maxi, maxfd;
	int listenfd, connfd, sockfd;
	socklen_t clilen;
	fd_set rset, allset;
	int nready, client[FD_SETSIZE];
	char buf[MAXLINE];
	struct sockaddr_in cliaddr, servaddr;
	struct sigaction act;
	int n;

	act.sa_handler = sig_chld;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_flags |= SA_RESTART;
	if(sigaction(SIGCHLD, &act, NULL) < 0)
	{
		printf("sigaction error: %s\n", strerror(errno));
		return -1;
	}


	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		printf("socket error: %s\n", strerror(errno));
		return -1;
	}
	memset(&servaddr, 0, sizeof(servaddr));	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PROT);
	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("bind error: %s\n", strerror(errno));
		return -1;
	}
	if(listen(listenfd, 5) < 0)
	{
		printf("listen error: %s\n", strerror(errno));
		return -1;
	}
	maxfd = listenfd;
	maxi = -1;
	for(i = 0; i < FD_SETSIZE; i++)
	{
		client[i] = -1;
	}
	for(;;)
	{
		FD_ZERO(&allset);
		FD_SET(listenfd, &allset);
		rset = allset;
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(listenfd, &rset))
		{
			clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd, (struct sockaddr *)&cliaddr,
						&clilen)) < 0)
			{
				if(errno == EINTR)
				{
					continue;
				}
				else
				{
					printf("listen error: %s\n", strerror(errno));
					return -1;
				}
			}
			for(i = 0; i < FD_SETSIZE; i++)
			{
				if(client[i] < 0)
				{
					client[i] = connfd;
					break;
				}
			}
			if(i == FD_SETSIZE)
			{
				printf("too many clients\n");
				return -1;
			}
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
			if((sockfd = client[i]) < 0)
				continue;
			if(FD_ISSET(sockfd, &rset))
			{
				if((n = read(sockfd, buf, MAXLINE)) == 0)
				{
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
				else
				{
					write(sockfd, buf, n);
				}
				if(--nready <= 0)
					break;
			}
		}
	}
	return 0;
}

#if 0
static void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE], buf_tmp[MAXLINE];
	struct sockaddr_in cliaddr;
	socklen_t addrlen = sizeof(cliaddr);
	char dst[100];

	memset(&cliaddr, 0, sizeof(cliaddr));
	if(getpeername(sockfd, (struct sockaddr *)&cliaddr, &addrlen) < 0)
	{
		printf("getpeername error: %s\n", strerror(errno));
		return;
	}
	if(NULL == inet_ntop(AF_INET, &cliaddr.sin_addr, dst, sizeof(dst)))
	{
		printf("inet_ntop error: %s\n", strerror(errno));
		return;
	}
again:
	while((n = read(sockfd, buf, MAXLINE)) > 0)
	{
		memcpy(buf_tmp, buf, n);
		buf_tmp[n]  = '\0';
		printf("from %s, get date: %s", dst, buf_tmp);
		if(write(sockfd, buf, n) < 0)
		{
			printf("write error: %s\n", strerror(errno));
			return;
		}
	}
	if(n < 0 && errno == EINTR)
		goto again;
	else if(n < 0)
		printf("str_echo: read error\n");
	return;
}










#endif


