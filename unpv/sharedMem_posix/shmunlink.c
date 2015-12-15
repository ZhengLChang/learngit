#include <unp.h>

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("usage: shmunlink <name>\n");
		return -1;
	}
	if(shm_unlink(argv[1]) < 0)
	{
		printf("shm_unlink error: %s\n", strerror(errno));
		return -1;
	}
	exit(0);
}
