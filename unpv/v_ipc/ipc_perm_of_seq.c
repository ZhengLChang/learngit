#include <unp.h>
#ifdef MSG_R
#define SVMSG_MODE (MSG_R|MSG_W|MSG_R>>3|MSG_R>>6)
#else
#define SVMSG_MODE (0644)
#endif

int main(int argc, char **argv)
{
	int i, msqid;

	for(i = 0; i < 10; i++)
	{
		if((msqid = msgget(IPC_PRIVATE, SVMSG_MODE|IPC_CREAT)) < 0)
		{
			printf("megget error: %s\n", strerror(errno));
			return -1;
		}
		printf("msqld = %d\n", msqid);
		if(msgctl(msqid, IPC_RMID, NULL) != 0)
		{
			printf("msgctl error: %s\n", strerror(errno));
			return -1;
		}
	}
	return 0;
}
