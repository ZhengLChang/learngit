#include <unp.h>

#define MAXMSG (8192 + sizeof(long))

int main(int argc, char *argv[])
{
	int mqid;
	ssize_t n;
	struct msgbuf *buf = NULL;

	if(argc != 2)
	{
		err_quit("usage: %s <mqid>", argv[1]);
	}
	mqid = atoi(argv[1]);

	if((buf = malloc(MAXMSG)) == NULL)
	{
		err_quit("malloc error: %s\n", strerror(errno));
	}
	if((n = msgrcv(mqid, buf, MAXMSG, 0, IPC_NOWAIT)) < 0)
	{
		err_quit("msgrcv error: %s", strerror(errno));
	}
	((char *)(buf->mtext))[n] = '\0';
	printf("read %d bytes, type = %ld, data: %s\n", n, buf->mtype, buf->mtext);
	return 0;
}
