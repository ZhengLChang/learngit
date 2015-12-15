#include	"unp.h"

void
str_echo(int sockfd)
{
	ssize_t		n;
	char		buf[MAXLINE], buf_tmp[MAXLINE];
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
	while ( (n = read(sockfd, buf, MAXLINE)) > 0)
	{
		memcpy(buf_tmp, buf, n); 
		buf_tmp[n] = '\0';	
		printf("from %s, get date: %s", dst, buf_tmp); 
		Writen(sockfd, buf, n);
	}

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo: read error");
}
