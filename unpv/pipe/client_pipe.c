#include <unp.h>

void client_pipe(int readfd, int writefd)
{
	size_t len;
	ssize_t n;
	char buff[MAXLINE];

	if(fgets(buff, MAXLINE, stdin) == NULL)
	{
		printf("fgets error: %s\n", strerror(errno));
		return ;
	}
	len = strlen(buff);
	if(buff[len - 1] == '\n')
		len--;
	if(write(writefd, buff, len) < 0)
	{
		printf("write error: %s\n", strerror(errno));
		return ;
	}
	while((n = read(readfd, buff, MAXLINE)) > 0)
	{
		if(write(STDOUT_FILENO, buff, n) < 0)
		{
			printf("write error: %s\n", strerror(errno));
			return;
		}
	}
	return ;
}











