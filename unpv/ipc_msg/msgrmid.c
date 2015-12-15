#include <unp.h>

int main(int argc, char **argv)
{
	int mqid;
	key_t key;
	if(argc != 2)
	{
		printf("usage: msgrmid <pathname>\n");
		return -1;
	}
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
	if((msgctl(mqid, IPC_RMID, NULL)) < 0)
	{
		printf("msgctl error: %s\n", strerror(errno));
		return -1;
	}
	exit(0);
}
