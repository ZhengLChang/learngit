#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


void int_handler(int signum)
{
	printf("int handler %d\n", signum);
	return;
}

int main(int argc, char **argv)
{
	char buf[100];
	ssize_t ret;
	struct sigaction act;

	act.sa_handler = int_handler;
	act.sa_flags = 0;
//	act.sa_flags |= SA_RESTART;
	sigemptyset(&act.sa_mask);
	if(-1 == sigaction(SIGINT, &act, NULL))
	{
		printf("sigaction error: %s\n", strerror(errno));
		return -1;
	}
	memset(buf, 0, sizeof(buf));
	if((ret = read(STDIN_FILENO, buf, sizeof(buf) - 1)) < 0)
	{
		printf("read error %s\n", strerror(errno));
		return -1;
	}
	printf("read %d bytes, content is %s\n", ret, buf);
	sleep(10);
	return 0;
}











