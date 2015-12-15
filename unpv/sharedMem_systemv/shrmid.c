#include <unp.h>

int main(int argc, char **argv)
{
	int id;
	key_t key;
	if(argc != 2)
	{
		printf("usage: shmrmid <pathname>\n");
		return -1;
	}
	if((key = ftok(argv[1], 0x01)) < 0)
	{
		printf("ftok error: %s\n", strerror(errno));
		return -1;
	}
	if((id = shmget(key, 0, 0)) < 0)
	{
		printf("shmget error: %s\n", strerror(errno));
		return -1;
	}
	if(shmctl(id, IPC_RMID, NULL) < 0)
	{
		printf("shmctl error: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}


