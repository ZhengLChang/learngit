#include <unp.h>

int main(int argc, char **argv)
{
	int mqid;
	key_t mykey;

	if(argc != 2)
	{
		err_quit("usage: msgrmid <pathname>");
	}
	if((mykey = ftok(argv[1], 0)) < 0)
	{
		err_quit("ftok error: %s\n", strerror(errno));
	}
	if((mqid = msgget(mykey, 0)) < 0)
	{
		err_quit("msgget error: %s\n", strerror(errno));
	}
	if(msgctl(mqid, IPC_RMID, NULL) < 0)
	{
		err_quit("msgctl error: %s\n", strerror(errno));
	}
	return 0;
}
