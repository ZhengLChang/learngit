#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define MAXLINE 1024
#define SERV_PORT 8998
#define NDG 2000
#define DGLEN 1400

static void recvfrom_int(int signo);
void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen);

static int count;

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("socket error: %s\n", strerror(errno));
		return -1;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("bind error: %s\n", strerror(errno));
		return -1;
	}
	dg_echo(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
	return 0;
}

void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
{
	int n; 
	socklen_t len; 
	char mesg[MAXLINE]; 
	struct sigaction act;
	
	act.sa_handler = recvfrom_int; 
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SA_RESTART);

	if(-1 == sigaction(SIGINT, &act, NULL))
	{
		printf("sigaction error: %s\n", strerror(errno));
		return ;
	}

	for(; ;) 
	{ 
		char str[128];
		len = clilen; 
		n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len); 
		if(n < 0)
		{
			printf("recvfrom error: %s\n", strerror(errno));
			continue;
		}
		if(inet_ntop(AF_INET, &((struct sockaddr_in *)pcliaddr)->sin_addr, str, sizeof(str)) == NULL)
		{
			printf("inet_ntop error: %s\n", strerror(errno));
			return;
		}
		printf("receive data from %s\n", str);
		count++;
	}
	return;
}

static void recvfrom_int(int signo)
{
	printf("\nreceived %d datagrams\n", count);
	exit(0);
}














