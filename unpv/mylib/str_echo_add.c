#include	"unp.h"

void
str_echo_add(int sockfd)
{
	ssize_t		n;
	long arg1, arg2;
//	char		buf[MAXLINE], buf_tmp[MAXLINE];
	char line[MAXLINE];
	struct sockaddr_in cliaddr;
	socklen_t addrlen = sizeof(cliaddr);
	char dst[100];
	bzero(&cliaddr, addrlen);
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
	for(;;)
	{
		if((n == Readline(sockfd, line, MAXLINE)) == 0)
		{
			return;
		}
		if(sscanf(line, "%ld%ld", &arg1, &arg2) == 2)
			snprintf(line, sizeof(line), "%ld\n", arg1 + arg2);
		else
			snprintf(line, sizeof(line), "input error\n");
		n = strlen(line);
		Writen(sockfd, line, n);
	}
}









