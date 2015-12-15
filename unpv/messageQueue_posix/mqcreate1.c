#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct mq_attr attr;

int main(int argc, char **argv)
{
	int c, flags;
	mqd_t mqd;

	flags = O_RDWR | O_CREAT;
	while((c = getopt(argc, argv, "em:z:")) != -1)
	{
		switch(c)
		{
			case 'e':
				flags |= O_EXCL;
				break;
			case 'm':	
				attr.mq_maxmsg = atol(optarg);
				break;
			case 'z':
				attr.mq_msgsize = atol(optarg);
				break;
		}
	}
	if(optind != argc - 1)
	{
		printf("usage: mqcreate [-e] <name>\n");
		return -1;
	}
	if((attr.mq_maxmsg != 0 && attr.mq_msgsize == 0) ||
		(attr.mq_maxmsg == 0 && attr.mq_msgsize != 0))
	{
		printf("must specify both -m maxmsg and -z msgsize\n");
		return -1;
	}
	if((mqd = mq_open(argv[optind], flags, 0666, 
			(attr.mq_maxmsg != 0)? &attr : NULL)) == (mqd_t)-1)
	{
		printf("mq_open error: %s\n", strerror(errno));
		return -1;
	}
	mq_close(mqd);
	return 0;
}














