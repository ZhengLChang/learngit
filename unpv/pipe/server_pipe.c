#include <unp.h>

void server_pipe(int readfd, int writefd)
{
	int fd;
	ssize_t n;
	char buf[MAXLINE + 1];

	if((n = read(readfd, buf, MAXLINE)) < 0)
	{
		printf("read file error: %s\n", strerror(errno));
		return ;
	}
	else if(n == 0)
	{
		printf("end-of-file while reading pathname\n");
		return ;
	}
	buf[n] = '\0';
	if((fd = open(buf, O_RDONLY)) < 0)
	{
		snprintf(buf + n, sizeof(buf) - n, ": can't open, %s\n", 
				strerror(errno));
		n = strlen(buf);
		if(write(writefd, buf, n) < 0)
		{
			printf("write error message occur error: %s\n", strerror(errno));
			return ;
		}
	}
	else
	{
		while((n = read(fd, buf, MAXLINE)) > 0)
		{
			if(write(writefd, buf, n) < 0)
			{
				printf("write message error: %s\n", strerror(errno));
				return ;
			}
		}
		close(fd);
	}
}







