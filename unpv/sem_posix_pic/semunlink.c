#include <unp.h>

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("usage: semunlink <name>\n");
		return -1;
	}
	if(sem_unlink(argv[1]) < 0)
	{
		printf("sem_unlink error: %s\n", strerror(errno));
		return -1;
	}
	exit(0);
}
