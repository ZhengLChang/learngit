#include <unp.h>

static void msg_client(int, int);

int main(int argc, char **argv)
{
	int readid, writeid;
	if((writeid = msgget(MQ_KEY1, IPC_CREAT)) < 0)
	{
		printf("writeid get error: %s\n", strerror(errno));
		return -1;
	}
	if((readid = msgget(MQ_KEY2, IPC_CREAT)) < 0)
	{
		printf("readid get error: %s\n", strerror(errno));
		return -1;
	}
	msg_client(readid, writeid);
	if(msgctl(readid, IPC_RMID, NULL) < 0)
	{
		printf("msgctl get error: %s\n", strerror(errno));
		return -1;
	}
	if(msgctl(writeid, IPC_RMID, NULL) < 0)
	{
		printf("msgctl get error: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

static void msg_client(int readid, int writeid)
{
	size_t len;
	ssize_t n;
	struct mymesg mesg;

	if(!fgets(mesg.mesg_data, MAXMESGDATA, stdin))
	{
		printf("fgets error: %s\n", strerror(errno));
		return ;
	}
	len = strlen(mesg.mesg_data);
	if(mesg.mesg_data[len - 1] == '\n')
	{
		len--;
	}
	mesg.mesg_len = len;
	mesg.mesg_type = 1;

	if(msgsnd(writeid, &(mesg.mesg_type), mesg.mesg_len, 0) < 0)
	{
		printf("msgsnd error: %s\n", strerror(errno));
		return ;
	}
	while((n = msgrcv(readid, &(mesg.mesg_type), MAXMESGDATA, mesg.mesg_type, 0)) > 0)
	{
		if(write(STDOUT_FILENO, mesg.mesg_data, n) < 0)
		{
			printf("msgsnd error: %s\n", strerror(errno));
			return;
		}
	}
	return;
}









