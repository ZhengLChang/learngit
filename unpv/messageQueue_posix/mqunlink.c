#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("usage: mqunlink <name>\n");
		return -1;
	}
	mq_unlink(argv[1]);
	return 0;
}
