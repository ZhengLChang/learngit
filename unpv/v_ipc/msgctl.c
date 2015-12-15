#include <unp.h>


int main(int argc, char **argv)
{
	int msqid;
	struct msqid_ds info;
#ifdef __USE_GNU
	struct msgbuf buf;
#else
	struct msgbuf
	{
		long int mtype;
		char mtext[1];
	}buf;
#endif

	if((msqid = msgget(IPC_PRIVATE, 0644|IPC_CREAT)) < 0)
	{
		printf("msgget error: %s\n", strerror(errno));
		return -1;
	}
	buf.mtype = 1;
	buf.mtext[0] = 1;
	
	if(msgsnd(msqid, &buf, 1, 0) < 0)
	{
		printf("msgsnd error: %s\n", strerror(errno));
		return -1;
	}
	if(msgctl(msqid, IPC_STAT, &info) < 0)
	{
		printf("msgctl error: %s\n", strerror(errno));
		return -1;
	}
	printf("read-write: %03o, cbyte = %lu,qnum = %lu, qbytes = %lu\n,",
			info.msg_perm.mode & 0777,
			(ulong)info.msg_cbytes,
			(ulong)info.msg_qnum,
			(ulong)info.msg_qbytes);
	system("ipcs -q");
	if(msgctl(msqid, IPC_RMID, NULL) < 0)
	{
		printf("msgctl error: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}


