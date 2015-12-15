#include <unp.h>

int main(int argc, char **argv)
{
	int readid, writeid;

	if((readid = msgget(MQ_KEY1, 0644|IPC_CREAT)) < 0)
	{
		err_sys("msgget error: %s\n", strerror(errno));
	}
	if((writeid = msgget(MQ_KEY2, 0644|IPC_CREAT)) < 0)
	{
		err_sys("msgget error: %s\n", strerror(errno));
	}
	server(readid, writeid);
	exit(0);
}
