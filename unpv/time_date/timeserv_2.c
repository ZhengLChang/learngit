#include "unp.h"

int main(int argc, char **argv)
{
	int listenfd, connfd;
	socklen_t len;
	struct sockaddr_in servaddr, cliaddr;
	char buf[MAXLINE], *ptr;
	time_t ticks;
	int backlog = 10;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		printf("listenfd error: %s\n", strerror(errno));
		return -1;
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVPORT);

	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("bind error: %s\n", strerror(errno));
		return -1;
	}
	if((ptr = getenv("LISTENQ")) != NULL)
	{
		backlog = atoi(ptr);
	}
	if(listen(listenfd, backlog) < 0)
	{
		printf("listen error: %s\n", strerror(errno));
		return -1;
	}
	for(;;)
	{
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
		if(connfd < 0)
		{
			printf("accept error: %s\n", strerror(errno));
			return -1;
		}
		printf("connect from %s, port %d\n",
				inet_ntop(AF_INET, &(cliaddr.sin_addr), buf, sizeof(buf)),
				ntohs(cliaddr.sin_port));
		ticks = time(NULL);
		snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
		write(connfd, buf, strlen(buf));
		close(connfd);
	}
	return 0;
}












