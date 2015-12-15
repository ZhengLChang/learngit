#include <unp.h>

int main(int argc, char **argv)
{
	int i, id;
	struct shmid_ds buff;
	unsigned char *ptr;
	key_t key;
	if(argc != 2)
	{
		printf("usage: shmwrite <pathname>\n");
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
		printf("write to share memory: %d\n", i);
		*ptr++ = i % 256;
	}
	return 0;
}














