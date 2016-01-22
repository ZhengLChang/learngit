#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **args)
{
	char buffer[20];
	int fd = atoi(args[1]);
	memset(buffer, 0, sizeof(buffer));
	ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
	if(bytes < 0)
	{
		perror("exec1: read fail\n");
		return -1;
	}
	else
	{
		printf("exec1: read %d, %s\n", bytes, buffer);
		return 0;
	}
}
