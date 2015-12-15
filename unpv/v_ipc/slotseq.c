#include <unp.h>

int main(int argc, char *argv[])
{
	int i, msqid;
	struct msqid_ds info;
	char printStr[108];

	for(i = 0; i < 10; i++)
	{
		if((msqid = msgget(IPC_PRIVATE, IPC_CREAT)) < 0)
		{
			snprintf(printStr, 100, "msgget error");
			goto END;
		}
		if(msgctl(msqid, IPC_STAT, &info) < 0)
		{
			snprintf(printStr, 100, "msgctl getstat error");
			goto END;
		}
		printf("msqid = %d, sql = %u\n", msqid, info.msg_perm.__seq);
		if(msgctl(msqid, IPC_RMID, NULL) < 0)
		{
			snprintf(printStr, 100, "msgctl rmid error");
			goto END;
		}
	}
	return 0;
END:
	printf("%s: %s\n", printStr, strerror(errno));
	return -1;
}
