#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(void)
{
	int fd, pid;
	char buffer[20];
	int val;
	ssize_t bytes;
	fd = open("wo.txt", O_RDONLY);
	if(fd < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	printf("%d\n", fd);
	val = fcntl(fd, F_GETFD);
	val |= FD_CLOEXEC;
	fcntl(fd, F_SETFD, val);
	pid = fork();
	if(pid == 0)
	{
		char child_buf[2];
		ssize_t bytes;
		char fd_str[5];
		int ret;
		memset(child_buf, 0, sizeof(child_buf));
		bytes = read(fd, child_buf, sizeof(child_buf) - 1);
		printf("child, bytes: %d, %s\n", bytes, child_buf);
		memset(fd_str, 0, sizeof(fd_str));
		snprintf(fd_str, sizeof(fd_str), "%d", fd);
		ret = execl("./exe1", "exe1", fd_str, NULL);
		if(-1 == ret)
		{
			printf("execl fail: %s\n", strerror(errno));
		}
		return -1;
	}
	waitpid(pid, NULL, 0);
	memset(buffer, 0, sizeof(buffer));
	bytes = read(fd, buffer, sizeof(buffer) - 1);
	printf("parent, bytes: %d, %s\n", bytes, buffer);
	return 0;
}













