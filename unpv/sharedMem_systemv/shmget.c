#include <unp.h>

int main(int argc, char **argv)
{
	int c, id, oflag;
	char *ptr = NULL;
	size_t length;
	oflag = IPC_CREAT;
	key_t key;
	while((c = getopt(argc, argv, "e")) != -1)
	{
		switch(c)
		{
			case 'e':
				oflag |= IPC_EXCL;
				break;
		}
	}
	if(optind != argc - 2)
	{
		printf("usage: shmget [-e] <pathname> <length>\n");
		return -1;
	}
	length = atoi(argv[optind + 1]);
	if((key = (ftok(argv[optind], 0x01))) < 0)	
	{
		printf("ftok error: %s\n", strerror(errno));
		return -1;
	}
	if((id = shmget(key, length, oflag)) < 0)
	{
		printf("shmget error: %s\n", strerror(errno));
		return -1;
	}
	if((ptr = shmat(id, NULL, 0)) == (void *)-1)
	{
		printf("shmget error: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}











