#include <unp.h>

static void msg_server_interation(int, int);
void sig_chld(int signo);

int main(int argc, char *argv[])
{
	int msqid;
	if((msqid = msgget(MQ_KEY1, IPC_CREAT)) < 0)
	{
		printf("msgget error: %s\n", strerror(errno));
		return -1;
	}
	msg_server_interation(msqid, msqid);
	exit(0);
}


static void msg_server_interation(int readid, int writeid)
{
	FILE *fp = NULL;
	char *ptr;
	ssize_t n;
	struct mymesg mesg;

	signal(SIGCHLD, sig_chld);
	for( ; ; )
	{
		mesg.mesg_type = 1;
		if((n = msgrcv(readid, &(mesg.mesg_type), 
				MAXMESGDATA, mesg.mesg_type, 0)) <= 0)
		{
			printf("msgrcv error: %s\n", strerror(errno));
			continue;
		}
		mesg.mesg_data[n] = '\0';
		if((ptr = strchr(mesg.mesg_data, ' ')) == NULL)
		{
			printf("bogus request: %s", mesg.mesg_data);
			continue;
		}
		*ptr++ = 0;
		writeid = atol(mesg.mesg_data);
		if(fork() == 0)
		{
		if((fp = fopen(ptr, "r")) == NULL)
		{
			snprintf(mesg.mesg_data + n, 
				sizeof(mesg.mesg_data) - n, 
				": can't open, %s\n", 
				strerror(errno));
			mesg.mesg_len = strlen(mesg.mesg_data);
			if(msgsnd(writeid, &(mesg.mesg_type),
					mesg.mesg_len, 0) < 0)
			{
				printf("msgrcv error: %s\n", strerror(errno));
				continue;
			}
		}
		else
		{
			while(fgets(mesg.mesg_data, MAXMESGDATA, fp) != NULL)
			{
				mesg.mesg_len = strlen(mesg.mesg_data);
				if(msgsnd(writeid, &(mesg.mesg_type),
						mesg.mesg_len, 0) < 0)
				{
					printf("msgrcv error: %s\n", strerror(errno));
					continue;
				}
			}
			fclose(fp);
		}
		mesg.mesg_len = 0;
		if(msgsnd(writeid, &(mesg.mesg_type),
				mesg.mesg_len, 0) < 0)
		{
			printf("msgrcv error: %s\n", strerror(errno));
			continue;
		}
		}
	}
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0);
	return;
}












