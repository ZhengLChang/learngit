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

void dg_cli(FILE *fp, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen);
int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	
	if(argc != 2)
	{
		printf("usage: udpcli <IPaddress>\n");
		return -1;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0)
	{
		printf("inet_pton error: %s\n", strerror(errno));
		return -1;
	}
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("socket error: %s\n", strerror(errno));
		return -1;
	}
	dg_cli(stdin, sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	exit(0);
}

void dg_cli(FILE *fp, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen)
{
	int n;
	char sendline[MAXLINE], recvline[MAXLINE + 1];
	socklen_t len;
	struct sockaddr *preply_addr;
	char ser_ip[128];
	
	if(inet_ntop(pservaddr->sa_family, &((struct sockaddr_in *)pservaddr)->sin_addr, ser_ip, sizeof(ser_ip)) == NULL)
	{
		printf("inet_ntop error: %s\n", strerror(errno));
		return;
	}
	preply_addr = malloc(servlen);


	if(connect(sockfd, (struct sockaddr *)pservaddr, servlen) < 0)
	{
		printf("connect error: %s\n", strerror(errno));
		return ;
	}
	else
	{
		printf("connect to server success, ip: %s, port: %d\n", ser_ip, ntohs(((struct sockaddr_in *)pservaddr)->sin_port));
	}
	
	while(fgets(sendline, MAXLINE, fp) != NULL)
	{
		write(sockfd, sendline, strlen(sendline));
		len = servlen;
		if((n = read(sockfd, recvline, MAXLINE)) < 0)
		{
			printf("read error: %s\n", strerror(errno));
			return ;
		}
		recvline[n] = 0;
		fputs(recvline, stdout);
	}
	free(preply_addr);
	preply_addr = NULL;
	return; 
}















