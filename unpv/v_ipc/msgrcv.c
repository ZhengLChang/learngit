#include <unp.h>

#define MAXMSG (8192 + sizeof(long))

int main(int argc, char **argv)
{
	int c, flag, mqid;
	long type;
	ssize_t n;
	struct msgbuf *buf;
	key_t mykey;
	type = flag = 0;
	while((c = getopt(argc, argv, "nt:")) != -1)
	{
		switch(c)
		{
			case 'n':
				flag |= IPC_NOWAIT;
				break;
			case 't':
				type = atol(optarg);
				break;
		}
	}
	if(optind != argc - 1)
	{
		err_quit("usage:%s [-n] [-t type] <pathname>", argv[0]);
	}
	if((mykey = ftok(argv[optind], 0)) < 0)
	{
		printf("ftok error: %s\n", strerror(errno));
		return -1;
	}
	if((mqid = msgget(mykey, MSG_R)) < 0)
	{
		printf("msgget error: %s\n", strerror(errno));
		return -1;
	}
	if((buf = malloc(MAXMSG)) == NULL)
	{
		printf("malloc error: %s\n", strerror(errno));
		return -1;
	}
	if((n = msgrcv(mqid, buf, MAXMSG, type, flag)) < 0)
	{
		printf("msgrcv error: %s\n", strerror(errno));
		return -1;
	}
	((char *)(buf->mtext))[n] = '\0';
	printf("read %d bytes, type = %ld, data: %s\n", n, buf->mtype, buf->mtext);
	return 0;
}














