#include <unp.h>

int main(int argc, char **argv)
{
	struct stat mystat;
	if(argc != 2)
	{
		err_quit("usage: ftok <pathname>\n");
	}
	if(-1 == stat(argv[1], &mystat))
	{
		err_quit("stat error: %s\n", strerror(errno));
	}
	printf("st_dev: %lx, st_ino: %lx, id: 0x57, key: %x\n",
			(u_long)mystat.st_dev, (u_long)mystat.st_ino,
			ftok(argv[1], 0x57));
	return 0;
}
