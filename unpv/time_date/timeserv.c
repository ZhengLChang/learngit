#include "../myinclude/unp.h"
#include <time.h>
int main(int argc, char **argv)
{
	int listenfd, connfd, i;
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	time_t ticks;
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		err_sys("socket error");
		return -1;
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVPORT);
	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		err_sys("bind error");
		return -1;
	}
	if(listen(listenfd, 10) < 0)
	{
		err_sys("listen error");
		return -1;
	}
	for( ; ;)
	{
		if((connfd = accept(listenfd, (struct sockaddr *)NULL, NULL)) < 0)
		{
			err_sys("accept error");
			return -1;
		}
		ticks = time(NULL);
		snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
		for(i = 0; i < strlen(buf); i++)
			if(write(connfd, &buf[i], 1) != 1)
			{
				err_sys("write error");
				return -1;
			}
		close(connfd);
	}
	return 0;
}













