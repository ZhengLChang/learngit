#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define SERV_PORT 8099

int main(int argc, char **argv)
{
	struct sockaddr_in servaddr, cliaddr;
	int sockfd;
	socklen_t addrlen;
	char str[128];

	if(argc != 2)
	{
		printf("usage: udpcli <IPaddress>\n");
		return -1;
	}

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("socket error: %s\n", strerror(errno));
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
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("connect error: %s\n", strerror(errno));
		return -1;
	}
	addrlen = sizeof(servaddr);
	if(getsockname(sockfd, (struct sockaddr *)&cliaddr, &addrlen) < 0)
	{
		printf("getsockname error: %s\n", strerror(errno));
		return -1;
	}
	if(inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)) == NULL)
	{
		printf("inet_ntop error: %s\n", strerror(errno));
		return -1;
	}
	printf("self ip: %s, port: %d\n", str, ntohs(cliaddr.sin_port));
	return 0;
}












