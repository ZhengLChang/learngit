#include <unp.h>
void sig_handler(int signo);
int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;
	FILE *fp = NULL;
	if(argc != 2)
	{
		printf("usage: tcpcli <IPaddress>\n");
		return -1;
	}
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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
	if(SIG_ERR == Signal(SIGPIPE, sig_handler))
	{
		printf("Signal error\n");
		return -1;
	}
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("connect error: %s\n", strerror(errno));
		return -1;
	}
	if((fp = fopen("./tcpTest.txt", "r")) == NULL)
	{
		printf("fopen error: %s\n", strerror(errno));
		return -1;
	}
#if 0
	if(-1 == dup2(fileno(fp), STDIN_FILENO))
	{
		printf("dup2 error: %s\n", strerror(errno));
		return -1;
	}
#endif
	str_cli_select(stdin, sockfd);
	exit(0);
}
void sig_handler(int signo)
{
	if(signo == SIGPIPE)
	{
		printf("get SIGPIPE\n");
		exit(1);
	}
	else
	{
		printf("get unknown signal\n");
	}
}
