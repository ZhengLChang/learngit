#include <unp.h>

int main(int argc, char *argv[])
{
	int c, flags;
	sem_t *sem;
	unsigned int value;

	flags = O_RDWR | O_CREAT;
	value = 1;
	while((c = getopt(argc, argv, "ei:")) != -1)
	{
		switch(c)
		{
			case 'e':
				flags |= O_EXCL;
				break;
			case 'i':
				value = atoi(optarg);
				break;
		}
	}
	if(optind != argc - 1)
	{
		printf("usage: semcreate [-e] [-i initalvalue] <name>\n");
		return -1;
	}
	if((sem = sem_open(argv[optind], flags, 0666, value)) == SEM_FAILED)
	{
		printf("sem_open error: %s\n", strerror(errno));
		return -1;
	}
	sem_close(sem);
	exit(0);
}










