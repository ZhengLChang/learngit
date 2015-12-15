#include <unp.h>

int main(int argc, char **argv)
{
	mqd_t mqd;
	struct mq_attr attr;
	if(argc != 2)
	{
		printf("usage: mqgetattr <name>\n");
		return -1;
	}
	if((mqd = mq_open(argv[1], O_RDONLY)) == (mqd_t)-1)
	{
		printf("mq_open error: %s\n", strerror(errno));
		return -1;
	}
	if(mq_getattr(mqd, &attr) == -1)
	{
		printf("mq_getattr error: %s\n", strerror(errno));
		return -1;
	}
	printf("max #msgs = %ld, max #bytes/msg = %ld,"
			"$currently on queue = %ld\n",
			attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
	mq_close(mqd);
	return 0;
}












