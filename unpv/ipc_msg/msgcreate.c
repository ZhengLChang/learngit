#include <unp.h>

int main(int argc, char *argv[])
{
	int c, oflag, mqid;
	key_t key;


	oflag = IPC_CREAT;

	while((c = getopt(argc, argv, "e")) != -1)
	{
		switch(c)
		{
			case 'e':
				oflag |= IPC_EXCL;
				break;
		}
	}
	if(optind != argc - 1)
	{
		printf("usage: msgcreate [-e] <pathname>\n");
		return -1;
	}
	if((key = ftok(argv[optind], 0x01)) < 0)
	{
		printf("ftok error: %s\n", strerror(errno));
		return -1;
	}
	if((mqid = msgget(key, oflag)) < 0)
	{
		printf("msgget error: %s\n", strerror(errno));
		return -1;
	}
	exit(0);
}







