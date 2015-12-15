#include	"unp.h"
#include <sum.h>

void str_echo_binary(int sockfd)
{
	ssize_t		n;
//	char		buf[MAXLINE], buf_tmp[MAXLINE];
	struct args args;
	struct result result;
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
again:
	while ( (n = read(sockfd, &args, MAXLINE)) > 0)
	{
		result.sum = args.arg1 + args.arg2;
		Writen(sockfd, &result, sizeof(result));
	}

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo: read error");
	return;
}
