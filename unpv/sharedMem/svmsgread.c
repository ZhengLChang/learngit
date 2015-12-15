#include <unp.h>

#defene MAXMSG (8192 + sizeof(long))

int main(int argc, char **argv)
{
	int pipe1[2, pipe2[2], mqid;
	char c;
	pid_t childpid;
	fd_set rset;
	ssize_t n, nread;
	struct msgbuf *buff;

	if(argc != 2)
	{
		printf("usage: svmsgread <pathname>\n");
		return -1;
	}
	if(pipe(pipe1) < 0)
	{
		printf("pipe error: %s\n", strerror(errno));
		return -1;
	}
	if(pipe(pipe2) < 0)
	{
		printf("pipe error: %s\n", strerror(errno));
		return -1;
	}
	if((buff = mmap(NULL, MAXMSG, PROT_READ | PROT_WRITE,
					MAP_ANON | MAP_SHARED, -1, 0)) == MAP_FAILED)
	{
		printf("mmap error: %s\n", strerror(errno));
		return -1;
	}
	if((childpid = fork()) == 0)
	{
		close(pipe1[1]);
		close(pipe2[2]);
		
		if((mqid = msgget(ftok(argv[1], 0x01), MSG_R)))
	}
}



















