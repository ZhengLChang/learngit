#include <unp.h>

int main(int argc, char *argv[])
{
	int sockfd[5], i;
	struct sockaddr_in servaddr;
	if(argc != 2)
	{
		printf("usage: tcpcli <IPaddress>\n");
		return -1;
	}
	for(i = 0; i < 5; i++)
	{
		if((sockfd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("socket error: %s\n", strerror(errno));
			return -1;
		}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVPORT);
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
	{
		printf("inet_pton error: %s\n", strerror(errno));
		return -1;
	}
	if(connect(sockfd[i], (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("connect error: %s\n", strerror(errno));
		return -1;
	}
	}
	str_cli(stdin, sockfd[0]);
	exit(0);
}
