#include <unp.h>

int main(int argc, char *argv[])
{
	int mqid;
	size_t len;
	long type;
	struct msgbuf *ptr;
	key_t key;
	if(argc != 4)
	{
		printf("usage: msgsnd <pathname> <#bytes> <type>\n");
		return -1;
	}
	len = atol(argv[2]);
	type = atol(argv[3]);
	if((key = ftok(argv[1], 0x01)) < 0)
	{
		printf("ftok error: %s\n", strerror(errno));
		return -1;
	}
	if((mqid = msgget(key, IPC_CREAT)) < 0)	
	{
		printf("msgget error: %s\n", strerror(errno));
		return -1;
	}
	if((ptr = calloc(1, sizeof(long) + len)) == NULL)
	{
		printf("calloc error: %s\n", strerror(errno));
		return -1;
	}
	ptr->mtype = type;
	if(msgsnd(mqid, ptr, len, 0) < 0)
	{
		printf("msgsnd error: %s\n", strerror(errno));
		return -1;
	}
	exit(0);
}












