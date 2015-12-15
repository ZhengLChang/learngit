#include <unp.h>

void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE],recvline[MAXLINE];
	int n;
	while(fgets(sendline, MAXLINE, fp) != NULL)
	{
		//Writen(sockfd, sendline, MAXLINE);
		Writen(sockfd, sendline, strlen(sendline));
#if 0
		Writen(sockfd, sendline, 1);
		sleep(1);
		Writen(sockfd, sendline + 1, strlen(sendline) - 1);
#endif
		if((n = Readline(sockfd, recvline, MAXLINE)) <= 0)
		{
			err_quit("str_cli: server terminated prematurely");
			exit(1);
		}
		fputs(recvline, stdout);
//		write(STDOUT_FILENO, recvline, n);
	}
}
