#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>

#define MAXLINE 128
#define SERV_PORT 8898

#define MAX(x,y) ((x) > (y) ? (x) : (y))

static void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if(argc != 2)
	{
		printf("usage: %s <IPaddress>\n", argv[0]);
		return -1;
	}
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error: %s\n", strerror(errno));
		return -1;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	if(1 != inet_pton(AF_INET, argv[1], &servaddr.sin_addr))
	{
		printf("inet_pton error: %s\n", strerror(errno));
		return -1;
	}
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("connect error: %s\n", strerror(errno));
		return -1;
	}
	str_cli(stdin, sockfd);
	exit(0);
}

static void str_cli(FILE *fp, int sockfd)
{
	int maxfdp1;
	fd_set rset;
	char sendline[MAXLINE], recvline[MAXLINE];
	int n;

	for(; ;)
	{
		FD_ZERO(&rset);
		FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = MAX(fileno(fp), sockfd) + 1;
		if(select(maxfdp1, &rset, NULL, NULL, NULL) > 0)
		{
			if(FD_ISSET(sockfd, &rset))
			{
				if((n = read(sockfd, recvline, MAXLINE)) < 0)
				{
					printf("read error: %s\n", strerror(errno));
					return;
				}
				else if(n == 0)
				{
					printf("str_cli: server terminated prematurely!\n");
					return;
				}
				recvline[n] = '\0';
				fputs(recvline, stdout);
			}
			if(FD_ISSET(fileno(fp), &rset))
			{
				if(fgets(sendline, MAXLINE, fp) == NULL)
				{
					return;
				}
				write(sockfd, sendline, strlen(sendline));
			}
		}
	}
	return;
}




