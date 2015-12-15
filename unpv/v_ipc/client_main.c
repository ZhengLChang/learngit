#include <unp.h>

int main(int argc, char **argv)
{
	int readid, writeid;

	if((writeid = msgget(MQ_KEY1, 0)) < 0)
	{		
		err_sys("msgget error: %s\n", strerror(errno));
	}
	if((readid = msgget(MQ_KEY2, 0)) < 0)
	{
		err_sys("msgget error: %s\n", strerror(errno));
	}
	client(readid, writeid);

	if(msgctl(readid, IPC_RMID, NULL) < 0)
	{
		err_sys("msgctl error: %s\n", strerror(errno));
	}
	if(msgctl(writeid, IPC_RMID, NULL) < 0)
	{
		err_sys("msgctl error: %s\n", strerror(errno));
	}
	exit(0);
}
