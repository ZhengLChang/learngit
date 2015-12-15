#include <unp.h>

int main(int argc, char **argv)
{
	int i, msqid, prev = 0;

	for(i = 0; i < 10; i++)
	{
		if((msqid = msgget(IPC_PRIVATE, IPC_CREAT)) < 0)
		{
			printf("msgget error: %s\n", strerror(errno));
			return -1;
		}
		printf("msqid = %d\n", msqid);
		if(prev != 0)
		{
			printf("msqid - prev = %d\n", msqid - prev);
		}
		prev = msqid;
		if(msgctl(msqid, IPC_RMID, NULL) < 0)
		{
			printf("msgctl error: %s\n", strerror(errno));
			return -1;
		}
	}
	return 0;
}
