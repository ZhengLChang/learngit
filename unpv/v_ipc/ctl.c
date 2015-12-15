#include <unp.h>


int main(int argc, char *argv[])
{
	int msqid;
	struct msqid_ds info;
	struct msgbuf buf;
	key_t key;

	if((key = ftok("/disk4", 0x01)) < 0)
	{
		printf("ftok error: %s\n", strerror(errno));
		return -1;
	}

	msqid = msgget(key, IPC_CREAT);
	if(msqid < 0)
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
	printf("read-write: %03o, cbytes = %lu, \
			qnum = %lu, qbytes = %lu\n", 
			info.msg_perm.mode & 0777,
			(ulong)info.msg_cbytes, (ulong)info.msg_qnum,
			(ulong)info.msg_qbytes);
	system("ipcs -q");
	if(msgctl(msqid, IPC_RMID, NULL) < 0)
	{
		printf("msgctl error: %s\n", strerror(errno));
		return -1;
	}
	exit(0);
}












