#include <unp.h>

int main(int argc, char *argv[])
{
	int c, oflag, mqld;
	key_t mykey;

	oflag = 0644|IPC_CREAT;
	
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
		err_quit("usage: msgcreate [-e] <pathname>");
	}
	if((mykey = ftok(argv[optind], 0)) < 0)
	{
		err_quit("ftok error: %s", strerror(errno));
	}
	if((mqld = msgget(mykey, oflag)) < 0)
	{
		err_quit("msgget error: %s", strerror(errno));
	}
	return 0;
}
