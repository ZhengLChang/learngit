#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define MAXLINE 1024
#define SERV_PORT 8998

void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen);

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
	for(; ;)
	{
		len = clilen;
		n = recvfrom(sockfd, mesg, MAXLINE, 0, 
				pcliaddr, &len);
		if(n < 0)
		{
			printf("recvfrom error: %s\n", strerror(errno));
			continue;
		}
		if(sendto(sockfd, mesg, n, 0, pcliaddr, len) < 0)
		{
			printf("sendto error: %s\n", strerror(errno));
			continue;
		}
	}
	return;
}
















