#include <unp.h>

static void msg_client(int, int);

int main(int argc, char **argv)
{
	int msqid;
	if((msqid = msgget(MQ_KEY1, 0)) < 0)
	{
		printf("msgget error: %s\n", strerror(errno));
		return -1;
	}
	
	msg_client(msqid, msqid);
	return 0;
}

static void msg_client(int readid, int writeid)
{
	size_t len;
	ssize_t n;
	char *ptr = NULL;
	struct mymesg mesg;
	
	snprintf(mesg.mesg_data, MAXMESGDATA, "%ld ", (long)getpid());
	len = strlen(mesg.mesg_data);
	ptr = mesg.mesg_data + len;
	fgets(ptr, MAXMESGDATA - len, stdin);
	len = strlen(mesg.mesg_data);
	if(mesg.mesg_data[len - 1] == '\n')
		len--;
	mesg.mesg_len = len;
	mesg.mesg_type = 1;
	if(msgsnd(writeid, &(mesg.mesg_type), mesg.mesg_len, 0) < 0)
	{
		printf("msgsnd error: %s\n", strerror(errno));
		return ;
	}
	mesg.mesg_type = getpid();
	while((n = msgrcv(readid, &(mesg.mesg_type), MAXMESGDATA, mesg.mesg_type, 0)) > 0)
	{
		write(STDOUT_FILENO, mesg.mesg_data, n);
	}
	return ;
}














