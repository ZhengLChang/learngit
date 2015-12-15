#include "../myinclude/unp.h"

int main(int argc, char **argv)
{
	int sockfd, n, counter = 0;
	struct sockaddr_in servaddr;
	char recvline[MAXLINE + 1];

	if(argc != 2)
		err_quit("usage: %s <IPaddress>", argv[0]);
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_quit("socket error");
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton");
	servaddr.sin_port = htons(SERVPORT);
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("connect error: %s\n", strerror(errno));
		return -1;
	}
	while((n = read(sockfd, recvline, MAXLINE)) > 0)
	{
		counter++;
		recvline[n] = 0;
		if(fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
	if(n < 0)
		err_sys("read error\n");
	else if(n == 0)
	{
		printf("service is close\n");
		printf("counter = %d\n", counter);
	}
	return 0;
}
