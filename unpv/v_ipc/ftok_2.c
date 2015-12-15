#include <unp.h>

int main(int argc, char **argv)
{
	struct stat buf;

	if(argc != 2)
	{
		printf("usage: ftok <pathname>\n");
		return -1;
	}
	if(stat(argv[1], &buf) < 0)
	{
		printf("stat: %s\n", strerror(errno));
		return -1;
	}
	printf("st_dev: %lx, st_ino: %lx, key: %x\n",
			(u_long)buf.st_dev, (u_long)buf.st_ino,
			ftok(argv[1], 0x57));
	exit(0);
}
