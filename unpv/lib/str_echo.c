#include	"unp.h"

void
str_echo(int sockfd)
{
	ssize_t		n;
	char		buf[MAXLINE];
	struct sockaddr_in cliaddr;
	socklen_t addrlen = sizeof(cliaddr);
	char dst[100];
	getpeername(AF_INET, (struct sockaddr *)&cliaddr, &addrlen);
	inet_ntop(AF_INET, &cliaddr.sin_addr, dst, sizeof(dst));
again:
	while ( (n = read(sockfd, buf, MAXLINE)) > 0)
	{
		printf("get date: %s, from %s\n", buf, dst); 
					));
		Writen(sockfd, buf, n);
	}

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo: read error");
}
