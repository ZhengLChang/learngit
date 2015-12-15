#include <unp.h>

int main(int argc, char **argv)
{
	int i, id;
	key_t key;
	struct shmid_ds buff;
	unsigned char  *ptr;
	if(argc != 2)
	{
		printf("usage: shmread <pathname>\n");
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
	if((ptr = shmat(id, NULL, 0)) == (void *)-1)
	{
		printf("shmat error: %s\n", strerror(errno));
		return -1;
	}
	if(shmctl(id, IPC_STAT, &buff) < 0)
	{
		printf("shmctl error: %s\n", strerror(errno));
		return -1;
	}
	for(i = 0; i < buff.shm_segsz; i++)
	{
		printf("read %d shared memory: %d\n", i, *ptr++);
	}
	return 0;
}














