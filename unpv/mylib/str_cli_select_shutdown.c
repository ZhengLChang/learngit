#include <unp.h>
void str_cli_select_shutdown(FILE *fp, int sockfd)
{
	int maxfdp1, stdineof;
	fd_set rset;
	char sendline[MAXLINE], recvline[MAXLINE];
	int n = 0;

	stdineof = 0;
	for(;;)
	{
		FD_ZERO(&rset);
		if(stdineof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		if(select(maxfdp1, &rset, NULL, NULL, NULL) < 0)
		{
			printf("select error: %s\n", strerror(errno));
			return;
		}
		if(FD_ISSET(sockfd, &rset))
		{
			if((n = read(sockfd, recvline, MAXLINE)) <= 0)
			{
				if(0 == n && stdineof == 1)
				{
					sleep(30);
					return;
				}
			}
		}
		if(FD_ISSET(fileno(fp), &rset))
		{
			if((n = read(fileno(fp), sendline, MAXLINE)) <= 0)
			{
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			Writen(sockfd, sendline, n);
		}
	}
	return;
}
