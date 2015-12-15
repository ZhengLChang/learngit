#include <unp.h>

#define MAXMSG (8192 + sizeof(long))

int main(int argc, char **argv)
{
	int c, flag, mqid;
	long type;
	ssize_t n;
	struct msgbuf *buf;
	key_t key;

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
		printf("usage: msgrcv [-n] [-t type] <pathname>\n");
		return -1;
	}
	if((key = ftok(argv[optind], 0x01)) < 0)
	{
		printf("ftok error: %s\n", strerror(errno));
		return -1;
	}
	if((mqid = msgget(key, IPC_CREAT)) < 0)
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
	printf("read %d bytes, type = %ld\n", n, buf->mtype);
	exit(0);
}










