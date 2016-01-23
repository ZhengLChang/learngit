#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#define MAXLINE 128
#define SERV_PROT 8898 
static void str_echo(int sockfd);

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
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	struct sigaction act;

	act.sa_handler = sig_chld;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_flags != SA_RESTART;
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
	for(;;)
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
		if((childpid = fork()) == 0)
		{
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
	}
	return 0;
}


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













