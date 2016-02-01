#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>

#define MAXLINE 1024
#define SERV_PORT 8099
#define MAX(a, b) ((a)>(b) ? (a) : (b))

void sig_chld(int);

int main(int argc, char **argv)
{
	int listenfd, connfd, udpfd, nready, maxfdp1;
	char mesg[MAXLINE];
	pid_t childpid;
	fd_set rset;
	ssize_t n;
	socklen_t len;
	const int on = 1;
	struct sockaddr_in cliaddr, servaddr;
	struct sigaction act;
	
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("tcp socket error: %s\n", strerror(errno));
		return -1;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		printf("setsockopt error: %s\n", strerror(errno));
		return -1;
	}
	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("bind error: %s\n", strerror(errno));
		return -1;
	}
	if(listen(listenfd, 8) < 0)
	{
		printf("listen error: %s\n", strerror(errno));
		return -1;
	}


	if((udpfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("udp socket error: %s\n", strerror(errno));
		return -1;
	}
	memset(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	if(bind(udpfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("udp bind error: %s\n", strerror(errno));
		return -1;
	}
	act.sa_handler = sig_chld;
	sigemptyset(&act);
	sigaddset(&act, SA_RESTART);
	if(sigaction(SIGCHLD, &act, NULL) < 0)
	{
		printf("sigaction error: %s\n", strerror(errno));
		return -1;
	}
	FD_ZERO(&rset);
	maxfdp1 = MAX(listenfd, udpfd) + 1;
	for(; ;)
	{
		FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);
		if((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
		{
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				printf("select error: %s\n", strerror(errno));
				return -1;
			}
		}
		if(FD_ISSET(listenfd, &rset))
		{
			len = sizeof(cliaddr);
			connfd = accept(listendf, (struct sockaddr *)&cliaddr, &len);
			if(connfd < 0)
			{
				printf("accept error: %s\n", strerror(errno));
				return -1;
			}
			if((childpid = fork()) == 0)
			{
				close(listenfd);
				str_echo(connfd);
				exit(0);
			}
			close(connfd);
		}
		if(FD_ISSET(udpfd, &rset))
		{
			len = sizeof(cliaddr);
			n = recvfrom(udpfd, mesg, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
			sendto(udpfd, mesg, n, 0, (struct sockaddr*)&cliaddr, len);
		}
	}
	return 0;
}

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






















