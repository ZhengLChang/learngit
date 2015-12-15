#include <unp.h>

static void msg_server(int, int);

int main(int argc, char **argv)
{
	int readid, writeid;
	if((readid = msgget(MQ_KEY1, IPC_CREAT)) < 0)
	{
		printf("readid get error: %s\n", strerror(errno));
		return -1;
	}
	if((writeid = msgget(MQ_KEY2, IPC_CREAT)) < 0)
	{
		printf("writeid get error: %s\n", strerror(errno));
		return -1;
	}
	
	msg_server(readid, writeid);
	return 0;
}


static void msg_server(int readid, int writeid)
{
	FILE *fp = NULL;
	ssize_t n;
	struct mymesg mesg;

	mesg.mesg_type = 1;
	if((n = msgrcv(readid, &(mesg.mesg_type), MAXMESGDATA,
				mesg.mesg_type, 0)) < 0)
	{
		printf("get pathname error: %s\n", strerror(errno));
		return ;
	}
	else if(n == 0)
	{
		printf("pathname missing\n");
		return ;
	}
	mesg.mesg_data[n] = '\0';
	if((fp = fopen(mesg.mesg_data, "r")) == NULL)
	{
		snprintf(mesg.mesg_data + n, sizeof(mesg.mesg_data) - n,
				": can't open, %s\n", strerror(errno));
		mesg.mesg_len = strlen(mesg.mesg_data);
		if(msgsnd(writeid, &(mesg.mesg_type), mesg.mesg_len, 0) < 0)
		{
			printf("msgsnd error: %s\n", strerror(errno));
			return ;
		}
	}
	else
	{
		while(fgets(mesg.mesg_data, MAXMESGDATA, fp))
		{
			mesg.mesg_len = strlen(mesg.mesg_data);
			if(msgsnd(writeid, &(mesg.mesg_type), mesg.mesg_len, 0) < 0)
			{
				printf("msgsnd error: %s\n", strerror(errno));
				return ; 
			} 
		}
		fclose(fp);
	} 
	mesg.mesg_len = 0;
	if(msgsnd(writeid, &(mesg.mesg_type), mesg.mesg_len, 0) < 0)
	{
		printf("msgsnd error: %s\n", strerror(errno));
		return ; 
	} 
	return ;
} 









